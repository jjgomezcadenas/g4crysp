#include "HistogramManager.hh"
#include <fstream>
#include <iostream>

HistogramManager* HistogramManager::Instance() {
    static HistogramManager instance;
    return &instance;
}

HistogramManager::HistogramManager() {}


HistogramManager::~HistogramManager() {
    for (auto& entry : fHistograms) {
        gsl_histogram_free(entry.second);
    }
}


void HistogramManager::CreateHistogram(const std::string& name, size_t bins, double min, double max) {
    gsl_histogram* hist = gsl_histogram_alloc(bins);
    gsl_histogram_set_ranges_uniform(hist, min, max);
    fHistograms[name] = hist;
}


void HistogramManager::FillHistogram(const std::string& name, double value) {
    auto it = fHistograms.find(name);
    if (it != fHistograms.end()) {
        gsl_histogram_increment(it->second, value);
    } else {
        std::cerr << "Histogram " << name << " not found!" << std::endl;
    }
}

void HistogramManager::WriteHistograms(const std::string& filename) {
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        std::cerr << "Error opening file " << filename << " for writing." << std::endl;
        return;
    }

    for (const auto& entry : fHistograms) {
        fprintf(file, "# Histogram: %s\n", entry.first.c_str());
        gsl_histogram_fprintf(file, entry.second, "%g", "%g");
        fprintf(file, "\n");
    }

    fclose(file);
}

