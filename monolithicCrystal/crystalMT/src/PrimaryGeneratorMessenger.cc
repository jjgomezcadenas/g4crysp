/// \brief Implementation of the DetectorMessenger class

#include "PrimaryGeneratorMessenger.hh"
//#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "GlobalPars.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* pg)
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger()
{

  G4cout << " In constructor of PrimaryGeneratorMessenger " << G4endl;
  //  fPrimaryGeneratorAction = pg;
  
  fDirectory = new G4UIdirectory("/primary/");
  fDirectory->SetGuidance("UI commands specific to this app.");
  

  fNPhotonsCmd = new G4UIcmdWithAnInteger("/primary/nphotons",this);
  fNPhotonsCmd->SetGuidance("Define the number of photons");
  fNPhotonsCmd->SetParameterName("nphotons",false);
  fNPhotonsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fGaussianCmd = new G4UIcmdWithABool("/primary/gaussian",this);
  fGaussianCmd->SetGuidance("Define is gaussian generator");
  fGaussianCmd->SetParameterName("gaussian",false);
  fGaussianCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFanoCmd = new G4UIcmdWithADouble("/primary/fano",this);
  fFanoCmd->SetGuidance("Fano factor");
  fFanoCmd->SetParameterName("fano",false);
  fFanoCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fDirectory ;
  delete fNPhotonsCmd;
  delete fGaussianCmd;
  delete fFanoCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command ==  fNPhotonsCmd )
    {GlobalPars::Instance()->fNphotons = fNPhotonsCmd->GetNewIntValue(newValue);}

  if( command ==  fGaussianCmd )
    {GlobalPars::Instance()->fGaussian = fGaussianCmd->GetNewBoolValue(newValue);}

  if( command ==  fFanoCmd )
    {GlobalPars::Instance()->fFano = fFanoCmd->GetNewDoubleValue(newValue);}
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

