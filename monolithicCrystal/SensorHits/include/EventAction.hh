#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include <string>
#include <vector>
#include <fstream>
#include "globals.hh"

class G4VHitsCollection;

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  
  void WriteSensorPos(unsigned int sensor_id, float x, float y, float z);
  void WriteSensorData(int64_t evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge);
  void WriteIntegratedSensorData(int64_t evt_number, unsigned int sensor_id, double amplitude);


  void StoreSensorHits(G4VHitsCollection* hc);

  G4int fEventNumber;
    
  std::string  sensorPosFileName ="sensor_pos.csv";
  std::string  sensorDataFileName ="sensor_data.csv";
  std::string  integratedSensorDataFileName ="integrated_sensor_data.csv";
  
  std::ofstream sensorPosFile;
  std::ofstream sensorDataFile;
  std::ofstream integratedSensorDataFile;
  
  std::map<G4String, G4double>  fSensDetBin;
  std::vector<G4int> fSnsPosvec;

};

#endif
