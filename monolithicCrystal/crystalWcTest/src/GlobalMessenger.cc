/// \brief Implementation of the DetectorMessenger class

#include "GlobalMessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
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

  fOpMinimumWlCmd = new G4UIcmdWithADouble("/globalpars/OpMinimumWlNm",this);
  fOpMinimumWlCmd->SetGuidance("Optical photons Minimum Wavelength in nm");
  fOpMinimumWlCmd->SetParameterName("OpMinimumWlNm",false);
  fOpMinimumWlCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSensorDataCmd = new G4UIcmdWithAString("/globalpars/SensorDataFileName", this);
  fSensorDataCmd->SetGuidance("Name of file with sensor data");
  fSensorDataCmd->SetParameterName("sensorData",false);
  fSensorDataCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fISensorDataCmd = new G4UIcmdWithAString("/globalpars/ISensorDataFileName", this);
  fISensorDataCmd->SetGuidance("Name of file with integrated sensor data");
  fISensorDataCmd->SetParameterName("integratedSensorData",false);
  fISensorDataCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fGammaDataCmd = new G4UIcmdWithAString("/globalpars/GammaInteractionsFileName", this);
  fGammaDataCmd->SetGuidance("Name of file with gamma interactions data");
  fGammaDataCmd->SetParameterName("gammaInteractions",false);
  fGammaDataCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fHistosCmd = new G4UIcmdWithAString("/globalpars/HistogramsFileName", this);
  fHistosCmd->SetGuidance("Name of file with histograms");
  fHistosCmd->SetParameterName("controlHistograms",false);
  fHistosCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fIDataOnlyCmd = new G4UIcmdWithABool("/globalpars/idataonly",this);
  fIDataOnlyCmd->SetGuidance("If true writes integrated data only");
  fIDataOnlyCmd->SetParameterName("idataonly",false);
  fIDataOnlyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fGOPCmd = new G4UIcmdWithABool("/globalpars/generateOpticalPhotons",this);
  fGOPCmd->SetGuidance("If true generate optical photons");
  fGOPCmd->SetParameterName("generateOpticalPhotons",false);
  fGOPCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GlobalMessenger::~GlobalMessenger()
{
  delete fDirectory ;
  delete fSeedCmd;
  delete fOpMinimumWlCmd;
  delete fSensorDataCmd ;
  delete fISensorDataCmd ;
  delete fGammaDataCmd ;
  delete fHistosCmd;
  delete fIDataOnlyCmd;
  delete fGOPCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GlobalMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command ==  fSeedCmd )
  {
    G4cout << " Got new value for seed = " << newValue << G4endl;
    GlobalPars::Instance()->fSeed = fSeedCmd->GetNewIntValue(newValue);
  }

  if( command ==  fOpMinimumWlCmd )
  {
    G4cout << " Got new value for optical photon minimum wavelength (nm) = " << newValue << G4endl;
    GlobalPars::Instance()->fOpMinimumWl = fOpMinimumWlCmd->GetNewDoubleValue(newValue);
  }

  if( command ==  fSensorDataCmd )
    {
      G4cout << " Got new value for sensor data = " << newValue << G4endl;
      GlobalPars::Instance()->fSensorDataFileName = newValue;
    }
  
  if( command ==  fISensorDataCmd )
    {
      G4cout << " Got new value for integrated sensor data = " << newValue << G4endl;
      GlobalPars::Instance()->fISensorDataFileName = newValue;
    }
  
  if( command ==  fGammaDataCmd )
    {
      G4cout << " Got new value for gamma sensor data = " << newValue << G4endl;
      GlobalPars::Instance()->fGammaDataFileName = newValue;
    }
  
  if( command ==  fHistosCmd )
    {
      G4cout << " Got new value for histograms files = " << newValue << G4endl;
      GlobalPars::Instance()->fHistoFileName = newValue;
    }

    if( command ==  fIDataOnlyCmd )
    { 
      GlobalPars::Instance()->fIDataOnly = fIDataOnlyCmd->GetNewBoolValue(newValue);
    } 

    if( command ==  fGOPCmd )
    { 
      GlobalPars::Instance()->fGOP = fGOPCmd->GetNewBoolValue(newValue);
    } 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

