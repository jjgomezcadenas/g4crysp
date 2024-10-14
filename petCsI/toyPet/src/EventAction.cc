#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "GlobalPars.hh"
#include "SensorSD.hh"
#include "HistogramManager.hh"
#include "G4Threading.hh"
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>


EventAction::EventAction()
    : G4UserEventAction()
{
}


EventAction::~EventAction()
{
}


void EventAction::BeginOfEventAction(const G4Event* event)
{
  
}


void EventAction::EndOfEventAction(const G4Event* event)
{

  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4HCtable* hct = sdmgr->GetHCtable();
  fEventNumber = event->GetEventID() + GlobalPars::Instance()->fEventShift ;
 
  
   // Loop through the hits collections
  for (auto i=0; i<hct->entries(); i++) {

    // Collection are identified univocally using
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


  for (auto i=0; i<hits->entries(); i++)
    {
      SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(i));
      if (!hit) continue;
      auto xyz = hit->fXYZ;
      

      WriteCrystalData(fEventNumber, (unsigned int)hit->fSensorID, 
                                hit->fEnergy, hit->fXYZ, hit->fPos);
                               
    
    }
  
}


void EventAction::WriteCrystalData(int64_t evt_number, unsigned int sensor_id, double energy,
                                   G4ThreeVector xyz, G4ThreeVector posxyz)

{
   
  std::lock_guard<std::mutex> guard(GlobalPars::Instance()->iSensorDataFileMutex);

  GlobalPars::Instance()->iSensorDataFile << evt_number << "," << sensor_id << "," << energy << ","
  << (float)xyz.x()<< "," << (float)xyz.y()<< "," << (float)posxyz.z()<< ","
  << (float)posxyz.x()<<"," <<(float)posxyz.y()<<"," << (float)posxyz.z()<<"\n";
}
