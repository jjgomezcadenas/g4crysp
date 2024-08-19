#ifndef GLOBAL_PARS_HH
#define GLOBAL_PARS_HH

#include "globals.hh"
#include <iostream>
#include <fstream>
#include <string>

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

  std::ofstream sensorDataFile;
  std::ofstream iSensorDataFile;
  std::ofstream gammaIntFile;


  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
