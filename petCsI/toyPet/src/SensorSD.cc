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

  // Get a pointer to the crystal will need it later. 
  fenvLV = G4LogicalVolumeStore::GetInstance()->GetVolume("CRYSTAL");

  if (!fenvLV) {
    G4Exception("[SensorSD]", "Constructor", FatalException,
                "could not get the pointer of logical volume CRYSTAL!");
  }

  G4cout << "Constructor SensorSD" << G4endl;
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
  
  //G4cout << "++++++inside ProcessHits+++++" << G4endl;

  // Get sure that we are in crystal
  G4Material* mat = fenvLV->GetMaterial();

  //G4cout << "material =" << mat->GetName() 
  // << " fMaterial = " << GlobalPars::Instance()->fMaterial
 // << G4endl;

  if (mat->GetName() != GlobalPars::Instance()->fMaterial){ // must be CsI, BGO...
    return false;
  }

  auto track = step->GetTrack();
  auto parentID = track->GetParentID();
  auto trkid = track->GetTrackID();

  //G4cout << "parentID =" << parentID << " trkid = " << trkid
  //<< G4endl;

  // Must be primary gamma

  if (parentID != 0 ) { // not primary gamma
    //track->SetTrackStatus(fStopAndKill); //kill the track to speed up the simulation
    return false;
  }

  auto postep = step->GetPostStepPoint();
  auto posxyz = postep->GetPosition(); 
  auto post   = postep->GetGlobalTime() ;
  auto poske  = postep->GetKineticEnergy() ;
    
     
  auto touchable = step->GetPostStepPoint()->GetTouchable();
  auto sensor_id = FindSensorID(touchable);
  
  if (poske == 0){
    return false;
  }

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
      //G4cout << "--Create a new hit =" << G4endl;

      hit = new SensorHit();
      hit->fSensorID = sensor_id;
      //hit->fPos = touchable->GetTranslation();
      //hit->fEnergy = track->GetTotalEnergy();
      hit->fEnergy = poske;
      hit->fXYZ = posxyz;
      hit->fTime = post ;
      fHitsCollection->insert(hit);

      // G4cout << "crystal number = " << sensor_id
      // << " energy = " << hit->fEnergy 
      // << " XYZ = " << hit->fXYZ
      // << " time = " << hit->fTime
      // << G4endl;
    }

  track->SetTrackStatus(fStopAndKill); //hit registered we can kill the track
  
  return true;
}


G4int SensorSD::FindSensorID(const G4VTouchable* touchable)
{

  //std::cout << "+++FindSensorID " << std::endl;

  auto sensorid = touchable->GetCopyNumber(0); // "CYRSTAL", one copy per CRYSTALT = 0
  //std::cout << "sensor id = " << sensorid  << std::endl;

  auto  motherid = touchable->GetCopyNumber(1);// Crystal id, ncrystal copies.
  //std::cout << " motherid =" << motherid << std::endl;

  sensorid = motherid + sensorid; //sensorid =0 always in this example, but keep like this for clarity
  
  return sensorid;
}


void SensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  // Hits are processed in EventAction

}

