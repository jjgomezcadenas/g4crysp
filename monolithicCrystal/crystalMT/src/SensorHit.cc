// ----------------------------------------------------------------------------

#include "SensorHit.hh"
#include "G4SystemOfUnits.hh"

G4ThreadLocal G4Allocator<SensorHit>* SensorHitAllocator = nullptr;
//G4Allocator<SensorHit> SensorHitAllocator;


// SensorHit::SensorHit():
//   G4VHit(),fSensorID(-1.), fBinSize(0.)
// {
// }


// SensorHit::SensorHit(G4int id, const G4ThreeVector& position, G4double bin_size):
//   G4VHit(),fSensorID(id),  fBinSize(bin_size), fSensorPos(position)
// {
// }


// SensorHit::~SensorHit()
// {
// }


// SensorHit::SensorHit(const SensorHit& other): G4VHit()
// {
//   *this = other;
// }


// const SensorHit& SensorHit::operator=(const SensorHit& other)
// {
//   fSensorID    = other.fSensorID;
//   fBinSize     = other.fBinSize;
//   fSensorPos   = other.fSensorPos;
//   fNphotons    = other.fNphotons;

//   return *this;
// }


// G4int SensorHit::operator==(const SensorHit& other) const
// {
//   return (this==&other) ? 1 : 0;
// }

G4bool SensorHit::operator==(const SensorHit& other) const
{
  return (this==&other) ? true : false;
}


void SensorHit::SetBinSize(G4double bin_size)
{
  if (fNphotons.size() == 0) {
    fBinSize = bin_size;
  }
  else {
    G4String msg = "A SensorHit cannot be rebinned once it has been filled.";
    G4Exception("[SensorHit]", "SetBinSize()", JustWarning, msg);
  }
}


void SensorHit::Fill(G4double time, G4int counts)
{
  G4double time_bin = floor(time/fBinSize) * fBinSize;
  fNphotons[time_bin] += counts;

  //G4cout << " in SensorHit::Fill --> time = " << time/ns << " fBinSize =" << fBinSize/ns << " time_bin = " << time_bin << G4endl;  
}

