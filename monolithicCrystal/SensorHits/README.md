# Optical photons:

In this example:

1. Geometry is a crystal wrapped by Teflon and readout by an array of SiPMs. The parameters for DetectorConstruction are defined in
DetectorConstructionMessenger.

2. Optical photons are generated in random points within the crystal, with the correct sampling of energy and random
directions (PrimaryGeneratorAction). The parameters (number of photons in each event, fano factor) are defined in PrimaryGeneratorMessenger.

3. Data is written in csv files. Three files are written:

sensor_pos.csv: contains the position of the 64 SiPMs used to read out the crystal.
sensor_data.csv: contains a table:  event, sensor_id, time, charge
integrated_sensor_data.csv: contains a table: event, sensor_id, amplitude










