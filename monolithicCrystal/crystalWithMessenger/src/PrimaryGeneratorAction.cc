#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), rng(std::random_device{}()), distPos(-10.0*mm, 10.0*mm), distEnergy(3.35*eV, 0.09*eV) {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    //    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    //G4ParticleDefinition* particle = particleTable->FindParticle("opticalphoton");
    // Generate an optical photon 
    G4ParticleDefinition* particle_definition = G4OpticalPhoton::Definition();
    fParticleGun->SetParticleDefinition(particle_definition);

    // Create the new primary particle and set it some properties
    G4PrimaryParticle* particle = new G4PrimaryParticle(particle_definition,
                                                        0.0, 0.0, -1.0);

    //    G4ThreeVector polarization = G4RandomDirection();
    //particle->SetPolarization(polarization);

    
    //    fParticleGun->SetParticleDefinition(particle);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    G4double xPos = distPos(rng);
    G4double yPos = distPos(rng);
    G4double zPos = distPos(rng);
    G4double energy = distEnergy(rng);

    fParticleGun->SetParticlePosition(G4ThreeVector(xPos, yPos, zPos));
    fParticleGun->SetParticleEnergy(energy);
// Set random momentum direction
//    G4ThreeVector momentumDirection = G4RandomDirection();
    auto momentumDirection = G4ThreeVector(0., 0., 1.);
    fParticleGun->SetParticleMomentumDirection(momentumDirection);

    // Set random polarization perpendicular to the momentum direction
    G4ThreeVector polarization = G4RandomDirection().cross(momentumDirection).unit();
    fParticleGun->SetParticlePolarization(polarization);

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
