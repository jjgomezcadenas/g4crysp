#ifndef StepAction_hh
#define StepAction_hh
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include <vector>

class StepAction : public G4UserSteppingAction 
{
    public:
    StepAction();
    ~StepAction() override = default;

    void UserSteppingAction (const G4Step *) override; 

};
#endif