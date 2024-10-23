// GlobalMessenger.hh
#ifndef Global_Messenger_h
#define Global_Messenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class GlobalMessenger : public G4UImessenger {
public:

  GlobalMessenger();
  ~GlobalMessenger() override;

  void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:

  G4UIdirectory*           fDirectory = nullptr;
  G4UIcmdWithAnInteger*    fSeedCmd = nullptr;
  G4UIcmdWithADouble*      fOpMinimumWlCmd = nullptr;
  G4UIcmdWithAString*      fSensorDataCmd = nullptr;
  G4UIcmdWithAString*      fISensorDataCmd = nullptr;
  G4UIcmdWithAString*      fGammaDataCmd = nullptr;
  G4UIcmdWithAString*      fHistosCmd = nullptr;
  G4UIcmdWithABool*        fIDataOnlyCmd = nullptr;
  G4UIcmdWithABool*        fGOPCmd = nullptr;

};
#endif 
