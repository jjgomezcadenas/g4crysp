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


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
      fMessenger = new PrimaryGeneratorMessenger(this);
      G4cout << "in PrimaryGeneratorAction::PrimaryGeneratorAction()" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //this function is called at the begining of ecah event
  // Here we will generate a Primary Vertex with random energy and position for each event
  // a number n of optical photons will be generated. 

  // Generate an optical photon 
  G4ParticleDefinition* particle_definition = G4OpticalPhoton::Definition();
  
  auto envSizeXY = 0;
  auto envSizeZ = 0;
  auto size = 0.99;
  auto time = 0;

  if (!fenvLV) // this is the pointer to logical volume
  {
    fenvLV = G4LogicalVolumeStore::GetInstance()->GetVolume("CRYSTAL");
    if ( fenvLV ) fEnvelopeBox = dynamic_cast<G4Box*>(fenvLV->GetSolid());
  }

  if ( fEnvelopeBox ) {
    envSizeXY = fEnvelopeBox->GetXHalfLength() * 2;
    envSizeZ = fEnvelopeBox->GetZHalfLength() * 2;

    auto x0 = size * envSizeXY * (G4UniformRand()-0.5);
    auto y0 = size * envSizeXY * (G4UniformRand()-0.5);
    auto z0 = -0.5 * envSizeZ;

    G4ThreeVector position(x0,y0,z0);
    
    G4Material* mat = fenvLV->GetMaterial();
    G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();

    if (!mpt) {
      G4Exception("[PrimaryGeneratorAction]", "GeneratePrimaries", FatalException,
                  "Material properties not defined for this material!");
    }
    // Using fast or slow component here is irrelevant, since we're not using time
    // and they're are the same in energy.
    
    G4MaterialPropertyVector* spectrum = mpt->GetProperty("SCINTILLATIONCOMPONENT1");

    if (!spectrum) {
      G4Exception("[PrimaryGeneratorAction]", "GeneratePrimaries", FatalException,
                  "Fast time decay constant not defined for this material!");
    }

    G4PhysicsOrderedFreeVector* spectrum_integral = new G4PhysicsOrderedFreeVector();
    ComputeCumulativeDistribution(*spectrum, *spectrum_integral);
    auto sc_max = spectrum_integral->GetMaxValue();

    // Create a new vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

    G4int scint_photons = fNphotons;

    if (fGaussian)
      {
        scint_photons = G4RandGauss::shoot(fNphotons, sqrt(fFano * fNphotons));
      }

    fN++;

    if(fN%50 ==0)
      {
        G4cout << "---:event = " << fN << " GeneratePrimaries:  " << scint_photons << " PHOTONS" << G4endl;
      }
    
    for ( G4int i = 0; i<scint_photons; i++)
      {
        // Generate random direction by default
        G4ThreeVector momentum_direction = G4RandomDirection();
        // Determine photon energy
        auto sc_value = G4UniformRand()*sc_max;
        auto pmod = spectrum_integral->GetEnergy(sc_value);
        auto px = pmod * momentum_direction.x();
        auto py = pmod * momentum_direction.y();
        auto pz = pmod * momentum_direction.z();

        if (fN%50 ==0 and i==0)
          {
            G4cout << " First photon: x0 (mm) = " << x0/mm <<" y0 = " << y0/mm << " z0 = " << z0/mm
                   << " px (eV) = "<< px/eV << " py ="  << py/eV <<" pz = " << pz/eV
                   << " energy = " << pmod/eV << G4endl;
          }

      // Create the new primary particle and set it some properties
        G4PrimaryParticle* particle = new G4PrimaryParticle(particle_definition, px, py, pz);

        G4ThreeVector polarization = G4RandomDirection();
        particle->SetPolarization(polarization);

        // Add particle to the vertex and this to the event
        vertex->SetPrimary(particle);
    }
  event->AddPrimaryVertex(vertex);

  }
  else  {
    G4ExceptionDescription msg;
    msg << "Envelope volume (CRYSTAL) of box shape not found.\n";
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be place at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
     "MyCode0002",JustWarning,msg);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::ComputeCumulativeDistribution(
  const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf)
{
  auto sum = 0.;
  cdf.InsertValues(pdf.Energy(0), sum);

  for (unsigned int i=1; i<pdf.GetVectorLength(); ++i) {
    auto area =
      0.5 * (pdf.Energy(i) - pdf.Energy(i-1)) * (pdf[i] + pdf[i-1]);
    sum = sum + area;
    cdf.InsertValues(pdf.Energy(i), sum);
  }
}
