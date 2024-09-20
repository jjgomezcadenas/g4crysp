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

    void print_step_info(std::string steptype, int eventID, int threadID, int trkid, double trkmass, 
                                 int parentID, std::string creatorProcessName, 
                                 double post, double poske, double stepl, double edep,
                                 G4ThreeVector posxyz) const  ;

    
    void write_step_info(int eventID,  double time, double edep, G4ThreeVector posxyz) const; 


    double fedep = 0.0;
    double fstep = 0.0; 
    double fke = 0.0;
};
#endif