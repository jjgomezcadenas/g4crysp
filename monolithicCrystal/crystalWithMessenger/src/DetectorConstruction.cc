#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
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



DetectorConstruction::DetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
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
         << " Size of SiPM case XY (mm) =" << fSipmXY/mm
         << " Size of SiPM case Z (mm) =" << fSipmZ/mm
         << " Size of SiPM caseactive XY (mm) =" << fActiveXY/mm
         << " Size of SiPM plastic Z (mm) =" << fPlasticZ/mm
         << " Size of SiPM epoxy Z (mm) =" << fEpoxyZ/mm
         << " Size of SiPM active Z (mm) =" << fActiveZ/mm
         << G4endl;

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
    return material;

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto material =  DefineMaterial();
  fPlasticZ = fSipmZ - fEpoxyZ - fActiveZ;

  display();
  
  // Laboratory (World)  
  auto          air       = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  auto          lab_solid = new G4Box("LAB", fCrystalWidth*2, fCrystalWidth*2, fCrystalWidth*2);
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
  auto teflon_thickness_tot = fTeflonThickness * fTeflonCoatings;
    
  // Make a box (full of teflon) of the same dimensions of the crystal + teflon thickness on +- x and +-y
  // same size that crystal in +-z
    
  auto teflon_coating_full  = new G4Box("TEFLON_FULL",
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalLength/2);

  // Subtracting above box from crystal box we obtain a teflon skin in +-xy
  auto teflon_coating    = new G4SubtractionSolid("TEFLON", teflon_coating_full, crystal);

  // Now define teflon sides wrap and place it in the lab
  auto fTeflonSidesLogic = new G4LogicalVolume(teflon_coating,
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

  //-------SiPMs------


  G4Material* plastic    = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");
  G4Material* silicon    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  G4Material* silicone   = cmat::OpticalSilicone();

  // Define a volume for SiPMs.
  auto sipm_case  = new G4Box("SiPmCase", fSipmXY/2, fSipmXY/2, fSipmZ/2);
  auto sipm_logic = new G4LogicalVolume(sipm_case, air, "SiPmCase");

  sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
  auto plastic_solid = new G4Box("Plastic", fActiveXY/2, fActiveXY/2, fPlasticZ/2);
  auto plastic_logic = new G4LogicalVolume(plastic_solid, plastic, "Plastic");
  plastic_logic->SetVisAttributes(cvis::LightBlue());


  // SiPM sensor (made of silicon) 
  auto active_solid  = new G4Box("PHOTODIODES", fActiveXY/2, fActiveXY/2, fActiveZ/2);
  auto active_logic  = new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");
  active_logic->SetVisAttributes(cvis::Red());

  // SiPM optical coupling (made of silicona --Epoxy) 
  auto epoxy_solid  = new G4Box("EPOXY", fActiveXY/2, fActiveXY/2, fEpoxyZ/2);
  auto epoxy_logic  = new G4LogicalVolume(epoxy_solid, silicon, "EPOXY");
  epoxy_logic->SetVisAttributes(cvis::Yellow());


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
