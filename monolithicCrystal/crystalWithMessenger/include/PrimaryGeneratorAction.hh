#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <random>

class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

private:
    G4ParticleGun* fParticleGun;
    std::mt19937 rng;
    std::uniform_real_distribution<> distPos;
    std::normal_distribution<> distEnergy;
};

#endif
