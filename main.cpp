#include "PatientManager.h"
#include <iostream>
#include <limits>
#include <string>

namespace
{
    int readInt(const std::string &prompt, int minimum, int maximum)
    {
        while (true)
        {
            std::cout << prompt;
            std::string line;
            if (!std::getline(std::cin, line))
                return minimum;

            try
            {
                std::size_t used = 0;
                int value = std::stoi(line, &used);
                if (used == line.size() && value >= minimum && value <= maximum)
                {
                    return value;
                }
            }
            catch (...)
            {
                // Ask again below.
            }
            std::cout << "Invalid input. Enter a number from "
                      << minimum << " to " << maximum << ".\n";
        }
    }

    std::string readText(const std::string &prompt)
    {
        while (true)
        {
            std::cout << prompt;
            std::string value;
            if (!std::getline(std::cin, value))
                return {};
            if (!value.empty() && value.find(',') == std::string::npos)
            {
                return value;
            }
            std::cout << "Input cannot be empty or contain commas. Try again.\n";
        }
    }

    void printPatient(const Patient &p)
    {
        std::cout << "Patient ID: " << p.id
                  << "\nName: " << p.name
                  << "\nAge: " << p.age
                  << "\nCondition: " << p.condition
                  << "\nPriority: " << priorityLabel(p.priority)
                  << "\nRegistration order: " << p.registrationOrder
                  << "\nStatus: " << p.status << "\n";
    }

    void showMenu()
    {
        std::cout << "\n========== UNIVERSITY CLINIC TRIAGE SYSTEM ==========\n"
                  << "1. Register patient\n"
                  << "2. Search patient\n"
                  << "3. Update patient\n"
                  << "4. Delete patient\n"
                  << "5. View next patient\n"
                  << "6. Attend to next patient\n"
                  << "7. Display ordered triage report\n"
                  << "8. Display all patient records\n"
                  << "9. Save records\n"
                  << "0. Save and exit\n"
                  << "=====================================================\n";
    }
}

int main()
{
    PatientManager manager("data/patients.txt");

    if (!manager.loadFromFile())
    {
        std::cerr << "Warning: There was a problem reading the patient data file.\n";
    }

    std::cout << "University Clinic Triage System\n"
              << "Loaded " << manager.totalCount() << " patient record(s); "
              << manager.waitingCount() << " waiting.\n";

    bool running = true;
    while (running)
    {
        showMenu();
        const int choice = readInt("Choose an option: ", 0, 9);
        std::string message;

        switch (choice)
        {
        case 1:
        {
            Patient p;
            p.id = readText("Patient ID: ");
            p.name = readText("Patient name: ");
            p.age = readInt("Patient age (1-120): ", 1, 120);
            p.condition = readText("Medical condition: ");
            std::cout << "Priority levels: 1 = Normal, 2 = Urgent, 3 = Emergency\n";
            p.priority = readInt("Enter priority (1-3): ", 1, 3);
            if (manager.addPatient(p, message))
                std::cout << message << '\n';
            else
                std::cout << "Registration failed: " << message << '\n';
            break;
        }
        case 2:
        {
            const std::string id = readText("Enter patient ID to search: ");
            const Patient *p = manager.findPatient(id);
            if (p)
                printPatient(*p);
            else
                std::cout << "Patient not found.\n";
            break;
        }
        case 3:
        {
            const std::string id = readText("Enter patient ID to update: ");
            const Patient *existing = manager.findPatient(id);
            if (!existing)
            {
                std::cout << "Patient not found.\n";
                break;
            }
            std::cout << "Enter the updated details.\n";
            const std::string name = readText("New name: ");
            const int age = readInt("New age (1-120): ", 1, 120);
            const std::string condition = readText("New medical condition: ");
            std::cout << "Priority levels: 1 = Normal, 2 = Urgent, 3 = Emergency\n";
            const int priority = readInt("New priority (1-3): ", 1, 3);
            manager.updatePatient(id, name, age, condition, priority, message);
            std::cout << message << '\n';
            break;
        }
        case 4:
        {
            const std::string id = readText("Enter patient ID to delete: ");
            manager.deletePatient(id, message);
            std::cout << message << '\n';
            break;
        }
        case 5:
        {
            const auto waiting = manager.orderedWaitingPatients();
            if (waiting.empty())
            {
                std::cout << "There are no patients waiting.\n";
            }
            else
            {
                std::cout << "Next patient according to triage priority:\n";
                printPatient(waiting.front());
            }
            break;
        }
        case 6:
        {
            Patient attended;
            if (manager.attendNextPatient(attended))
            {
                std::cout << "Patient marked as served:\n";
                printPatient(attended);
            }
            else
            {
                std::cout << "There are no patients waiting to be attended to.\n";
            }
            break;
        }
        case 7:
        {
            const auto waiting = manager.orderedWaitingPatients();
            std::cout << "\n--- ORDERED TRIAGE REPORT (WAITING PATIENTS) ---\n";
            if (waiting.empty())
            {
                std::cout << "No waiting patients.\n";
            }
            else
            {
                for (std::size_t i = 0; i < waiting.size(); ++i)
                {
                    std::cout << "\nPosition " << (i + 1) << '\n';
                    printPatient(waiting[i]);
                }
            }
            std::cout << "Waiting patients: " << waiting.size() << '\n';
            break;
        }
        case 8:
        {
            const auto records = manager.allPatients();
            std::cout << "\n--- ALL PATIENT RECORDS ---\n";
            if (records.empty())
                std::cout << "No patient records found.\n";
            for (const auto &p : records)
            {
                printPatient(p);
                std::cout << "--------------------------\n";
            }
            break;
        }
        case 9:
            std::cout << (manager.save() ? "Records saved successfully.\n"
                                         : "Could not save records. Check the data folder.\n");
            break;
        case 0:
            if (manager.save())
            {
                std::cout << "Records saved. Exiting safely.\n";
                running = false;
            }
            else
            {
                std::cout << "Could not save records. The program will remain open so you can retry.\n";
            }
            break;
        }
    }
    return 0;
}
