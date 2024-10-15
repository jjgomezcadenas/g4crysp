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

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* pg)
//PrimaryGeneratorMessenger::PrimaryGeneratorMessenger()
{
  fPrimaryGeneratorAction = pg;
  fEnergyCmd = new G4UIcmdWithADoubleAndUnit("/primary/gammaEnergy",this);
  fEnergyCmd->SetGuidance("Energy");
  fEnergyCmd->SetParameterName("Energy",false);
  fEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fDirectory ;
  delete fEnergyCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  
  if( command ==  fEnergyCmd )
    {GlobalPars::Instance()->fGammaEnergy = fEnergyCmd->GetNewDoubleValue(newValue);}
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

