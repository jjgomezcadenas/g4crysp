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
         GlobalPars::Instance()->fMaterial = fMaterial;
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
      
    } else if (fMaterial == "BGO") {

      fCrystalLength = fCrystalX0Length *  bgox0;
      material = G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO");
      material->SetMaterialPropertiesTable(copt::BGO());
      
    } else if (fMaterial == "LYSO") {
      fCrystalLength = fCrystalX0Length *  lysox0;
      
      material = cmat::LYSO();
      material->SetMaterialPropertiesTable(copt::LYSO());
      
    } else if (fMaterial == "CsITl") {
      fCrystalLength = fCrystalX0Length *  csix0;
      material = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");
      material->SetMaterialPropertiesTable(copt::CsITl());
      
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
  auto teflon_thickness_tot = 9*mm;
   
  // ----Crystal + Teflon box ------

  auto eCrystalWidth = fCrystalWidth + teflon_thickness_tot;
  auto eCrystalLength = fCrystalLength ;
  auto crystalT  = new G4Box("CRYSTALT", eCrystalWidth/2, eCrystalWidth/2, eCrystalLength/2);
                                        

  auto crystalLogicT = new G4LogicalVolume(crystal, air, "CRYSTALT");
  crystalLogicT->SetVisAttributes(G4VisAttributes::GetInvisible());


  // Place crystal inside crystalT
  auto crystalPhysicsT  = new G4PVPlacement(0, G4ThreeVector(), fCrystalLogic,  "CRYSTAL",
                                             crystalLogicT,false, 0, true);    
                                                                 
  // Make a box (full of teflon) of the same dimensions of the crystal + teflon thickness on +- x and +-y
  // same size that crystal in +-z
    
  // auto teflon_coating_full  = new G4Box("TEFLON_FULL",
  //                                       fCrystalWidth/2 + teflon_thickness_tot/2,
  //                                       fCrystalWidth/2 + teflon_thickness_tot/2,
  //                                       fCrystalLength/2);

  // // Subtracting above box from crystal box we obtain a teflon skin in +-xy
  // auto teflon_coating    = new G4SubtractionSolid("TEFLON", teflon_coating_full, crystal);

  // // Now define teflon sides wrap 
  // auto fTeflonSidesLogic = new G4LogicalVolume(teflon_coating,
  //                                              G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
  //                                              "TEFLON_SIDES");
 
  // fTeflonSidesLogic->SetVisAttributes(G4Colour::White ());

  // // Define back wrapping: This is a thin slab of xy dimensions equal to crytal + wrap and z dimension equal to wrap
  // auto teflon_back = new G4Box("TEFLON_BACK",
  //                                fCrystalWidth/2 + teflon_thickness_tot/2,
  //                                fCrystalWidth/2 + teflon_thickness_tot/2,
  //                                teflon_thickness_tot/2);
      
  // auto fTeflonBackLogic   = new G4LogicalVolume(teflon_back,
  //                                               G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
  //                                               "TEFLON_BACK");
  
  // fTeflonBackLogic->SetVisAttributes(G4Colour::White ());

  // // Place teflon inside crystalT

  // auto teflon_sides_phys = new G4PVPlacement(0, G4ThreeVector(), fTeflonSidesLogic, "TEFLON_SIDES",
  //                                            crystalLogicT,false, 0, true);

  // auto teflon_back_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, 
  //                                           -(fCrystalLength +  teflon_thickness_tot) /2), fTeflonBackLogic,
  //                                           "TEFLON_BACK",
  //                                           crystalLogicT,false, 0, true);

 
  // Build the scanner
  auto angles = floor(fPetDiameter * M_PI  / eCrystalWidth);
  
  auto step = 2.0 * M_PI / angles;
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateX(90 * deg);
  rot->rotateZ(180 * deg);

  auto rings = floor(fPetLength / fCrystalWidth);

  G4cout << G4endl
         << " Crystals per ring =" << angles
         << " Number of rings =" << rings
         << " total crystals =" << angles * rings
         << G4endl;

  
  GlobalPars::Instance()->fCrystalsPerRing = angles;
  GlobalPars::Instance()->fNumberOfRings = rings; 
  GlobalPars::Instance()->fNumberOfCrystals = angles * rings;  

  for (auto iring=0; iring < 2; iring++) {
    for (auto itheta=0; itheta < angles; itheta++) {
      auto theta = 2 * M_PI * itheta / angles ;
      auto label = std::to_string(iring*angles + itheta);

      auto y = (fPetDiameter / 2 + eCrystalLength / 2) * std::cos(theta);
      auto x = (fPetDiameter / 2 + eCrystalLength / 2) * std::sin(theta);
      auto z = -fPetLength / 2 + iring * fCrystalWidth + fCrystalWidth / 2;

      G4cout << "ring number = " << iring << "  block number = " << itheta << G4endl;
      G4cout << "CRYSTAL" << iring*angles + itheta << " " << x << " " << y << " " << z << " " << G4endl;
      fSensorPosFile << iring*angles + itheta <<  "," << x << "," << y << "," << x <<"\n";

      
      new G4PVPlacement(G4Transform3D(*rot, G4ThreeVector(x, y, z)),
                fCrystalLogic, "CRYSTAL"+label, lab_logic,
                true, iring*angles + itheta, true);
      }
      
      rot->rotateZ(-step);
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
