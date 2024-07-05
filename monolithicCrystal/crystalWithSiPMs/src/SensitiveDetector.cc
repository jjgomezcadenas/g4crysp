#include "SensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4VPhysicalVolume.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name) {}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::Initialize(G4HCofThisEvent* hce) {
    hits.clear();
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    G4Track* track = step->GetTrack();
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4ThreeVector position = preStepPoint->GetPosition();
    G4double energy = preStepPoint->GetTotalEnergy();

    PhotonHit hit;
    hit.position = position;
    hit.energy = energy;

    hits.push_back(hit);

    G4cout << G4endl
            << "->ProcessHits, found hit: energy =" << hit.energy
           << " position =" << hit.position 
           << G4endl;


    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* hce) {
    // Hits are processed in EventAction
}

const std::vector<PhotonHit>& SensitiveDetector::GetHits() const {
    return hits;
}
