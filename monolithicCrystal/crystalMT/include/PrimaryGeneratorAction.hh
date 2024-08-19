
//
/// \file M1/include/PrimaryGeneratorAction.hh
/// \brief Definition of the M1::PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4PhysicsOrderedFreeVector.hh>
#include "globals.hh"
#include <fstream>
#include <mutex>

class G4Event;
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
    
private:
  void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf);

private:
  //auto fN = 0;
  G4double fXY ;
  G4double fZ; 
  G4int fScintPhotons;
  G4double fScmax;
  G4PhysicsOrderedFreeVector* fSpectrumIntegral;

  //static std::mutex gammaIntFileMutex;

  PrimaryGeneratorMessenger* fMessenger = nullptr; // messenger
};  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
