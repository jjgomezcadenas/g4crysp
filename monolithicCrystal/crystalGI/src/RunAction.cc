#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "GlobalPars.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // set printing event number per each 100 events
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  GlobalPars::Instance()->fNumberOfEvents = run->GetNumberOfEventToBeProcessed();
  // Handle event numbering as a function of the seed.
  
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  G4int eventShift = (seedL4D -1) * GlobalPars::Instance()->fNumberOfEvents;
  GlobalPars::Instance()->fEventShift = eventShift;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* )
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


