#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>
#include <algorithm>

using namespace std;

// convert whole string to uppercase
string toUpperStr(string s);

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

struct ExamResult {
    string studentID;
    string subjectCode;
    double mark;
    string grade;
};

struct Payment {
    string invoiceID;
    string studentID;
    string date;
    double amountDue;
    double amountPaid;
    double balance;
    string status;
    double discountPercent;
    double discountAmount;
};

struct ScholarshipEligibility
{
    string criteriaName;
    double discountRate;
};

struct ScholarshipApplication
{
    string studentID;
    string criteriaName;
    string applicationStatus;
    double averageMark;
    bool isSibling;
    double discountRate;
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
int readMenuChoice(int min, int max);

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
double calculateMonthlyFee(string studentID, const vector<StudentSubject>& enrolRecords, const vector<Subject>& subjectList);

bool addSubject(vector<Subject>& subjects);
bool editSubject(vector<Subject>& subjects, string editCode);
bool deleteSubject(vector<Subject>& subjects, string delCode, vector<StudentSubject>& enrolList);
void viewAllRegisteredStudents(const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol);

void studentSubjectMenu(string studentID, vector<Subject>& subjList, vector<StudentSubject>& enrolList);
void adminSubjectMenu(vector<Subject>& subjList, vector<StudentSubject>& enrolList);

// Schedule Management

struct Schedule
{
    string studentID;
    string subjectCode;
    string teacherName;
    string classroom;
    int day;
    int hour;
    int minute;
};

// File Processing
void loadScheduleData(vector<Schedule>& schedules);
void saveScheduleData(const vector<Schedule>& schedules);

// Admin Functions
bool createSchedule(Schedule& schedule, vector<Schedule>& schedules);
bool updateSchedule(Schedule& schedule, vector<Schedule>& schedules);
bool searchSchedule(string subjectCode, const vector<Schedule>& schedules);

bool assignTeacher(Schedule& schedule,
    string teacherName,
    const vector<Schedule>& schedules);

bool assignClassroom(Schedule& schedule,
    string classroom,
    const vector<Schedule>& schedules);

void displaySchedule(const vector<Schedule>& schedules);
string formatTime12Hour(int hour24, int minute);
string getDayNameFromNumber(int dayNum);
bool validateClassroomFormat(const string& room);

// Student Function
void viewSchedule(string studentID,
    const vector<Schedule>& schedules);

// Menus
void studentScheduleMenu(string studentID,
    const vector<Schedule>& schedules);

void adminScheduleMenu(vector<Schedule>& schedules);

// Payment & Reports
void loadPaymentsFromFile(vector<Payment>& payments);
void savePaymentsToFile(vector<Payment>& payments);

// Payment Core Operations
void generateInvoice(Payment& invoice, double totalFee);
void makePayment(Payment& payment, double paymentAmount);
void updatePaymentStatus(Payment& payment);
void generateReceipt(const Payment& payment);
void paymentReport(const vector<Payment>& payments);
bool searchPayment(string studentID, const vector<Payment>& payments);

// Payment Menus
void studentPaymentMenu(string studentID,
    vector<Payment>& payments,
    const vector<Student>& students,
    const vector<Subject>& subjectList,
    const vector<StudentSubject>& enrolRecords,
    const vector<ScholarshipApplication>& scholarshipRecords,
    const vector<ScholarshipEligibility>& eligibilityList);
void adminPaymentMenu(vector<Payment>& payments,
    const vector<Student>& students,
    const vector<Subject>& subjectList,
    const vector<StudentSubject>& enrolRecords);
typedef Payment Invoice;

// Exam Result Tracker
const int TOTAL_SUBJECTS = 5;

void saveExamResultToFile(vector<ExamResult>& resultList);
void loadExamResultFromFile(vector<ExamResult>& resultList);

bool searchExamResult(const vector<ExamResult>& examResults, string studentID, int& foundIndex);
double calculateAverage(const double marks[], int totalSubjects);
char calculateGrade(double averageMark);

bool insertExamResult(vector<ExamResult>& examResults, string studentID, string studentName);
bool updateExamResult(vector<ExamResult>& examResults, string studentID);
bool deleteExamResult(vector<ExamResult>& examResults, string studentID);
void displayExamResult(const vector<ExamResult>& examResults, string studentID);
void generateResultReport(const vector<ExamResult>& examResults);

void adminExamResultMenu(vector<ExamResult>& resultList, vector<Student>& students, vector<Subject>& subjList);
void studentExamMenu(vector<ExamResult>& resultList, string loginStudentID);

// Scholarship/Discount Eligibility
bool scholarshipRecordExists(const vector<ScholarshipApplication>& scholarshipRecords, string sid, int& idx);

bool viewScholarshipEligibility(string studentID, double averageMark);
bool applyScholarship(vector<ScholarshipApplication>& applications, string studentID, double avgMark, bool siblingStatus);
double viewTuitionFeeDiscount(const vector<ScholarshipApplication>& appRecords, string studentID);
void setScholarshipCriteria(double minAverageMark, double scholarshipDiscount, double siblingDiscount);
double calculateDiscount(double averageMark, bool isSibling);
string approveScholarshipApplication(vector<ScholarshipApplication>& appRecords, string studentID);
void updateScholarshipInformation(vector<ScholarshipApplication>& appRecords, string studentID);
void generateScholarshipReport(const vector<ScholarshipApplication>& appRecords);
double getDiscountByCriteriaName(string criteria, const vector<ScholarshipEligibility>& eligibilityList);

void studentScholarshipMenu(string studentID, vector<ScholarshipApplication>& scholarshipRecords);
void adminScholarshipMenu(vector<ScholarshipApplication>& scholarshipRecords);

void loadEligibilityFromFile(vector<ScholarshipEligibility>& eligibilityList);
void saveEligibilityToFile(vector<ScholarshipEligibility>& eligibilityList);

void loadScholarshipApplicationData(vector<ScholarshipApplication>& scholarshipRecords);
void saveScholarshipApplicationData(const vector<ScholarshipApplication>& scholarshipRecords);


#endif