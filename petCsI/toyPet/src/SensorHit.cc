// ----------------------------------------------------------------------------

#include "SensorHit.hh"
#include "G4SystemOfUnits.hh"
#include "GlobalPars.hh"
#include "HistogramManager.hh"

G4ThreadLocal G4Allocator<SensorHit>* SensorHitAllocator = nullptr;

G4bool SensorHit::operator==(const SensorHit& other) const
{
  return (this==&other) ? true : false;
}


