// ----------------------------------------------------------------------------
// nexus | OpticalMaterialProperties.h
//
// Optical properties of relevant materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef OPTICAL_MATERIAL_PROPERTIES_H
#define OPTICAL_MATERIAL_PROPERTIES_H

#include <globals.hh>
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

class G4MaterialPropertiesTable;


namespace copt {


  G4MaterialPropertiesTable* Vacuum();

  G4MaterialPropertiesTable* GlassEpoxy();

  G4MaterialPropertiesTable* FusedSilica();

  G4MaterialPropertiesTable* Sapphire();

  G4MaterialPropertiesTable* OptCoupler();

  G4MaterialPropertiesTable* CsITl();

  G4MaterialPropertiesTable* BGO();

  G4MaterialPropertiesTable* LYSO();

  G4MaterialPropertiesTable* CsI();

  G4MaterialPropertiesTable* PTFE();

  G4MaterialPropertiesTable* ESR();

  G4MaterialPropertiesTable* PolishedAl();

  G4MaterialPropertiesTable* EJ280();

  G4MaterialPropertiesTable* EJ286();

  G4MaterialPropertiesTable* Y11();

  G4MaterialPropertiesTable* B2();

  G4MaterialPropertiesTable* Pethylene();

  G4MaterialPropertiesTable* FPethylene();

  G4MaterialPropertiesTable* PMMA();

  G4MaterialPropertiesTable* XXX();


  constexpr G4double optPhotMinE_ =  0.2  * eV;
  constexpr G4double optPhotMaxE_ = 11.5  * eV;
  constexpr G4double noAbsLength_ = 1.e8  * m;

  // Constant that allows to convert nm to eV:
  // nm_to_eV_ / wavelength (nm) = energy (eV)
  constexpr G4double nm_to_eV_ = h_Planck * c_light * 1.e6;


} // end namespace opticalprops

#endif
