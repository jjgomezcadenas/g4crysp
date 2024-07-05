#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include <arrow/api.h>
#include <string>
#include <vector>
#include "SensitiveDetector.hh"

class EventAction : public G4UserEventAction {
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void SetOutputFileName(const std::string& filename);
    void WriteDataToDisk();

private:
    std::shared_ptr<arrow::DoubleBuilder> x_builder;
    std::shared_ptr<arrow::DoubleBuilder> y_builder;
    std::shared_ptr<arrow::DoubleBuilder> z_builder;
    std::shared_ptr<arrow::DoubleBuilder> e_builder;

    std::string output_file_name;
};

#endif
