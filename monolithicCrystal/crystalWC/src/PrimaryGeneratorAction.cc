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
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
//#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "GlobalPars.hh"
#include "HistogramManager.hh"

#include <iostream>
#include <fstream>
#include <mutex>


//std::mutex PrimaryGeneratorAction::gammaIntFileMutex;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
 
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
 
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // Define particle type as gamma
  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);

  // Set particle energy to 511 keV
  fParticleGun->SetParticleEnergy(GlobalPars::Instance()->fGammaEnergy);

  // Set particle momentum direction (initially set along z-axis)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  //this function is called at the begining of ecah event
  // Here we will generate a Primary Vertex with random energy and position for each event
  // a number n of optical photons will be generated. 
  
 
  // Create a new vertex uniform in xy and starting at the beginning of crystal.  
  auto x0 = fXY * (G4UniformRand()-0.5);
  auto y0 = fXY * (G4UniformRand()-0.5);
  auto z0 = -fZ/2 + 0.1;
  

  HistogramManager::Instance()->FillHistogram("X0", x0);
  HistogramManager::Instance()->FillHistogram("Y0", y0);
  HistogramManager::Instance()->FillHistogram("Z0", z0); 

  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->GeneratePrimaryVertex(event);

}

