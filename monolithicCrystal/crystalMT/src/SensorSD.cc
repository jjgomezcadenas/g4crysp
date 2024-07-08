// ----------------------------------------------------------------------------
//
// This class is the sensitive detector that allows for the registration
// of the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SensorSD.hh"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4RunManager.hh>
#include "G4SystemOfUnits.hh"
#include "GlobalPars.hh"
#include <iostream>

SensorSD::SensorSD(G4String sdname): G4VSensitiveDetector(sdname)  
{
  
  // Register the name of the collection of hits
  collectionName.insert(GlobalPars::Instance()->gSDCollection);
}


// SensorSD::~SensorSD()
// {
// }


// G4String SensorSD::GetCollectionUniqueName()
// {
//   return "SensorHitsCollection";
// }


/// Initialization of the sensitive detector. Invoked at the beginning
/// of every event. The collection of hits is created here and registered
/// in the event (so that it can be retrieved thru the G4HCofThisEvent object).

void SensorSD::Initialize(G4HCofThisEvent* HCE)
{
  // Create a new collection of sensorSD hits
  fHitsCollection = new SensorHitsCollection(this->GetName(), this->GetCollectionName(0));

  G4int HCID = G4SDManager::GetSDMpointer()->
    GetCollectionID(this->GetName()+"/"+this->GetCollectionName(0));

  HCE->AddHitsCollection(HCID, fHitsCollection);
}



G4bool SensorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  // Check whether the track is an optical photon
  
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* pdef = track->GetDefinition();
  if (pdef != G4OpticalPhoton::Definition()) return false;

  const G4VTouchable* touchable = step->GetPostStepPoint()->GetTouchable();

  G4int sensor_id = FindSensorID(touchable);

  SensorHit* hit = nullptr;
  for (size_t i=0; i<fHitsCollection->entries(); i++) {
    if ((*fHitsCollection)[i]->fSensorID == sensor_id) {
      hit = (*fHitsCollection)[i];
      break;
    }
  }

  // If no hit associated to this sensor exists already,
  // create it and set main properties
  
  if (!hit)
    {
      hit = new SensorHit();
      hit->fSensorID = sensor_id;
      hit->fBinSize =  fTimeBinning;
      hit->fSensorPos = touchable->GetTranslation();
      fHitsCollection->insert(hit);
    }

  auto time = step->GetPostStepPoint()->GetGlobalTime();
  hit->Fill(time);

  //track->SetTrackStatus(fStopAndKill);

  return true;
}


G4int SensorSD::FindSensorID(const G4VTouchable* touchable)
{
  auto sensorid = touchable->GetCopyNumber(0); // "PHOTODIODES", one copy per SiPM = 0
  auto  motherid = touchable->GetCopyNumber(1);// SiPM id, 64 copies.

  //  std::cout << "sensor id = " << sensorid << " motherid =" << motherid << std::endl;
  sensorid = motherid + sensorid; //sensorid =0 always in this example, but keep like this for clarity
  
  // auto sensorid = touchable->GetCopyNumber(0);
  // if (fNamingOrder != 0) {
  //   auto  motherid = touchable->GetCopyNumber(fMotherDepth);
  //   sensorid = fNamingOrder * motherid + sensorid;
  // }
  return sensorid;
}


void SensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  // Hits are processed in EventAction

}

