// ********************************************************************
/// \file apacheDemo.cc
/// \brief Main program of the apache example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"
#include "G4OpticalPhysics.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "GlobalPars.hh"
#include "GlobalMessenger.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "HistogramManager.hh"
#include <iostream>
#include <string>
#include <cstdlib>  // or #include <stdlib.h>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void book_histograms();
void set_output_files();
std::string set_histo_filename();

int main(int argc,char** argv)
{
  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Get globals 
  GlobalMessenger* gMessenger = new GlobalMessenger();

  UImanager->ApplyCommand("/control/execute init_global.mac");

  // set the random seed
  G4cout << "Random seed set to: " << GlobalPars::Instance()->fSeed
         << G4endl;

  G4Random::setTheSeed(GlobalPars::Instance()->fSeed);

  // Set the name of the sensor hit collection (arbitrary) in globals
  GlobalPars::Instance()->gSDCollection = "SensorHitsCollection";

  // open files
  set_output_files();

  // set histofile name
  std::string hfn = set_histo_filename();

  // Book histograms.
  book_histograms();

  //std::exit(1); 

  // Detect interactive mode (if no arguments) and define UI session
  
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }
  
  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  //use G4SteppingVerboseWithUnits
  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

  // Construct the default run manager
  
  auto runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  // Set mandatory initialization classes

  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());

  // Invoke here the PrimaryGeneratorMessenger, so that values can be
  // passed to globals (and thus accepted by the PrimaryGenerator constructor)
  
  PrimaryGeneratorMessenger* pgMessenger = new PrimaryGeneratorMessenger();

  // Physics list
  auto physicsList = new QBBC;
  physicsList->SetVerboseLevel(0);
  auto opticalPhysics = new G4OpticalPhysics();

  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  
  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization);

  // Initialize visualization with the default graphics system
  auto visManager = new G4VisExecutive(argc, argv);
  visManager->Initialize();


  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Write histograms to file
  HistogramManager::Instance()->WriteHistograms(hfn);

  // close files
  GlobalPars::Instance()->sensorDataFile.close();
  GlobalPars::Instance()->iSensorDataFile.close();

  delete visManager;
  delete runManager;
  //delete pgMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void book_histograms()
{
  HistogramManager::Instance()->CreateHistogram("PrimaryParticleSpectrum_nm", 80, 0.0, 800.0);
  HistogramManager::Instance()->CreateHistogram("Yield", 50, 40e+3, 60e+3);
  HistogramManager::Instance()->CreateHistogram("DecayTime", 50, 0.0, 5e+3);
  HistogramManager::Instance()->CreateHistogram("TotalEnergy", 50, 45e+3, 55e+3);
  HistogramManager::Instance()->CreateHistogram("TotalEnergyTime", 50, 40e+3, 60e+3);
}

void set_output_files()
{
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  std::string l4d = std::to_string(seedL4D);

  std::string sdf = "sensor_data_" + l4d + ".csv";
  std::string isdf = "integrated_sensor_data_" + l4d + ".csv";

  GlobalPars::Instance()->sensorDataFile.open(sdf);
  GlobalPars::Instance()->iSensorDataFile.open(isdf);

  GlobalPars::Instance()->sensorDataFile << "event,sensor_id,time,charge\n";
  GlobalPars::Instance()->iSensorDataFile << "event,sensor_id,amplitude\n";

}

std::string set_histo_filename()
{
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  std::string l4d = std::to_string(seedL4D);

  return "histograms_" + l4d + ".txt";

}
  
