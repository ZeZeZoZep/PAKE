#include "timerset.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
TimerSet::TimerSet(const std::string& className, const std::string& functionName)
    : className_(className), functionName_(functionName) {}

void TimerSet::start(const std::string& label) {
    starts_[label] = std::chrono::high_resolution_clock::now();
}

void TimerSet::stop(const std::string& label) {
    auto end = std::chrono::high_resolution_clock::now();
    if (starts_.find(label) != starts_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - starts_[label]).count();
        durations_[label] = duration;
    } else {
        std::cerr << "[TimerSet] Warning: stop chiamato su '" << label << "' senza start corrispondente.\n";
    }
}

void TimerSet::commit(int d) {
    std::string filename = className_ + "." + functionName_ + ".csv";
    bool exists = std::filesystem::exists(filename);

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile aprire il file CSV: " + filename);
    }

    // Prepara le etichette ordinate
    std::vector<std::string> labels;
    for (const auto& [label, _] : durations_) {
        labels.push_back(label);
    }

    // Header
    if (!exists) {
        file << "d";
        for (const auto& label : labels) file << "," << label;
        file << "\n";
    }

    // Riga dati
    file << d;
    for (const auto& label : labels) file << "," << durations_[label];
    file << "\n";
}