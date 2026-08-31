/*
    ================================================================
    AMCS2123 - Systems And Programming Concepts
    Module 1: Student Account & Student Management
    ================================================================
    This module handles:
        - Student registration, login, and profile viewing
        - Admin login and full student record management
          (add, edit, delete, search, display)
        - File processing (students.txt) for permanent storage
    ================================================================
*/

/*
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// ================= Constants =================
const int MAX_STUDENTS = 100;
const string STUDENT_FILE = "students.txt";

// Hardcoded admin credentials (single administrator account)
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

// ================= Structure =================
struct Student
{
    string studentID;
    string name;
    string gender;
    int age;
    string phone;
    string school;
    string username;
    string password;
};

// ================= Function Prototypes =================
// Student functions
void studentRegister(vector<Student>& students);
bool studentLogin(vector<Student>& students, string username, string password, string& loggedInID);
bool adminLogin(string username, string password);
void viewProfile(const vector<Student>& students, string studentID);

// Admin functions
void addStudent(vector<Student>& students);
void editStudent(vector<Student>& students, string studentID);
void deleteStudent(vector<Student>& students, string studentID);
bool searchStudent(const vector<Student>& students, string studentID, int& foundIndex);
void displayStudents(const vector<Student>& students);

// File processing
void loadStudentData(vector<Student>& students);
void saveStudentData(const vector<Student>& students);

// Validation helpers
bool isValidPhone(const string& phone);
bool isValidAge(int age);
bool usernameExists(const vector<Student>& students, const string& username);
bool studentIDExists(const vector<Student>& students, const string& studentID);
string generateStudentID(const vector<Student>& students);
void clearInputBuffer();
string readNonEmptyLine(const string& prompt);

// Menus
void studentMenu(vector<Student>& students);
void adminMenu(vector<Student>& students);
void mainMenu();

// ================= main =================
//int main()
//{
//    mainMenu();
//    return 0;
//}
*/
#include "common.h"

// ================= Main Menu =================
/*void mainMenu()
{
    vector<Student> students;
    loadStudentData(students);

    int choice;
    do
    {
        cout << "\n===================================\n";
        cout << "   TUITION CENTER MANAGEMENT SYSTEM\n";
        cout << "===================================\n";
        cout << "1. Student\n";
        cout << "2. Administrator\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            studentMenu(students);
            break;
        case 2:
            adminMenu(students);
            break;
        case 0:
            saveStudentData(students);
            cout << "Exiting system. Goodbye!\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}

// ================= Student Menu =================
void studentMenu(vector<Student>& students)
{
    int choice;
    do
    {
        cout << "\n----------- STUDENT MENU -----------\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "0. Return to Main Menu\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            studentRegister(students);
            saveStudentData(students);
            break;
        case 2:
        {
            string username, password, loggedInID;
            username = readNonEmptyLine("Enter username: ");
            password = readNonEmptyLine("Enter password: ");

            if (studentLogin(students, username, password, loggedInID))
            {
                cout << "Login successful! Welcome, " << username << ".\n";

                int subChoice;
                do
                {
                    cout << "\n----- STUDENT PROFILE MENU -----\n";
                    cout << "1. View Profile\n";
                    cout << "0. Return to Student Menu\n";
                    cout << "Enter your choice: ";
                    if (!(cin >> subChoice))
                    {
                        cout << "Invalid input.\n";
                        clearInputBuffer();
                        continue;
                    }
                    clearInputBuffer();

                    if (subChoice == 1)
                        viewProfile(students, loggedInID);
                    else if (subChoice != 0)
                        cout << "Invalid menu choice.\n";

                } while (subChoice != 0);
            }
            else
            {
                cout << "Login failed. Invalid username or password.\n";
            }
            break;
        }
        case 0:
            cout << "Returning to Main Menu...\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}
*/

void studentProfileMenu(string loggedID, const vector<Student>& students)
{
    int subChoice;
    do
    {
        cout << "\n----- STUDENT PROFILE MENU -----\n";
        cout << "1. View Profile\n";
        cout << "0. Return to Student Menu\n";
        cout << "Enter your choice: ";

        subChoice = readMenuChoice(0, 1);

        if (subChoice == 1)
        {
            viewProfile(students, loggedID);
        }
        else if (subChoice == 0)
        {
            cout << "Returning to Student Menu...\n";
        }

    } while (subChoice != 0);
}

// ================= Admin Menu =================
void adminMenu(vector<Student>& students)
{
    // NOTE: Admin login is now handled once at the top level in main.cpp
    // (when "2. Administrator" is selected), so this function no longer
    // asks for credentials again here.

    int choice;
    do
    {
        cout << "\n----------- ADMIN: STUDENT MANAGEMENT -----------\n";
        cout << "1. Add Student\n";
        cout << "2. Edit Student\n";
        cout << "3. Delete Student\n";
        cout << "4. Search Student\n";
        cout << "5. Display All Students\n";
        cout << "0. Logout\n";
        cout << "Enter your choice: ";

        choice = readMenuChoice(0, 5);

        switch (choice)
        {
        case 1:
            addStudent(students);
            saveStudentData(students);
            break;
        case 2:
        {
            string id = readNonEmptyLine("Enter Student ID to edit: ");
            editStudent(students, id);
            saveStudentData(students);
            break;
        }
        case 3:
        {
            string id = readNonEmptyLine("Enter Student ID to delete: ");
            deleteStudent(students, id);
            saveStudentData(students);
            break;
        }
        case 4:
        {
            string id = readNonEmptyLine("Enter Student ID to search: ");
            int idx;
            if (searchStudent(students, id, idx))
            {
                cout << "Student found!\n";
                viewProfile(students, id);
            }
            else
            {
                cout << "Student ID not found.\n";
            }
            break;
        }
        case 5:
            displayStudents(students);
            break;
        case 0:
            cout << "Logging out of menu...\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}

// ================= Helper Functions =================
int readMenuChoice(int min, int max)
{
    string input;

    while (true)
    {
        getline(cin, input);

        if (input.empty())
        {
            cout << "Input cannot be empty. Please enter a number: ";
            continue;
        }

        try
        {
            size_t pos;
            int choice = stoi(input, &pos);

            // Check if there are extra characters
            if (pos != input.length())
            {
                cout << "Invalid input. Please enter a number: ";
                continue;
            }

            if (choice < min || choice > max)
            {
                cout << "Invalid choice. Please enter a number from "
                    << min << " to " << max << ": ";
                continue;
            }

            return choice;
        }
        catch (...)
        {
            cout << "Invalid input. Please enter a number: ";
        }
    }
}
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readNonEmptyLine(const string& prompt)
{
    string input;
    do
    {
        cout << prompt;
        getline(cin, input);
        if (input.empty())
            cout << "Input cannot be empty. Please try again.\n";
    } while (input.empty());
    return input;
}

bool isValidPhone(const string& phone)
{
    // Must be all digits and 10 or 11 digits long
    if (phone.length() != 10 && phone.length() != 11)
        return false;

    for (char c : phone)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

bool isValidAge(int age)
{
    return (age >= 13 && age <= 17);
}

bool usernameExists(const vector<Student>& students, const string& username)
{
    for (const auto& student : students)
    {
        if (student.username == username)
            return true;
    }
    return false;
}

bool studentIDExists(const vector<Student>& students, const string& studentID)
{
    for (const auto& student : students)
    {
        if (student.studentID == studentID)
            return true;
    }
    return false;
}

// Auto-generates a new unique Student ID in the format S001, S002, ...
string generateStudentID(const vector<Student>& students)
{
    int nextNum = static_cast<int>(students.size()) + 1;
    string newID;
    do
    {
        ostringstream oss;
        oss << "S" << setw(3) << setfill('0') << nextNum;
        newID = oss.str();
        nextNum++;
    } while (studentIDExists(students, newID));

    return newID;
}

// ================= Student Functions =================
void studentRegister(vector<Student>& students)
{
    if (static_cast<int>(students.size()) >= MAX_STUDENTS)
    {
        cout << "Registration failed. Maximum number of students reached.\n";
        return;
    }

    Student newStudent;
    cout << "\n----- Student Registration -----\n";

    newStudent.name = readNonEmptyLine("Enter Full Name: ");

    // Gender validation (M/F)
    string genderInput;
    do
    {
        genderInput = readNonEmptyLine("Enter Gender (M/F): ");
        for (auto& c : genderInput) c = toupper(c);
        if (genderInput != "M" && genderInput != "F")
            cout << "Invalid gender. Please enter M or F.\n";
    } while (genderInput != "M" && genderInput != "F");
    newStudent.gender = genderInput;

    // Age validation (13-17)
    int age;
    while (true)
    {
        cout << "Enter Age (13-17): ";
        if (!(cin >> age))
        {
            cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        if (!isValidAge(age))
        {
            cout << "Age must be between 13 and 17.\n";
            continue;
        }
        break;
    }
    newStudent.age = age;

    // Phone validation (10 or 11 digits)
    string phone;
    do
    {
        phone = readNonEmptyLine("Enter Phone Number (10-11 digits): ");
        if (!isValidPhone(phone))
            cout << "Invalid phone number. Must be 10-11 digits.\n";
    } while (!isValidPhone(phone));
    newStudent.phone = phone;

    newStudent.school = readNonEmptyLine("Enter School Name: ");

    // Username validation (non-empty, unique)
    string username;
    do
    {
        username = readNonEmptyLine("Enter Username: ");
        if (usernameExists(students, username))
            cout << "Username already exists. Please choose another.\n";
    } while (usernameExists(students, username));
    newStudent.username = username;

    // Password validation (minimum length 6)
    string password;
    do
    {
        password = readNonEmptyLine("Enter Password (min 6 characters): ");
        if (password.length() < 6)
            cout << "Password too short. Minimum 6 characters required.\n";
    } while (password.length() < 6);
    newStudent.password = password;

    // Auto-generate unique Student ID
    newStudent.studentID = generateStudentID(students);

    students.push_back(newStudent);

    cout << "\nRegistration successful! Your Student ID is: " << newStudent.studentID << "\n";
}

bool studentLogin(vector<Student>& students, string username, string password, string& loggedInID)
{
    for (const auto& student : students)
    {
        if (student.username == username && student.password == password)
        {
            loggedInID = student.studentID;
            return true;
        }
    }
    return false;
}

bool adminLogin(string username, string password)
{
    return (username == ADMIN_USERNAME && password == ADMIN_PASSWORD);
}

void viewProfile(const vector<Student>& students, string studentID)
{
    int idx;
    if (!searchStudent(students, studentID, idx))
    {
        cout << "Profile not found.\n";
        return;
    }

    const Student& s = students[idx];
    cout << "\n----------- STUDENT PROFILE -----------\n";
    cout << left << setw(18) << "Student ID:" << s.studentID << "\n";
    cout << left << setw(18) << "Full Name:" << s.name << "\n";
    cout << left << setw(18) << "Gender:" << s.gender << "\n";
    cout << left << setw(18) << "Age:" << s.age << "\n";
    cout << left << setw(18) << "Phone Number:" << s.phone << "\n";
    cout << left << setw(18) << "School:" << s.school << "\n";
    cout << left << setw(18) << "Username:" << s.username << "\n";
    cout << "----------------------------------------\n";
}

// ================= Admin Functions =================
void addStudent(vector<Student>& students)
{
    cout << "\n----- Admin: Add New Student -----\n";
    // Reuses the same validated input flow as self-registration
    studentRegister(students);
}

void editStudent(vector<Student>& students, string studentID)
{
    int idx;
    if (!searchStudent(students, studentID, idx))
    {
        cout << "Student ID not found. Cannot edit.\n";
        return;
    }

    Student& s = students[idx];
    cout << "\nEditing record for Student ID: " << s.studentID << "\n";
    cout << "Leave field blank and press Enter to keep the current value.\n";

    clearInputBuffer();

    cout << "Current Full Name: " << s.name << "\nNew Full Name: ";
    string input;
    getline(cin, input);
    if (!input.empty()) s.name = input;


    cout << "Current Age: " << s.age << "\nNew Age (13-17, or blank to keep): ";
    getline(cin, input);
    if (!input.empty())
    {
        try
        {
            int newAge = stoi(input);
            if (isValidAge(newAge))
                s.age = newAge;
            else
                cout << "Invalid age. Age not updated.\n";
        }
        catch (...)
        {
            cout << "Invalid input. Age not updated.\n";
        }
    }


    cout << "Current Phone Number: " << s.phone << "\nNew Phone Number (or blank to keep): ";
    getline(cin, input);
    if (!input.empty())
    {
        if (isValidPhone(input))
            s.phone = input;
        else
            cout << "Invalid phone number. Phone not updated.\n";
    }


    cout << "Current School: " << s.school << "\nNew School (or blank to keep): ";
    getline(cin, input);
    if (!input.empty()) s.school = input;


    cout << "Student record updated successfully.\n";
}


void deleteStudent(vector<Student>& students, string studentID)
{
    int idx;
    if (!searchStudent(students, studentID, idx))
    {
        cout << "Student ID not found. Cannot delete.\n";
        return;
    }

    cout << "Are you sure you want to delete student " << students[idx].name
        << " (" << studentID << ")? (Y/N): ";
    char confirmDelete;
    cin >> confirmDelete;
    clearInputBuffer();

    if (toupper(confirmDelete) == 'Y')
    {
        students.erase(students.begin() + idx);
        cout << "Student record deleted successfully.\n";
    }
    else
    {
        cout << "Deletion cancelled.\n";
    }
}

bool searchStudent(const vector<Student>& students, string studentID, int& foundIndex)
{
    for (int i = 0; i < static_cast<int>(students.size()); i++)
    {
        if (students[i].studentID == studentID)
        {
            foundIndex = i;
            return true;
        }
    }
    foundIndex = -1;
    return false;
}

void displayStudents(const vector<Student>& students)
{
    if (students.empty())
    {
        cout << "No student records found.\n";
        return;
    }

    cout << "\n" << left
        << setw(12) << "StudentID"
        << setw(20) << "FullName"
        << setw(6) << "Age"
        << setw(8) << "Gender"
        << setw(15) << "ContactNumber"
        << setw(20) << "School" << "\n";
    cout << string(81, '-') << "\n";

    for (int i = 0; i < static_cast<int>(students.size()); i++)
    {
        cout << left
            << setw(12) << students[i].studentID
            << setw(20) << students[i].name
            << setw(6) << students[i].age
            << setw(8) << students[i].gender
            << setw(15) << students[i].phone
            << setw(20) << students[i].school << "\n";
    }
}

// ================= File Processing =================
// Format per line: studentID|name|gender|age|phone|school|username|password
void loadStudentData(vector<Student>& students)
{
    ifstream inFile(STUDENT_FILE);
    if (!inFile)
    {
        // File doesn't exist yet (first run) - not an error
        return;
    }

    students.clear();
    string line;
    while (getline(inFile, line))
    {
        if (line.empty()) continue;

        stringstream ss(line);
        Student s;
        string ageStr;

        getline(ss, s.studentID, '|');
        getline(ss, s.name, '|');
        getline(ss, s.gender, '|');
        getline(ss, ageStr, '|');
        getline(ss, s.phone, '|');
        getline(ss, s.school, '|');
        getline(ss, s.username, '|');
        getline(ss, s.password, '|');

        try { s.age = stoi(ageStr); }
        catch (...) { s.age = 0; }

        students.push_back(s);
    }
    inFile.close();
}

void saveStudentData(const vector<Student>& students)
{
    ofstream outFile(STUDENT_FILE);
    if (!outFile)
    {
        cout << "Error: Unable to save student data.\n";
        return;
    }

    for (const auto& s : students)
    {
        outFile << s.studentID << "|" << s.name << "|" << s.gender << "|"
            << s.age << "|" << s.phone << "|" << s.school << "|"
            << s.username << "|" << s.password << "\n";
    }
    outFile.close();
}