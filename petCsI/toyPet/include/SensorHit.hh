// ----------------------------------------------------------------------------

#ifndef PMT_HIT_H
#define PMT_HIT_H

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>
#include "tls.hh"

class SensorHit: public G4VHit
{
public:

  SensorHit() = default;
  SensorHit(const SensorHit&) = default;
  ~SensorHit() override =default;

  SensorHit& operator=(const SensorHit&) = default;
  G4bool operator==(const SensorHit&) const;
  
  inline void* operator new(size_t);
  inline void operator delete(void*);

  
  G4int fSensorID = -1;        // Crystal ID number
  G4double fEnergy;            // energy of gamma
  G4double fTime;            // time of gamma
  //G4ThreeVector fPos;   // Position of crystal
  G4ThreeVector fXYZ;   // Position of impact of gamma

};

using SensorHitsCollection = G4THitsCollection<SensorHit>;
//typedef G4THitsCollection<SensorHit> SensorHitsCollection;
extern  G4ThreadLocal G4Allocator<SensorHit>* SensorHitAllocator;


// INLINE DEFINITIONS ////////////////////////////////////////////////

inline void* SensorHit::operator new(size_t)
{
  if(!SensorHitAllocator)
    SensorHitAllocator = new G4Allocator<SensorHit>;
  return (void *) SensorHitAllocator->MallocSingle();
}


inline void SensorHit::operator delete(void* hit)
{ SensorHitAllocator->FreeSingle((SensorHit*) hit); }


#endif
