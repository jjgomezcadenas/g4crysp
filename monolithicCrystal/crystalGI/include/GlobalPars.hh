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
  G4int fNumberOfEvents;
  G4int fEventShift;

  std::string fSensorDataFileName;
  std::string fISensorDataFileName;
  std::string fGammaDataFileName;
  std::string fHistoFileName;
  
  std::ofstream sensorDataFile;
  std::ofstream iSensorDataFile;
  std::ofstream gammaIntFile;
  std::ofstream globalsFile;

  std::mutex sensorDataFileMutex;
  std::mutex iSensorDataFileMutex;
  std::mutex gammaIntFileMutex;

  bool fIDataOnly;
  bool fGOP; 
  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
