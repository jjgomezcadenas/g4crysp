#include "PrimaryGeneratorAction.hh"
#include "OpticalMaterialsProperties.hh"
#include <G4MaterialPropertiesTable.hh>
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include <G4VPhysicalVolume.hh>
#include "G4Transform3D.hh"
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
#include "G4ThreeVector.hh"
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
  //fMessenger = new PrimaryGeneratorMessenger(this);
  // Get the pointer to the logical volume
  auto heartLogVol = G4LogicalVolumeStore::GetInstance()->GetVolume("Heart");
  auto trunkLogVol = G4LogicalVolumeStore::GetInstance()->GetVolume("logicalTrunk");
  auto bodyLogVol = G4LogicalVolumeStore::GetInstance()->GetVolume("PHANTOM");
  
  if (!heartLogVol) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get Heart logical volume");
  }

 // Find the physical volume associated with the logical volume of the heart
  auto heartPhysVol = GetPhysicalVolumeFromLogical(heartLogVol);
  if (!heartPhysVol) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
              "Physical volume for the heart not found!");
      return;
  }

  auto trunkPhysVol = GetPhysicalVolumeFromLogical(trunkLogVol);
  if (!trunkPhysVol) {
      G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Physical volume for the trunk not found!");
        return;
    }

  auto bodyPhysVol = GetPhysicalVolumeFromLogical(bodyLogVol);
  if (!bodyPhysVol) {
      G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Physical volume for the PHANTOM not found!");
        return;
    }

  auto heartSolid = dynamic_cast<G4Sphere*>(heartLogVol->GetSolid());
  if (!heartSolid) {
    G4Exception("[PrimaryGeneratorAction]", "Constructor", FatalException,
                "Cannot get Heart solid volume");
  }

  fHeartRadius = heartSolid->GetOuterRadius();

  // Retrieve the translation and rotation of each physical volume
    auto heartPositionInTrunk = heartPhysVol->GetTranslation();
    auto heartRotationInTrunk = heartPhysVol->GetRotation();

    auto trunkPositionInBody = trunkPhysVol->GetTranslation();
    auto trunkRotationInBody = trunkPhysVol->GetRotation();

    auto bodyPositionInLab = bodyPhysVol->GetTranslation();
    auto bodyRotationInLab = bodyPhysVol->GetRotation();

    // Accumulate the transformations
    // Start with the heart's transformation in the trunk
    G4Transform3D heartToTrunkTransform;
    if (heartRotationInTrunk) {
        heartToTrunkTransform = G4Transform3D(*heartRotationInTrunk, heartPositionInTrunk);
    } else {
        heartToTrunkTransform = G4Transform3D(G4RotationMatrix(), heartPositionInTrunk);
    }

    // Then, apply the trunk's transformation in the body
    G4Transform3D trunkToBodyTransform;
    if (trunkRotationInBody) {
        trunkToBodyTransform = G4Transform3D(*trunkRotationInBody, trunkPositionInBody);
    } else {
        trunkToBodyTransform = G4Transform3D(G4RotationMatrix(), trunkPositionInBody);
    }

    // Finally, apply the body's transformation in the lab
    G4Transform3D bodyToLabTransform;
    if (bodyRotationInLab) {
        bodyToLabTransform = G4Transform3D(*bodyRotationInLab, bodyPositionInLab);
    } else {
        bodyToLabTransform = G4Transform3D(G4RotationMatrix(), bodyPositionInLab);
    }

    // Combine all transformations to get the global (lab) position of the heart
    G4Transform3D heartToLabTransform = bodyToLabTransform * trunkToBodyTransform * heartToTrunkTransform;

    // Extract the final position of the heart in the lab
    G4ThreeVector heartPositionInLab = heartToLabTransform.getTranslation();
    G4cout << "Heart position in lab (x, y, z): " 
           << heartPositionInLab.x() / CLHEP::cm << " cm, " 
           << heartPositionInLab.y() / CLHEP::cm << " cm, " 
           << heartPositionInLab.z() / CLHEP::cm << " cm" << G4endl;

  fHeartPosition = heartPositionInLab;

  // Get the position of the heart inside the parent (e.g., body)
  // fHeartPosition = heartPhysVol->GetTranslation() + trunkPhysVol->GetTranslation();
  // G4cout << "Heart position (x, y, z): " 
  //          << fHeartPosition.x() << ", " 
  //          << fHeartPosition.y() << ", " 
  //          << fHeartPosition.z() << G4endl;

  G4cout << "Primary generator: radius of heart (mm) = " << fHeartRadius/mm << G4endl;

  fParticleGun = new G4ParticleGun(1);
  
  // Define particle type as gamma
  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);

  // Set particle energy to 511 keV
  G4cout << "Primary generator: gamma energy (keV) = " << GlobalPars::Instance()->fGammaEnergy/keV << G4endl;
  fParticleGun->SetParticleEnergy(GlobalPars::Instance()->fGammaEnergy);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  //delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  auto cosTheta = 2.0 * G4UniformRand() - 1.0;
  auto sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
  auto phi = 2.0 * M_PI * G4UniformRand();
  auto r = fHeartRadius * std::cbrt(G4UniformRand());  // Cube root for uniform sampling

  // Generate the random point in spherical coordinates
  auto x = r * sinTheta * std::cos(phi);
  auto y = r * sinTheta * std::sin(phi);
  auto z = r * cosTheta;

  auto randomPoint = fHeartPosition + G4ThreeVector(x, y, z);
  //auto randomPoint = fHeartPosition ;

  // G4cout << "Random point in heart (x, y, z): " 
  //         << randomPoint.x() << ", " 
  //         << randomPoint.y() << ", " 
  //         << randomPoint.z() << G4endl;


  //G4ThreeVector position = fRadius * G4RandomDirection();

  //G4cout << "Primary generator: position = " << position << G4endl;
  //fParticleGun->SetParticlePosition(position);
  fParticleGun->SetParticlePosition(randomPoint);

  // Set opposite directions for the two gammas
  auto direction = G4RandomDirection();

  //G4cout << "Primary generator: direction = " << direction << G4endl;
  
  fParticleGun->SetParticleMomentumDirection(direction);
  fParticleGun->GeneratePrimaryVertex(event);

  // Generate second gamma (opposite direction)
  fParticleGun->SetParticleMomentumDirection(-direction);
  fParticleGun->GeneratePrimaryVertex(event);
 
}

G4VPhysicalVolume* PrimaryGeneratorAction::GetPhysicalVolumeFromLogical(G4LogicalVolume* lVol) 
{
  // Get the physical volume store (contains all physical volumes)
  G4PhysicalVolumeStore* pvStore = G4PhysicalVolumeStore::GetInstance();

  // Iterate over all physical volumes
  for (auto pv : *pvStore) {
      // Check if the physical volume has the same logical volume as the heart
      if (pv->GetLogicalVolume() == lVol) {
          return pv;  // Found the corresponding physical volume
      }
  }

  // If no physical volume found
  return nullptr;
}