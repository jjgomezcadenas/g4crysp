#include "PrimaryGeneratorAction.hh"
#include "OpticalMaterialsProperties.hh"
#include <G4MaterialPropertiesTable.hh>
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
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
  auto fenvLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Organ");
  
  if (!fenvLV) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get Organ logical volume");
  }

  auto fEnvelopeBox = dynamic_cast<G4Sphere*>(fenvLV->GetSolid());
  if (!fEnvelopeBox) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get Organ solid volume");
  }

 
  fRadius = fEnvelopeBox->GetOuterRadius();

  
  fParticleGun = new G4ParticleGun(1);
  
  // Define particle type as gamma
  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);

  // Set particle energy to 511 keV
  fParticleGun->SetParticleEnergy(GlobalPars::Instance()->fGammaEnergy);

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
  
  G4ThreeVector position = fRadius * G4RandomDirection();
  fParticleGun->SetParticlePosition(position);

  // Set opposite directions for the two gammas
  G4ThreeVector direction1 = G4RandomDirection();
  
  fParticleGun->SetParticleMomentumDirection(direction1);
  fParticleGun->GeneratePrimaryVertex(event);

  // Generate second gamma (opposite direction)
  fParticleGun->SetParticleMomentumDirection(-direction1);
  fParticleGun->GeneratePrimaryVertex(event);

 
}

