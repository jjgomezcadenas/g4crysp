// ********************************************************************

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
#include "DetectorMessenger.hh"
#include "HistogramManager.hh"
#include <iostream>
#include <string>
#include <cstdlib>  // or #include <stdlib.h>
#include <chrono>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 // Start time measurement
auto start = std::chrono::high_resolution_clock::now();
void book_histograms();
void set_output_files(std::string l4d);
std::string set_histo_filename(std::string l4d);
void write_global_pars(std::string l4d);

int main(int argc,char** argv)
{
   // Get globals 
  GlobalMessenger* gMessenger = new GlobalMessenger();
  PrimaryGeneratorMessenger* pgMessenger = new PrimaryGeneratorMessenger();
  DetectorMessenger* dMessenger = new DetectorMessenger();


  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  G4UIExecutive* ui = nullptr;

  
  if ( argc == 2 ) { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
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
      G4cout << "Wrong arguments" << G4endl;
      G4cout << " In interactive mode <./crystalApp init_global.mac>" << G4endl;
      G4cout << " In batch mode.  <./CrystalApp init_global.mac run.mac>" << G4endl;
     return 1;
    }
  
  // set the random seed
  G4cout << "Random seed set to: " << GlobalPars::Instance()->fSeed
         << G4endl;

  G4Random::setTheSeed(GlobalPars::Instance()->fSeed);
  int seedL4D = GlobalPars::Instance()->fSeed % 10000;
  std::string l4d = std::to_string(seedL4D);

  // Set the name of the sensor hit collection (arbitrary) in globals
  GlobalPars::Instance()->gSDCollection = "SensorHitsCollection";

  // open files
  set_output_files(l4d);

  // set histofile name
  std::string hfn = set_histo_filename(l4d);

  // Book histograms.
  book_histograms();

  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

  // Construct the default run manager
  
  auto runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  // Set mandatory initialization classes

  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());
  
  // Physics list
  auto physicsList = new QBBC;
  physicsList->SetVerboseLevel(0);

  if (GlobalPars::Instance()->fGOP == true){
    auto opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
  }

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

  write_global_pars(l4d);
  // close files

  GlobalPars::Instance()->globalsFile.close();

  if (GlobalPars::Instance()->fIDataOnly == false){
  GlobalPars::Instance()->sensorDataFile.close();
  }

  GlobalPars::Instance()->iSensorDataFile.close();
  GlobalPars::Instance()->gammaIntFile.close();

// End time measurement
  auto end = std::chrono::high_resolution_clock::now();

  // Calculate elapsed time
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

  delete visManager;
  delete runManager;
  //delete pgMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void book_histograms()
{
  HistogramManager::Instance()->CreateHistogram("X0", 50, -25.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("Y0", 50, -25.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("Z0", 50, -25.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("STEPL", 50, 0.0, 1000.0);
  HistogramManager::Instance()->CreateHistogram("EDEP", 50, 0.0, 600.0);
  HistogramManager::Instance()->CreateHistogram("DecayTime", 50, 0.0, 5e+3);
  HistogramManager::Instance()->CreateHistogram("TimeBin", 50, 0.0, 25.0);
  HistogramManager::Instance()->CreateHistogram("Charge", 50, 0.0, 500.0);
  HistogramManager::Instance()->CreateHistogram("TotalEnergy", 50, 0.0, 50e+3);
  HistogramManager::Instance()->CreateHistogram("TotalEnergyFirstTimeBin", 50, 0.0, 50e+3);

  // for (const auto& entry : HistogramManager::Instance()->fHistograms) {
  //      G4cout << "# Histogram name: %s\n", entry.first.c_str();
  //   }
}


void set_output_files(std::string l4d)
{
  std::string sdf = GlobalPars::Instance()->fSensorDataFileName + l4d + ".csv";
  std::string isdf = GlobalPars::Instance()->fISensorDataFileName + l4d + ".csv";
  std::string gint = GlobalPars::Instance()->fGammaDataFileName + l4d + ".csv";

  if (GlobalPars::Instance()->fIDataOnly == false){
    GlobalPars::Instance()->sensorDataFile.open(sdf);
    GlobalPars::Instance()->sensorDataFile << "event,sensor_id,time,charge\n";
  }

  GlobalPars::Instance()->iSensorDataFile.open(isdf);
  GlobalPars::Instance()->gammaIntFile.open(gint);
  GlobalPars::Instance()->iSensorDataFile << "event,sensor_id,amplitude\n";
  //GlobalPars::Instance(
  //)->gammaIntFile << "event,trkid,trkmass,motherid,x,y,z,trkl,edep,proc\n";

  GlobalPars::Instance()->gammaIntFile << "event,time,x,y,z,edep\n";
}


std::string set_histo_filename(std::string l4d)
{
  return GlobalPars::Instance()->fHistoFileName + l4d + ".txt";
}

void write_global_pars(std::string l4d)
{
  std::string sdf = "global_pars_" + l4d + ".csv";
  GlobalPars::Instance()->globalsFile.open(sdf);
  std::string xl1 ="seed,timeBinning,gammaEnergy,crystalWidth,crystalLength,";
  std::string xl2 ="material,sipmXY,numberOfEvents\n";
  GlobalPars::Instance()->globalsFile << xl1 + xl2;
  GlobalPars::Instance()->globalsFile << GlobalPars::Instance()->fSeed << "," << 
                                         GlobalPars::Instance()->fTimeBinning << "," <<
                                         GlobalPars::Instance()->fGammaEnergy << "," <<
                                         GlobalPars::Instance()->fCrystalWidth << "," <<
                                         GlobalPars::Instance()->fCrystalLength << "," <<
                                         GlobalPars::Instance()->fMaterial << "," <<
                                         GlobalPars::Instance()->fSipmXY << "," <<
                                         GlobalPars::Instance()->fNumberOfEvents ; 

}
