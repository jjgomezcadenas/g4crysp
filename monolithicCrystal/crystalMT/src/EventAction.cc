#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "GlobalPars.hh"
#include "SensorSD.hh"
#include "HistogramManager.hh"
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>


// std::mutex EventAction::sensorDataFileMutex;
// std::mutex EventAction::iSensorDataFileMutex;

//std::ofstream EventAction::sensorDataFile(GlobalPars::Instance()->fSdf);
//std::ofstream EventAction::iSensorDataFile(GlobalPars::Instance()->fIsdf);

//std::atomic<bool> EventAction::sensorDataFileWritten(false);
//std::atomic<bool> EventAction::isensorDataFileWritten(false);


EventAction::EventAction()
    : G4UserEventAction()
{
}


EventAction::~EventAction()
{
}


void EventAction::BeginOfEventAction(const G4Event* event)
{
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  G4int eventShift = (seedL4D -1) * GlobalPars::Instance()->fNumberOfEvents;
  fEventNumber = event->GetEventID() + eventShift;
}


void EventAction::EndOfEventAction(const G4Event* event)
{

  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4HCtable* hct = sdmgr->GetHCtable();

  
   // Loop through the hits collections
  for (auto i=0; i<hct->entries(); i++) {

    // Collection are identified univocally (in principle) using
    // their id number, and this can be obtained using the collection
    // and sensitive detector names.
    
    auto hcname = hct->GetHCname(i);
    auto sdname = hct->GetSDname(i);
    auto hcid = sdmgr->GetCollectionID(sdname+"/"+hcname);
    auto hce  = event->GetHCofThisEvent();

    // Fetch collection using the id number
    G4VHitsCollection* hits = hce->GetHC(hcid);

    //    if (hcname == SensorSD::GetCollectionUniqueName())
    if (hcname == GlobalPars::Instance()->gSDCollection)
      {
        StoreSensorHits(hits);
      }
    else
      {
        G4String msg =
          "Collection of hits '" + sdname + "/" + hcname
          + "' is of an unknown type and will not be stored.";
        G4Exception("[PersistencyManager]", "StoreHits()", JustWarning, msg);
      }
  }
      
}


void EventAction::StoreSensorHits(G4VHitsCollection* hc)
{

  SensorHitsCollection* hits = dynamic_cast<SensorHitsCollection*>(hc);
  if (!hits) return;

  auto sdname = hits->GetSDname();

  std::map<G4String, G4double>::const_iterator sensdet_it = fSensDetBin.find(sdname);
  
  if (sensdet_it == fSensDetBin.end())
    {
      for (auto j=0; j<hits->entries(); j++)
        {
          SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(j));
          if (!hit) continue;
          fSensDetBin[sdname] = GlobalPars::Instance()->fTimeBinning;;
          break;
        }
    }

  auto totalEnergy = 0.0;
  auto totalEnergyTime = 0.0;
  for (auto i=0; i<hits->entries(); i++)
    {
      SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(i));
      if (!hit) continue;
      auto xyz = hit->fSensorPos;
      //auto binsize =  GlobalPars::Instance()->fTimeBinning;;

      const std::map<G4double, G4int> wvfm = hit->fNphotons;
      std::map<G4double, G4int>::const_iterator it;
      G4double amplitude = 0.;
      G4double amplitudeTime = 0.;

      for (it = wvfm.begin(); it != wvfm.end(); ++it)
        {
          unsigned int time_bin = (unsigned int)((*it).first + 0.5);
          unsigned int charge = (unsigned int)((*it).second + 0.5);

          HistogramManager::Instance()->FillHistogram("TimeBin2", 
          time_bin, charge);
          HistogramManager::Instance()->FillHistogram("Charge", charge);

          amplitude = amplitude + (*it).second;

          if (time_bin <= 1){ // amplitude in first bin 
            amplitudeTime = amplitude;
          }

         if (GlobalPars::Instance()->fIDataOnly == false){
         WriteSensorData(fEventNumber, (unsigned int)hit->fSensorID,
                         time_bin, charge);
         }
        }

      WriteIntegratedSensorData(fEventNumber, (unsigned int)hit->fSensorID, 
                                amplitude);

      totalEnergy += amplitude;
      totalEnergyTime += amplitudeTime;
    
    }
  HistogramManager::Instance()->FillHistogram("TotalEnergy", 
                                                    totalEnergy);
  HistogramManager::Instance()->FillHistogram("TotalEnergyTime", 
                                                    totalEnergyTime);
}


void EventAction::WriteSensorData(int64_t evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge)
{

  std::lock_guard<std::mutex> guard(GlobalPars::Instance()->sensorDataFileMutex);
  // Write event data to the first file

  GlobalPars::Instance()->sensorDataFile << evt_number << "," << sensor_id << "," << time_bin << "," << charge <<"\n";
}


void EventAction::WriteIntegratedSensorData(int64_t evt_number, unsigned int sensor_id, double amplitude)
{
  std::lock_guard<std::mutex> guard(GlobalPars::Instance()->iSensorDataFileMutex);

  GlobalPars::Instance()->iSensorDataFile << evt_number << "," << sensor_id << "," << amplitude <<"\n";
}
