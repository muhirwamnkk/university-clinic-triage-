#ifndef PATIENT_MANAGER_H
#define PATIENT_MANAGER_H

#include "MaxHeap.h"
#include "Patient.h"
#include <string>
#include <unordered_map>
#include <vector>

class PatientManager {
private:
    // Hash table: patient ID is the unique key.
    std::unordered_map<std::string, Patient> patients;
    // Custom binary max-heap containing only patients whose status is Waiting.
    MaxHeap triageQueue;
    unsigned long long nextRegistrationOrder{1}; //this keeps track of order in which patients register
    std::string dataFile;// this stores  the location of the text file where patient records are saved.

    void rebuildQueue();
    bool saveToFile() const;

public:
    explicit PatientManager(const std::string& filePath);

    bool loadFromFile();
    bool addPatient(const Patient& patient, std::string& message);
    const Patient* findPatient(const std::string& id) const;
    bool updatePatient(const std::string& id, const std::string& name,
                       int age, const std::string& condition,
                       int priority, std::string& message);
    bool deletePatient(const std::string& id, std::string& message);
    bool attendNextPatient(Patient& attendedPatient);
    std::vector<Patient> orderedWaitingPatients() const;
    std::vector<Patient> allPatients() const;
    bool save() const;
    std::size_t waitingCount() const;
    std::size_t totalCount() const;
};

#endif
