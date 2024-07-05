#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  void ConstructSDandField() override;

    virtual G4VPhysicalVolume* Construct() override;
};

#endif
