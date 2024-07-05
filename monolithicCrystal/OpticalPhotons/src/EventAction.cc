#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
//#include <arrow/io/api.h>
//#include <parquet/arrow/writer.h>
#include <iostream>
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction()
    : G4UserEventAction() {
  
  outputFile.open(output_file_name);
  outputFile << "ID,X,Y,Z,E\n";
  //x_builder = std::make_shared<arrow::DoubleBuilder>();
  // y_builder = std::make_shared<arrow::DoubleBuilder>();
  // z_builder = std::make_shared<arrow::DoubleBuilder>();
  // e_builder = std::make_shared<arrow::DoubleBuilder>();
}

EventAction::~EventAction() {
  outputFile.close();
  //    WriteDataToDisk();
}

void EventAction::BeginOfEventAction(const G4Event* event) {
    // No initialization needed at the beginning of the event
}

void EventAction::EndOfEventAction(const G4Event* event) {
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    SensitiveDetector* sensitiveDetector = dynamic_cast<SensitiveDetector*>(sdManager->FindSensitiveDetector("/SensitiveDetector"));
    if (!sensitiveDetector) return;

    const std::vector<PhotonHit>& hits = sensitiveDetector->GetHits();
    for (const auto& hit : hits) {
      outputFile << 1001 << "," << hit.position.x() << "," << hit.position.y() << "," << hit.position.z()
                  << "," <<hit.energy/eV <<"\n";
      
      // arrow::Status status;
      
      // status = x_builder->Append(hit.position.x());
      //   if (!status.ok()) {
      //     std::cerr << "x_builder:Append Error appending element: " << status.ToString() << std::endl;
      //   }
        
      //   status = y_builder->Append(hit.position.y());
      //   if (!status.ok()) {
      //     std::cerr << "y_builder:Append Error appending element: " << status.ToString() << std::endl;
      //   }

      //   status = z_builder->Append(hit.position.z());
      //   if (!status.ok()) {
      //     std::cerr << "z_builder:Append Error appending element: " << status.ToString() << std::endl;
      //   }

      //   status = e_builder->Append(hit.energy);
      //   if (!status.ok()) {
      //     std::cerr << "e_builder:Append Error appending element: " << status.ToString() << std::endl;
      //   }

    }
}

void EventAction::SetOutputFileName(const std::string& filename) {
    output_file_name = filename;
}

// void EventAction::WriteDataToDisk() {
//     // Create Arrow arrays from builders
//     std::shared_ptr<arrow::Array> x_array;
//     std::shared_ptr<arrow::Array> y_array;
//     std::shared_ptr<arrow::Array> z_array;
//     std::shared_ptr<arrow::Array> e_array;

//     arrow::Status status;
//     status = x_builder->Finish(&x_array);
//     if (!status.ok()) {
//       std::cerr << "x_builder:Finish Error appending element: " << status.ToString() << std::endl;
//     }

//     status = y_builder->Finish(&y_array);
//     if (!status.ok()) {
//       std::cerr << "y_builder:Finish Error appending element: " << status.ToString() << std::endl;
//     }

//     status = z_builder->Finish(&z_array);
//     if (!status.ok()) {
//       std::cerr << "z_builder:Finish Error appending element: " << status.ToString() << std::endl;
//     }

//     status = e_builder->Finish(&e_array);
//     if (!status.ok()) {
//       std::cerr << "e_builder:Finish Error appending element: " << status.ToString() << std::endl;
//     }


//     std::cout << "x_array" << x_array << std::endl;

//     // Define schema
//     std::vector<std::shared_ptr<arrow::Field>> schema_vector = {
//         arrow::field("x", arrow::float64()),
//         arrow::field("y", arrow::float64()),
//         arrow::field("z", arrow::float64()),
//         arrow::field("e", arrow::float64())
//     };

//     auto schema = std::make_shared<arrow::Schema>(schema_vector);

//     // Create a table
//     auto table = arrow::Table::Make(schema, {x_array, y_array, z_array, e_array});

//     // Write the table to a Parquet file
//     std::shared_ptr<arrow::io::FileOutputStream> outfile;
//     status = arrow::io::FileOutputStream::Open(output_file_name).Value(&outfile);

//     status = parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 1024);
//     status = outfile->Close();
// }
