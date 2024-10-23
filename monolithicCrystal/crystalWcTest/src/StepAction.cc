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
    auto eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    auto threadID = G4Threading::G4GetThreadId();
    auto fEventNumber = eventID + GlobalPars::Instance()->fEventShift ;
    //+ GlobalPars::Instance()->fNumberOfEvents;
    
    //G4cout <<  "Stepping action ++ Event id = " << eventID << " Thread id =" 
    //<< threadID << " Shifted event id = " << fEventNumber << G4endl;
    // Get the particle associated with the step
    G4Track* track = step->GetTrack();

    // loop away optical photons
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) return;

    G4int parentID = track->GetParentID();

    // Get the energy deposited in this step
    //G4double edep = step->GetTotalEnergyDeposit();

    // trk id and creator process
    auto trkid = track->GetTrackID();
    
    auto stepl  = step->GetStepLength() ;
    auto postep = step->GetPostStepPoint() ;
    auto post   = postep->GetGlobalTime() ;
    auto poske  = postep->GetKineticEnergy() ;
    G4ThreeVector posxyz = postep->GetPosition(); 
        
    auto ged = 0.0;
    
    HistogramManager::Instance()->FillHistogram("STEPL", stepl);

    if (parentID == 0 && trkid==1)  // primary particle
    {
        //print_step_info("STEP", eventID, threadID, trkid, trkmass, parentID, "PrimaryGamma", 
        //                post, poske, stepl, edep, posxyz);
        
        if (poske < GlobalPars::Instance()->fGammaEnergy && fke == 0.0)
        {
            ged = GlobalPars::Instance()->fGammaEnergy - poske;
            fke = poske;
            //print_step_info("GAMMAINT", eventID, threadID, trkid, trkmass, parentID, "FirstInt", 
            //            post, poske, stepl, ged, posxyz);
            
            write_step_info(fEventNumber, post, ged, posxyz);
            HistogramManager::Instance()->FillHistogram("EDEP", ged/keV);
        }
        else if (poske < fke)
        {
            ged = fke - poske;
            fke = poske;
            //print_step_info("GAMMAINT", eventID, threadID, trkid, trkmass, parentID, "SecInt", 
            //            post, poske, stepl, ged, posxyz);
            
            write_step_info(fEventNumber, post, ged, posxyz);
            HistogramManager::Instance()->FillHistogram("EDEP", ged/keV);
        }
    }

}


void StepAction::print_step_info(std::string steptype, int eventID, int threadID, int trkid, double trkmass, 
                                 int parentID, std::string creatorProcessName, 
                                 double post, double poske, double stepl, double edep,
                                 G4ThreeVector posxyz) const 
{
    G4cout << std::fixed << std::setprecision(1);
    G4cout << " \n event number = " << eventID << 
    " Thread id =" << threadID <<
    " Track id =" << trkid << 
    " STEP TYPE =" << steptype << " \n" << 
    " Track mass = " << trkmass/keV <<
    " Mother id = " << parentID << 
    " Process = " <<  creatorProcessName << 
    " time (ns) =" << post/ns  << " kinetic energy (keV) =" << poske/keV << 
    " Stepl (um) =" << stepl/um  << 
    " edep  (keV) =" << edep/keV <<
    " x (mm) =" << (float)posxyz.x()<< 
    " y (mm) =" << (float)posxyz.y() << 
    " z (mm) ="  <<(float)posxyz.z() << G4endl;

}


void StepAction::write_step_info(int eventID,  double time, double edep, G4ThreeVector posxyz) const 
{
    std::lock_guard<std::mutex> guard(GlobalPars::Instance()->gammaIntFileMutex);
    GlobalPars::Instance()->gammaIntFile << std::fixed << std::setprecision(1);
    
    GlobalPars::Instance()->gammaIntFile << eventID << "," << time/ps <<"," <<
    (float)posxyz.x()<< "," << posxyz.y() << ","  <<(float)posxyz.z() << "," << 
    edep/keV << "\n";
}