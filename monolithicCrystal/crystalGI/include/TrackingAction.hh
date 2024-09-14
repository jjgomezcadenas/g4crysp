#ifndef XTRACKING_ACTION_HH
#define XTRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

class G4Trajectory; 

class TrackingAction : public G4UserTrackingAction {
public:
    TrackingAction();
    virtual ~TrackingAction();

    virtual void PreUserTrackingAction(const G4Track* track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    void print_track_info(const G4Trajectory* trj, const G4Track* track,
                          int primary, int mother_id, double kin_energy, 
                          G4ThreeVector ini_xyz, 
                          double time, 
                          std::string creatorProcessName,
                          std::string initialVolumeName) const ;

    void write_track_info(const G4Trajectory* trj, const G4Track* track,
                          int primary, int mother_id, double kin_energy, 
                          G4ThreeVector ini_xyz, 
                          double time, 
                          std::string creatorProcessName,
                          std::string initialVolumeName) const ;
};

#endif // MY_TRACKING_ACTION_HH
