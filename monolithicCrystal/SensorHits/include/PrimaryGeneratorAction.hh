
//
/// \file M1/include/PrimaryGeneratorAction.hh
/// \brief Definition of the M1::PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "globals.hh"

class G4Event;
class G4Box;
class G4LogicalVolume;
class PrimaryGeneratorMessenger;

/// The primary generator action class with particle gun.
///

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  // method from the base class
  void GeneratePrimaries(G4Event*) override;


public:
  G4int fNphotons = 100;
  G4double fFano = 1.1;
  G4bool fGaussian = true;
    
private:
  void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf);

private:
  G4int fN = 0;
  G4Box* fEnvelopeBox = nullptr;
  G4LogicalVolume* fenvLV = nullptr;
  PrimaryGeneratorMessenger* fMessenger = nullptr; // messenger
};  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
