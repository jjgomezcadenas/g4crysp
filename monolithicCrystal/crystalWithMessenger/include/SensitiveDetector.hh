#ifndef SensitiveDetector_hh
#define SensitiveDetector_hh

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include <vector>

struct PhotonHit {
    G4ThreeVector position;
    G4double energy;
};

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();

    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);

    const std::vector<PhotonHit>& GetHits() const;

private:
    std::vector<PhotonHit> hits;
};

#endif
