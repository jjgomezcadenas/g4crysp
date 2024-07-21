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

  // SensorHit();
  // SensorHit(G4int ID, const G4ThreeVector& position, G4double bin_size);

  // SensorHit(const SensorHit&);


  SensorHit& operator=(const SensorHit&) = default;
  G4bool operator==(const SensorHit&) const;
  
  //const SensorHit& operator=(const SensorHit&);
  /// Equality operator

  //G4int operator==(const SensorHit&) const;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  //void SetBinSize(G4double bin_size);
  void Fill(G4double time, G4int counts=1);
  const std::map<G4double, G4int>& GetPhotonHistogram() const {return fNphotons;}
  
  G4int fSensorID = -1;          ///< Detector ID number
  //G4double fBinSize = 0;       ///< Size of time bin
  G4ThreeVector fSensorPos; ///< Detector position

  /// Sparse histogram with number of photons detected per time bin
  std::map<G4double, G4int> fNphotons;
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

//inline void* SensorHit::operator new(size_t)
//{ return ((void*) SensorHitAllocator.MallocSingle()); }

inline void SensorHit::operator delete(void* hit)
{ SensorHitAllocator->FreeSingle((SensorHit*) hit); }

//inline void SensorHit::operator delete(void* hit)
//{ SensorHitAllocator.FreeSingle((SensorHit*) hit); }


#endif
