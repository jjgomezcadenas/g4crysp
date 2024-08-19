#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <atomic>
#include "globals.hh"

class G4VHitsCollection;

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  
  void WriteSensorData(int64_t evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge);
  void WriteIntegratedSensorData(int64_t evt_number, unsigned int sensor_id, double amplitude);
  void StoreSensorHits(G4VHitsCollection* hc);

  G4int fEventNumber;
    
  //static std::ofstream sensorDataFile;
  //static std::ofstream iSensorDataFile;

  static std::mutex sensorDataFileMutex;
  static std::mutex iSensorDataFileMutex;
  
  //static std::atomic<bool> sensorDataFileWritten;
  //static std::atomic<bool> isensorDataFileWritten;
    
  std::map<G4String, G4double>  fSensDetBin;


};

#endif
