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
#include "G4TouchableHandle.hh"
#include "G4AffineTransform.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include <iostream>
#include <fstream>
#include <mutex>

StepAction::StepAction() {}

void StepAction::UserSteppingAction(const G4Step* step) 
{
    auto eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    auto threadID = G4Threading::G4GetThreadId();
    auto fEventNumber = eventID + GlobalPars::Instance()->fEventShift;

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
    //const G4VProcess* creatorProcess = track->GetCreatorProcess();
    //std::string creatorProcessName = (creatorProcess != nullptr) ? creatorProcess->GetProcessName() : "Primary";

    auto stepl  = step->GetStepLength() ;
    auto postep = step->GetPostStepPoint() ;
    auto post   = postep->GetGlobalTime() ;
    auto poske  = postep->GetKineticEnergy() ;
    //auto trkmass   = postep->GetMass();
    G4ThreeVector posxyz = postep->GetPosition(); 

    // const G4VTouchable* touchable = postep->GetTouchable();
    // auto box = dynamic_cast<G4Box*>(touchable->GetVolume()->GetLogicalVolume()->GetSolid());
    
    // G4ThreeVector globalPosition = touchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0, 0, 0));

    // auto xHL = box->GetXHalfLength();
    // auto yHL = box->GetYHalfLength();
    // auto zHL = box->GetZHalfLength();
    
    
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
            
            write_step_info2(fEventNumber, post, ged, posxyz);
            HistogramManager::Instance()->FillHistogram("EDEP", ged/keV);
        }
        else if (poske < fke)
        {
            ged = fke - poske;
            fke = poske;
            //print_step_info("GAMMAINT", eventID, threadID, trkid, trkmass, parentID, "SecInt", 
            //            post, poske, stepl, ged, posxyz);
            
            write_step_info2(fEventNumber, post, ged, posxyz);
            HistogramManager::Instance()->FillHistogram("EDEP", ged/keV);
        }
        //else
        //{
        //     G4cout << "Global position of volume = " << globalPosition / mm << " mm" << G4endl;

        //    G4cout << "Dimensions of the volume: x =  "
        //        << 2 * xHL / mm << " mm;  y = "
        //        << 2 * yHL / mm << " mm;  z = "
        //        << 2 * zHL / mm << " mm" << G4endl;

            
            //G4cout << " \n position of gamma:  x (mm) =" << (float)posxyz.x()<< 
            //" y (mm) =" << (float)posxyz.y() << 
            //" z (mm) ="  <<(float)posxyz.z() << G4endl;

            // G4cout << " \n crystal width (mm) =" << GlobalPars::Instance()->fCrystalWidth<< 
            // " crystal length =" << GlobalPars::Instance()->fCrystalLength <<  G4endl;

        //}
    }

    
    //print_step_info("STEP", eventID, threadID, trkid, trkmass, parentID, creatorProcessName, 
    //                post, poske, stepl, edep, posxyz);
    // if(poske == 0 && edep == 0)
    // {
    //     //print_step_info("END", fEventNumber, threadID, trkid, trkmass, parentID, creatorProcessName, 
    //     //                post, poske, fstep, fedep, posxyz);
    //     write_step_info(fEventNumber, trkid, trkmass, parentID, creatorProcessName, 
    //                     post, poske, fstep, fedep, posxyz);

        
        
    //     fedep = 0.0;
    //     fstep = 0.0;
    //     track->SetTrackStatus(fStopAndKill);
    // }
    // else
    // {
    //     fedep += edep;
    //     fstep += stepl;
    // }

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

void StepAction::write_step_info(int eventID, int trkid, double trkmass, int parentID, 
                                 std::string creatorProcessName, 
                                 double post, double poske, double stepl, double edep,
                                 G4ThreeVector posxyz) const 
{
    std::lock_guard<std::mutex> guard(GlobalPars::Instance()->gammaIntFileMutex);
    GlobalPars::Instance()->gammaIntFile << std::fixed << std::setprecision(1);
    GlobalPars::Instance()->gammaIntFile << eventID << "," << 
    trkid << "," << trkmass/keV << "," << parentID << "," <<
    (float)posxyz.x()<< "," << posxyz.y() << ","  <<(float)posxyz.z() << "," << 
    stepl/um << "," << edep/keV << "," <<  
    creatorProcessName << "\n";
}

void StepAction::write_step_info2(int eventID,  double time, double edep, G4ThreeVector posxyz) const 
{
    std::lock_guard<std::mutex> guard(GlobalPars::Instance()->gammaIntFileMutex);
    GlobalPars::Instance()->gammaIntFile << std::fixed << std::setprecision(1);
    
    GlobalPars::Instance()->gammaIntFile << eventID << "," << time/ps <<"," <<
    (float)posxyz.x()<< "," << posxyz.y() << ","  <<(float)posxyz.z() << "," << 
    edep/keV << "\n";
}