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

// ================= Scholarship Criteria (admin-adjustable) =================
// Defaults match the report: >85 average -> 25% scholarship, +5% sibling discount
static double minAverageMark        = 85.0;
static double scholarshipDiscount   = 0.25;
static double siblingDiscount       = 0.05;

const string SCHOLARSHIP_FILE = "scholarship.txt";

// ================= Helper =================
bool scholarshipRecordExists(const vector<Scholarship>& scholarships, string studentID, int& foundIndex)
{
    for (int i = 0; i < static_cast<int>(scholarships.size()); i++)
    {
        if (scholarships[i].studentID == studentID)
        {
            foundIndex = i;
            return true;
        }
    }
    foundIndex = -1;
    return false;
}

// ================= File Processing =================
// Format per line: studentID|averageMark|isSibling(1/0)|discountRate|applicationStatus
void loadScholarshipData(vector<Scholarship>& scholarships)
{
    ifstream inFile(SCHOLARSHIP_FILE);
    if (!inFile.is_open())
    {
        cout << "scholarship.txt not found, starting with no scholarship records.\n";
        return;
    }

    scholarships.clear();
    string line;
    while (getline(inFile, line))
    {
        if (line.empty()) continue;

        stringstream ss(line);
        Scholarship s;
        string avgStr, sibStr, discStr;

        getline(ss, s.studentID, '|');
        getline(ss, avgStr, '|');
        getline(ss, sibStr, '|');
        getline(ss, discStr, '|');
        getline(ss, s.applicationStatus, '|');

        try { s.averageMark  = stod(avgStr); }  catch (...) { s.averageMark  = 0.0; }
        try { s.isSibling    = (stoi(sibStr) == 1); } catch (...) { s.isSibling = false; }
        try { s.discountRate = stod(discStr); } catch (...) { s.discountRate = 0.0; }

        scholarships.push_back(s);
    }
    inFile.close();
}

void saveScholarshipData(const vector<Scholarship>& scholarships)
{
    ofstream outFile(SCHOLARSHIP_FILE);
    if (!outFile.is_open())
    {
        cout << "Warning: Cannot open scholarship.txt to save!\n";
        return;
    }

    for (const auto& s : scholarships)
    {
        outFile << s.studentID << "|"
                << fixed << setprecision(2) << s.averageMark << "|"
                << (s.isSibling ? 1 : 0) << "|"
                << fixed << setprecision(4) << s.discountRate << "|"
                << s.applicationStatus << "\n";
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
    cout << "\n----- Scholarship Eligibility Check -----\n";
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

bool applyScholarship(vector<Scholarship>& scholarships, string studentID)
{
    int idx;
    if (scholarshipRecordExists(scholarships, studentID, idx))
    {
        if (scholarships[idx].applicationStatus == "Pending" ||
            scholarships[idx].applicationStatus == "Approved")
        {
            cout << "Error: You already have a " << scholarships[idx].applicationStatus
                 << " scholarship application.\n";
            return false;
        }
        // Previously "Rejected" - allow the student to re-apply below, overwriting the record.
    }

    double averageMark;
    cout << "\n----- Scholarship Application -----\n";
    while (true)
    {
        cout << "Enter your average mark (0-100): ";
        if (!(cin >> averageMark) || averageMark < 0 || averageMark > 100)
        {
            cout << "Invalid input. Average mark must be between 0 and 100.\n";
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        break;
    }

    if (averageMark < minAverageMark)
    {
        cout << "Sorry, your average mark does not meet the minimum requirement ("
             << minAverageMark << "). Application not submitted.\n";
        return false;
    }

    char sibChoice;
    cout << "Do you have a sibling currently enrolled? (Y/N): ";
    cin >> sibChoice;
    clearInputBuffer();
    bool isSibling = (toupper(sibChoice) == 'Y');

    double discount = calculateDiscount(averageMark, isSibling);

    if (idx != -1)
    {
        // Overwrite the previous (rejected) record
        scholarships[idx].averageMark = averageMark;
        scholarships[idx].isSibling = isSibling;
        scholarships[idx].discountRate = discount;
        scholarships[idx].applicationStatus = "Pending";
    }
    else
    {
        Scholarship newApp;
        newApp.studentID = studentID;
        newApp.averageMark = averageMark;
        newApp.isSibling = isSibling;
        newApp.discountRate = discount;
        newApp.applicationStatus = "Pending";
        scholarships.push_back(newApp);
    }

    saveScholarshipData(scholarships);
    cout << "Scholarship application submitted successfully! Status: Pending approval.\n";
    return true;
}

double viewTuitionFeeDiscount(const vector<Scholarship>& scholarships, string studentID)
{
    int idx;
    if (!scholarshipRecordExists(scholarships, studentID, idx))
    {
        cout << "No scholarship/discount record found. Please apply first.\n";
        return 0.0;
    }

    const Scholarship& s = scholarships[idx];
    cout << "\n----- Tuition Fee Discount Details -----\n";
    cout << "Application Status: " << s.applicationStatus << "\n";

    if (s.applicationStatus != "Approved")
    {
        cout << "Discount will be applied once your application is approved.\n";
        return 0.0;
    }

    cout << "Average Mark: " << fixed << setprecision(2) << s.averageMark << "\n";
    cout << "Sibling Discount: " << (s.isSibling ? "Yes (+5%)" : "No") << "\n";
    cout << "Total Discount: " << fixed << setprecision(2) << (s.discountRate * 100) << "%\n";

    return s.discountRate * 100;
}

// ================= Admin Functions =================
void setScholarshipCriteria(double newMinAverageMark, double newScholarshipDiscount, double newSiblingDiscount)
{
    minAverageMark      = newMinAverageMark;
    scholarshipDiscount = newScholarshipDiscount;
    siblingDiscount     = newSiblingDiscount;
    cout << "Scholarship criteria updated successfully.\n";
}

string approveScholarshipApplication(vector<Scholarship>& scholarships, string studentID)
{
    int idx;
    if (!scholarshipRecordExists(scholarships, studentID, idx))
    {
        cout << "No scholarship application found for this Student ID.\n";
        return "not_found";
    }

    Scholarship& s = scholarships[idx];

    if (s.applicationStatus != "Pending")
    {
        cout << "This application is already " << s.applicationStatus << ".\n";
        return s.applicationStatus;
    }

    cout << "\n----- Review Application -----\n";
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

    saveScholarshipData(scholarships);
    return s.applicationStatus;
}

void updateScholarshipInformation(vector<Scholarship>& scholarships, string studentID)
{
    int idx;
    if (!scholarshipRecordExists(scholarships, studentID, idx))
    {
        cout << "No scholarship record found for this Student ID.\n";
        return;
    }

    Scholarship& s = scholarships[idx];
    cout << "\nEditing scholarship record for Student ID: " << s.studentID << "\n";
    cout << "Leave a field blank and press Enter to keep the current value.\n";

    cout << "Current Average Mark: " << s.averageMark << "\nNew Average Mark (0-100, or blank to keep): ";
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

    // Recalculate the discount rate based on the (possibly) updated values
    s.discountRate = calculateDiscount(s.averageMark, s.isSibling);

    cout << "Scholarship record updated. New discount rate: "
         << fixed << setprecision(2) << (s.discountRate * 100) << "%\n";

    saveScholarshipData(scholarships);
}

void generateScholarshipReport(const vector<Scholarship>& scholarships)
{
    if (scholarships.empty())
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

    for (const auto& s : scholarships)
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
    cout << "Total Applications: " << scholarships.size() << "\n";
    cout << "Eligible by Average Mark: " << eligibleCount << "\n";
    cout << "Approved: " << approvedCount << "\n";
    cout << "=====================================================\n";
}

// ================= Menus =================
void studentScholarshipMenu(string studentID, vector<Scholarship>& scholarships)
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
                applyScholarship(scholarships, studentID);
                break;
            case 3:
                viewTuitionFeeDiscount(scholarships, studentID);
                break;
            case 0:
                cout << "Returning to Student Menu...\n";
                break;
            default:
                cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}

void adminScholarshipMenu(vector<Scholarship>& scholarships)
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
                approveScholarshipApplication(scholarships, id);
                break;
            }
            case 3:
            {
                string id = readNonEmptyLine("Enter Student ID to update: ");
                updateScholarshipInformation(scholarships, id);
                break;
            }
            case 4:
                generateScholarshipReport(scholarships);
                break;
            case 0:
                cout << "Returning to Admin Menu...\n";
                break;
            default:
                cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 0);
}
