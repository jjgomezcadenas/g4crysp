#ifndef GLOBAL_PARS_HH
#define GLOBAL_PARS_HH

#include "globals.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class GlobalPars
{
public:
  static GlobalPars* Instance();
  G4String  gSDCollection ;
  G4int     fSeed;
  G4double  fTimeBinning;
  G4double  fGammaEnergy;
  G4double  fCrystalWidth;
  G4double  fCrystalLength;
  G4double  fPetDiameter;
  G4double  fPetLength; 

  G4int fCrystalsPerRing ;
  G4int fNumberOfRings ; 
  G4int fNumberOfCrystals ;  

  std::string  fMaterial;
 
  G4int fNumberOfEvents;
  G4int fEventShift;

  std::string fISensorDataFileName;
  std::string fGlobalDataFileName;
  std::string fHistoFileName;
  
  std::ofstream iSensorDataFile;
  std::ofstream globalsFile;

  std::mutex iSensorDataFileMutex;
  
  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
