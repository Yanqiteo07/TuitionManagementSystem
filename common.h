#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>

using namespace std;

// Login Constants
const int MAX_STUDENTS = 100;
const string STUDENT_FILE = "students.txt";
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

// Structs
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

struct Subject {
    string subjectCode;
    string subjectName;
    double monthlyFee;
};

struct StudentSubject {
    string studentID;
    string subjectCode;
};

struct Scholarship {
    string studentID;
    double averageMark;
    bool isSibling;
    double discountRate;        // stored as a fraction, e.g. 0.25 = 25%
    string applicationStatus;   // "Pending", "Approved", "Rejected"
};

// Student Account & Student Management
void studentRegister(vector<Student>& students);
bool studentLogin(vector<Student>& students, string username, string password, string& loggedInID);
bool adminLogin(string username, string password);
void viewProfile(const vector<Student>& students, string studentID);

void addStudent(vector<Student>& students);
void editStudent(vector<Student>& students, string studentID);
void deleteStudent(vector<Student>& students, string studentID);
bool searchStudent(const vector<Student>& students, string studentID, int& foundIndex);
void displayStudents(const vector<Student>& students);

void loadStudentData(vector<Student>& students);
void saveStudentData(const vector<Student>& students);

bool isValidPhone(const string& phone);
bool isValidAge(int age);
bool usernameExists(const vector<Student>& students, const string& username);
bool studentIDExists(const vector<Student>& students, const string& studentID);
string generateStudentID(const vector<Student>& students);
void clearInputBuffer();
string readNonEmptyLine(const string& prompt);

//void studentMenu(vector<Student>& students); un use
//void mainMenu(); un use
void adminMenu(vector<Student>& students);
void extendedMainMenu();
void studentProfileMenu(string loggedID, const vector<Student>& students);

// Subject Registration
void saveSubjectToFile(const vector<Subject>& subjects);
void loadSubjectFromFile(vector<Subject>& subjects);
void saveEnrolToFile(const vector<StudentSubject>& enrolList);
void loadEnrolFromFile(vector<StudentSubject>& enrolList);

void viewAvailableSubjects(const vector<Subject>& subjects);
bool registerSubject(string studentID, string subjectCode, const vector<Subject>& subjects, vector<StudentSubject>& studentEnrol);
bool dropSubject(string studentID, string subjectCode, vector<StudentSubject>& studentEnrol);
void viewRegisteredSubjects(string studentID, const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol);
double calculateMonthlyFee(string studentID, const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol);

bool addSubject(vector<Subject>& subjects);
bool editSubject(vector<Subject>& subjects, string editCode);
bool deleteSubject(vector<Subject>& subjects, string delCode, vector<StudentSubject>& enrolList);
void viewAllRegisteredStudents(const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol);

void studentSubjectMenu(string studentID, vector<Subject>& subjList, vector<StudentSubject>& enrolList);
void adminSubjectMenu(vector<Subject>& subjList, vector<StudentSubject>& enrolList);

// Schedule Management

// Payment & Reports

// Exam Result Tracker

// Scholarship/Discount Eligibility
void loadScholarshipData(vector<Scholarship>& scholarships);
void saveScholarshipData(const vector<Scholarship>& scholarships);

bool scholarshipRecordExists(const vector<Scholarship>& scholarships, string studentID, int& foundIndex);

bool viewScholarshipEligibility(string studentID, double averageMark);
bool applyScholarship(vector<Scholarship>& scholarships, string studentID);
double viewTuitionFeeDiscount(const vector<Scholarship>& scholarships, string studentID);
void setScholarshipCriteria(double minAverageMark, double scholarshipDiscount, double siblingDiscount);
double calculateDiscount(double averageMark, bool isSibling);
string approveScholarshipApplication(vector<Scholarship>& scholarships, string studentID);
void updateScholarshipInformation(vector<Scholarship>& scholarships, string studentID);
void generateScholarshipReport(const vector<Scholarship>& scholarships);

void studentScholarshipMenu(string studentID, vector<Scholarship>& scholarships);
void adminScholarshipMenu(vector<Scholarship>& scholarships);

#endif
