#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensorSD.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
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


#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
    //fMessenger = new DetectorMessenger(this);
    G4cout << " DetectorConstruction " << G4endl;

    fSensorPosFile.open("sensor_positions.csv");
    fSensorPosFile << "sensor_id,sensor_x,sensor_y,sensor_z\n";
}


DetectorConstruction::~DetectorConstruction()
{
    //delete fMessenger;
    fSensorPosFile.close();
}


void DetectorConstruction::display() const
{
  
  G4cout << G4endl
         << " ---Detector Construction---"
         << " crystal material =" << GlobalPars::Instance()->fMaterial
         << " crystal width (mm) =" << GlobalPars::Instance()->fCrystalWidth/mm
         << " crystal length (x0) =" << GlobalPars::Instance()->fCrystalX0Length
         << " teflon thicness (mm) =" << GlobalPars::Instance()->fTeflonThickness/mm
         << " Size of SiPM case XY (mm) =" << GlobalPars::Instance()->fSipmXY/mm
         << " Size of SiPM case Z (mm) =" << GlobalPars::Instance()->fSipmZ/mm
         //<< " Size of SiPM caseactive XY (mm) =" << fActiveXY/mm
         //<< " Size of SiPM plastic Z (mm) =" << fPlasticZ/mm
         << " Size of SiPM epoxy Z (mm) =" << GlobalPars::Instance()->fEpoxyZ/mm
         //<< " Size of SiPM active Z (mm) =" << GlobalPars::Instance()->fActiveZ/mm
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

    GlobalPars::Instance()->fCrystalLength = fCrystalLength;
    return material;

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  fMaterial = GlobalPars::Instance()->fMaterial;
  fCrystalWidth = GlobalPars::Instance()->fCrystalWidth;
  fCrystalX0Length = GlobalPars::Instance()->fCrystalX0Length;
  
  fTeflonThickness = GlobalPars::Instance()->fTeflonThickness;
  fEpoxyZ = GlobalPars::Instance()->fEpoxyZ;
 
  fSipmXY = GlobalPars::Instance()->fSipmXY;
  fSipmZ = GlobalPars::Instance()->fSipmZ;
  fActiveXY = GlobalPars::Instance()->fActiveXY;
  fActiveZ = GlobalPars::Instance()->fActiveZ;

  auto fPlasticZ = fSipmZ - fEpoxyZ - fActiveZ;

// Needs to go to option card

  //auto wcBaseXY = fCrystalWidth;
  //auto wcTopXY =  fCrystalWidth/2;
  

  auto wcBaseXY = 6*mm;
  auto wcTopXY =  3*mm;
  auto wcLengthZ = 10 * mm;

  auto material =  DefineMaterial();
  
  display();

  // materials
  auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  auto plastic  = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");
  
  auto teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  teflon->SetMaterialPropertiesTable(copt::PTFE());

  auto silicon    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  silicon->SetMaterialPropertiesTable(copt::SipmHamamatsu());

  auto silicone   = cmat::OpticalSilicone();
  silicone->SetMaterialPropertiesTable(copt::OptCoupler());

  auto fsilica   = cmat::FusedSilica();
  fsilica->SetMaterialPropertiesTable(copt::FusedSilica());
  
  // define an optical surface for Teflon (LUT model)
  auto ptfe_surface = new G4OpticalSurface("PTFE_SURFACE");
  ptfe_surface->SetType(dielectric_LUT);
  ptfe_surface->SetFinish(groundteflonair);
  ptfe_surface->SetModel(LUT);

  // Create optical surfaces for grease interfaces
  auto greaseSurface = new G4OpticalSurface("GreaseSurface");
  greaseSurface->SetType(dielectric_dielectric);
  greaseSurface->SetFinish(polished);
  greaseSurface->SetModel(unified);
  
  // Laboratory (World)  
  
  auto          lab_solid = new G4Box("LAB", fCrystalWidth*2, fCrystalWidth*2, fCrystalWidth*2);
  auto          lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");
  auto          lab_phys  = new G4PVPlacement(0, G4ThreeVector(), lab_logic, "LAB",
                                              0, false, 0, true);
  lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  // ----Crystal block ------
  auto crystal  = new G4Box("CRYSTAL", fCrystalWidth/2, fCrystalWidth/2, fCrystalLength/2);
  auto fCrystalLogic = new G4LogicalVolume(crystal, material, "CRYSTAL");
  fCrystalLogic->SetVisAttributes(cvis::LightBlueAlpha());

  // Place block  in the Lab
  auto crystal_phys      = new G4PVPlacement(0, G4ThreeVector(), fCrystalLogic,  "CRYSTAL",
                                             lab_logic,false, 0, true);    
                                                                 

  //----Teflon ----------
  
  auto teflon_thickness_tot = fTeflonThickness ;
  auto teflonWC = fTeflonThickness/2;
    
  // Make a box (full of teflon) of the same dimensions of the crystal + teflon thickness on +- x and +-y
  // same size that crystal in +-z
    
  auto teflon_coating_full  = new G4Box("TEFLON_FULL",
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalWidth/2 + teflon_thickness_tot/2,
                                        fCrystalLength/2);

  // Subtracting above box from crystal box we obtain a teflon skin in +-xy
  auto teflon_coating    = new G4SubtractionSolid("TEFLON_SIDES", teflon_coating_full, crystal);

  // Now define teflon sides wrap and place it in the lab
  auto fTeflonSidesLogic = new G4LogicalVolume(teflon_coating,
                                               teflon,
                                               "TEFLON_SIDES");
 
  fTeflonSidesLogic->SetVisAttributes(G4Colour::White ());
  auto teflon_sides_phys = new G4PVPlacement(0, G4ThreeVector(), fTeflonSidesLogic, "TEFLON_SIDES",
                                             lab_logic,false, 0, true);

  // Define back wrapping: This is a thin slab of xy dimensions equal to crytal + wrap and z dimension equal to wrap
  G4Box* teflon_back = new G4Box("TEFLON_BACK",
                                 fCrystalWidth/2 + teflon_thickness_tot/2,
                                 fCrystalWidth/2 + teflon_thickness_tot/2,
                                 teflon_thickness_tot/2);
      
  auto fTeflonBackLogic   = new G4LogicalVolume(teflon_back,
                                                teflon,
                                                "TEFLON_BACK");
  fTeflonBackLogic->SetVisAttributes(G4Colour::White ());
  auto teflon_back_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, -(fCrystalLength +  teflon_thickness_tot) /2), 
                                            fTeflonBackLogic,"TEFLON_BACK",
                                            lab_logic,false, 0, true); 

  
  // G4LogicalBorderSurface defines the optical properties at the boundary between two physical volumes.
  new G4LogicalBorderSurface("CRYSTAL_PTFE", crystal_phys, teflon_sides_phys, ptfe_surface);
  new G4LogicalBorderSurface("CRYSTAL_PTFE_BACK", crystal_phys, teflon_back_phys, ptfe_surface);

  // -------Wiston Cone = epoxy + fused silica + epoxy 
  //Define an envelop for the WC. This will be a G4Trd filled with epoxy. The fused silica
  // is placed in the center of the envelop, with its lateral faces surrounded by Teflon.
  // In this way, epoxy fills the entrance and exit faces of the fused silica. 

  // WC Envelop
  auto solidWC = new G4Trd("WC", wcBaseXY/2, wcTopXY/2, wcBaseXY/2, wcTopXY/2, wcLengthZ/2);
  auto logicWC = new G4LogicalVolume(solidWC, silicone, "WC");
  logicWC->SetVisAttributes(cvis::Yellow());

// Fused silica
  auto xp = (wcBaseXY - teflonWC) /2 ;
  auto yp = (wcTopXY - teflonWC) /2 ;
  auto zp = (wcLengthZ - 2*fEpoxyZ)/2 ;

  auto solidFsilica = new G4Trd("FSILICA", xp, yp, xp, yp, zp);
  auto logicFsilica = new G4LogicalVolume(solidFsilica, fsilica, "FSILICA");
  logicFsilica->SetVisAttributes(cvis::RedAlpha());

  // Position the fused silica 
  
  auto physFsilica = new G4PVPlacement(0, G4ThreeVector(0, 0., 0), logicFsilica,
                    "FSILICA", logicWC, false, 0, true);
  

  // Make a box (full of teflon) of the same dimensions of the fused silica + teflon thickness on +- x and +-y
  // same size that crystal in +-z

  xp = (wcBaseXY - teflonWC - 10*um) /2 ; //adjust by hand to avoid overlaps  
  auto teflonFull  = new G4Trd("TEFLON_FFULL", xp, wcTopXY/2, xp, wcTopXY/2, zp);
                                        

  // Substract from solid Fsilica we obtain a teflon skin in +-xy
  auto teflonCoating    = new G4SubtractionSolid("TEFLON_FSIDES", teflonFull, solidFsilica);

  // Now define teflon sides wrap and place it in the lab
  auto fFTeflonSidesLogic = new G4LogicalVolume(teflonCoating, teflon, "TEFLON_FSIDES");
 
  fFTeflonSidesLogic->SetVisAttributes(G4Colour::White ());
  auto fFTeflonSidesPhys = new G4PVPlacement(0, G4ThreeVector(), fFTeflonSidesLogic, "TEFLON_FSIDES",
                                             logicWC,false, 0, true);
  
  new G4LogicalBorderSurface("FSILICA_PTFE", physFsilica, fFTeflonSidesPhys, ptfe_surface);

  //--------
  // new G4LogicalBorderSurface("EPOXY_FSILICA", wc_epoxy_phys, physFsilica, greaseSurface);

  // auto wc_epoxy_solid  = new G4Box("WCEPOXY", fCrystalWidth/2, fCrystalWidth/2, fEpoxyZ/2);
  // auto wc_epoxy_logic  = new G4LogicalVolume(wc_epoxy_solid, silicone, "WCEPOXY");
  // wc_epoxy_logic->SetVisAttributes(cvis::Yellow());

  // auto si_epoxy_solid  = new G4Box("SIEPOXY", wcTopXY/2, wcTopXY/2, fEpoxyZ/2);
  // auto si_epoxy_logic  = new G4LogicalVolume(si_epoxy_solid, silicone, "SIEPOXY");
  // si_epoxy_logic->SetVisAttributes(cvis::Yellow());

  
  // // Position the expoxy (crystal-fsilica)
  // auto posz = (fCrystalLength + fEpoxyZ)/2;
  // auto wc_epoxy_phys = new G4PVPlacement(0, G4ThreeVector(0, 0., posz), wc_epoxy_logic,
  //                   "WCEPOXY", lab_logic, false, 0, true);
  
  // new G4LogicalBorderSurface("CRYSTAL_EPOXY", crystal_phys, wc_epoxy_phys, greaseSurface);

  // Position the fused silica 
  // posz = fCrystalLength/2 + fEpoxyZ + wcLengthZ/2;
  
  // auto physFsilica = new G4PVPlacement(0, G4ThreeVector(0, 0., posz), logicFsilica,
  //                   "FSILICA", lab_logic, false, 0, true);
  
  // new G4LogicalBorderSurface("EPOXY_FSILICA", wc_epoxy_phys, physFsilica, greaseSurface);

  // // Position the expoxy (fsilica-sipm)
  // posz = fCrystalLength/2 + fEpoxyZ + wcLengthZ + fEpoxyZ/2;
  // auto si_epoxy_phys = new G4PVPlacement(0, G4ThreeVector(0, 0., posz), si_epoxy_logic,
  //                   "SIEPOXY", lab_logic, false, 0, true);
  
  // new G4LogicalBorderSurface("FSILICA_EPOXY", physFsilica, si_epoxy_phys, greaseSurface);

  //-------SiPMs------
  //fSipmZ = 1.0*mm;
  //fActiveZ = 0.01 * mm;
  
  // Define a case for SiPMs.
  auto sipm_case  = new G4Box("SiPM", fSipmXY/2, fSipmXY/2, fSipmZ/2);
  auto sipm_logic = new G4LogicalVolume(sipm_case, air, "SiPM");
  sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  
  // plastic
  auto plastic_solid = new G4Box("Plastic", fSipmXY/2, fSipmXY/2, fPlasticZ/2);
  auto plastic_logic = new G4LogicalVolume(plastic_solid, plastic, "Plastic");
  plastic_logic->SetVisAttributes(cvis::LightBlue());

  // SiPM sensor (made of silicon) 
  auto active_solid  = new G4Box("PHOTODIODES", fActiveXY/2, fActiveXY/2, fActiveZ/2);
  auto active_logic  = new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");
  active_logic->SetVisAttributes(cvis::Red());

  // SiPM optical coupling (made of silicona --Epoxy) 
  auto epoxy_solid  = new G4Box("SEPOXY", fActiveXY/2, fActiveXY/2, fEpoxyZ/2);
  auto epoxy_logic  = new G4LogicalVolume(epoxy_solid, silicone, "SEPOXY");
  epoxy_logic->SetVisAttributes(cvis::Yellow());

  // Position the expoxy  
  auto activePosZ = -(fSipmZ - fEpoxyZ)/2;
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), epoxy_logic,
                    "SEPOXY", sipm_logic, false, 0, true);
  
  // Position the photodiodes after the epoxy
  activePosZ = -fSipmZ/2 + fEpoxyZ + fActiveZ/2; 
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), active_logic,
                    "PHOTODIODES", sipm_logic, false, 0, true);
  
  // Position the plastic after the active
  activePosZ = -fSipmZ/2  + fEpoxyZ + fActiveZ + fPlasticZ/2; 
  new G4PVPlacement(0, G4ThreeVector(0, 0., activePosZ), plastic_logic,
                                     "PLASTIC", sipm_logic, false, 0, true);

  //---Position the WC and the SiPM

  // Position the WC
  // zp = (fCrystalLength + wcLengthZ)/2;
  
  // auto physWC = new G4PVPlacement(0, G4ThreeVector(0, 0., zp), logicWC,
  //                    "WC", lab_logic, false, 0, true);

  // // Position the SiPM case in the lab
  
  // new G4PVPlacement(0, G4ThreeVector(0, 0, xz),
  //                           sipm_logic, "SiPM", lab_logic, false, 0, true);

  // // Position the SiPMs at the exit (z >0) of the crystal and across xy
  // auto xz = (fCrystalLength + fSipmZ )/2; //
    
  //auto n_rows = (int)fCrystalWidth/fSipmXY;
  auto n_rows = 8;
  auto n_cols = n_rows;
  auto xz = fCrystalLength/2 + wcLengthZ + fSipmZ/2; // position in z of the SiPMs
  auto wc = (fCrystalLength + wcLengthZ)/2; // position in z of the WC

  for (auto irow = 0; irow < n_rows; irow++)
    {
      for (auto icol = 0; icol < n_cols; icol++)
        {
          auto label = std::to_string(irow * n_cols + icol);
          auto silabel = "SiPM" + label;
          auto xr = irow * wcBaseXY - fCrystalWidth/2 + wcBaseXY/2;
          auto yr = icol * wcBaseXY - fCrystalWidth/2 + wcBaseXY/2;
          // G4cout << "Placing SiPM copy " << silabel
          //        <<" row = " << irow << " column =" << icol
          //        <<" x = " << xr 
          //        <<" y = " << yr
          //        <<" z = " << xz
          //        << G4endl;
          fSensorPosFile << irow * n_cols + icol <<  "," << xr << "," << yr << "," << xz <<"\n";
          new G4PVPlacement(0, G4ThreeVector(xr, yr, xz),
                            sipm_logic, "SiPM" + label, lab_logic, true, irow * n_cols + icol);
          new G4PVPlacement(0, G4ThreeVector(xr, yr, wc),
                            logicWC, "WC" + label, lab_logic, true, irow * n_cols + icol);
        }
    }
  
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
  SetSensitiveDetector("PHOTODIODES", sensorsd);
  //fActive->SetSensitiveDetector(sensorsd);
  

}
