#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
  auto runAction = new RunAction;
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);
  SetUserAction(new RunAction);
  SetUserAction(new EventAction);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

