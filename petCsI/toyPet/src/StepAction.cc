#include "G4RunManager.hh"
#include "StepAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include <G4VProcess.hh>
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "GlobalPars.hh"
#include "HistogramManager.hh"
#include "G4Threading.hh"
#include <iostream>
#include <fstream>
#include <mutex>

StepAction::StepAction() {}

void StepAction::UserSteppingAction(const G4Step* step) 
{   
}


