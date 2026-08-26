#include "common.h"

/*
    ================================================================
    Module 5: Exam Result Tracker
    ================================================================
    - Admin inserts and updates each student's marks for 5 subjects
      (Bahasa Melayu, English, Mathematics, Science, Sejarah).
    - Average mark and grade are calculated automatically.
    - Students can view their own results; admin can search, edit,
      delete, and generate a full result report.

    NOTE FOR MODULE 6 (Scholarship/Discount Eligibility):
    Each record's averageMarks field is the value Module 6 should use
    for scholarship eligibility once the two modules are linked
    (searchExamResult() -> examResults[i].averageMarks), instead of
    the manual average-mark entry currently used there.
    ================================================================
*/

const string EXAM_RESULT_FILE = "examResults.txt";

// ================= Helper =================
bool searchExamResult(const vector<ExamResult>& examResults, string studentID, int& foundIndex)
{
    for (int i = 0; i < static_cast<int>(examResults.size()); i++)
    {
        if (examResults[i].studentID == studentID)
        {
            foundIndex = i;
            return true;
        }
    }
    foundIndex = -1;
    return false;
}

// Auto-generates a new unique Exam ID in the format E001, E002, ...
string generateExamID(const vector<ExamResult>& examResults)
{
    int nextNum = static_cast<int>(examResults.size()) + 1;
    string newID;
    bool exists;
    do
    {
        ostringstream oss;
        oss << "E" << setw(3) << setfill('0') << nextNum;
        newID = oss.str();

        exists = false;
        for (const auto& r : examResults)
        {
            if (r.examID == newID) { exists = true; break; }
        }
        nextNum++;
    } while (exists);

    return newID;
}

// ================= Calculations =================
double calculateAverage(const double marks[], int totalSubjects)
{
    double total = 0.0;
    for (int i = 0; i < totalSubjects; i++)
        total += marks[i];

    return total / totalSubjects;
}

char calculateGrade(double averageMark)
{
    char grade;

    if (averageMark >= 80)
        grade = 'A';
    else if (averageMark >= 70)
        grade = 'B';
    else if (averageMark >= 60)
        grade = 'C';
    else if (averageMark >= 50)
        grade = 'D';
    else
        grade = 'F';

    return grade;
}

// ================= Input Helper =================
// Prompts for one subject's mark, re-prompting until it is within 0-100.
double readSubjectMark(const string& subjectLabel)
{
    double mark;
    while (true)
    {
        cout << "Enter " << subjectLabel << " marks (0-100): ";
        if (!(cin >> mark) || mark < 0 || mark > 100)
        {
            cout << "Invalid marks. Must be between 0 and 100.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        break;
    }
    return mark;
}

// ================= Core Functions =================
bool insertExamResult(vector<ExamResult>& examResults, string studentID, string studentName)
{
    int idx;
    if (searchExamResult(examResults, studentID, idx))
    {
        cout << "Error: An exam record already exists for this Student ID.\n";
        cout << "Use 'Update Exam Marks' instead.\n";
        return false;
    }

    ExamResult newResult;
    newResult.studentID = studentID;
    newResult.studentName = studentName;

    cout << "\n----- Insert Exam Marks for " << studentName << " (" << studentID << ") -----\n";
    newResult.bmMarks          = readSubjectMark("Bahasa Melayu");
    newResult.englishMarks     = readSubjectMark("English");
    newResult.mathematicsMarks = readSubjectMark("Mathematics");
    newResult.scienceMarks     = readSubjectMark("Science");
    newResult.sejarahMarks     = readSubjectMark("Sejarah");

    double marks[TOTAL_SUBJECTS] = {
        newResult.bmMarks, newResult.englishMarks, newResult.mathematicsMarks,
        newResult.scienceMarks, newResult.sejarahMarks
    };

    newResult.averageMarks = calculateAverage(marks, TOTAL_SUBJECTS);
    newResult.grade = calculateGrade(newResult.averageMarks);
    newResult.examID = generateExamID(examResults);

    examResults.push_back(newResult);

    cout << "\nExamination record added successfully.\n";
    cout << "Exam ID: " << newResult.examID
         << " | Average: " << fixed << setprecision(2) << newResult.averageMarks
         << " | Grade: " << newResult.grade << "\n";

    return true;
}

bool updateExamResult(vector<ExamResult>& examResults, string studentID)
{
    int idx;
    if (!searchExamResult(examResults, studentID, idx))
    {
        cout << "Error: No exam record found for this Student ID.\n";
        return false;
    }

    ExamResult& r = examResults[idx];
    cout << "\n----- Update Exam Marks for " << r.studentName << " (" << studentID << ") -----\n";
    cout << "Re-enter all 5 subject marks below.\n";

    r.bmMarks          = readSubjectMark("Bahasa Melayu");
    r.englishMarks     = readSubjectMark("English");
    r.mathematicsMarks = readSubjectMark("Mathematics");
    r.scienceMarks      = readSubjectMark("Science");
    r.sejarahMarks     = readSubjectMark("Sejarah");

    double marks[TOTAL_SUBJECTS] = {
        r.bmMarks, r.englishMarks, r.mathematicsMarks, r.scienceMarks, r.sejarahMarks
    };

    r.averageMarks = calculateAverage(marks, TOTAL_SUBJECTS);
    r.grade = calculateGrade(r.averageMarks);

    cout << "\nExamination record updated successfully.\n";
    cout << "New Average: " << fixed << setprecision(2) << r.averageMarks
         << " | New Grade: " << r.grade << "\n";

    return true;
}

bool deleteExamResult(vector<ExamResult>& examResults, string studentID)
{
    int idx;
    if (!searchExamResult(examResults, studentID, idx))
    {
        cout << "Error: No exam record found for this Student ID.\n";
        return false;
    }

    cout << "Are you sure you want to delete the exam record for "
         << examResults[idx].studentName << " (" << studentID << ")? (Y/N): ";
    char confirm;
    cin >> confirm;
    clearInputBuffer();

    if (toupper(confirm) == 'Y')
    {
        examResults.erase(examResults.begin() + idx);
        cout << "Examination record deleted successfully.\n";
        return true;
    }

    cout << "Deletion cancelled.\n";
    return false;
}

void displayExamResult(const vector<ExamResult>& examResults, string studentID)
{
    int idx;
    if (!searchExamResult(examResults, studentID, idx))
    {
        cout << "No exam record found for this Student ID.\n";
        return;
    }

    const ExamResult& r = examResults[idx];
    cout << "\n----------- EXAMINATION RESULT -----------\n";
    cout << left << setw(18) << "Exam ID:" << r.examID << "\n";
    cout << left << setw(18) << "Student ID:" << r.studentID << "\n";
    cout << left << setw(18) << "Student Name:" << r.studentName << "\n";
    cout << string(43, '-') << "\n";
    cout << left << setw(18) << "Bahasa Melayu:" << fixed << setprecision(2) << r.bmMarks << "\n";
    cout << left << setw(18) << "English:" << r.englishMarks << "\n";
    cout << left << setw(18) << "Mathematics:" << r.mathematicsMarks << "\n";
    cout << left << setw(18) << "Science:" << r.scienceMarks << "\n";
    cout << left << setw(18) << "Sejarah:" << r.sejarahMarks << "\n";
    cout << string(43, '-') << "\n";
    cout << left << setw(18) << "Average Marks:" << r.averageMarks << "\n";
    cout << left << setw(18) << "Grade:" << r.grade << "\n";
    cout << "-------------------------------------------\n";
}

void generateResultReport(const vector<ExamResult>& examResults)
{
    if (examResults.empty())
    {
        cout << "No examination records found.\n";
        return;
    }

    cout << "\n================= STUDENT RESULT REPORT =================\n";
    cout << left
         << setw(10) << "ExamID"
         << setw(12) << "StudentID"
         << setw(18) << "Name"
         << setw(10) << "Average"
         << setw(6)  << "Grade" << "\n";
    cout << string(56, '-') << "\n";

    double totalOfAverages = 0.0;
    int gradeCount[5] = {0, 0, 0, 0, 0}; // A B C D F

    for (const auto& r : examResults)
    {
        cout << left
             << setw(10) << r.examID
             << setw(12) << r.studentID
             << setw(18) << r.studentName
             << setw(10) << fixed << setprecision(2) << r.averageMarks
             << setw(6)  << r.grade << "\n";

        totalOfAverages += r.averageMarks;

        switch (r.grade)
        {
            case 'A': gradeCount[0]++; break;
            case 'B': gradeCount[1]++; break;
            case 'C': gradeCount[2]++; break;
            case 'D': gradeCount[3]++; break;
            default:  gradeCount[4]++; break;
        }
    }

    cout << string(56, '-') << "\n";
    cout << "Total Students: " << examResults.size() << "\n";
    cout << "Class Average: " << fixed << setprecision(2)
         << (totalOfAverages / examResults.size()) << "\n";
    cout << "Grade Distribution -> A:" << gradeCount[0]
         << "  B:" << gradeCount[1]
         << "  C:" << gradeCount[2]
         << "  D:" << gradeCount[3]
         << "  F:" << gradeCount[4] << "\n";
    cout << "===========================================================\n";
}

// ================= File Processing =================
// Format per line: examID|studentID|studentName|bm|english|math|science|sejarah|average|grade
void loadExamResultData(vector<ExamResult>& examResults)
{
    ifstream inFile(EXAM_RESULT_FILE);
    if (!inFile.is_open())
    {
        cout << "examResults.txt not found, starting with no exam records.\n";
        return;
    }

    examResults.clear();
    string line;
    while (getline(inFile, line))
    {
        if (line.empty()) continue;

        stringstream ss(line);
        ExamResult r;
        string bmStr, engStr, mathStr, sciStr, sejStr, avgStr, gradeStr;

        getline(ss, r.examID, '|');
        getline(ss, r.studentID, '|');
        getline(ss, r.studentName, '|');
        getline(ss, bmStr, '|');
        getline(ss, engStr, '|');
        getline(ss, mathStr, '|');
        getline(ss, sciStr, '|');
        getline(ss, sejStr, '|');
        getline(ss, avgStr, '|');
        getline(ss, gradeStr, '|');

        try { r.bmMarks          = stod(bmStr); }   catch (...) { r.bmMarks = 0; }
        try { r.englishMarks     = stod(engStr); }  catch (...) { r.englishMarks = 0; }
        try { r.mathematicsMarks = stod(mathStr); } catch (...) { r.mathematicsMarks = 0; }
        try { r.scienceMarks     = stod(sciStr); }  catch (...) { r.scienceMarks = 0; }
        try { r.sejarahMarks     = stod(sejStr); }  catch (...) { r.sejarahMarks = 0; }
        try { r.averageMarks     = stod(avgStr); }  catch (...) { r.averageMarks = 0; }
        r.grade = gradeStr.empty() ? 'F' : gradeStr[0];

        examResults.push_back(r);
    }
    inFile.close();
}

void saveExamResultData(const vector<ExamResult>& examResults)
{
    ofstream outFile(EXAM_RESULT_FILE);
    if (!outFile.is_open())
    {
        cout << "Warning: Cannot open examResults.txt to save!\n";
        return;
    }

    for (const auto& r : examResults)
    {
        outFile << r.examID << "|" << r.studentID << "|" << r.studentName << "|"
                << fixed << setprecision(2)
                << r.bmMarks << "|" << r.englishMarks << "|" << r.mathematicsMarks << "|"
                << r.scienceMarks << "|" << r.sejarahMarks << "|" << r.averageMarks << "|"
                << r.grade << "\n";
    }
    outFile.close();
}

// ================= Menus =================
void studentExamMenu(string studentID, const vector<ExamResult>& examResults)
{
    int choice;
    do
    {
        cout << "\n========== STUDENT EXAM RESULT MENU ==========\n";
        cout << "1. View Exam Result\n";
        cout << "2. View Subject Marks\n";
        cout << "3. View Average Marks\n";
        cout << "0. Back to Student Menu\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        int idx;
        switch (choice)
        {
            case 1:
                displayExamResult(examResults, studentID);
                break;
            case 2:
                if (searchExamResult(examResults, studentID, idx))
                {
                    const ExamResult& r = examResults[idx];
                    cout << "\n----- Subject Marks -----\n";
                    cout << left << setw(16) << "Bahasa Melayu:" << fixed << setprecision(2) << r.bmMarks << "\n";
                    cout << left << setw(16) << "English:" << r.englishMarks << "\n";
                    cout << left << setw(16) << "Mathematics:" << r.mathematicsMarks << "\n";
                    cout << left << setw(16) << "Science:" << r.scienceMarks << "\n";
                    cout << left << setw(16) << "Sejarah:" << r.sejarahMarks << "\n";
                }
                else
                {
                    cout << "No exam record found. Please check back after results are released.\n";
                }
                break;
            case 3:
                if (searchExamResult(examResults, studentID, idx))
                {
                    cout << "\nYour Average Marks: " << fixed << setprecision(2)
                         << examResults[idx].averageMarks
                         << " (Grade " << examResults[idx].grade << ")\n";
                }
                else
                {
                    cout << "No exam record found. Please check back after results are released.\n";
                }
                break;
            case 0:
                cout << "Returning to Student Menu...\n";
                break;
            default:
                cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}

void adminExamMenu(vector<ExamResult>& examResults, const vector<Student>& students)
{
    int choice;
    do
    {
        cout << "\n========== ADMIN EXAM RESULT MENU ==========\n";
        cout << "1. Insert Exam Marks for Student\n";
        cout << "2. Update Exam Marks\n";
        cout << "3. Delete Exam Record\n";
        cout << "4. Search / Display Exam Result\n";
        cout << "5. Generate Student Result Report\n";
        cout << "0. Back to Admin Menu\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
            case 1:
            {
                string id = readNonEmptyLine("Enter Student ID: ");
                int sIdx;
                if (!searchStudent(students, id, sIdx))
                {
                    cout << "Error: Student ID not found in Student Management records.\n";
                    break;
                }
                insertExamResult(examResults, id, students[sIdx].name);
                saveExamResultData(examResults);
                break;
            }
            case 2:
            {
                string id = readNonEmptyLine("Enter Student ID: ");
                updateExamResult(examResults, id);
                saveExamResultData(examResults);
                break;
            }
            case 3:
            {
                string id = readNonEmptyLine("Enter Student ID: ");
                deleteExamResult(examResults, id);
                saveExamResultData(examResults);
                break;
            }
            case 4:
            {
                string id = readNonEmptyLine("Enter Student ID: ");
                displayExamResult(examResults, id);
                break;
            }
            case 5:
                generateResultReport(examResults);
                break;
            case 0:
                cout << "Returning to Admin Menu...\n";
                break;
            default:
                cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}
