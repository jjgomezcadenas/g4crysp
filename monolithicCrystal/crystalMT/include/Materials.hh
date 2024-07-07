// ----------------------------------------------------------------------------
// nexus | MaterialsList.h
//
// Definition of materials of common use.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MATERIALS_LIST_H
#define MATERIALS_LIST_H

#include <globals.hh>

#include "G4SystemOfUnits.hh"
#include "G4SystemOfUnits.hh"

class G4Material;

namespace cmat {

  // Stainless Steel (grade 304L)
  G4Material* Steel();

  // Stainless steel grade 316Ti
  G4Material* Steel316Ti();

  // Epoxy resin
  G4Material* Epoxy();

  // Kovar (nickel-cobalt ferrous alloy)
  G4Material* Kovar();

  // PEEK (Polyether ether ketone)
  G4Material* PEEK();

  /// Sapphire
  G4Material* Sapphire();

  // Fused silica (synthetic quartz)
  G4Material* FusedSilica();

  // LYSO
  G4Material* LYSO();

  // PS (Polystyrene)
  G4Material* PS();

  // PVT (Polyvinyltoluene)
  G4Material* PVT();

  // KEVLAR (-NH-C6H4-NH-CO-C6H4-CO-)*n
  G4Material* Kevlar();

  /// High density polyethylene
  G4Material* HDPE();

  /// Optical Silicone
  G4Material* OpticalSilicone();

  /// PE1000 (PE UHMW, ultra-high molecular weight polyethylene)
  G4Material* PE1000();

  /// PE500 (PE HD, high density)
  G4Material* PE500();

  // FR4
  G4Material* FR4();

  // WLS EJ-280
  G4Material* EJ280();

  // Kuraray Fiber Y11
  G4Material* Y11();

  // Kuraray Fiber B2
  G4Material* B2();

  // Pethylene (cladding)
  G4Material* Pethylene();

  // FPethylene Fluorated Phethylene (cladding)
  G4Material* FPethylene();

  // PolyMethylmethacrylate (cladding)
  G4Material* PMMA();

  G4Material* CopyMaterial(G4Material*, const G4String&);


} 

#endif
