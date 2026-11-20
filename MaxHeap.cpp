#include "MaxHeap.h"
#include <utility>

// this function compares two patients 
bool MaxHeap::hasHigherPriority(const Patient& a, const Patient& b) {
    if (a.priority != b.priority) {
        return a.priority > b.priority; // checks there priority levels, in the case where the priority levels are the same then the one who was registered first comes first
    }
    return a.registrationOrder < b.registrationOrder;
}
//this function maintians the heap property when we insert a new patient 
// by moving a newly inserted patient upwards until they are in the correct position according to thier priority 
void MaxHeap::heapifyUp(std::size_t index) {
    while (index > 0) {
        const std::size_t parent = (index - 1) / 2;
        if (!hasHigherPriority(heap[index], heap[parent])) {
            break;
        }
        std::swap(heap[index], heap[parent]);
        index = parent;
    }
}

// this is used when we romove the highest-priority patient 
// works in the oposite direction of heaifyUp()
void MaxHeap::heapifyDown(std::size_t index) {
    while (true) {
        const std::size_t left = 2 * index + 1;
        const std::size_t right = 2 * index + 2;
        std::size_t best = index;

        if (left < heap.size() && hasHigherPriority(heap[left], heap[best])) {
            best = left;
        }
        if (right < heap.size() && hasHigherPriority(heap[right], heap[best])) {
            best = right;
        }
        if (best == index) {
            break;
        }
        std::swap(heap[index], heap[best]);
        index = best;
    }
}
//inserting a patient
void MaxHeap::insert(const Patient& patient) {
    heap.push_back(patient);
    heapifyUp(heap.size() - 1);
}

bool MaxHeap::empty() const {
    return heap.empty();
}

std::size_t MaxHeap::size() const {
    return heap.size();
}

const Patient* MaxHeap::peek() const {
    return heap.empty() ? nullptr : &heap.front();
}
// removing the highest prority patient
bool MaxHeap::removeTop(Patient& removedPatient) {
    if (heap.empty()) {// checks whether the heap is empty 
        return false;
    }

    removedPatient = heap.front();
    if (heap.size() == 1) {
        heap.pop_back();
        return true;
    }

    heap.front() = heap.back();
    heap.pop_back();
    heapifyDown(0);
    return true;
}
// removes all elements from the vector 
void MaxHeap::clear() {
    heap.clear();
}
