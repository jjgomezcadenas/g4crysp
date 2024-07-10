#ifndef XTRACKING_ACTION_HH
#define XTRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

class TrackingAction : public G4UserTrackingAction {
public:
    TrackingAction();
    virtual ~TrackingAction();

    virtual void PreUserTrackingAction(const G4Track* track) override;
};

#endif // MY_TRACKING_ACTION_HH
