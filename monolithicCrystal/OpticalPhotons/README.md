# Optical photons:

In this example:

1. Geometry is a crystal wrapped by Teflon and readout by an array of SiPMs. The parameters for DetectorConstruction are defined in
DetectorConstructionMessenger.

2. Optical photons are generated in random points within the crystal, with the correct sampling of energy and random
directions (PrimaryGeneratorAction). The parameters (number of photons in each event, fano factor) are defined in PrimaryGeneratorMessenger.

3. Data is written in csv files.

There is a python application that reads the csv file using pandas.









