#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "Patient.h"
#include <vector>
#include <cstddef>

// A custom max-heap. Higher medical priority comes first.
// Patients with equal priority are served in registration order (FIFO).
class MaxHeap {
private:
    std::vector<Patient> heap; // stores patients

    static bool hasHigherPriority(const Patient& a, const Patient& b); // compares two patients
    void heapifyUp(std::size_t index); //moves a newly inserted patient upwards when necessary
    void heapifyDown(std::size_t index); // moves a patient downwards when necessary 

public:
    void insert(const Patient& patient); // adds a patient to the heap
    bool empty() const; // checks whether the heaphas patients 
    std::size_t size() const; //returns the number of patients
    const Patient* peek() const; //Looks at the highest-priority patient without removing them
    bool removeTop(Patient& removedPatient); //removes the highest-priority patient
    void clear();// removes all patients from the heap
};

#endif
