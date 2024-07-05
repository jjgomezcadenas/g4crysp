# Crystal with SiPMs

Example of a Geant4 application. It can run in MT mode and outputs data using Apache-Arrow parquet format. 

## Geometry

The geometry is defined in *DetectorContruction*. It includes:
1. A crystal filled with CsI, and wrapped with Teflon. 
2. A SiPM array, with 8 x 8 SiPMs. The SiPMs themselves are modeled as an active area (made of Silicon), on top of a plastic support, and coupled to the crystal through optical Epoxy.

## Optical photons
1. Optical photons of fixed energy and momentum (along the z axis) are shot in the crystal (random positions in xy) and recorded in the SiPMs, defined as sensitive volume.

## Use apache-arrow/parquet to record and save the data.

There is a python application that reads the parquet file using pandas.









