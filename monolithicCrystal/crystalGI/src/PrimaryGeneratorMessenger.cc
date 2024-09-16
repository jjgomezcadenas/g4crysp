/// \brief Implementation of the DetectorMessenger class

#include "PrimaryGeneratorMessenger.hh"
//#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "GlobalPars.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* pg)
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger()
{
  fTimeBinningCmd = new G4UIcmdWithADoubleAndUnit("/primary/timeBinning",this);
  fTimeBinningCmd->SetGuidance("Time binning");
  fTimeBinningCmd->SetParameterName("timeBinning",false);
  fTimeBinningCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fDirectory ;
  delete fTimeBinningCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command ==  fTimeBinningCmd )
    {GlobalPars::Instance()->fTimeBinning = fTimeBinningCmd->GetNewDoubleValue(newValue);}
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

