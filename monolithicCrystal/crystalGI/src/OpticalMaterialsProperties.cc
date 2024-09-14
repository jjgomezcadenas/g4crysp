// ----------------------------------------------------------------------------
// nexus | OpticalMaterialProperties.cc
//
// Optical properties of relevant materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalMaterialsProperties.hh"
#include "SellmeierEquation.hh"

#include <G4MaterialPropertiesTable.hh>

#include <assert.h>


namespace copt {

  /// SiPM
  G4MaterialPropertiesTable* SipmHamamatsu()
  {

    // SiPM efficiency set using the official Hamamatsu specs.

    const G4int entries = 15;
   
    G4double energies[entries]     = {
          optPhotMinE_,
          h_Planck * c_light / (809.722 * nm), h_Planck * c_light / (675.000 * nm),
          h_Planck * c_light / (587.500 * nm), h_Planck * c_light / (494.444 * nm),
          h_Planck * c_light / (455.556 * nm), h_Planck * c_light / (422.222 * nm),
          h_Planck * c_light / (395.833 * nm), h_Planck * c_light / (366.667 * nm),
          h_Planck * c_light / (344.444 * nm), h_Planck * c_light / (311.111 * nm),
          h_Planck * c_light / (293.056 * nm), h_Planck * c_light / (288.889 * nm),
          h_Planck * c_light / (279.167 * nm),
          optPhotMaxE_
		};
    
    G4double reflectivity[entries] = {
      0,
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0,
      0
    };

    G4double efficiency[entries]   = {
      0,
      0.0087, 0.192,
      0.311, 0.467,
      0.511, 0.502,
      0.469, 0.406,
      0.393, 0.324,
      0.180, 0.048,
      0.02,
      0
    };

   G4double um2 = micrometer*micrometer;

    G4double constTerm  = 1.4954;
    G4double squareTerm = 0.008022 * um2;
    G4double quadTerm   = 0.;

    const G4int ri_entries = 100;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }
    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double wl = h_Planck * c_light / ri_energy[i];
      rIndex.push_back(constTerm + squareTerm/(wl*wl) + quadTerm/pow(wl,4));
    }
    sipm_mt->AddProperty("RINDEX", ri_energy, rIndex);
    
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);
    return sipm_mt;
  }


  /// Vacuum ///
  G4MaterialPropertiesTable* Vacuum()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> photEnergy = {optPhotMinE_, optPhotMaxE_};

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex = {1., 1.};
    mpt->AddProperty("RINDEX", photEnergy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> absLength = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", photEnergy, absLength);

    return mpt;
  }


  /// Fused Silica ///
  G4MaterialPropertiesTable* FusedSilica()
  {
    // Optical properties of Suprasil 311/312(c) synthetic fused silica.
    // Obtained from http://heraeus-quarzglas.com

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    // The range is chosen to be up to ~10.7 eV because Sellmeier's equation
    // for fused silica is valid only in that range

    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    // The following values for the refractive index have been calculated
    // using Sellmeier's equation:
    //    n^2 - 1 = B_1 * \lambda^2 / (\lambda^2 - C_1) +
    //            + B_2 * \lambda^2 / (\lambda^2 - C_2) +
    //            + B_3 * \lambda^2 / (\lambda^2 - C_3),
    // with wavelength \lambda in micrometers and
    //    B_1 = 4.73E-1, B_2 = 6.31E-1, B_3 = 9.06E-1
    //    C_1 = 1.30E-2, C_2 = 4.13E-3, C_3 = 9.88E+1.

    G4double B_1 = 4.73e-1;
    G4double B_2 = 6.31e-1;
    G4double B_3 = 9.06e-1;
    G4double C_1 = 1.30e-2;
    G4double C_2 = 4.13e-3;
    G4double C_3 = 9.88e+1;

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
      G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
        + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
        + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
      rIndex.push_back(sqrt(n2));
    }
    
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,  6.46499 * eV,
      6.54000 * eV,  6.59490 * eV,  6.64000 * eV,  6.72714 * eV,
      6.73828 * eV,  6.75000 * eV,  6.82104 * eV,  6.86000 * eV,
      6.88000 * eV,  6.89000 * eV,  7.00000 * eV,  7.01000 * eV,
      7.01797 * eV,  7.05000 * eV,  7.08000 * eV,  7.08482 * eV,
      7.30000 * eV,  7.36000 * eV,  7.40000 * eV,  7.48000 * eV,
      7.52000 * eV,  7.58000 * eV,  7.67440 * eV,  7.76000 * eV,
      7.89000 * eV,  7.93000 * eV,  8.00000 * eV,
      optPhotMaxE_
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      200.0 * cm,   200.0 * cm,  90.0 * cm,  45.0 * cm,
      45.0 * cm,    30.0 * cm,  24.0 * cm,  21.0 * cm,
      20.0 * cm,    19.0 * cm,  16.0 * cm,  14.0 * cm,
      13.0 * cm,     8.5 * cm,   8.0 * cm,   6.0 * cm,
       1.5 * cm,     1.2 * cm,   1.0 * cm,   .65 * cm,
        .4 * cm,     .37 * cm,   .32 * cm,   .28 * cm,
        .22 * cm,    .215 * cm,  .00005*cm,
      .00005* cm
    };

    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }


  /// Glass Epoxy ///
  G4MaterialPropertiesTable* GlassEpoxy()
  {
    // Optical properties of Optorez 1330 glass epoxy.
    // Obtained from http://refractiveindex.info and
    // https://www.zeonex.com/Optics.aspx.html#glass-like

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    // The range is chosen to be up to ~10.7 eV because Sellmeier's equation
    // for fused silica is valid only in that range
    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
      G4double n2 = 2.291142 - 3.311944E-2*pow(lambda,2) - 1.630099E-2*pow(lambda,-2) +
                    7.265983E-3*pow(lambda,-4) - 6.806145E-4*pow(lambda,-6) +
                    1.960732E-5*pow(lambda,-8);
      rIndex.push_back(sqrt(n2));
      // G4cout << "* GlassEpoxy rIndex:  " << std::setw(5)
      //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_, 2.000 * eV,
      2.132 * eV,   2.735 * eV,  2.908 * eV,  3.119 * eV,
      3.320 * eV,   3.476 * eV,  3.588 * eV,  3.749 * eV,
      3.869 * eV,   3.973 * eV,  4.120 * eV,  4.224 * eV,
      4.320 * eV,   4.420 * eV,  5.018 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      326.00 * mm,  117.68 * mm,  85.89 * mm,  50.93 * mm,
      31.25 * mm,   17.19 * mm,  10.46 * mm,   5.26 * mm,
        3.77 * mm,    2.69 * mm,   1.94 * mm,   1.33 * mm,
        0.73 * mm,    0.32 * mm,   0.10 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Sapphire ///
  G4MaterialPropertiesTable* Sapphire()
  {
    // Input data: Sellmeier equation coeficients extracted from:
    // https://refractiveindex.info/?shelf=3d&book=crystals&page=sapphire
    //C[i] coeficients at line 362 are squared.

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    G4double um2 = micrometer*micrometer;
    G4double B[3] = {1.4313493, 0.65054713, 5.3414021};
    G4double C[3] = {0.0052799261 * um2, 0.0142382647 * um2, 325.017834 * um2};
    copt::SellmeierEquation seq(B, C);

    const G4int ri_entries = 100;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      rIndex.push_back(seq.RefractiveIndex(h_Planck*c_light/ri_energy[i]));
      //G4cout << "* Sapphire rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_, 0.900 * eV,
      1.000 * eV,   1.296 * eV,  1.683 * eV,  2.075 * eV,
      2.585 * eV,   3.088 * eV,  3.709 * eV,  4.385 * eV,
      4.972 * eV,   5.608 * eV,  6.066 * eV,  6.426 * eV,
      6.806 * eV,   7.135 * eV,  7.401 * eV,  7.637 * eV,
      7.880 * eV,   8.217 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      3455.0  * mm,  3455.0  * mm,  3455.0  * mm,  3455.0  * mm,
      3455.0  * mm,  3140.98 * mm,  2283.30 * mm,  1742.11 * mm,
      437.06 * mm,   219.24 * mm,  117.773 * mm,   80.560 * mm,
      48.071 * mm,   28.805 * mm,   17.880 * mm,   11.567 * mm,
        7.718 * mm,    4.995 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Optical Coupler ///
  G4MaterialPropertiesTable* OptCoupler()
  {
    // gel NyoGel OCK-451
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    G4double um2 = micrometer*micrometer;

    G4double constTerm  = 1.4954;
    G4double squareTerm = 0.008022 * um2;
    G4double quadTerm   = 0.;

    const G4int ri_entries = 100;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double wl = h_Planck * c_light / ri_energy[i];
      rIndex.push_back(constTerm + squareTerm/(wl*wl) + quadTerm/pow(wl,4));
      //G4cout << "* OptCoupler rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    // Values estimated from printed plot (to be improved).
    std::vector<G4double> abs_energy = {
      optPhotMinE_,  1.70 * eV,
      1.77 * eV,     2.07 * eV,  2.48 * eV,  2.76 * eV,
      2.92 * eV,     3.10 * eV,  3.31 * eV,  3.54 * eV,
      3.81 * eV,     4.13 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      1332.8 * mm,  1332.8 * mm,  1332.8 * mm,  666.17 * mm,
      499.5 * mm,   399.5 * mm,   199.5 * mm,  132.83 * mm,
        99.5 * mm,     4.5 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }


  G4MaterialPropertiesTable* LYSO()
  {

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> energy = {
        optPhotMinE_,
        h_Planck * c_light / (600 * nm),
        h_Planck * c_light / (504 * nm),
        h_Planck * c_light / (477 * nm),
        h_Planck * c_light / (452 * nm),
        h_Planck * c_light / (417 * nm),
        h_Planck * c_light / (410 * nm),
        h_Planck * c_light / (402 * nm),
        h_Planck * c_light / (389 * nm),
        h_Planck * c_light / (380 * nm),
        optPhotMaxE_};

    std::vector<G4double> rIndex = {
        1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82
    };

    mpt->AddProperty("RINDEX", energy, rIndex);

    std::vector<G4double> absorption = {
      0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m
    };

    mpt->AddProperty("ABSLENGTH", energy, absorption);

    std::vector<G4double> emission_intensity = {
      0.0,  0.007, 0.063, 0.169, 0.457, 0.960, 0.964, 0.950, 0.377, 0., 0.
    };

    mpt->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission_intensity);
    mpt->AddConstProperty("SCINTILLATIONYIELD", 30000. / MeV);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", 1. );
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 40 * ns);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);

    return mpt;
  }

  G4MaterialPropertiesTable* BGO()
  {

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> energy = {
        optPhotMinE_,
        h_Planck * c_light / (713 * nm),
        h_Planck * c_light / (638 * nm),
        h_Planck * c_light / (584 * nm),
        h_Planck * c_light / (529 * nm),
        h_Planck * c_light / (476 * nm),
        h_Planck * c_light / (443 * nm),
        h_Planck * c_light / (407 * nm),
        h_Planck * c_light / (367 * nm),
        optPhotMaxE_};

    std::vector<G4double> rIndex = {
        2.05, 2.08, 2.10, 2.11, 2.13, 2.16, 2.18, 2.20, 2.260, 2.41
    };

    mpt->AddProperty("RINDEX", energy, rIndex);

    std::vector<G4double> absorption = {
      0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m
    };

    mpt->AddProperty("ABSLENGTH", energy, absorption);

    std::vector<G4double> emission_intensity = {
      0.0,  0.01, 0.03, 0.056, 0.103, 0.126, 0.096, 0.040, 0.007, 0.
    };

    mpt->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission_intensity);

    mpt->AddConstProperty("SCINTILLATIONYIELD", 9000. / MeV);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", 1. );
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 300 * ns);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);

    return mpt;
  }


  G4MaterialPropertiesTable* CsITl()
  {

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();


    std::vector<G4double> energy = {
        optPhotMinE_,
        h_Planck * c_light / (722 * nm),
        h_Planck * c_light / (678 * nm),
        h_Planck * c_light / (611 * nm),
        h_Planck * c_light / (554 * nm),
        h_Planck * c_light / (523 * nm),
        h_Planck * c_light / (493 * nm),
        h_Planck * c_light / (437 * nm),
        h_Planck * c_light / (351 * nm),
        optPhotMaxE_};

    std::vector<G4double> rIndex = {
        1.766, 1.766, 1.794, 1.806, 1.845, 1.867, 1.902, 1.955, 2.043, 2.043
    };

    mpt->AddProperty("RINDEX", energy, rIndex);

    std::vector<G4double> absorption = {
      0.45 * m, 0.42 * m, 0.4 * m, 0.37 * m, 0.33 * m, 0.31 * m, 0.30 * m, 0.26 * m, 0.12 * m, 0.02 * m
    };

    mpt->AddProperty("ABSLENGTH", energy, absorption);

    std::vector<G4double> emission_intensity = {
      0.0,  0.12, 0.29, 0.70, 0.98, 0.87, 0.56, 0.19, 0.01, 0.
    };

    mpt->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission_intensity);

    G4double csi_time  =     1.2 * us;
    
    mpt->AddConstProperty("SCINTILLATIONYIELD", 54000. / MeV);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", 1. );
    // mpt->AddConstProperty("SCINTILLATIONYIELD1", 0. );
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   csi_time);
    // mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   csi_time_slow);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);

    return mpt;
  }

  G4MaterialPropertiesTable* CsI()
  {

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();


    std::vector<G4double> energy = {
        optPhotMinE_,
        h_Planck * c_light / (460 * nm),
        h_Planck * c_light / (400 * nm),
        h_Planck * c_light / (380 * nm),
        h_Planck * c_light / (340 * nm),
        h_Planck * c_light / (320 * nm),
        h_Planck * c_light / (300 * nm),
        h_Planck * c_light / (280 * nm),
        h_Planck * c_light / (260 * nm),
        optPhotMaxE_};

    std::vector<G4double> rIndex = {
        1.766, 1.766, 1.794, 1.806, 1.845, 1.867, 1.902, 1.955, 2.043, 2.043
    };

    mpt->AddProperty("RINDEX", energy, rIndex);

    std::vector<G4double> absorption = {
      0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m, 0.5 * m
    };

    mpt->AddProperty("ABSLENGTH", energy, absorption);

    std::vector<G4double> emission_intensity_warm = {
      0.0,  0.13, 0.17, 0.55, 0.98, 0.97, 0.83, 0.44, 0.16, 0.
    };

    std::vector<G4double> emission_intensity_cold = {
      0.0,  0.04, 0.10, 0.29, 0.67, 0.88, 0.29, 0.10, 0.02, 0
    };

    mpt->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission_intensity_cold);
    // mpt->AddProperty("SCINTILLATIONCOMPONENT2", energy, emission_intensity);
    // mpt->AddProperty("ELSPECTRUM"            , energy, emission_intensity, 1);
    G4double csi_time_fast  =     0.81 * us;
    // G4double csi_time_slow  =    28 * ns;
    // CONST PROPERTIES https://www.osti.gov/servlets/purl/1514707
    mpt->AddConstProperty("SCINTILLATIONYIELD", 100000 / MeV);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", 1. );
    // mpt->AddConstProperty("SCINTILLATIONYIELD1", 0. );
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   csi_time_fast);
    // mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   csi_time_slow);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);

    return mpt;
  }


  /// ESR (== Vikuiti) ///
  G4MaterialPropertiesTable* ESR()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFLECTIVITY
    std::vector<G4double> ENERGIES = {
      optPhotMinE_,  2.8 * eV,  3.5 * eV,  4. * eV,
      6. * eV,       7.2 * eV,  optPhotMaxE_
    };
    std::vector<G4double> REFLECTIVITY = {
      .99,  .99,  .99,  .99,
      .99,  .99,  .99
    };
    // std::vector<G4double> REFLECTIVITY = {
    //   1., 1., 1., 1.,
    //   1., 1., 1.
    // };
    mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY);

    // REFLEXION BEHAVIOR
    std::vector<G4double> ENERGIES_2    = {optPhotMinE_, optPhotMaxE_};
    // Specular reflection about the normal to a microfacet.
    // Such a vector is chosen according to a gaussian distribution with
    // sigma = SigmaAlhpa (in rad) and centered in the average normal.
    std::vector<G4double> specularlobe  = {0., 0.};
    // specular reflection about the average normal
    std::vector<G4double> specularspike = {0., 0.};
    // 180 degrees reflection.
    std::vector<G4double> backscatter   = {0., 0.};
    // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

    mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES_2, specularlobe);
    mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2, specularspike);
    mpt->AddProperty("BACKSCATTERCONSTANT",  ENERGIES_2, backscatter);

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex = {1.41, 1.41};
    mpt->AddProperty("RINDEX", ENERGIES_2, rIndex);

    return mpt;
  }


  /// PTFE (== TEFLON) ///
  G4MaterialPropertiesTable* PTFE()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFLECTIVITY
    std::vector<G4double> ENERGIES = {
      optPhotMinE_,  2.8 * eV,  3.5 * eV,  4. * eV,
      6. * eV,       7.2 * eV,  optPhotMaxE_
    };
    std::vector<G4double> REFLECTIVITY = {
      .9875,  .9875,  .9875,  .9875,
      .9875,  .9875,  .9875
    };
    // std::vector<G4double> REFLECTIVITY = {
    //   1., 1., 1., 1.,
    //   1., 1., 1.
    // };
    mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY);

    // REFLEXION BEHAVIOR
    std::vector<G4double> ENERGIES_2    = {optPhotMinE_, optPhotMaxE_};
    // Specular reflection about the normal to a microfacet.
    // Such a vector is chosen according to a gaussian distribution with
    // sigma = SigmaAlhpa (in rad) and centered in the average normal.
    std::vector<G4double> specularlobe  = {0., 0.};
    // specular reflection about the average normal
    std::vector<G4double> specularspike = {0., 0.};
    // 180 degrees reflection.
    std::vector<G4double> backscatter   = {0., 0.};
    // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

    mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES_2, specularlobe);
    mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2, specularspike);
    mpt->AddProperty("BACKSCATTERCONSTANT",  ENERGIES_2, backscatter);

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex = {1.5, 1.5};
    mpt->AddProperty("RINDEX", ENERGIES_2, rIndex);

    return mpt;
  }

  /// PolishedAl ///
  G4MaterialPropertiesTable* PolishedAl()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> ENERGIES = {
       h_Planck * c_light / (2456.42541 * nm), h_Planck * c_light / (2396.60266 * nm),
       h_Planck * c_light / (2276.95716 * nm), h_Planck * c_light / (2159.52733 * nm),
       h_Planck * c_light / (2037.66617 * nm), h_Planck * c_light / (1918.02068 * nm),
       h_Planck * c_light / (1798.37518 * nm), h_Planck * c_light / (1676.51403 * nm),
       h_Planck * c_light / (1559.08419 * nm), h_Planck * c_light / (1437.22304 * nm),
       h_Planck * c_light / (1319.79321 * nm), h_Planck * c_light / (1197.93205 * nm),
       h_Planck * c_light / (1078.28656 * nm), h_Planck * c_light / (956.42541 * nm),
       h_Planck * c_light / (838.99557 * nm), h_Planck * c_light / (717.13442 * nm),
       h_Planck * c_light / (597.48892 * nm), h_Planck * c_light / (477.84343 * nm),
       h_Planck * c_light / (418.02068 * nm), h_Planck * c_light / (358.19793 * nm),
       h_Planck * c_light / (293.94387 * nm)
    };
    std::vector<G4double> REFLECTIVITY = {
      .99088, .99082, .98925, .98623, .98611,
      .98163, .98006, .97849, .97401, .97098,
      .96941, .96784, .96481, .96033, .96167,
      .96301, .96289, .96278, .96126, .95830,
      .94224
    };
    // DOI:10.4236/ampc.2015.511046
    mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY);

    // REFLEXION BEHAVIOR
    std::vector<G4double> ENERGIES_2    = {optPhotMinE_, optPhotMaxE_};
    // Specular reflection about the normal to a microfacet.
    // Such a vector is chosen according to a gaussian distribution with
    // sigma = SigmaAlhpa (in rad) and centered in the average normal.
    std::vector<G4double> specularlobe  = {0., 0.};
    // specular reflection about the average normal
    std::vector<G4double> specularspike = {0., 0.};
    // 180 degrees reflection.
    std::vector<G4double> backscatter   = {0., 0.};
    // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

    mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES_2, specularlobe);
    mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2, specularspike);
    mpt->AddProperty("BACKSCATTERCONSTANT",  ENERGIES_2, backscatter);

    // REFRACTIVE INDEX
    std::vector<G4double> ENERGIES_3    = {
      0.005 * eV, 0.19581 * eV, 0.43227 * eV,
      0.84211 * eV, 1.2254 * eV, 1.4477 * eV,
      1.7831 * eV, 2.8203 * eV, 3.6216 * eV,
      5.0548 * eV, 7.0554 * eV, 9.4450 * eV,
      12.645 * eV, 14.939 * eV, 16.238 * eV,
      18.4 * eV, 20. * eV
    };
    std::vector<G4double> rIndex = {
      473.49, 12.843, 3.8841, 1.437, 1.4821, 2.4465, 1.6203, 0.58336, 0.32634, 0.1686,
      0.089866, 0.051461, 0.039232, 0.11588, 0.39013, 0.58276, 0.66415
    };
    // from https://refractiveindex.info/?shelf=3d&book=metals&page=aluminium
    mpt->AddProperty("RINDEX", ENERGIES_3, rIndex);

    return mpt;
  }



  /// EJ-280 ///
  G4MaterialPropertiesTable* EJ280()
  {
    // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
    // and data sheets from the provider.
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,
      h_Planck * c_light / (609. * nm),  h_Planck * c_light / (589.26 * nm),
      h_Planck * c_light / (550. * nm),  h_Planck * c_light / (530.   * nm),
      h_Planck * c_light / (500. * nm),  h_Planck * c_light / (490.   * nm),
      h_Planck * c_light / (481. * nm),  h_Planck * c_light / (460.   * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (425.   * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780,  1.5800,   // 609 , 589.26 nm
      1.5845,  1.5870,   // 550 , 530 nm
      1.5913,  1.5929,   // 500 , 490 nm
      1.5945,  1.5986,   // 481 , 460 nm
      1.6050,  1.6080,   // 435 , 425 nm
      1.608
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.0 * m,       3.0 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  h_Planck * c_light / (350. * nm),
      h_Planck * c_light / (345. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,        noAbsLength_,
      (1. / 0.0080) * cm,  (1. / 0.0165) * cm,    // 495 , 490 nm
      (1. / 0.0325) * cm,  (1. / 0.0815) * cm,    // 485 , 480 nm
      (1. / 0.2940) * cm,  (1. / 0.9640) * cm,    // 475 , 470 nm
      (1. / 2.8600) * cm,  (1. / 6.3900) * cm,    // 465 , 460 nm
      (1. / 9.9700) * cm,  (1. / 11.0645)* cm,    // 455 , 450 nm
      (1. / 10.198) * cm,  (1. / 9.4465) * cm,    // 445 , 440 nm
      (1. / 10.2145)* cm,  (1. / 12.240) * cm,    // 435 , 430 nm
      (1. / 12.519) * cm,  (1. / 10.867) * cm,    // 425 , 420 nm
      (1. / 9.0710) * cm,  (1. / 8.0895) * cm,    // 415 , 410 nm
      (1. / 7.6650) * cm,  (1. / 6.7170) * cm,    // 405 , 400 nm
      (1. / 5.2460) * cm,  (1. / 4.1185) * cm,    // 395 , 390 nm
      (1. / 3.3175) * cm,  (1. / 2.6800) * cm,    // 385 , 380 nm
      (1. / 1.9610) * cm,  (1. / 1.4220) * cm,    // 375 , 370 nm
      (1. / 1.0295) * cm,  (1. / 0.7680) * cm,    // 365 , 360 nm
      (1. / 0.6865) * cm,  (1. / 0.5885) * cm,    // 355 , 350 nm
      noAbsLength_,        noAbsLength_
    };
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ280 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,                      h_Planck * c_light / (610. * nm),
      h_Planck * c_light / (605. * nm),  h_Planck * c_light / (600. * nm),
      h_Planck * c_light / (595. * nm),  h_Planck * c_light / (590. * nm),
      h_Planck * c_light / (585. * nm),  h_Planck * c_light / (580. * nm),
      h_Planck * c_light / (575. * nm),  h_Planck * c_light / (570. * nm),
      h_Planck * c_light / (565. * nm),  h_Planck * c_light / (560. * nm),
      h_Planck * c_light / (555. * nm),  h_Planck * c_light / (550. * nm),
      h_Planck * c_light / (545. * nm),  h_Planck * c_light / (540. * nm),
      h_Planck * c_light / (535. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,    0.000,   //     , 610 nm
      0.003,    0.006,   // 605 , 600 nm
      0.007,    0.009,   // 595 , 590 nm
      0.014,    0.017,   // 585 , 580 nm
      0.024,    0.033,   // 575 , 570 nm
      0.042,    0.051,   // 565 , 560 nm
      0.063,    0.081,   // 555 , 550 nm
      0.112,    0.157,   // 545 , 540 nm
      0.211,    0.274,   // 535 , 530 nm
      0.329,    0.341,   // 525 , 520 nm
      0.325,    0.346,   // 515 , 510 nm
      0.433,    0.578,   // 505 , 500 nm
      0.792,    1.000,   // 495 , 490 nm
      0.966,    0.718,   // 485 , 480 nm
      0.604,    0.681,   // 475 , 470 nm
      0.708,    0.525,   // 465 , 460 nm
      0.242,    0.046,   // 455 , 450 nm
      0.012,    0.003,   // 445 , 440 nm
      0.000,    0.000    // 435 ,     nm
    };
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.86);

    return mpt;
  }



  /// EJ-286 ///
  G4MaterialPropertiesTable* EJ286()
  {
    // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
    // and data sheets from the provider.
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,
      h_Planck * c_light / (609. * nm),    h_Planck * c_light / (589.26 * nm),
      h_Planck * c_light / (550. * nm),    h_Planck * c_light / (530.   * nm),
      h_Planck * c_light / (500. * nm),    h_Planck * c_light / (490.   * nm),
      h_Planck * c_light / (481. * nm),    h_Planck * c_light / (460.   * nm),
      h_Planck * c_light / (435. * nm),    h_Planck * c_light / (425.   * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780,  1.5800,   // 609 , 589.26 nm
      1.5845,  1.5870,   // 550 , 530 nm
      1.5913,  1.5929,   // 500 , 490 nm
      1.5945,  1.5986,   // 481 , 460 nm
      1.6050,  1.6080,   // 435 , 425 nm
      1.6080
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.0 * m,       3.0 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (445. * nm),
                                        h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  h_Planck * c_light / (350. * nm),
      h_Planck * c_light / (345. * nm),  h_Planck * c_light / (340. * nm),
      h_Planck * c_light / (335. * nm),  h_Planck * c_light / (330. * nm),
      h_Planck * c_light / (325. * nm),  h_Planck * c_light / (320. * nm),
      h_Planck * c_light / (315. * nm),  h_Planck * c_light / (310. * nm),
      h_Planck * c_light / (305. * nm),  h_Planck * c_light / (300. * nm),
      h_Planck * c_light / (295. * nm),  h_Planck * c_light / (290. * nm),
      h_Planck * c_light / (285. * nm),  h_Planck * c_light / (280. * nm),
      h_Planck * c_light / (275. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,         noAbsLength_,
                            (1. / 0.00007) * cm,    //       440 nm
      (1. /  0.0003) * cm,  (1. / 0.00104) * cm,    // 435 , 430 nm
      (1. / 0.00223) * cm,  (1. / 0.00408) * cm,    // 425 , 420 nm
      (1. /  0.0104) * cm,  (1. / 0.18544) * cm,    // 415 , 410 nm
      (1. /  1.4094) * cm,  (1. /  3.7088) * cm,    // 405 , 400 nm
      (1. /  7.4176) * cm,  (1. / 11.8682) * cm,    // 395 , 390 nm
      (1. / 16.6155) * cm,  (1. / 22.2529) * cm,    // 385 , 380 nm
      (1. / 27.8162) * cm,  (1. / 33.3794) * cm,    // 375 , 370 nm
      (1. / 37.8671) * cm,  (1. / 40.4262) * cm,    // 365 , 360 nm
      (1. / 41.5388) * cm,  (1. / 41.1679) * cm,    // 355 , 350 nm
      (1. / 38.9426) * cm,  (1. / 35.0113) * cm,    // 345 , 340 nm
      (1. / 31.1541) * cm,  (1. / 27.4453) * cm,    // 335 , 330 nm
      (1. / 23.4398) * cm,  (1. / 20.0276) * cm,    // 325 , 320 nm
      (1. / 16.3188) * cm,  (1. / 13.3518) * cm,    // 315 , 310 nm
      (1. / 10.5331) * cm,  (1. /  8.1594) * cm,    // 305 , 300 nm
      (1. /  6.1196) * cm,  (1. /  4.6731) * cm,    // 295 , 290 nm
      (1. /  3.6346) * cm,  (1. /  3.0412) * cm,    // 285 , 280 nm
      noAbsLength_,         noAbsLength_
    };
    // XXX We are assuming that EJ286 doesn't absorb wave lengths shorter than 280 nm
    // although the spectrum continues ...
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ286 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      h_Planck * c_light / (535. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.0000,
      0.0000,  0.0089,   // 535 , 530 nm
      0.0100,  0.0181,   // 525 , 520 nm
      0.0210,  0.0270,   // 515 , 510 nm
      0.0380,  0.0496,   // 505 , 500 nm
      0.0600,  0.0721,   // 495 , 490 nm
      0.0900,  0.1125,   // 485 , 480 nm
      0.1500,  0.1848,   // 475 , 470 nm
      0.2100,  0.2388,   // 465 , 460 nm
      0.2800,  0.3289,   // 455 , 450 nm
      0.4000,  0.4956,   // 445 , 440 nm
      0.5700,  0.6230,   // 435 , 430 nm
      0.6450,  0.6667,   // 425 , 420 nm
      0.8000,  0.9800,   // 415 , 410 nm
      0.9900,  0.8559,   // 405 , 400 nm
      0.7118,  0.7400,   // 395 , 390 nm
      0.8000,  0.6702,   // 385 , 380 nm
      0.3800,  0.1082,   // 375 , 370 nm
      0.0400,  0.0089,   // 365 , 360 nm
      0.0000,  0.0000    // 355 ,     nm
    };
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 1.2 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.92);

    return mpt;
  }



  /// Y-11 ///
  G4MaterialPropertiesTable* Y11()
  {
    // http://kuraraypsf.jp/psf/index.html
    // http://kuraraypsf.jp/psf/ws.html
    // Excel provided by kuraray with Tabulated WLS absorption lengths
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,  optPhotMaxE_
    };
    std::vector<G4double> rIndex = {
      1.59,  1.59
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.5 * m,       3.5 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (475. * nm),
      h_Planck * c_light / (454. * nm),  h_Planck * c_light / (443. * nm),
      h_Planck * c_light / (430. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (359. * nm),
      h_Planck * c_light / (350. * nm),  h_Planck * c_light / (345. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> WLS_absLength = {
      noAbsLength_,  noAbsLength_,    //     , 490 nm
      44.2  * mm,    5.39 * mm,       // 485 , 475 nm
      0.395 * mm,    0.462 * mm,      // 454 , 443 nm
      0.354 * mm,    0.571 * mm,      // 430 , 410 nm
      0.612 * mm,    4.51 * mm,       // 405 , 359 nm
      4.81  * mm,    noAbsLength_,    // 350 , 345 nm
      noAbsLength_
    };
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* Y11 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,                      h_Planck * c_light / (580. * nm),
      h_Planck * c_light / (550. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,    0.000,   //     , 580 nm
      0.200,    0.300,   // 550 , 530 nm
      0.400,    0.600,   // 525 , 520 nm
      0.750,    0.750,   // 515 , 510 nm
      0.720,    0.700,   // 505 , 500 nm
      0.680,    0.650,   // 495 , 490 nm
      0.700,    0.900,   // 485 , 480 nm
      1.000,    0.950,   // 475 , 470 nm
      0.500,    0.300,   // 465 , 460 nm
      0.100,    0.050,   // 455 , 450 nm
      0.000,    0.000    // 445 ,     nm
    };
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.87);

    return mpt;
  }


/// B-2 ///
  G4MaterialPropertiesTable* B2()
  {
    // http://kuraraypsf.jp/psf/index.html
    // http://kuraraypsf.jp/psf/ws.html
    // Excel provided by kuraray with Tabulated WLS absorption lengths
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,  optPhotMaxE_
    };
    std::vector<G4double> rIndex = {
      1.59,  1.59
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.5 * m,       3.5 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    // For B2 fibers Kuraray provides absorption spectrum and not
    // absorption length. We assume that the absorption length at the
    // absorption maximum is the same as with the Y11 fiber and
    // scale according to the absorption spectrum. This is not perfect
    // but it was verified to be a good approximation with the Y11 fiber,
    // for which Kuraray did provide the absorption length.

    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      h_Planck * c_light / (418. * nm), h_Planck * c_light / (412. * nm),
      h_Planck * c_light / (405. * nm), h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (394. * nm), h_Planck * c_light / (387. * nm),
      h_Planck * c_light / (384. * nm), h_Planck * c_light / (382. * nm),
      h_Planck * c_light / (378. * nm), h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (361. * nm), h_Planck * c_light / (353. * nm),
      h_Planck * c_light / (345. * nm), h_Planck * c_light / (341. * nm),
      h_Planck * c_light / (336. * nm), h_Planck * c_light / (331. * nm),
      h_Planck * c_light / (316. * nm), h_Planck * c_light / (301. * nm),
      h_Planck * c_light / (280. * nm),
      optPhotMaxE_
    };

    float minAbsLength = 0.395 * mm;

    std::vector<float> B2_absorption {
      -0.01,        // 418
      -0.06, -0.26, // 405, 412
      -0.44, -0.59, // 394, 400
      -0.59, -0.64, // 384, 387
      -0.77, -0.92, // 378, 382
      -1.00, -0.93, // 361, 370
      -0.85, -0.87, // 345, 353
      -0.87, -0.77, // 336, 341
      -0.56, -0.35, // 316, 331
      -0.22, -0.12  // 280, 301
    };

    std::vector<G4double> WLS_absLength {noAbsLength_};

    for (auto &abs_value : B2_absorption)
      WLS_absLength.push_back(- minAbsLength / abs_value);

    WLS_absLength.push_back(noAbsLength_);

    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      h_Planck * c_light / (542 * nm), h_Planck * c_light / (525 * nm),
      h_Planck * c_light / (508 * nm), h_Planck * c_light / (497 * nm),
      h_Planck * c_light / (488 * nm), h_Planck * c_light / (479 * nm),
      h_Planck * c_light / (473 * nm), h_Planck * c_light / (467 * nm),
      h_Planck * c_light / (463 * nm), h_Planck * c_light / (458 * nm),
      h_Planck * c_light / (454 * nm), h_Planck * c_light / (449 * nm),
      h_Planck * c_light / (445 * nm), h_Planck * c_light / (442 * nm),
      h_Planck * c_light / (440 * nm), h_Planck * c_light / (438 * nm),
      h_Planck * c_light / (433 * nm), h_Planck * c_light / (429 * nm),
      h_Planck * c_light / (424 * nm), h_Planck * c_light / (420 * nm),
      h_Planck * c_light / (418 * nm), h_Planck * c_light / (416 * nm),
      h_Planck * c_light / (411 * nm), h_Planck * c_light / (404 * nm),
      h_Planck * c_light / (402 * nm), h_Planck * c_light / (399 * nm),
      h_Planck * c_light / (398 * nm), h_Planck * c_light / (396 * nm),
      h_Planck * c_light / (395 * nm), h_Planck * c_light / (394 * nm),
      h_Planck * c_light / (392 * nm), h_Planck * c_light / (391 * nm),
      h_Planck * c_light / (386 * nm), h_Planck * c_light / (380 * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,
      0.053, 0.070, // 542, 525
      0.109, 0.143, // 508, 497
      0.199, 0.270, // 488, 479
      0.337, 0.423, // 473, 467
      0.497, 0.582, // 463, 458
      0.615, 0.645, // 454, 449
      0.679, 0.750, // 445, 442
      0.801, 0.857, // 440, 438
      0.957, 0.999, // 433, 429
      0.949, 0.906, // 424, 420
      0.855, 0.809, // 418, 416
      0.750, 0.750, // 411, 404
      0.719, 0.671, // 402, 399
      0.590, 0.500, // 398, 396
      0.421, 0.327, // 395, 394
      0.217, 0.138, // 392, 391
      0.065, 0.023, // 386, 380
      0.000
    };

    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.87);

    return mpt;
  }



  /// Pethylene ///
  G4MaterialPropertiesTable* Pethylene()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.49, 1.49};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// FPethylene ///
  G4MaterialPropertiesTable* FPethylene()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.42, 1.42};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// PMMA == PolyMethylmethacrylate ///
  G4MaterialPropertiesTable* PMMA()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.49, 1.49};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,
      2.722 * eV,  3.047 * eV,  3.097 * eV,  3.136 * eV,  3.168 * eV,  3.229 * eV,  3.291 * eV,
      3.323 * eV,  3.345 * eV,  3.363 * eV,  3.397 * eV,  3.451 * eV,  3.511 * eV,  3.590 * eV,
      optPhotMaxE_
    };
    std::vector<G4double> abslength = {
      noAbsLength_,
      noAbsLength_,  4537. * mm,  329.7 * mm,  98.60 * mm,  36.94 * mm,  10.36 * mm,  4.356 * mm,
      2.563 * mm,    1.765 * mm,  1.474 * mm,  1.153 * mm,  0.922 * mm,  0.765 * mm,  0.671 * mm,
      0.671 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, abslength);

    return mpt;
  }



  /// XXX ///
  G4MaterialPropertiesTable* XXX()
  {
    // Playing material properties
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.0    , 1.0};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {10.*cm, 10.*cm};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> WLS_absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy  = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> WLS_emiSpectrum = {1.0, 1.0};
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 1. * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 1.);

    return mpt;
  }
}
