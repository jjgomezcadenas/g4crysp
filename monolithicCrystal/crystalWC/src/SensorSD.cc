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
#include <globals.hh>
#include "G4PhysicalConstants.hh"
#include <G4MaterialPropertiesTable.hh>
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include <G4Material.hh>
#include "Randomize.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"

#include "HistogramManager.hh"
#include <iostream>

SensorSD::SensorSD(G4String sdname): G4VSensitiveDetector(sdname)  
{
  
  // Register the name of the collection of hits
  collectionName.insert(GlobalPars::Instance()->gSDCollection);

}


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
  //G4cout << "inside ProcessHits" << G4endl;
  
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* pdef = track->GetDefinition();
  if (pdef != G4OpticalPhoton::Definition()) {
    return false;
  }

  //time that photon needs to propagate from vertex to sensor
  auto time = step->GetPostStepPoint()->GetGlobalTime(); 
  auto en = track->GetKineticEnergy();
  auto wl = 1240.0 / (en/eV);

  //G4cout << "optical photon of wl (nm) =" << wl << G4endl;


  const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();

  //G4cout << "volume =" << touchable->GetVolume(0)->GetName() << G4endl;

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
      hit->fSensorPos = touchable->GetTranslation();
      fHitsCollection->insert(hit);
    }


  HistogramManager::Instance()->FillHistogram("DecayTime", time);

  //G4cout << " SensorSD::ProcessHits:: time (ns)= " << time/ns <<  " wl (nm) = " << wl <<  G4endl; 
  hit->Fill(time);

  track->SetTrackStatus(fStopAndKill); //hit registered we can kill the track
  
  return true;
}


G4int SensorSD::FindSensorID(const G4VTouchable* touchable)
{
  auto sensorid = touchable->GetCopyNumber(0); // "PHOTODIODES", one copy per SiPM = 0
  auto  motherid = touchable->GetCopyNumber(1);// SiPM id, 64 copies.

  //std::cout << "sensor id = " << sensorid << " motherid =" << motherid << std::endl;
  sensorid = motherid + sensorid; //sensorid =0 always in this example, but keep like this for clarity
  
  return sensorid;
}


void SensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  // Hits are processed in EventAction

}

