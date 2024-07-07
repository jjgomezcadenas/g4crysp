#ifndef GLOBAL_PARS_HH
#define GLOBAL_PARS_HH

#include "globals.hh"

class GlobalPars
{
public:
  static GlobalPars* Instance();
  G4String  gSDCollection ;
  
private:
    GlobalPars();
    static GlobalPars* instance;
};

#endif 
