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
   // Get globals 
  GlobalMessenger* gMessenger = new GlobalMessenger();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  G4UIExecutive* ui = nullptr;

  
  if ( argc == 1 ) { 
    ui = new G4UIExecutive(argc,argv); // call the UIExecutive only if no parameters
    }
  else if (argc ==3){ // need to init files
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
    //UImanager->ApplyCommand("/control/execute init_global.mac");
    }
  else
    {
      G4cout << "Wrong arguments. Call is <./crystalMT init_global.mac run.mac>" << G4endl;
     return 1;
    }
  //UImanager->ApplyCommand("/control/execute init_global.mac");
 
  

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
  
  //G4UIExecutive* ui = nullptr;
  //if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }
  
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
  //auto visManager = new G4VisExecutive(argc, argv);
  auto visManager = new G4VisExecutive;
  visManager->Initialize();


  if ( ! ui ) {
    // batch mode, now init with run.mac params
    G4String command = "/control/execute ";
    G4String fileName = argv[2];
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
  GlobalPars::Instance()->gammaIntFile.close();

  delete visManager;
  delete runManager;
  //delete pgMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void book_histograms()
{
  HistogramManager::Instance()->CreateHistogram("PrimaryParticleSpectrum_nm", 80, 0.0, 800.0);
  HistogramManager::Instance()->CreateHistogram("Yield", 50, 50e+3, 55e+3);
  HistogramManager::Instance()->CreateHistogram("DecayTime", 50, 0.0, 5e+3);
  HistogramManager::Instance()->CreateHistogram("TimeBin", 50, 0.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("TimeBin2", 50, 0.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("Charge", 50, 0.0, 500.0);
  HistogramManager::Instance()->CreateHistogram("TotalEnergy", 50, 0.0, 60e+3);
  HistogramManager::Instance()->CreateHistogram("TotalEnergyTime", 50, 0.0, 60e+3);
}

void set_output_files()
{
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  std::string l4d = std::to_string(seedL4D);

  std::string sdf = GlobalPars::Instance()->fSensorDataFileName + l4d + ".csv";
  std::string isdf = GlobalPars::Instance()->fISensorDataFileName + l4d + ".csv";
  std::string gint = GlobalPars::Instance()->fGammaDataFileName + l4d + ".csv";

  GlobalPars::Instance()->sensorDataFile.open(sdf);
  GlobalPars::Instance()->iSensorDataFile.open(isdf);
  GlobalPars::Instance()->gammaIntFile.open(gint);

  GlobalPars::Instance()->sensorDataFile << "event,sensor_id,time,charge\n";
  GlobalPars::Instance()->iSensorDataFile << "event,sensor_id,amplitude\n";
  GlobalPars::Instance()->gammaIntFile << "event,x, y, z\n";

}

std::string set_histo_filename()
{
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  std::string l4d = std::to_string(seedL4D);

  return GlobalPars::Instance()->fHistoFileName + l4d + ".txt";

}
  
