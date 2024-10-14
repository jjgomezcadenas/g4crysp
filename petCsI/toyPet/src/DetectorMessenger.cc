/// \brief Implementation of the DetectorMessenger class

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
{
  fDetectorConstruction = det;

  fDetDirectory = new G4UIdirectory("/detector/");
  fDetDirectory->SetGuidance("Detector construction control");
  
  fMaterialCmd = new G4UIcmdWithAString("/detector/crystalMaterial", this);
  fMaterialCmd->SetGuidance("Select crystal material: CsI, CsITl, LYSO, BGO.");
  fMaterialCmd->SetParameterName("crystalMaterial",false);
  fMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fCrystalWidthCmd = new G4UIcmdWithADoubleAndUnit("/detector/crystalWidth", this);
  fCrystalWidthCmd->SetGuidance("Define the crystal width");
  fCrystalWidthCmd->SetParameterName("crystalWidth",false);
  fCrystalWidthCmd->SetUnitCategory("Length");
  fCrystalWidthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  fCrystalX0LengthCmd = new G4UIcmdWithADouble("/detector/crystalX0Length",this);
  fCrystalX0LengthCmd->SetGuidance("Define the crystal length (in x0)");
  fCrystalX0LengthCmd->SetParameterName("crystalX0Length",false);
  fCrystalX0LengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTeflonThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/teflonThickness",this);
  fTeflonThicknessCmd->SetGuidance("Define the thickness of one teflon wrap");
  fTeflonThicknessCmd->SetParameterName("teflonThickness",false);
  fTeflonThicknessCmd->SetUnitCategory("Length");
  fTeflonThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTeflonCoatingsCmd = new G4UIcmdWithADouble("/detector/teflonCoatings",this);
  fTeflonCoatingsCmd->SetGuidance("Define the number of teflon coatins");
  fTeflonCoatingsCmd->SetParameterName("teflonCoatings",false);
  fTeflonCoatingsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPetDiameterCmd = new G4UIcmdWithADoubleAndUnit("/detector/petDiameter",this);
  fPetDiameterCmd->SetGuidance("diameter of PET");
  fPetDiameterCmd->SetParameterName("petDiameter",false);
  fPetDiameterCmd->SetUnitCategory("Length");
  fPetDiameterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPetLengthCmd = new G4UIcmdWithADoubleAndUnit("/detector/petLength",this);
  fPetLengthCmd->SetGuidance("Length of PET");
  fPetLengthCmd->SetParameterName("petLength",false);
  fPetLengthCmd->SetUnitCategory("Length");
  fPetLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  //delete fDirectory ;
  delete fDetDirectory ;
  
  delete  fMaterialCmd ;
  delete  fCrystalWidthCmd ;
  delete  fCrystalX0LengthCmd ;
  
  delete  fTeflonThicknessCmd ;
  delete  fTeflonCoatingsCmd ;
  
  delete  fPetDiameterCmd ;
  delete  fPetLengthCmd ;
  

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

  if( command ==  fPetDiameterCmd )
    {fDetectorConstruction->fPetDiameter = fPetDiameterCmd->GetNewDoubleValue(newValue);}
  if( command ==  fPetLengthCmd )
    {fDetectorConstruction->fPetLength = fPetLengthCmd->GetNewDoubleValue(newValue);}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

