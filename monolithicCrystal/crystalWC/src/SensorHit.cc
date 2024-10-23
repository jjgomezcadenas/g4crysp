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


void SensorHit::Fill(G4double time, G4int counts)
{
  auto fBinSize = GlobalPars::Instance()->fTimeBinning;
  auto time_bin = floor(time/fBinSize);
  fNphotons[time_bin] += counts;
  HistogramManager::Instance()->FillHistogram("TimeBin", time_bin);

  //G4cout << " in SensorHit::Fill --> time = " << time/ns << " fBinSize =" << fBinSize/ns << " time_bin = " << time_bin << G4endl;  
}

