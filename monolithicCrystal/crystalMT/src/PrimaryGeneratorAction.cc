#include "PrimaryGeneratorAction.hh"
#include "OpticalMaterialsProperties.hh"
#include <G4MaterialPropertiesTable.hh>
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include <G4Material.hh>
#include <G4PhysicsOrderedFreeVector.hh>
#include "G4RunManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "GlobalPars.hh"
#include "HistogramManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  //  fMessenger = new PrimaryGeneratorMessenger(this);


  // Get the pointer to the logical volume
  auto fenvLV = G4LogicalVolumeStore::GetInstance()->GetVolume("CRYSTAL");
  if (!fenvLV) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get CRYSTAL logical volume");
  }

  auto fEnvelopeBox = dynamic_cast<G4Box*>(fenvLV->GetSolid());
  if (!fEnvelopeBox) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get CRYSTAL solid volume");
  }

  // dimensions of box in XY and z
  fXY = fEnvelopeBox->GetXHalfLength() * 2;
  fZ = fEnvelopeBox->GetZHalfLength() * 2;
 
  // Get material properties 
  G4Material* mat = fenvLV->GetMaterial();
  G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();

  if (!mpt) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Material properties not defined for this material!");
  }
  
  // Get the spectrum
  auto spectrum = mpt->GetProperty("SCINTILLATIONCOMPONENT1");

  if (!spectrum) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Scintillation component not defined for this material!");
  }

  fSpectrumIntegral = new G4PhysicsOrderedFreeVector();
  ComputeCumulativeDistribution(*spectrum, *fSpectrumIntegral);
  fScmax = fSpectrumIntegral->GetMaxValue();


  // Get yield
  
  auto yieldMeV = mpt->GetConstProperty("SCINTILLATIONYIELD");
  if (!yieldMeV) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Scintillation yield not defined for this material!");
  }

  fScintPhotons = static_cast<int>(yieldMeV * 0.511) ; // energy of gammas

  
  G4cout << "in PrimaryGeneratorAction::PrimaryGeneratorAction()" << G4endl;
  G4cout << " box XY =" << fXY/mm << " box Z = " << fZ/mm << " max spectrum "
         << fScmax/eV << " yield/MeV = " << yieldMeV
         << " avg photons = " << fScintPhotons <<G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //  delete fMessenger;
  delete fSpectrumIntegral;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  //this function is called at the begining of ecah event
  // Here we will generate a Primary Vertex with random energy and position for each event
  // a number n of optical photons will be generated. 
  
 
  // Create a new vertex
  auto x0 = fXY * (G4UniformRand()-0.5);
  auto y0 = fXY * (G4UniformRand()-0.5);
  auto z0 = -0.5 * fZ  * (G4UniformRand()-0.5);
  auto time = 0; // all optical photons generated here with same time.
  G4ThreeVector position(x0,y0,z0);
    
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);


  // Depending of the value of fGaussian, generate photons with a gaussian distribution
  // around fScintPhotons (obtained in the constructor from properties of the crystal)
  // of generate a fixed number of photons.
  
  auto scint_photons = 0;
  if (GlobalPars::Instance()->fGaussian)
    {
      scint_photons = G4RandGauss::shoot(fScintPhotons, sqrt(GlobalPars::Instance()->fFano * fScintPhotons));
    }
  else
    {
      scint_photons = GlobalPars::Instance()->fNphotons;
    }

  HistogramManager::Instance()->FillHistogram("Yield", scint_photons);

  // Define optical photons
  G4ParticleDefinition* particle_definition = G4OpticalPhoton::Definition();
  
  // For each photon generate random direction and sample the energy from the spectrum of emission
  for ( G4int i = 0; i<scint_photons; i++)
    {
      // Generate random direction by default
      G4ThreeVector momentum_direction = G4RandomDirection();
      // Determine photon energy
      auto sc_value = G4UniformRand()*fScmax;
      auto pmod = fSpectrumIntegral->GetEnergy(sc_value);
      auto wl = 1240.0 / (pmod/eV);
      auto px = pmod * momentum_direction.x();
      auto py = pmod * momentum_direction.y();
      auto pz = pmod * momentum_direction.z();

      HistogramManager::Instance()->FillHistogram("PrimaryParticleSpectrum_nm", wl);
    
      // Create the new primary particle (an optical photon) and set it some properties
      G4PrimaryParticle* particle = new G4PrimaryParticle(particle_definition, px, py, pz);
      
      G4ThreeVector polarization = G4RandomDirection();
      particle->SetPolarization(polarization);
      
      // Add particle to the vertex and this to the event
      vertex->SetPrimary(particle);
    }
  event->AddPrimaryVertex(vertex);

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::ComputeCumulativeDistribution(
  const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf)
{

  // This is the CDF used to sample the energy 
  auto sum = 0.;
  cdf.InsertValues(pdf.Energy(0), sum);

  for (unsigned int i=1; i<pdf.GetVectorLength(); ++i) {
    auto area =
      0.5 * (pdf.Energy(i) - pdf.Energy(i-1)) * (pdf[i] + pdf[i-1]);
    sum = sum + area;
    cdf.InsertValues(pdf.Energy(i), sum);
  }
}
