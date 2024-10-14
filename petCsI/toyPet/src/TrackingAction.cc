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
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) 
{      
} 



 
