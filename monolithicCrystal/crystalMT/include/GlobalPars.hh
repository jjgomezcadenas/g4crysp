#ifndef GLOBAL_PARS_HH
#define GLOBAL_PARS_HH

#include "globals.hh"

class GlobalPars
{
public:
  static GlobalPars* Instance();
  G4String  gSDCollection ;
  G4int fNphotons;
  G4double fFano;
  G4bool fGaussian ;

  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
