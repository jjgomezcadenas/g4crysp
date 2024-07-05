#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class DetectorMessenger;
class G4Material; 
class DetectorConstruction  : public G4VUserDetectorConstruction
{
 public:
  DetectorConstruction();
  ~DetectorConstruction() override;
  
  void ConstructSDandField() override;

  G4VPhysicalVolume* Construct() override;
  void display () const;

 private:
  G4Material* DefineMaterial();
  
 public:
  G4String fMaterial ;
  G4double fCrystalWidth;
  G4double fCrystalX0Length;
  G4double fCrystalLength;
  G4double fTeflonThickness ;
  G4double fTeflonCoatings ;
  G4double fSipmXY ;
  G4double fActiveXY ;
  G4double fSipmZ ;
  G4double fEpoxyZ ;
  G4double fPlasticZ ;
  G4double fActiveZ ;

  DetectorMessenger* fMessenger = nullptr; // messenger

};

#endif
