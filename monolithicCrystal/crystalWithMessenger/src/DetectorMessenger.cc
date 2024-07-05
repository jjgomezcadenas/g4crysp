/// \brief Implementation of the DetectorMessenger class

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* det): fDetectorConstruction(det)
{
  fDirectory = new G4UIdirectory("/crystal");
  fDirectory->SetGuidance("UI commands specific to this app.");
  
  fDetDirectory = new G4UIdirectory("/crystal/det/");
  fDetDirectory->SetGuidance("Detector construction control");
  
  fMaterialCmd = new G4UIcmdWithAString("/crystal/det/crystalMaterial", this);
  fMaterialCmd->SetGuidance("Select crystal material: CsI, CsITl, LYSO, BGO.");
  fMaterialCmd->SetParameterName("crystalMaterial",false);
  fMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fCrystalWidthCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/crystalWidth", this);
  fCrystalWidthCmd->SetGuidance("Define the crystal width");
  fCrystalWidthCmd->SetParameterName("crystalWidth",false);
  fCrystalWidthCmd->SetUnitCategory("Length");
  fCrystalWidthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  fCrystalX0LengthCmd = new G4UIcmdWithADouble("/crystal/det/crystalX0Length",this);
  fCrystalX0LengthCmd->SetGuidance("Define the crystal length (in x0)");
  fCrystalX0LengthCmd->SetParameterName("crystalX0Length",false);
  fCrystalX0LengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTeflonThicknessCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/teflonThickness",this);
  fTeflonThicknessCmd->SetGuidance("Define the thickness of one teflon wrap");
  fTeflonThicknessCmd->SetParameterName("teflonThickness",false);
  fTeflonThicknessCmd->SetUnitCategory("Length");
  fTeflonThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTeflonCoatingsCmd = new G4UIcmdWithADouble("/crystal/det/teflonCoatings",this);
  fTeflonCoatingsCmd->SetGuidance("Define the number of teflon coatins");
  fTeflonCoatingsCmd->SetParameterName("teflonCoatings",false);
  fTeflonCoatingsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSipmXYCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/sipmXY",this);
  fSipmXYCmd->SetGuidance("dimensions of SiPM");
  fSipmXYCmd->SetParameterName("sipmXY",false);
  fSipmXYCmd->SetUnitCategory("Length");
  fSipmXYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSipmZCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/sipmZ",this);
  fSipmZCmd->SetGuidance("dimensions in z of SiPM plastic case");
  fSipmZCmd->SetParameterName("sipmZ",false);
  fSipmZCmd->SetUnitCategory("Length");
  fSipmZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEpoxyZCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/epoxyZ",this);
  fEpoxyZCmd->SetGuidance("thickness of epoxy");
  fEpoxyZCmd->SetParameterName("expoxyZ",false);
  fEpoxyZCmd->SetUnitCategory("Length");
  fEpoxyZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fActiveXYCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/sipmActiveXY",this);
  fActiveXYCmd->SetGuidance("sipm active XY");
  fActiveXYCmd->SetParameterName("sipmActiveXY",false);
  fActiveXYCmd->SetUnitCategory("Length");
  fActiveXYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fActiveZCmd = new G4UIcmdWithADoubleAndUnit("/crystal/det/sipmActiveZ",this);
  fActiveZCmd->SetGuidance("sipm active Z");
  fActiveZCmd->SetParameterName("sipmActiveZ",false);
  fActiveZCmd->SetUnitCategory("Length");
  fActiveZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


    // fScintDirectory = new G4UIdirectory("/M1/scintgen/");
    // fScintDirectory->SetGuidance("Scintillation generator");

    // fNPhotonsCmd = new G4UIcmdWithAInt("/M1/scintgen/nphotons",this);
    // fNPhotonsCmd->SetGuidance("Define the number of photons");
    // fNPhotonsCmd->SetParameterName("NPhotons",false);
    // fNPhotonsCmd->AvailableForStates(G4State_Idle);

    // fGaussianCmd = new G4UIcmdWithABool("/M1/scintgen/gaussian",this);
    // fGaussianCmd->SetGuidance("Define is gaussian generator");
    // fGaussianCmd->SetParameterName("Gaussian",false);
    // fGaussianCmd->AvailableForStates(G4State_Idle);

    // fFanoCmd = new G4UIcmdWithADouble("/M1/scintgen/fano",this);
    // fFanoCmd->SetGuidance("Fano factor");
    // fFanoCmd->SetParameterName("Fano",false);
    // fFanoCmd->AvailableForStates(G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fDirectory ;
  delete fDetDirectory ;
  
  delete  fMaterialCmd ;
  delete  fCrystalWidthCmd ;
  delete  fCrystalX0LengthCmd ;
  
  delete  fTeflonThicknessCmd ;
  delete  fTeflonCoatingsCmd ;
  
  delete  fSipmXYCmd ;
  delete  fSipmZCmd ;
  delete  fEpoxyZCmd ;
  delete  fActiveXYCmd ;
  delete  fActiveZCmd ;

  //  delete fNPhotonsCmd;
  //delete fGaussianCmd;
  //delete fFanoCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fMaterialCmd )
    {fDetectorConstruction->fMaterial = newValue;}

  if( command == fCrystalWidthCmd )
    {fDetectorConstruction->fCrystalWidth = fCrystalWidthCmd->GetNewDoubleValue(newValue);}

  if( command ==  fCrystalX0LengthCmd )
    {fDetectorConstruction->fCrystalX0Length = fCrystalX0LengthCmd->GetNewDoubleValue(newValue);}

  if( command ==  fTeflonThicknessCmd )
    {fDetectorConstruction->fTeflonThickness = fTeflonThicknessCmd->GetNewDoubleValue(newValue);}

  if( command ==  fTeflonCoatingsCmd )
    {fDetectorConstruction->fTeflonCoatings = fTeflonCoatingsCmd->GetNewDoubleValue(newValue);}

  if( command ==  fSipmXYCmd )
    {fDetectorConstruction->fSipmXY = fSipmXYCmd->GetNewDoubleValue(newValue);}
  if( command ==  fSipmZCmd )
    {fDetectorConstruction->fSipmZ = fSipmZCmd->GetNewDoubleValue(newValue);}

  if( command ==  fEpoxyZCmd )
    {fDetectorConstruction->fEpoxyZ = fEpoxyZCmd->GetNewDoubleValue(newValue);}
  
  if( command ==  fActiveXYCmd )
    {fDetectorConstruction->fActiveXY = fActiveXYCmd->GetNewDoubleValue(newValue);}

  if( command ==  fActiveZCmd )
    {fDetectorConstruction->fActiveZ = fActiveZCmd->GetNewDoubleValue(newValue);}


  // if( command ==  fNPhotonsCmd )
  //     {fPrimaryGeneratorAction->fNPhotons = fNPhotonsCmd->GetNewIntValue(newValue);}

  //   if( command ==  fGaussianCmd )
  //     {fPrimaryGeneratorAction->fGaussian = fGaussianCmd->GetNewBoolValue(newValue);}

  //   if( command ==  fFanoCmd )
  //     {fPrimaryGeneratorAction->fFano = fFanoCmd->GetNewDoubleValue(newValue);}
  
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

