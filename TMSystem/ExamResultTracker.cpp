/*
============================================================
AMCS2123 - Systems And Programming Concepts
Module 5: Exam Result Management
============================================================
This module handles:
    - Admin add, edit, delete student exam marks (original code restored)
    - Student exam result sub‑menu (loop menu: view / return)
    - File processing: examresults.txt permanent storage
============================================================
*/
#include "common.h"

void calculateGrade(ExamResult& res)
{
    if (res.mark >= 80)
        res.grade = "A";
    else if (res.mark >= 70)
        res.grade = "B";
    else if (res.mark >= 60)
        res.grade = "C";
    else if (res.mark >= 50)
        res.grade = "D";
    else
        res.grade = "F";
}

void saveExamResultToFile(vector<ExamResult>& resultList)
{
    ofstream out("examresults.txt");
    if (!out.is_open())
    {
        cout << "Warning: Cannot open examresults.txt for saving!\n";
        return;
    }
    for (size_t i = 0; i < resultList.size(); i++)
    {
        out << resultList[i].studentID << "|"
            << resultList[i].subjectCode << "|"
            << fixed << setprecision(2) << resultList[i].mark << "|"
            << resultList[i].grade << endl;
    }
    out.close();
}

void loadExamResultFromFile(vector<ExamResult>& resultList)
{
    ifstream in("examresults.txt");
    resultList.clear();
    if (!in.is_open())
    {
        cout << "Notice: examresults.txt not found, will create new file.\n";
        return;
    }
    string line;
    while (getline(in, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        string sID, subCode, markStr, grd;
        getline(ss, sID, '|');
        getline(ss, subCode, '|');
        getline(ss, markStr, '|');
        getline(ss, grd, '|');

        double markVal;
        try
        {
            markVal = stod(markStr);
        }
        catch (...)
        {
            cout << "Warning: skip corrupted line in examresults.txt\n";
            continue;
        }

        ExamResult temp;
        temp.studentID = sID;
        temp.subjectCode = subCode;
        temp.mark = markVal;
        temp.grade = grd;
        resultList.push_back(temp);
    }
    in.close();
}

void adminExamResultMenu(vector<ExamResult>& resultList, vector<Student>& students, vector<Subject>& subjList)
{
    int opt;
    do
    {
        cout << "\n======== ADMIN EXAM RESULT MENU ========\n";
        cout << "1. Add Exam Result\n";
        cout << "2. Edit Exam Result\n";
        cout << "3. Delete Exam Result\n";
        cout << "4. Display All Exam Results\n";
        cout << "0. Return Admin Menu\n";
        cout << "Enter option: ";
        cin >> opt;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opt)
        {
        case 1:
        {
            ExamResult newRes;
            cout << "Enter Student ID: ";
            getline(cin, newRes.studentID);
            cout << "Enter Subject Code: ";
            getline(cin, newRes.subjectCode);
            cout << "Enter Mark (0‑100): ";
            cin >> newRes.mark;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            calculateGrade(newRes);
            resultList.push_back(newRes);
            saveExamResultToFile(resultList);
            cout << "Exam result added.\n";
            break;
        }
        case 2:
        {
            string sid, scode;
            cout << "Enter Student ID: ";
            getline(cin, sid);
            cout << "Enter Subject Code: ";
            getline(cin, scode);
            bool found = false;
            for (size_t i = 0; i < resultList.size(); i++)
            {
                if (resultList[i].studentID == sid && resultList[i].subjectCode == scode)
                {
                    cout << "Enter new mark: ";
                    cin >> resultList[i].mark;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    calculateGrade(resultList[i]);
                    saveExamResultToFile(resultList);
                    cout << "Updated successfully.\n";
                    found = true;
                    break;
                }
            }
            if (!found)
                cout << "Record not found.\n";
            break;
        }
        case 3:
        {
            string sid, scode;
            cout << "Enter Student ID: ";
            getline(cin, sid);
            cout << "Enter Subject Code: ";
            getline(cin, scode);
            for (size_t i = 0; i < resultList.size(); i++)
            {
                if (resultList[i].studentID == sid && resultList[i].subjectCode == scode)
                {
                    resultList.erase(resultList.begin() + i);
                    saveExamResultToFile(resultList);
                    cout << "Deleted.\n";
                    break;
                }
            }
            break;
        }
        case 4:
        {
            cout << "\n==== Exam Results List ====\n";
            cout << left << setw(12) << "StudentID" << setw(12) << "Subject" << setw(8) << "Mark" << setw(6) << "Grade" << endl;
            for (auto& item : resultList)
            {
                cout << left << setw(12) << item.studentID
                    << setw(12) << item.subjectCode
                    << setw(8) << item.mark
                    << setw(6) << item.grade << endl;
            }
            break;
        }
        case 0:
            cout << "Returning to admin menu...\n";
            break;
        default:
            cout << "Invalid option.\n";
            break;
        }
    } while (opt != 0);
}

void studentExamMenu(vector<ExamResult>& resultList, string loginStudentID)
{
    int opt;
    do
    {
        cout << "\n----- STUDENT EXAM RESULT MENU -----\n";
        cout << "1. View My Exam Result\n";
        cout << "0. Return to Student Menu\n";
        cout << "Enter your choice: ";
        cin >> opt;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (opt == 1)
        {
            cout << "\n======== YOUR EXAMINATION RESULT ========\n";
            bool hasRecord = false;
            cout << left << setw(12) << "Subject Code" << setw(8) << "Mark" << setw(6) << "Grade" << endl;
            for (auto& r : resultList)
            {
                if (r.studentID == loginStudentID)
                {
                    cout << left << setw(12) << r.subjectCode
                        << setw(8) << r.mark
                        << setw(6) << r.grade << endl;
                    hasRecord = true;
                }
            }
            if (!hasRecord)
            {
                cout << "No examination result record available.\n";
            }
        }
        else if (opt == 0)
        {
            cout << "Return back to student menu...\n";
        }
        else
        {
            cout << "Invalid option.\n";
        }
    } while (opt != 0);
}
