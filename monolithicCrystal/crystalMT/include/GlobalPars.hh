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
  G4int     fNphotons;
  G4int     fSeed;
  G4double  fFano;
  G4double  fTimeBinning;
  G4bool    fGaussian ;
  G4bool    fZUniform;
  G4bool    fZReverse;
  G4int fNumberOfEvents;

  std::string fSensorDataFileName;
  std::string fISensorDataFileName;
  std::string fGammaDataFileName;
  std::string fHistoFileName;
  
  std::ofstream sensorDataFile;
  std::ofstream iSensorDataFile;
  std::ofstream gammaIntFile;

  std::mutex sensorDataFileMutex;
  std::mutex iSensorDataFileMutex;
  std::mutex gammaIntFileMutex;

  bool fIDataOnly;
  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
