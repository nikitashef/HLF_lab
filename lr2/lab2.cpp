#include "lab2.h"
#include <iomanip>

// ============ Work with dynamic array (double) ============
void workWithArray() {
    cout << "\n========== WORK WITH DYNAMIC ARRAY (double) ==========" << endl;
    
    // Creating a dynamic array of 10 elements
    int size = 10;
    double* arr = new double[size];
    
    // Filling array with values i*10
    cout << "\nFilling array with values i*10:" << endl;
    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }
    
    // Output all values to screen
    cout << "Array values: ";
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    // Calculate arithmetic average
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    double average = sum / size;
    cout << "Arithmetic average: " << average << endl;
    
    // Free memory
    delete[] arr;
}

// ============ Work with vector (int) ============
void workWithVector() {
    cout << "\n========== WORK WITH VECTOR (int) ==========" << endl;
    
    // Creating a vector of 15 elements
    vector<int> vec(15);
    
    // Filling vector with random values in range 0 to 5
    cout << "\nFilling vector with random values (0-5):" << endl;
    srand((unsigned)time(0));
    for (int i = 0; i < 15; i++) {
        vec[i] = rand() % 6;  // Values from 0 to 5
    }
    
    // Output all values
    cout << "Original vector: ";
    for (int val : vec) {
        cout << val << " ";
    }
    cout << endl;
    
    // Find maximum element
    int maxElem = *max_element(vec.begin(), vec.end());
    cout << "Maximum element: " << maxElem << endl;
    
    // Calculate sum of all elements
    int sum = accumulate(vec.begin(), vec.end(), 0);
    cout << "Sum of all elements: " << sum << endl;
    
    // Remove zeros
    cout << "\nRemoving zeros from vector..." << endl;
    auto newEnd = remove(vec.begin(), vec.end(), 0);
    vec.resize(distance(vec.begin(), newEnd));
    cout << "Vector after removing zeros: ";
    for (int val : vec) {
        cout << val << " ";
    }
    cout << endl;
    
    // Sort in ascending order
    cout << "\nSorting vector in ascending order..." << endl;
    sort(vec.begin(), vec.end());
    cout << "Sorted vector: ";
    for (int val : vec) {
        cout << val << " ";
    }
    cout << endl;
    
    // Remove duplicate values
    cout << "\nRemoving duplicates from vector..." << endl;
    auto uniqueEnd = unique(vec.begin(), vec.end());
    vec.resize(distance(vec.begin(), uniqueEnd));
    cout << "Vector after removing duplicates: ";
    for (int val : vec) {
        cout << val << " ";
    }
    cout << endl;
}

// ============ Work with list (double) ============
void workWithList() {
    cout << "\n========== WORK WITH LIST (double) ==========" << endl;
    
    // Creating an empty list
    list<double> lst;
    
    // Filling list from keyboard
    cout << "\nEnter values for the list (enter negative number to finish):" << endl;
    double value;
    while (true) {
        cout << "Enter number: ";
        cin >> value;
        
        if (value < 0) {
            cout << "Input completed." << endl;
            break;
        }
        
        lst.push_back(value);
        cout << "Value added. Continue? (Enter number < 0 to exit)" << endl;
    }
    
    // Output all values
    cout << "\nList values: ";
    if (lst.empty()) {
        cout << "List is empty." << endl;
    } else {
        for (double val : lst) {
            cout << val << " ";
        }
        cout << endl;
        
        // Find element with maximum absolute value
        double maxAbsElem = *max_element(lst.begin(), lst.end(), 
                                         [](double a, double b) {
                                             return fabs(a) < fabs(b);
                                         });
        cout << "Element with maximum absolute value: " << maxAbsElem 
             << " (absolute value: " << fabs(maxAbsElem) << ")" << endl;
    }
}

// ============ Work with map (string, double) ============
void workWithMap() {
    cout << "\n========== WORK WITH MAP (string, double) -> REFRACTIVE INDICES ==========" << endl;
    
    // Creating map to store refractive indices of optical glass
    map<string, double> glassMap;
    
    // Reading data from file
    cout << "\nReading data from glass_data.txt file..." << endl;
    
    // Try to open file - first in current directory, then in exe directory
    ifstream file("glass_data.txt");
    
    if (!file.is_open()) {
        // Try to find file in the same directory as exe
        file.open("..\\lr2\\glass_data.txt");
    }
    
    if (!file.is_open()) {
        file.open(".\\glass_data.txt");
    }
    
    if (!file.is_open()) {
        cerr << "Error: Cannot open glass_data.txt file. Make sure it's in the same directory as the program." << endl;
        return;
    }
    
    string glassName;
    double refraction;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Find the last space to split name and refraction index
        size_t lastSpace = line.rfind(' ');
        if (lastSpace != string::npos) {
            glassName = line.substr(0, lastSpace);
            try {
                refraction = stod(line.substr(lastSpace + 1));
                glassMap[glassName] = refraction;
            } catch (...) {
                // Skip lines that can't be parsed
            }
        }
    }
    file.close();
    
    if (glassMap.empty()) {
        cout << "File is empty or contains no data." << endl;
        return;
    }
    
    // Output entire list of glasses
    cout << "\nList of all glasses and their refractive indices:" << endl;
    cout << left << setw(25) << "Glass Name" << setw(20) << "Refractive Index" << endl;
    cout << string(45, '-') << endl;
    for (const auto& pair : glassMap) {
        cout << left << setw(25) << pair.first << setw(20) << fixed << setprecision(4) << pair.second << endl;
    }
    
    // Find minimum and maximum refractive index
    cout << "\n" << string(45, '-') << endl;
    auto minPair = *min_element(glassMap.begin(), glassMap.end(),
                                [](const pair<string, double>& a, const pair<string, double>& b) {
                                    return a.second < b.second;
                                });
    
    auto maxPair = *max_element(glassMap.begin(), glassMap.end(),
                                [](const pair<string, double>& a, const pair<string, double>& b) {
                                    return a.second < b.second;
                                });
    
    cout << "Minimum refractive index:" << endl;
    cout << "  " << minPair.first << " = " << fixed << setprecision(4) << minPair.second << endl;
    
    cout << "Maximum refractive index:" << endl;
    cout << "  " << maxPair.first << " = " << fixed << setprecision(4) << maxPair.second << endl;
}

// ============ Work with deque and set ============
void workWithDequeAndSet() {
    cout << "\n========== WORK WITH DEQUE AND SET ==========" << endl;
    
    // Creating deque of 15 elements
    deque<int> dq(15);
    
    // Filling deque with random values in range 0 to 5
    cout << "\nFilling deque with random values (0-5):" << endl;
    srand((unsigned)time(0) + 1);  // Adding offset for different sequence
    for (int i = 0; i < 15; i++) {
        dq[i] = rand() % 6;  // Values from 0 to 5
    }
    
    // Output all deque values
    cout << "Deque values: ";
    for (int val : dq) {
        cout << val << " ";
    }
    cout << endl;
    
    // Creating set and writing all values from deque
    cout << "\nCreating set from deque elements..." << endl;
    set<int> s(dq.begin(), dq.end());
    
    // Output all set values
    cout << "Set values (unique and sorted): ";
    for (int val : s) {
        cout << val << " ";
    }
    cout << endl;
    cout << "Number of unique values: " << s.size() << endl;
}

// ============ MAIN FUNCTION ============
int main() {
    cout << "\n========================================================" << endl;
    cout << "         LABORATORY WORK 2 - STL CONTAINERS" << endl;
    cout << "========================================================" << endl;
    
    // Calling all functions
    workWithArray();
    workWithVector();
    workWithList();
    workWithMap();
    workWithDequeAndSet();
    
    cout << "\n========================================================" << endl;
    cout << "                   WORK COMPLETED" << endl;
    cout << "========================================================" << endl;
    
    return 0;
}
