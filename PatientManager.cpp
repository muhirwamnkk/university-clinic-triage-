#include "PatientManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {
    std::vector<std::string> splitCsvLine(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream stream(line);
        std::string field;
        while (std::getline(stream, field, ',')) {
            fields.push_back(field);
        }
        return fields;
    }

    bool validTextField(const std::string& value) {
        return !value.empty() &&
               value.find(',') == std::string::npos &&
               value.find('\n') == std::string::npos &&
               value.find('\r') == std::string::npos;
    }
}

PatientManager::PatientManager(const std::string& filePath)
    : dataFile(filePath) {}

bool PatientManager::loadFromFile() {
    std::ifstream input(dataFile);// input file stream
    if (!input.is_open()) {
        // A missing data file is normal on the first run.
        return true;
    }

    patients.clear();
    triageQueue.clear();
    nextRegistrationOrder = 1;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;

        const auto fields = splitCsvLine(line);
        if (fields.size() != 7) continue;

        try {
            Patient p;
            p.id = fields[0];
            p.name = fields[1];
            p.age = std::stoi(fields[2]);
            p.condition = fields[3];
            p.priority = std::stoi(fields[4]);
            p.registrationOrder = std::stoull(fields[5]);
            p.status = fields[6];

            if (!validTextField(p.id) || !validTextField(p.name) ||
                !validTextField(p.condition) || p.age < 1 || p.age > 120 ||
                p.priority < 1 || p.priority > 3 ||
                (p.status != "Waiting" && p.status != "Served")) {
                continue;
            }

            if (patients.find(p.id) == patients.end()) {
                patients.emplace(p.id, p);
                if (p.registrationOrder >= nextRegistrationOrder) {
                    nextRegistrationOrder = p.registrationOrder + 1;
                }
            }
        } catch (...) {
            // Ignore malformed records and continue loading valid records.
        }
    }

    rebuildQueue();
    return !input.bad();
}

bool PatientManager::addPatient(const Patient& patient, std::string& message) {
    if (!validTextField(patient.id) || !validTextField(patient.name) ||
        !validTextField(patient.condition)) {
        message = "ID, name and condition must be non-empty and cannot contain commas.";
        return false;
    }
    if (patient.age < 1 || patient.age > 120) {
        message = "Age must be between 1 and 120.";
        return false;
    }
    if (patient.priority < 1 || patient.priority > 3) {
        message = "Priority must be 1 (Normal), 2 (Urgent) or 3 (Emergency).";
        return false;
    }
    if (patients.find(patient.id) != patients.end()) {
        message = "A patient with that ID already exists.";
        return false;
    }

    Patient stored = patient;
    stored.registrationOrder = nextRegistrationOrder++;
    stored.status = "Waiting";
    patients.emplace(stored.id, stored);
    rebuildQueue();

    if (!saveToFile()) {
        message = "Patient was added in memory, but saving to the data file failed.";
        return false;
    }
    message = "Patient registered successfully.";
    return true;
}

const Patient* PatientManager::findPatient(const std::string& id) const {
    const auto it = patients.find(id);
    return it == patients.end() ? nullptr : &it->second;
}

bool PatientManager::updatePatient(const std::string& id, const std::string& name,
                                   int age, const std::string& condition,
                                   int priority, std::string& message) {
    auto it = patients.find(id);
    if (it == patients.end()) {
        message = "Patient ID was not found.";
        return false;
    }
    if (!validTextField(name) || !validTextField(condition)) {
        message = "Name and condition must be non-empty and cannot contain commas.";
        return false;
    }
    if (age < 1 || age > 120) {
        message = "Age must be between 1 and 120.";
        return false;
    }
    if (priority < 1 || priority > 3) {
        message = "Priority must be 1, 2 or 3.";
        return false;
    }

    it->second.name = name;
    it->second.age = age;
    it->second.condition = condition;
    it->second.priority = priority;
    rebuildQueue();

    if (!saveToFile()) {
        message = "Record updated in memory, but saving to the data file failed.";
        return false;
    }
    message = "Patient record updated successfully.";
    return true;
}

bool PatientManager::deletePatient(const std::string& id, std::string& message) {
    const auto erased = patients.erase(id);
    if (erased == 0) {
        message = "Patient ID was not found.";
        return false;
    }
    rebuildQueue();
    if (!saveToFile()) {
        message = "Record deleted in memory, but saving to the data file failed.";
        return false;
    }
    message = "Patient record deleted successfully.";
    return true;
}

bool PatientManager::attendNextPatient(Patient& attendedPatient) {
    Patient next;
    if (!triageQueue.removeTop(next)) {
        return false;
    }

    auto it = patients.find(next.id);
    if (it == patients.end() || it->second.status != "Waiting") {
        rebuildQueue();
        return attendNextPatient(attendedPatient);
    }

    it->second.status = "Served";
    attendedPatient = it->second;
    rebuildQueue();
    saveToFile();
    return true;
}

std::vector<Patient> PatientManager::orderedWaitingPatients() const {
    MaxHeap copy = triageQueue;
    std::vector<Patient> result;
    Patient patient;
    while (copy.removeTop(patient)) {
        result.push_back(patient);
    }
    return result;
}

std::vector<Patient> PatientManager::allPatients() const {
    std::vector<Patient> result;
    result.reserve(patients.size());
    for (const auto& entry : patients) {
        result.push_back(entry.second);
    }
    std::sort(result.begin(), result.end(),
              [](const Patient& a, const Patient& b) {
                  return a.registrationOrder < b.registrationOrder;
              });
    return result;
}

bool PatientManager::saveToFile() const {
    std::ofstream output(dataFile, std::ios::trunc);
    if (!output.is_open()) return false;

    for (const auto& entry : patients) {
        const Patient& p = entry.second;
        output << p.id << ',' << p.name << ',' << p.age << ','
               << p.condition << ',' << p.priority << ','
               << p.registrationOrder << ',' << p.status << '\n';
    }
    return output.good();
}

bool PatientManager::save() const {
    return saveToFile();
}

void PatientManager::rebuildQueue() {
    triageQueue.clear();
    for (const auto& entry : patients) {
        const Patient& p = entry.second;
        if (p.status == "Waiting") {
            triageQueue.insert(p);
        }
    }
}

std::size_t PatientManager::waitingCount() const {
    return triageQueue.size();
}

std::size_t PatientManager::totalCount() const {
    return patients.size();
}
