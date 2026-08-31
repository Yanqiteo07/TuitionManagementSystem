#include "common.h"
/*
    ================================================================
    Module 6: Scholarship / Discount Eligibility
    ================================================================
    NOTE ON INTEGRATION WITH MODULE 5 (Exam Result Tracker):
    The report links scholarship eligibility to a student's average
    exam mark (ExamResult.averageMarks). Since Module 5 is not built
    yet, this module currently asks the student to enter their
    average mark directly when checking eligibility or applying.
    Once Module 5 exists, replace the manual entry with a lookup of
    that student's stored averageMarks (e.g. searchExamResult()).
    ================================================================
*/
// ================= Scholarship Criteria (admin‑adjustable) =================
// Defaults match the report: >85 average -> 25% scholarship, +5% sibling discount
static double minAverageMark = 85.0;
static double scholarshipDiscount = 0.25;
static double siblingDiscount = 0.05;
const string SCHOLARSHIP_RECORD_FILE = "scholarshipRecords.txt";

// ================= Helper =================
bool scholarshipRecordExists(const vector<ScholarshipApplication>& scholarshipRecords, string sid, int& idx)
{
    for (int i = 0; i < scholarshipRecords.size(); i++)
    {
        if (scholarshipRecords[i].studentID == sid)
        {
            idx = i;
            return true;
        }
    }
    idx = -1;
    return false;
}

// ================= File Processing =================
// Format per line: studentID|criteriaName|averageMark|isSibling(1/0)|discountRate|applicationStatus
void loadScholarshipApplicationData(vector<ScholarshipApplication>& scholarshipRecords)
{
    ifstream inFile(SCHOLARSHIP_RECORD_FILE);
    if (!inFile.is_open())
    {
        cout << "scholarshipRecords.txt not found, starting empty.\n";
        return;
    }
    scholarshipRecords.clear();
    string line;
    while (getline(inFile, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        ScholarshipApplication rec;
        string avgStr, sibStr, discStr;

        getline(ss, rec.studentID, '|');
        getline(ss, rec.criteriaName, '|');
        getline(ss, avgStr, '|');
        getline(ss, sibStr, '|');
        getline(ss, discStr, '|');
        getline(ss, rec.applicationStatus, '|');

        try { rec.averageMark = stod(avgStr); }
        catch (...) { rec.averageMark = 0.0; }
        try { rec.isSibling = (stoi(sibStr) == 1); }
        catch (...) { rec.isSibling = false; }
        try { rec.discountRate = stod(discStr); }
        catch (...) { rec.discountRate = 0.0; }

        scholarshipRecords.push_back(rec);
    }
    inFile.close();
}

void saveScholarshipApplicationData(const vector<ScholarshipApplication>& scholarshipRecords)
{
    ofstream outFile(SCHOLARSHIP_RECORD_FILE);
    if (!outFile.is_open())
    {
        cout << "Warning: Cannot open scholarshipRecords.txt to save!\n";
        return;
    }
    for (const auto& rec : scholarshipRecords)
    {
        outFile << rec.studentID << "|"
            << rec.criteriaName << "|"
            << fixed << setprecision(2) << rec.averageMark << "|"
            << (rec.isSibling ? 1 : 0) << "|"
            << fixed << setprecision(4) << rec.discountRate << "|"
            << rec.applicationStatus << "\n";
    }
    outFile.close();
}

void loadEligibilityFromFile(vector<ScholarshipEligibility>& eligibilityList)
{
    ifstream inFile("eligibility.txt");
    if (!inFile.is_open())
    {
        return;
    }
    eligibilityList.clear();
    string line;
    while (getline(inFile, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        ScholarshipEligibility item;
        string discountStr;
        getline(ss, item.criteriaName, '|');
        getline(ss, discountStr, '|');
        item.discountRate = stod(discountStr);
        eligibilityList.push_back(item);
    }
    inFile.close();
}

void saveEligibilityToFile(vector<ScholarshipEligibility>& eligibilityList)
{
    ofstream outFile("eligibility.txt");
    if (!outFile.is_open())
    {
        cout << "Warning: Cannot save eligibility.txt\n";
        return;
    }
    for (auto& e : eligibilityList)
    {
        outFile << e.criteriaName << "|"
            << fixed << setprecision(2) << e.discountRate << endl;
    }
    outFile.close();
}

// ================= Core Functions =================
double calculateDiscount(double averageMark, bool isSibling)
{
    double rate = 0.0;
    if (averageMark >= minAverageMark)
        rate += scholarshipDiscount;
    if (isSibling)
        rate += siblingDiscount;
    return rate;
}

bool viewScholarshipEligibility(string studentID, double averageMark)
{
    cout << "\n---------- Scholarship Eligibility Check ----------\n";
    cout << "Student ID: " << studentID << "\n";
    cout << "Average Mark: " << fixed << setprecision(2) << averageMark << "\n";
    cout << "Minimum Required Average: " << minAverageMark << "\n";
    if (averageMark >= minAverageMark)
    {
        cout << "Result: ELIGIBLE for scholarship.\n";
        return true;
    }
    else
    {
        cout << "Result: NOT ELIGIBLE for scholarship.\n";
        return false;
    }
}

bool applyScholarship(vector<ScholarshipApplication>& applications, string studentID, double avgMark, bool siblingStatus) {
    int idx;
    if (scholarshipRecordExists(applications, studentID, idx))
    {
        if (applications[idx].applicationStatus == "Pending" ||
            applications[idx].applicationStatus == "Approved")
        {
            cout << "Error: You already have a " << applications[idx].applicationStatus
                << " scholarship application.\n";
            return false;
        }
    }
    ScholarshipApplication newApp;
    newApp.studentID = studentID;
    newApp.criteriaName = "";
    newApp.averageMark = avgMark;
    newApp.isSibling = siblingStatus;
    newApp.discountRate = calculateDiscount(avgMark, siblingStatus);
    newApp.applicationStatus = "Pending";

    if (idx != -1)
    {
        applications[idx] = newApp;
    }
    else
    {
        applications.push_back(newApp);
    }
    saveScholarshipApplicationData(applications);
    cout << "Scholarship application submitted successfully, status: Pending\n";
    return true;
}

double viewTuitionFeeDiscount(const vector<ScholarshipApplication>& appRecords, string studentID) {
    int idx;
    if (!scholarshipRecordExists(appRecords, studentID, idx))
    {
        cout << "No scholarship/discount record found. Please apply first.\n";
        return 0.0;
    }
    const ScholarshipApplication& app = appRecords[idx];
    cout << "\n---------- Tuition Fee Discount Details ----------\n";
    cout << "Application Status: " << app.applicationStatus << "\n";
    if (app.applicationStatus != "Approved")
    {
        cout << "Discount will be applied once your application is approved.\n";
        return 0.0;
    }
    cout << "Average Mark: " << fixed << setprecision(2) << app.averageMark << "\n";
    cout << "Sibling Discount: " << (app.isSibling ? "Yes (+5%)" : "No") << "\n";
    cout << "Total Discount: " << fixed << setprecision(2) << (app.discountRate * 100) << "%\n";
    return app.discountRate * 100;
}

// ================= Admin Functions =================
void setScholarshipCriteria(double newMinAverageMark, double newScholarshipDiscount, double newSiblingDiscount)
{
    minAverageMark = newMinAverageMark;
    scholarshipDiscount = newScholarshipDiscount;
    siblingDiscount = newSiblingDiscount;
    cout << "Scholarship criteria updated successfully.\n";
}

string approveScholarshipApplication(vector<ScholarshipApplication>& appRecords, string studentID) {
    int idx;
    if (!scholarshipRecordExists(appRecords, studentID, idx))
    {
        cout << "No scholarship application found for this Student ID.\n";
        return "not_found";
    }
    ScholarshipApplication& s = appRecords[idx];
    if (s.applicationStatus != "Pending")
    {
        cout << "This application is already " << s.applicationStatus << ".\n";
        return s.applicationStatus;
    }
    cout << "\n---------- Review Application ----------\n";
    cout << "Student ID: " << s.studentID << "\n";
    cout << "Average Mark: " << fixed << setprecision(2) << s.averageMark << "\n";
    cout << "Sibling Discount: " << (s.isSibling ? "Yes" : "No") << "\n";
    cout << "Calculated Discount: " << (s.discountRate * 100) << "%\n";
    char decision;
    cout << "Approve this application? (Y/N): ";
    cin >> decision;
    clearInputBuffer();
    if (toupper(decision) == 'Y')
    {
        s.applicationStatus = "Approved";
        cout << "Application approved.\n";
    }
    else
    {
        s.applicationStatus = "Rejected";
        cout << "Application rejected.\n";
    }
    saveScholarshipApplicationData(appRecords);
    return s.applicationStatus;
}

void updateScholarshipInformation(vector<ScholarshipApplication>& appRecords, string studentID)
{
    int idx;
    if (!scholarshipRecordExists(appRecords, studentID, idx))
    {
        cout << "No scholarship record found for this Student ID.\n";
        return;
    }
    ScholarshipApplication& s = appRecords[idx];
    cout << "\nEditing scholarship record for Student ID: " << s.studentID << "\n";
    cout << "Leave a field blank and press Enter to keep the current value.\n";
    cout << "Current Average Mark: " << s.averageMark << "\nNew Average Mark (0‑100, or blank to keep): ";
    string input;
    getline(cin, input);
    if (!input.empty())
    {
        try
        {
            double newAvg = stod(input);
            if (newAvg >= 0 && newAvg <= 100)
                s.averageMark = newAvg;
            else
                cout << "Invalid average mark. Not updated.\n";
        }
        catch (...)
        {
            cout << "Invalid input. Average mark not updated.\n";
        }
    }
    cout << "Current Sibling Status: " << (s.isSibling ? "Yes" : "No")
        << "\nUpdate sibling status? (Y/N, or blank to keep): ";
    getline(cin, input);
    if (!input.empty())
    {
        if (toupper(input[0]) == 'Y')
            s.isSibling = true;
        else if (toupper(input[0]) == 'N')
            s.isSibling = false;
    }
    s.discountRate = calculateDiscount(s.averageMark, s.isSibling);
    cout << "Scholarship record updated. New discount rate: "
        << fixed << setprecision(2) << (s.discountRate * 100) << "%\n";
    saveScholarshipApplicationData(appRecords);
}

void generateScholarshipReport(const vector<ScholarshipApplication>& appRecords)
{
    if (appRecords.empty())
    {
        cout << "No scholarship records found.\n";
        return;
    }
    cout << "\n========== SCHOLARSHIP / DISCOUNT REPORT ==========\n";
    cout << left
        << setw(12) << "StudentID"
        << setw(14) << "AvgMark"
        << setw(10) << "Sibling"
        << setw(12) << "Discount"
        << setw(12) << "Status" << "\n";
    cout << string(60, '-') << "\n";
    int eligibleCount = 0;
    int approvedCount = 0;
    for (const auto& s : appRecords)
    {
        cout << left
            << setw(12) << s.studentID
            << setw(14) << fixed << setprecision(2) << s.averageMark
            << setw(10) << (s.isSibling ? "Yes" : "No")
            << setw(12) << (to_string(static_cast<int>(s.discountRate * 100)) + "%")
            << setw(12) << s.applicationStatus << "\n";
        if (s.averageMark >= minAverageMark) eligibleCount++;
        if (s.applicationStatus == "Approved") approvedCount++;
    }
    cout << string(60, '-') << "\n";
    cout << "Total Applications: " << appRecords.size() << "\n";
    cout << "Eligible by Average Mark: " << eligibleCount << "\n";
    cout << "Approved: " << approvedCount << "\n";
    cout << "=====================================================\n";
}

double getDiscountByCriteriaName(string criteria, const vector<ScholarshipEligibility>& eligibilityList)
{
    for (auto& e : eligibilityList)
    {
        if (e.criteriaName == criteria)
        {
            return e.discountRate;
        }
    }
    return 0.0;
}

// ================= Menus =================
void studentScholarshipMenu(string studentID, vector<ScholarshipApplication>& scholarshipRecords)
{
    int choice;
    do
    {
        cout << "\n========== STUDENT SCHOLARSHIP MENU ==========\n";
        cout << "1. View Scholarship Eligibility\n";
        cout << "2. Apply for Scholarship\n";
        cout << "3. View Tuition Fee Discount\n";
        cout << "0. Back to Student Menu\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 3);
        switch (choice)
        {
        case 1:
        {
            double avgMark;
            cout << "Enter your average mark (0-100) to check eligibility: ";
            while (!(cin >> avgMark) || avgMark < 0 || avgMark > 100)
            {
                cout << "Invalid input. Enter a value between 0 and 100: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            viewScholarshipEligibility(studentID, avgMark);
            break;
        }
        case 2:
        {
            double avgMark;
            char sibChar;
            cout << "Enter your average mark (0-100): ";
            while (!(cin >> avgMark) || avgMark < 0 || avgMark>100)
            {
                cout << "Invalid input: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            cout << "Do you have sibling studying at this tuition centre? (Y/N): ";
            cin >> sibChar;
            clearInputBuffer();
            applyScholarship(scholarshipRecords, studentID, avgMark, (toupper(sibChar) == 'Y'));
            break;
        }
        case 3:
            (void)viewTuitionFeeDiscount(scholarshipRecords, studentID);
            break;
        case 0:
            cout << "Returning to Student Menu...\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}

void adminScholarshipMenu(vector<ScholarshipApplication>& scholarshipRecords)
{
    int choice;
    do
    {
        cout << "\n========== ADMIN SCHOLARSHIP MENU ==========\n";
        cout << "1. Set Scholarship Criteria\n";
        cout << "2. Approve/Reject Scholarship Application\n";
        cout << "3. Update Scholarship Information\n";
        cout << "4. Generate Scholarship Report\n";
        cout << "0. Back to Admin Menu\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 4);
        switch (choice)
        {
        case 1:
        {
            double newMin, newSchoDisc, newSibDisc;
            cout << "Enter minimum average mark (0-100): ";
            while (!(cin >> newMin) || newMin < 0 || newMin > 100)
            {
                cout << "Invalid input. Enter a value between 0 and 100: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            cout << "Enter scholarship discount percentage (0-100): ";
            while (!(cin >> newSchoDisc) || newSchoDisc < 0 || newSchoDisc > 100)
            {
                cout << "Invalid input. Enter a value between 0 and 100: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            cout << "Enter sibling discount percentage (0-100): ";
            while (!(cin >> newSibDisc) || newSibDisc < 0 || newSibDisc > 100)
            {
                cout << "Invalid input. Enter a value between 0 and 100: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            setScholarshipCriteria(newMin, newSchoDisc / 100.0, newSibDisc / 100.0);
            break;
        }
        case 2:
        {
            string id = readNonEmptyLine("Enter Student ID to review: ");
            approveScholarshipApplication(scholarshipRecords, id);
            break;
        }
        case 3:
        {
            string id = readNonEmptyLine("Enter Student ID to update: ");
            updateScholarshipInformation(scholarshipRecords, id);
            break;
        }
        case 4:
            generateScholarshipReport(scholarshipRecords);
            break;
        case 0:
            cout << "Returning to Admin Menu...\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}
