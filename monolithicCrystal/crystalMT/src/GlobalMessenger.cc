/// \brief Implementation of the DetectorMessenger class

#include "GlobalMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "GlobalPars.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GlobalMessenger::GlobalMessenger()
{

  G4cout << " In constructor of GlobalMessenger " << G4endl;
  
  fDirectory = new G4UIdirectory("/globals/");
  fDirectory->SetGuidance("UI commands specific to this app.");
  
  fSeedCmd = new G4UIcmdWithAnInteger("/globals/seed",this);
  fSeedCmd->SetGuidance("Define the random seed");
  fSeedCmd->SetParameterName("seed",false);
  fSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GlobalMessenger::~GlobalMessenger()
{
  delete fDirectory ;
  delete fSeedCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GlobalMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command ==  fSeedCmd )
    {G4cout << " Got new value for seed = " << newValue << G4endl;
      GlobalPars::Instance()->fSeed = fSeedCmd->GetNewIntValue(newValue);}
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

