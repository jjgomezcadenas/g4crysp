#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4Trajectory.hh"
#include "G4SystemOfUnits.hh"
#include "GlobalPars.hh"
#include "HistogramManager.hh"
#include "G4EventManager.hh"
#include <iostream>
#include <fstream>
#include <mutex>

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}


void TrackingAction::PreUserTrackingAction(const G4Track* track) {
    // Enable trajectory storage for optical photons if generation of OP allowed.
   
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {

        if (GlobalPars::Instance()->fGOP == true){ // fGOP = true, generation of OP allowed
            G4TrackingManager* trackingManager = G4EventManager::GetEventManager()->GetTrackingManager();
            trackingManager->SetStoreTrajectory(true);
            trackingManager->SetTrajectory(new G4Trajectory(track));
        }

            // How come this has ever worked?? fpTrackingManager should be private!!!
            //fpTrackingManager->SetStoreTrajectory(true);
            //fpTrackingManager->SetTrajectory(new G4Trajectory(track));
    }
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) 
{      
} 



 
