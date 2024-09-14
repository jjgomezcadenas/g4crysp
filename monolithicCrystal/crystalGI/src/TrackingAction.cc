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
    // Enable trajectory storage for optical photons
    // *** not for the time being, to avoide cluttering while I debug secondaries **/

    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
        return ;
        //G4TrackingManager* trackingManager = G4EventManager::GetEventManager()->GetTrackingManager();

        //trackingManager->SetStoreTrajectory(true);
        //trackingManager->SetTrajectory(new G4Trajectory(track));

        // How come this has ever worked?? It uses some sort of private field!!! ???
        //fpTrackingManager->SetStoreTrajectory(true);
        //fpTrackingManager->SetTrajectory(new G4Trajectory(track));
    }

    auto trj = new G4Trajectory(track); 
    auto mother_id = 0;
    auto primary = 0;

    if (!track->GetParentID()){
        primary = 1;
    } 
    else{
        mother_id = track->GetParentID();
    }  

    auto kin_energy = track->GetKineticEnergy();
 
    //   if (kin_energy < 1 * keV) {
    //     return;
    // }

    G4VPhysicalVolume* initialVolume = track->GetVolume();
    std::string initialVolumeName = (initialVolume != nullptr) ? initialVolume->GetName() : "Unknown";
  
    G4TrajectoryPoint* firstPoint = dynamic_cast<G4TrajectoryPoint*>(trj->GetPoint(0));
    auto ini_xyz = firstPoint->GetPosition();
    auto time = track->GetGlobalTime(); 

    // Get the creator process
    const G4VProcess* creatorProcess = track->GetCreatorProcess();
    std::string creatorProcessName = (creatorProcess != nullptr) ? creatorProcess->GetProcessName() : "Primary";
   
    //print_track_info(trj, track, primary, mother_id, 
    //                 kin_energy, ini_xyz, time, creatorProcessName, initialVolumeName);
    write_track_info(trj, track, primary, mother_id,
                     kin_energy, ini_xyz, time, creatorProcessName, initialVolumeName) ;
    
    delete trj; 
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) 
{
    
} 

void TrackingAction::print_track_info(const G4Trajectory* trj, const G4Track* track,
                                      int primary, int mother_id, double kin_energy, 
                                      G4ThreeVector ini_xyz, 
                                      double time, std::string creatorProcessName,
                                      std::string initialVolumeName) const 
{
     
    G4cout << "event number = " << GlobalPars::Instance()->fEventNumber << 
    " Track id =" << trj->GetTrackID() << 
    " Paricle name =" << trj->GetParticleName()<< 
    " Primary? "<< primary<< 
    " Mother ide" << mother_id << 
    " x (mm) =" << (float)ini_xyz.x()<< " y (mm) =" << (float)ini_xyz.y() << " z (mm) ="  <<(float)ini_xyz.z()<< 
    " time (ns) =" << (float)time/ns  << " kinetic energy (keV) =" << kin_energy/keV 
    << " Process" <<  creatorProcessName << " Volume " << initialVolumeName <<"\n";
}

void TrackingAction::write_track_info(const G4Trajectory* trj, const G4Track* track,
                                      int primary, int mother_id, double kin_energy, 
                                      G4ThreeVector ini_xyz, 
                                      double time, std::string creatorProcessName,
                                      std::string initialVolumeName) const
{
    std::lock_guard<std::mutex> guard(GlobalPars::Instance()->gammaIntFileMutex);

    GlobalPars::Instance()->gammaIntFile << GlobalPars::Instance()->fEventNumber << "," << 
    trj->GetTrackID() << "," << trj->GetParticleName()<< ","<< primary<< "," << mother_id << "," <<
    (float)ini_xyz.x()<< "," << (float)ini_xyz.y() << ","  <<(float)ini_xyz.z()<< "," << 
    (float)time << "," <<
    kin_energy << "," <<  
    creatorProcessName << "," << initialVolumeName <<"\n";
   
}

 
