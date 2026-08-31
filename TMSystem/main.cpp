#include "common.h"
#include <algorithm>

string toUpperStr(string s)
{
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return s;
}

void extendedMainMenu()
{
    vector<Student> students;
    vector<Subject> subjectList;
    vector<StudentSubject> enrolRecords;
    vector<ScholarshipApplication> scholarshipRecords;
    vector<Schedule> scheduleList;
    vector<ExamResult> examResultList;
    vector<Payment> payments;
    vector<ScholarshipEligibility> eligibilityList;

    loadStudentData(students);
    loadSubjectFromFile(subjectList);
    loadEnrolFromFile(enrolRecords);
    loadScholarshipApplicationData(scholarshipRecords);
    loadScheduleData(scheduleList);
    loadExamResultFromFile(examResultList);
    loadPaymentsFromFile(payments);
    loadEligibilityFromFile(eligibilityList);

    if (subjectList.empty())
    {
        subjectList.push_back({ "BM01", "Bahasa Melayu", 120.00 });
        subjectList.push_back({ "ENG01", "English", 120.00 });
        subjectList.push_back({ "SEJ01", "Sejarah", 110.00 });
        subjectList.push_back({ "SCI01", "Science", 130.00 });
        subjectList.push_back({ "MATH01", "Mathematics", 130.00 });

        saveSubjectToFile(subjectList);
    }


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

        choice = readMenuChoice(0, 2);

        if (choice == 1)
        {
            int stuOpt;
            do
            {
                cout << "\n----------- STUDENT MENU -----------\n";
                cout << "1. Register\n";
                cout << "2. Login\n";
                cout << "0. Back to Main Menu\n";
                cout << "Enter your choice: ";

                stuOpt = readMenuChoice(0, 2);

                if (stuOpt == 1) {
                    studentRegister(students);
                    saveStudentData(students);
                }
                else if (stuOpt == 2) {
                    string u, p, loggedID;
                    u = readNonEmptyLine("Enter username: ");
                    p = readNonEmptyLine("Enter password: ");
                    if (studentLogin(students, u, p, loggedID))

                    {
                        cout << "Login successful!\n";

                        int stuMainOpt;
                        do
                        {
                            cout << "\n===== STUDENT MENU =====\n";
                            cout << "1. Account\n";
                            cout << "2. Subject Registration\n";
                            cout << "3. Schedule\n";
                            cout << "4. Payment\n";
                            cout << "5. Exam Result\n";
                            cout << "6. Scholarship\n";
                            cout << "0. Logout\n";
                            cout << "Enter your choice: ";

                            stuMainOpt = readMenuChoice(0, 6);

                            switch (stuMainOpt)
                            {
                            case 1:
                                studentProfileMenu(loggedID, students);
                                break;

                            case 2:
                                studentSubjectMenu(loggedID, subjectList, enrolRecords);
                                break;

                            case 3:
                                studentScheduleMenu(loggedID, scheduleList);
                                break;

                            case 4:
                                studentPaymentMenu(loggedID, payments, students, subjectList, enrolRecords, scholarshipRecords, eligibilityList);

                                break;

                            case 5:
                                studentExamMenu(examResultList, loggedID);

                                break;

                            case 6:
                                studentScholarshipMenu(loggedID, scholarshipRecords);
                                break;

                            case 0:
                                cout << "Logging out... Return to student login menu.\n";
                                break;

                            default:
                                cout << "Invalid menu choice! Please try again.\n";
                            }
                        } while (stuMainOpt != 0);
                    }
                    else
                    {
                        cout << "Login failed.\n";
                    }
                }
            } while (stuOpt != 0);
        }
        else if (choice == 2)
        {
            string adminUser, adminPass;
            adminUser = readNonEmptyLine("Enter admin username: ");
            adminPass = readNonEmptyLine("Enter admin password: ");

            if (!adminLogin(adminUser, adminPass))
            {
                cout << "Admin login failed. Invalid credentials.\n";
            }
            else
            {
                cout << "Admin login successful!\n";

                int adminTopOpt;
                do
                {
                    cout << "\n===== ADMIN MENU =====\n";
                    cout << "1. Student Management\n";
                    cout << "2. Subject Registration\n";
                    cout << "3. Schedule\n";
                    cout << "4. Payment\n";
                    cout << "5. Exam Result\n";
                    cout << "6. Scholarship\n";
                    cout << "0. Logout\n";
                    cout << "Enter your choice: ";

                    adminTopOpt = readMenuChoice(0, 6);

                    switch (adminTopOpt)
                    {
                    case 1:
                        adminMenu(students);
                        break;

                    case 2:
                        adminSubjectMenu(subjectList, enrolRecords);
                        break;

                    case 3:
                        adminScheduleMenu(scheduleList);
                        break;

                    case 4:
                        adminPaymentMenu(payments, students, subjectList, enrolRecords);
                        break;

                    case 5:
                        adminExamResultMenu(examResultList, students, subjectList);
                        break;

                    case 6:
                        adminScholarshipMenu(scholarshipRecords);
                        break;

                    case 0:
                        cout << "Admin logging out, return to main menu.\n";
                        break;

                    default:
                        cout << "Invalid menu choice! Please try again.\n";
                    }
                } while (adminTopOpt != 0);

            }
        }
        else if (choice == 0)
        {
            saveStudentData(students);
            saveSubjectToFile(subjectList);
            saveEnrolToFile(enrolRecords);
            savePaymentsToFile(payments);
            saveScheduleData(scheduleList);
            cout << "Exiting.\n";
        }
    } while (choice != 0);
}

int main()
{
    extendedMainMenu();
    return 0;
}