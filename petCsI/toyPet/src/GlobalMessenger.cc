/// \brief Implementation of the DetectorMessenger class

#include "GlobalMessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "GlobalPars.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GlobalMessenger::GlobalMessenger()
{

  G4cout << " In constructor of GlobalMessenger " << G4endl;
  
  fDirectory = new G4UIdirectory("/globalpars/");
  fDirectory->SetGuidance("UI commands specific to this app.");
  
  fSeedCmd = new G4UIcmdWithAnInteger("/globalpars/seed",this);
  fSeedCmd->SetGuidance("Define the random seed");
  fSeedCmd->SetParameterName("seed",false);
  fSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  fISensorDataCmd = new G4UIcmdWithAString("/globalpars/ISensorDataFileName", this);
  fISensorDataCmd->SetGuidance("Name of file with integrated sensor data");
  fISensorDataCmd->SetParameterName("integratedSensorData",false);
  fISensorDataCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fGlobalDataCmd = new G4UIcmdWithAString("/globalpars/GlobalDataFileName", this);
  fGlobalDataCmd->SetGuidance("Name of file with global data");
  fGlobalDataCmd->SetParameterName("gobalData",false);
  fGlobalDataCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fHistosCmd = new G4UIcmdWithAString("/globalpars/HistogramsFileName", this);
  fHistosCmd->SetGuidance("Name of file with histograms");
  fHistosCmd->SetParameterName("controlHistograms",false);
  fHistosCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GlobalMessenger::~GlobalMessenger()
{
  delete fDirectory ;
  delete fSeedCmd;
  delete fISensorDataCmd ;
  delete fGlobalDataCmd ;
  delete fHistosCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GlobalMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command ==  fSeedCmd )
  {
    G4cout << " Got new value for seed = " << newValue << G4endl;
    GlobalPars::Instance()->fSeed = fSeedCmd->GetNewIntValue(newValue);
  }

  if( command ==  fISensorDataCmd )
    {
      G4cout << " Got new value for integrated sensor data = " << newValue << G4endl;
      GlobalPars::Instance()->fISensorDataFileName = newValue;
    }

  if( command ==  fGlobalDataCmd )
    {
      G4cout << " Got new value for global data = " << newValue << G4endl;
      GlobalPars::Instance()->fGlobalDataFileName = newValue;
    }
  
  if( command ==  fHistosCmd )
    {
      G4cout << " Got new value for histograms files = " << newValue << G4endl;
      GlobalPars::Instance()->fHistoFileName = newValue;
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

