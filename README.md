# University Clinic Patient Triage and Management System

A C++17 console application for the Scenario 2 coursework: University Clinic Triage System.

## Data structures

1. **Hash table:** `std::unordered_map<std::string, Patient>` stores patient records using the patient ID as the unique key.
2. **Custom binary max-heap:** implemented in `MaxHeap.h` and `MaxHeap.cpp`. It prioritizes Emergency (3), Urgent (2), then Normal (1). Patients with equal priority are ordered first-registered, first-served.

The custom heap is the core data structure implemented by the project team. The hash table uses the C++ STL.

## Features

- Register, search, update and delete patient records.
- Assign triage priority (1 = Normal, 2 = Urgent, 3 = Emergency).
- View and attend to the next patient.
- Generate an ordered report without destroying the live queue.
- Load and save records in `data/patients.txt`.
- Validate IDs, names, conditions, ages and priority values.
- Keep served patients in the record file while removing them from the waiting queue.

## Project structure

```text
UniversityClinicTriageSystem/
├── include/
│   ├── Patient.h
│   ├── MaxHeap.h
│   └── PatientManager.h
├── src/
│   ├── main.cpp
│   ├── MaxHeap.cpp
│   └── PatientManager.cpp
├── data/
│   └── patients.txt
├── Makefile
└── README.md
```

## Compile and run

From the project root, using g++:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/main.cpp src/MaxHeap.cpp src/PatientManager.cpp -o clinic_triage
./clinic_triage
```

Or, using clang++:

```bash
clang++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/main.cpp src/MaxHeap.cpp src/PatientManager.cpp -o clinic_triage
./clinic_triage
```

On macOS, if `g++` is not installed, use `clang++`.

## Data file format

Each line in `data/patients.txt` stores:

```text
patientID,name,age,condition,priority,registrationOrder,status
```

Text fields must not contain commas because this starter version uses a simple comma-separated text format.

## Complexity

| Operation | Average / worst-case time |
|---|---:|
| Hash table insert, search, delete | Average O(1), worst O(n) |
| Heap insertion | O(log n) |
| View highest-priority patient | O(1) |
| Remove highest-priority patient | O(log n) |
| Generate ordered waiting report | O(n log n) |

## Important implementation note

The heap stores copies of waiting patient records. The application rebuilds the heap after patient updates, deletions and attendance so the queue does not retain stale priority information.

This is an educational triage simulation. Real medical triage decisions must be made by qualified healthcare personnel.
