#ifndef Primary_Messenger_h
#define Primary_Messenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class PrimaryGeneratorAction;


class PrimaryGeneratorMessenger : public G4UImessenger {
public:
  //PrimaryGeneratorMessenger(PrimaryGeneratorAction* pg);
 
  PrimaryGeneratorMessenger();
  ~PrimaryGeneratorMessenger() override;

  void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:

  //PrimaryGeneratorAction*  fPrimaryGeneratorAction = nullptr;
  G4UIdirectory*              fDirectory = nullptr;
  G4UIcmdWithADoubleAndUnit*  fEnergyCmd = nullptr;
  
};
#endif 
