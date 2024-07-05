# Crystal with SiPMs

This example defines the same geometry and physics than example *CrystalWithSiPMs*, adding a Messenger to define the parameters of DetectorConstruction from external command file

## Geometry

The geometry is defined in *DetectorContruction*. It includes:
1. A crystal filled with CsI/LYSO/BGO/CsITl, and wrapped with Teflon. 
2. A SiPM array, with 8 x 8 SiPMs. The SiPMs themselves are modeled as an active area (made of Silicon), on top of a plastic support, and coupled to the crystal through optical Epoxy.
3. The dimensions of the crystal, teflon and the SiPMs components are defined via commands. 

## Optical photons
1. Optical photons of fixed energy and momentum (along the z axis) are shot in the crystal (random positions in xy) and recorded in the SiPMs, defined as sensitive volume.

## Use apache-arrow/parquet to record and save the data.

There is a python application that reads the parquet file using pandas.









