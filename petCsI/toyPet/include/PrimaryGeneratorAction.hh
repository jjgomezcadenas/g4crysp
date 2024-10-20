
//
/// \file M1/include/PrimaryGeneratorAction.hh
/// \brief Definition of the M1::PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4PhysicsOrderedFreeVector.hh>
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <fstream>
#include <mutex>

class G4Event;
//class PrimaryGeneratorMessenger;
class G4ParticleGun;
class G4VPhysicalVolume;
class G4LogicalVolume;
//class G4ThreeVector;

/// The primary generator action class with particle gun.
///

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  // method from the base class
  void GeneratePrimaries(G4Event*) override;
  G4VPhysicalVolume* GetPhysicalVolumeFromLogical(G4LogicalVolume* lVol);


public:

G4ParticleGun* fParticleGun;    
//PrimaryGeneratorMessenger* fMessenger = nullptr; // messenger

private:
  G4double fHeartRadius ;
  G4ThreeVector fHeartPosition;
};  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
