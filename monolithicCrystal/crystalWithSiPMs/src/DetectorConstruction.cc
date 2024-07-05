#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include <G4VisAttributes.hh>
#include "G4SubtractionSolid.hh"
#include "Materials.hh"
#include "Visibilities.hh"
#include "OpticalMaterialsProperties.hh"



//inline G4VisAttributes LightBlueAlpha()    { return { { .6 ,  .8 ,  .79, .5} }; }
DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto fMaterial = "CsI";
  auto fCrystalName ="CRYSTAL";
  auto fCrystalWidth = 48 * mm;
  auto fCrystalX0Length = 2;
  auto fTeflonThickness = 0.08 * mm;
  auto fTeflonCoatings = 5;
  auto fSipmXY = 6 * mm;
  auto fActiveXY = 5.9 * mm;
  auto fSipmZ = 6 * mm;
  auto fEpoxyZ = 2 * mm;
  auto fPlasticZ = 2 * mm;
  auto fActiveZ = 2 * mm;
  auto csix0  = 18.6 * mm;

  auto red = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  auto blue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  auto yellow = new G4VisAttributes(G4Colour(1.0, 1.0, .0));

  // Laboratory (World)  
  auto          air       = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  auto          lab_solid = new G4Box("LAB", fCrystalWidth*2, fCrystalWidth*2, fCrystalWidth*2);
  auto          lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");
  auto          lab_phys  = new G4PVPlacement(0, G4ThreeVector(), lab_logic, "LAB",
                                              0, false, 0, true);

  lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());


  auto fCrystalLength = fCrystalX0Length *  csix0;
  auto material = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");    

  // Create the logical volume

  auto crystal  = new G4Box(fCrystalName, fCrystalWidth/2, fCrystalWidth/2, fCrystalLength/2);
  auto fCrystalLogic = new G4LogicalVolume(crystal, material, fCrystalName);
  fCrystalLogic->SetVisAttributes(cvis::LightBlueAlpha());


  //Teflon
  // Teflown wrapping = thickness of wrap x number of wraps
    
  auto teflon_thickness_tot = fTeflonThickness * fTeflonCoatings;
    
  // Make a box (full of teflon) of the same dimensions of the crystal + teflon thickness on +- x and +-y
  // same size that crystal in +-z
    
  auto teflon_coating_full  = new G4Box("TEFLON_FULL",
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalLength/2);

  // Subtracting above box from crystal box we obtain a teflon skin in +-xy
  auto teflon_coating   = new G4SubtractionSolid("TEFLON", teflon_coating_full, crystal);

  // Now define teflon sides wrap and place it in the lab
  auto fTeflonSidesLogic                    = new G4LogicalVolume(teflon_coating,
                                                             G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                                                             "TEFLON_SIDES");
  
  fTeflonSidesLogic->SetVisAttributes(G4Colour::White ());

  // Define back wrapping: This is a thin slab of xy dimensions equal to crytal + wrap and z dimension equal to wrap
    
  G4Box* teflon_back = new G4Box("TEFLON_BACK",
                                 fCrystalWidth/2 + teflon_thickness_tot/2,
                                 fCrystalWidth/2 + teflon_thickness_tot/2,
                                 teflon_thickness_tot/2);
      
  auto fTeflonBackLogic   = new G4LogicalVolume(teflon_back,
                                           G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                                           "TEFLON_BACK");
  fTeflonBackLogic->SetVisAttributes(G4Colour::White ());

  G4Material* plastic    = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");
  G4Material* silicon    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  G4Material* silicone   = cmat::OpticalSilicone();

  // Define a volume for SiPMs.
  auto sipm_case  = new G4Box("SiPmCase", fSipmXY/2, fSipmXY/2, fSipmZ/2);
  auto sipm_logic = new G4LogicalVolume(sipm_case, air, "SiPmCase");

  sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  //sipm_logic->SetVisAttributes(G4Colour::Black());
  
  //auto          lab_phys  = new G4PVPlacement(0, G4ThreeVector(), lab_logic, "LAB",
  //                                            0, false, 0, true);
  
  // The Back of the SiPM is made of plastic
    
  auto plastic_solid = new G4Box("Plastic", fActiveXY/2, fActiveXY/2, fPlasticZ/2);
  auto plastic_logic = new G4LogicalVolume(plastic_solid, plastic, "Plastic");
  plastic_logic->SetVisAttributes(blue);


  // SiPM sensor (made of silicon) 
  auto active_solid  = new G4Box("PHOTODIODES", fActiveXY/2, fActiveXY/2, fActiveZ/2);
  auto active_logic  = new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");
  active_logic->SetVisAttributes(red);

  // SiPM optical coupling (made of silicona --Epoxy) 
  auto epoxy_solid  = new G4Box("EPOXY", fActiveXY/2, fActiveXY/2, fEpoxyZ/2);
  auto epoxy_logic  = new G4LogicalVolume(epoxy_solid, silicon, "EPOXY");
  epoxy_logic->SetVisAttributes(yellow);


  // Position the plastic
  
  auto activePosZ = -(fSipmXY - fSipmZ)/2; //
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), epoxy_logic,
                    "EPOXY", sipm_logic, false, 0, false);
  
  // Position the photodiodes after the plastic
  
  activePosZ = -(fSipmXY - fSipmZ - fActiveZ)/2; //
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), active_logic,
                    "PHOTODIODES", sipm_logic, false, 0, false);
  
  // Position the epoxy after the active
  activePosZ = -(fSipmXY  - fSipmZ - fActiveZ  - fEpoxyZ)/2; //
  auto sipm_active_phys = new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), plastic_logic,
                                            "SiPMCase", sipm_logic, false, 0, false);


  // Place block and Teflon in the Lab
    
  auto crystal_phys      = new G4PVPlacement(0, G4ThreeVector(), fCrystalLogic,  "CRYSTAL",
                                             lab_logic,false, 0, true);    

  auto teflon_sides_phys = new G4PVPlacement(0, G4ThreeVector(), fTeflonSidesLogic, "TEFLON_SIDES",
                                             lab_logic,false, 0, true);

  auto teflon_back_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, -(fCrystalLength +  teflon_thickness_tot) /2), fTeflonBackLogic,"TEFLON_BACK",
                                            lab_logic,false, 0, true);

  // Position the SiPMs at the exit (z >0) of the crystal and across xy
  
  auto xz = (fCrystalLength)/2; //
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), epoxy_logic,
                    "EPOXY", lab_logic, false, 0, false);

  auto n_rows = (int)fCrystalWidth/fSipmXY;
  auto n_cols = n_rows;
    
  for (auto irow = 0; irow < n_rows; irow++)
    {
      for (auto icol = 0; icol < n_cols; icol++)
        {
          auto label = std::to_string(irow * n_cols + icol);
          auto silabel = "SiPM" + label;
          auto xr = irow * fSipmXY - fCrystalWidth/2 + fSipmXY/2;
          auto yr = icol * fSipmXY - fCrystalWidth/2 + fSipmXY/2;
          std::cout << "Placing SiPM copy " << silabel
                    <<" row = " << irow << " column =" << icol
                    <<" x = " << xr 
                    <<" y = " << yr
                    <<" z = " << xz
                    << std::endl;
        
          new G4PVPlacement(0, G4ThreeVector(xr, yr, xz),
                            sipm_logic, "SiPM" + label, lab_logic, true, irow * n_cols + icol);
      }
    }

  
  
  return lab_phys;
}


void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

      // Set the bottom face (in z) as a sensitive detector
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    SensitiveDetector* sensitiveDetector = new SensitiveDetector("/SensitiveDetector");
    sdManager->AddNewDetector(sensitiveDetector);
    SetSensitiveDetector("PHOTODIODES", sensitiveDetector);

}
