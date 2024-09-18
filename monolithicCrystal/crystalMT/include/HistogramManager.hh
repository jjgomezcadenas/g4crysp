#ifndef HISTOGRAM_MANAGER_HH
#define HISTOGRAM_MANAGER_HH

#include <gsl/gsl_histogram.h>
#include <map>
#include <string>

class HistogramManager {
public:
    static HistogramManager* Instance();
    void CreateHistogram(const std::string& name, size_t bins, double min, double max);
    void FillHistogram(const std::string& name, double value);
    void FillHistogram(const std::string& name, 
                       double value, double weight);
    void WriteHistograms(const std::string& filename);
    std::map<std::string, gsl_histogram*> fHistograms;

private:
    HistogramManager();
    ~HistogramManager();
    HistogramManager(const HistogramManager&) = delete;
    HistogramManager& operator=(const HistogramManager&) = delete;

   
};

#endif // HISTOGRAM_MANAGER_HH
