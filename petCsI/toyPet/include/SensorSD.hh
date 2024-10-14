// ----------------------------------------------------------------------------
// nexus | SensorSD.h
//
// This class is the sensitive detector that allows for the registration
// of the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_SD_H
#define PMT_SD_H

#include <G4VSensitiveDetector.hh>
#include "SensorHit.hh"
#include "G4SystemOfUnits.hh"

class G4Step;
class G4HCofThisEvent;
//class G4VTouchable;
class G4TouchableHistory;
class G4LogicalVolume;
//class G4OpBoundaryProcess;


class SensorSD: public G4VSensitiveDetector
{
public:
  /// Constructor providing names for the sensitive detector
  /// and the collection of hits
  SensorSD(G4String sdname);
  ~SensorSD() override = default;
  
  /// Initialization of the sensitive detector. Invoked at the beginning
  /// of every event. The collection of hits is created here and registered
  /// in the event (so that it can be retrieved thru the G4HCofThisEvent object).
  void Initialize(G4HCofThisEvent*) override;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
  void EndOfEvent(G4HCofThisEvent*) override;

  /// Return the time binning chosen for the pmt hits
  //G4double GetTimeBinning() const;
  /// Set a time binning for the pmt hits
  //void SetTimeBinning(G4double);

  G4int FindSensorID(const G4VTouchable*);
                                      
public:

  G4LogicalVolume* fenvLV = nullptr;
  SensorHitsCollection* fHitsCollection = nullptr; 
};


#endif
