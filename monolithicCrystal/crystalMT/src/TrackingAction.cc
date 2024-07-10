#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4Trajectory.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
    // Enable trajectory storage for optical photons
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
        fpTrackingManager->SetStoreTrajectory(true);
        fpTrackingManager->SetTrajectory(new G4Trajectory(track));
    }
}
