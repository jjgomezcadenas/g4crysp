#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensorSD.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include <G4VisAttributes.hh>
#include "G4SubtractionSolid.hh"
#include "Materials.hh"
#include "Visibilities.hh"
#include "OpticalMaterialsProperties.hh"
#include "GlobalPars.hh"





//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);
    G4cout << " DetectorConstruction " << G4endl;

    fSensorPosFile.open("crystal_positions.csv");
    fSensorPosFile << "crystal_id,x,y,z\n";
}


DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
    fSensorPosFile.close();
}


void DetectorConstruction::display() const
{
  
  G4cout << G4endl
         << " ---Detector Construction---"
         << " crystal material =" << fMaterial
         << " crystal width (mm) =" << fCrystalWidth/mm
         << " crystal length (x0) =" << fCrystalX0Length
         << " teflon thicness (mm) =" << fTeflonThickness/mm
         << " nof teflon coatings =" << fTeflonCoatings
         << " Pet diameter =" << fPetDiameter/cm
         << " Pet length   =" << fPetLength/cm
         << G4endl;

         GlobalPars::Instance()->fCrystalWidth = fCrystalWidth;
         GlobalPars::Instance()->fPetDiameter = fPetDiameter;
         GlobalPars::Instance()->fPetLength = fPetLength; 
}


G4Material* DetectorConstruction::DefineMaterial()
{
  G4Material* material = nullptr;

  // Radiation lengths of CsI, LYSO and BGO in mm
  auto csix0  = 18.6 * mm;
  auto lysox0 = 11.4 * mm;
  auto bgox0  = 11.2 * mm;

  // Define geant4 material and optical properties
  
  if (fMaterial == "CsI") {
    fCrystalLength = fCrystalX0Length *  csix0;
    material = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");    
    material->SetMaterialPropertiesTable(copt::CsI());
    GlobalPars::Instance()->fMaterial = "G4_CESIUM_IODIDE";
    
  } else if (fMaterial == "BGO") {

    fCrystalLength = fCrystalX0Length *  bgox0;
    material = G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO");
    material->SetMaterialPropertiesTable(copt::BGO());
    GlobalPars::Instance()->fMaterial = "G4_BGO";
    
  } else if (fMaterial == "LYSO") {
    fCrystalLength = fCrystalX0Length *  lysox0;
    
    material = cmat::LYSO();
    material->SetMaterialPropertiesTable(copt::LYSO());
    GlobalPars::Instance()->fMaterial = "LYSO";
    
  } else if (fMaterial == "CsITl") {
    fCrystalLength = fCrystalX0Length *  csix0;
    material = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    material->SetMaterialPropertiesTable(copt::CsITl());
    GlobalPars::Instance()->fMaterial = "G4_CESIUM_IODIDE";
    
  } else {
    G4Exception("[DetectorConstruction]", "Construct()", FatalException,
                "Unknown crystal material!");
  }

  GlobalPars::Instance()->fCrystalLength = fCrystalLength;
  return material;

}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto material =  DefineMaterial();
  
  display();
  
  // Laboratory (World) is a box of dimensions 2 x 2 x 2 m3 
  
  auto          air       = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  auto          lab_solid = new G4Box("LAB", 2*m, 2*m, 2*m);
  auto          lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");
  auto          lab_phys  = new G4PVPlacement(0, G4ThreeVector(), lab_logic, "LAB",
                                              0, false, 0, true);

  lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());


  // ----Crystal block ------
  auto crystal  = new G4Box("CRYSTAL", fCrystalWidth/2, fCrystalWidth/2, fCrystalLength/2);
  auto fCrystalLogic = new G4LogicalVolume(crystal, material, "CRYSTAL");
  fCrystalLogic->SetVisAttributes(cvis::LightBlueAlpha());

  //----Teflon ----------
  // Teflown wrapping = thickness of wrap x number of wraps
  //auto teflon_thickness_tot = fTeflonThickness * fTeflonCoatings;
  auto teflon_thickness_tot = 1*mm;
   
  // ----Crystal + Teflon box ------

  auto eCrystalWidth = fCrystalWidth + teflon_thickness_tot;
  auto eCrystalLength = fCrystalLength + teflon_thickness_tot;
  auto crystalT  = new G4Box("CRYSTALT", eCrystalWidth/2, eCrystalWidth/2, eCrystalLength/2);
                                        

  auto crystalLogicT = new G4LogicalVolume(crystalT, air, "CRYSTALT");
  crystalLogicT->SetVisAttributes(G4VisAttributes::GetInvisible());


  // Place crystal inside crystalT
  new G4PVPlacement(0, G4ThreeVector(), fCrystalLogic,  "CRYSTAL",
                    crystalLogicT,false, 0, true);    
                                                                 
  
 
  // Build the scanner
     
  auto angles = static_cast<G4int>(fPetDiameter * M_PI  / eCrystalWidth);
  auto step = 2.0 * M_PI / angles;

  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateX(90 * deg);
  rot->rotateZ(180 * deg);

  auto rings = static_cast<G4int>(fPetLength / fCrystalWidth);

  G4cout << G4endl
         << " Crystals per ring =" << angles
         << " Number of rings =" << rings
         << " total crystals =" << angles * rings
         << G4endl;

  
  GlobalPars::Instance()->fCrystalsPerRing = angles;
  GlobalPars::Instance()->fNumberOfRings = rings; 
  GlobalPars::Instance()->fNumberOfCrystals = angles * rings;  

  for (auto iring=0; iring < rings; iring++) {
    for (auto itheta=0; itheta < angles; itheta++) {

      auto radius = fPetDiameter / 2;
      auto theta = 2 * M_PI * itheta / angles ;
      auto label = std::to_string(iring*angles + itheta);
      
      //auto y = radius * std::cos(theta);
      //auto x = radius * std::sin(theta);

      auto y = (fPetDiameter / 2 + eCrystalLength / 2) * std::cos(theta);
      auto x = (fPetDiameter / 2 + eCrystalLength / 2) * std::sin(theta);
      auto z = -fPetLength / 2 + iring * eCrystalWidth + eCrystalWidth / 2;

      //G4cout << "ring number = " << iring << "  block number = " << itheta << G4endl;
      //G4cout << "CRYSTAL" << iring*angles + itheta << " " << x << " " << y << " " << z << " " << G4endl;
      fSensorPosFile << iring*angles + itheta <<  "," << x << "," << y << "," << z <<"\n";

      
      new G4PVPlacement(G4Transform3D(*rot, G4ThreeVector(x, y, z)),
                crystalLogicT, "CRYSTALT"+label, lab_logic,
                true, iring*angles + itheta, true);

      //step = theta + M_PI / 2;
      rot->rotateZ(-step);
      }
    }

  // start with a mynimalistic phantom. A sphere of 1 cm placed in the center of the scanner.
  // Define the sphere geometry

  auto radius = 0.5 * cm;  // Sphere radius (1 cm diameter)
  auto solidSphere = new G4Sphere("Sphere", 0.0, radius, 0.0, 360.0 * deg, 0.0, 180.0 * deg);

  // Create the logical volume
  auto logicSphere = new G4LogicalVolume(solidSphere, air, "Organ");
  logicSphere->SetVisAttributes(G4Colour::Red ());

  // Place the sphere in the world 
  new G4PVPlacement(0, G4ThreeVector(), logicSphere, "Organ",  lab_logic,      
                                      false,  0,  true); 

  return lab_phys;
}


void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors
  //auto sdname =SensorSD::GetCollectionUniqueName();
  auto sdname =  GlobalPars::Instance()->gSDCollection;
  
  // Set the bottom face (in z) as a sensitive detector
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  SensorSD* sensorsd = new SensorSD(sdname);
  sdManager->AddNewDetector(sensorsd);
  SetSensitiveDetector("CRYSTAL", sensorsd);
}
