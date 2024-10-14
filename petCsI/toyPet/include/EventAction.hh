#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <atomic>
#include "globals.hh"
#include <G4ThreeVector.hh>

class G4VHitsCollection;


class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  void StoreSensorHits(G4VHitsCollection* hc);
  void WriteCrystalData(int64_t evt_number, unsigned int sensor_id, double energy,
                                   G4ThreeVector xyz, G4ThreeVector posxyz);


  G4int fEventNumber;
    
  static std::mutex iSensorDataFileMutex;
    
};

#endif
