// DetectorMessenger.hh
#ifndef DetectorMessenger_h
#define BDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class DetectorConstruction;


class DetectorMessenger : public G4UImessenger {
public:
    DetectorMessenger(DetectorConstruction* detector);
    ~DetectorMessenger() override;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
  DetectorConstruction*  fDetectorConstruction = nullptr;
  G4UIdirectory*         fDirectory = nullptr;
  G4UIdirectory*         fDetDirectory = nullptr;

  G4UIcmdWithAString*    fMaterialCmd = nullptr;

  G4UIcmdWithADoubleAndUnit* fCrystalWidthCmd = nullptr;
  G4UIcmdWithADouble*        fCrystalX0LengthCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fTeflonThicknessCmd = nullptr;
  G4UIcmdWithADouble*        fTeflonCoatingsCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fSipmXYCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fActiveXYCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fSipmZCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fEpoxyZCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fActiveZCmd = nullptr;
};
#endif 
