#ifndef PATIENT_H
#define PATIENT_H

#include <string>

struct Patient {
    std::string id;
    std::string name;
    int age{}; //initialization to 0
    std::string condition;
    int priority{}; // 3 = Emergency, 2 = Urgent, 1 = Normal
    unsigned long long registrationOrder{};
    std::string status{"Waiting"}; // Waiting or Served
};
//priority labeling , coverts the numerical priority in to a readable description 
inline std::string priorityLabel(int priority) {
    switch (priority) { // checks the value of a variable against the different cases
        case 3: return "Emergency";
        case 2: return "Urgent";
        case 1: return "Normal";
        default: return "Invalid";
    }
}

#endif
