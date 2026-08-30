#include "common.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>

using namespace std;

// ==========================================
// FILE I/O OPERATIONS
// ==========================================

void savePaymentsToFile(const vector<PaymentRecord>& payments) {
    ofstream outFile("payments.txt");
    if (!outFile.is_open()) {
        cout << "Error: Unable to open payments.txt for writing.\n";
        return;
    }
    for (const auto& p : payments) {
        outFile << p.invoiceID << "|"
                << p.studentID << "|"
                << p.date << "|"
                << fixed << setprecision(2) << p.amountDue << "|"
                << p.amountPaid << "|"
                << p.status << endl;
    }
    outFile.close();
}

void loadPaymentsFromFile(vector<PaymentRecord>& payments) {
    ifstream inFile("payments.txt");
    if (!inFile.is_open()) {
        return; // File will automatically be created upon first save
    }
    payments.clear();
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string item;
        PaymentRecord p;

        getline(ss, p.invoiceID, '|');
        getline(ss, p.studentID, '|');
        getline(ss, p.date, '|');
        getline(ss, item, '|'); p.amountDue = stod(item);
        getline(ss, item, '|'); p.amountPaid = stod(item);
        getline(ss, p.status, '|');

        payments.push_back(p);
    }
    inFile.close();
}

// ==========================================
// CORE INVOICE & PAYMENT LOGIC
// ==========================================

// 1. Generate Invoice (Calculates fees, applies approved scholarship/sibling discounts)
void generateInvoice(string studentID, 
                     const vector<Subject>& subjects, 
                     const vector<StudentSubject>& enrolList, 
                     const vector<Scholarship>& scholarships, 
                     vector<PaymentRecord>& payments) 
{
    double originalFee = calculateMonthlyFee(studentID, subjects, enrolList);
    if (originalFee <= 0) {
        cout << "Error: Student has no registered subjects or total monthly fee is RM0.00.\n";
        return;
    }

    // Check for approved scholarship or sibling discount eligibility
    double discountRate = 0.0;
    int idx;
    if (scholarshipRecordExists(scholarships, studentID, idx)) {
        if (scholarships[idx].applicationStatus == "Approved") {
            discountRate = scholarships[idx].discountRate;
        }
    }

    double finalFee = originalFee * (1.0 - discountRate);

    string date;
    cout << "Enter Invoice Date (YYYY-MM-DD): ";
    cin >> date;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string invoiceID = "INV-" + studentID + "-" + date.substr(0, 7);

    // Prevent duplicated invoices for the same billing period
    for (const auto& p : payments) {
        if (p.invoiceID == invoiceID) {
            cout << "Notice: Invoice " << invoiceID << " already exists for this student.\n";
            return;
        }
    }

    PaymentRecord newInv;
    newInv.invoiceID = invoiceID;
    newInv.studentID = studentID;
    newInv.date = date;
    newInv.amountDue = finalFee;
    newInv.amountPaid = 0.0;
    newInv.status = "UNPAID";

    payments.push_back(newInv);
    savePaymentsToFile(payments);

    cout << "\n==========================================\n";
    cout << "       INVOICE GENERATED SUCCESSFULLY     \n";
    cout << "==========================================\n";
    cout << "Invoice ID      : " << newInv.invoiceID << endl;
    cout << "Student ID      : " << newInv.studentID << endl;
    cout << "Date            : " << newInv.date << endl;
    cout << "Original Fee    : RM" << fixed << setprecision(2) << originalFee << endl;
    cout << "Discount Applied: " << fixed << setprecision(2) << (discountRate * 100) << "%" << endl;
    cout << "Final Amount Due: RM" << fixed << setprecision(2) << newInv.amountDue << endl;
    cout << "Status          : " << newInv.status << endl;
    cout << "==========================================\n";
}

// 2. Receive Payment & Generate Receipt
void processPayment(string studentID, vector<PaymentRecord>& payments) {
    vector<int> unpaidIndices;
    cout << "\n==================== PENDING INVOICES ====================\n";
    cout << left << setw(6) << "Index" << setw(18) << "Invoice ID" << setw(12) << "Date" << setw(14) << "Due (RM)" << setw(14) << "Paid (RM)" << endl;
    cout << "----------------------------------------------------------\n";

    for (size_t i = 0; i < payments.size(); i++) {
        if (payments[i].studentID == studentID && payments[i].status != "PAID") {
            unpaidIndices.push_back(i);
            cout << left << setw(6) << unpaidIndices.size()
                 << setw(18) << payments[i].invoiceID
                 << setw(12) << payments[i].date
                 << "RM" << fixed << setprecision(2) << setw(12) << payments[i].amountDue
                 << "RM" << fixed << setprecision(2) << payments[i].amountPaid << endl;
        }
    }

    if (unpaidIndices.empty()) {
        cout << "No pending invoices found for Student ID: " << studentID << endl;
        return;
    }

    int choice;
    cout << "\nSelect invoice index to pay (0 to cancel): ";
    if (!(cin >> choice) || choice < 1 || choice > static_cast<int>(unpaidIndices.size())) {
        cout << "Payment process cancelled or invalid selection.\n";
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int targetIdx = unpaidIndices[choice - 1];
    double outstanding = payments[targetIdx].amountDue - payments[targetIdx].amountPaid;
    
    cout << "Outstanding Balance for " << payments[targetIdx].invoiceID << ": RM" << fixed << setprecision(2) << outstanding << endl;
    cout << "Enter Payment Amount: RM";
    double payAmt;
    if (!(cin >> payAmt) || payAmt <= 0) {
        cout << "Error: Payment amount must be greater than RM0.00!\n";
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    payments[targetIdx].amountPaid += payAmt;
    if (payments[targetIdx].amountPaid >= payments[targetIdx].amountDue) {
        payments[targetIdx].status = "PAID";
    } else {
        payments[targetIdx].status = "PARTIAL";
    }

    savePaymentsToFile(payments);

    // Print Receipt
    cout << "\n===============================================\n";
    cout << "               OFFICIAL RECEIPT                \n";
    cout << "===============================================\n";
    cout << "Invoice ID   : " << payments[targetIdx].invoiceID << endl;
    cout << "Student ID   : " << payments[targetIdx].studentID << endl;
    cout << "Amount Paid  : RM" << fixed << setprecision(2) << payAmt << endl;
    cout << "Total Paid   : RM" << fixed << setprecision(2) << payments[targetIdx].amountPaid << endl;
    cout << "Balance Due  : RM" << fixed << setprecision(2) << max(0.0, payments[targetIdx].amountDue - payments[targetIdx].amountPaid) << endl;
    cout << "Status       : " << payments[targetIdx].status << endl;
    cout << "===============================================\n";
}

// 3. View Payment History & Balance Calculation
void viewPaymentHistory(string studentID, const vector<PaymentRecord>& payments) {
    cout << "\n==================== PAYMENT HISTORY ====================" << endl;
    cout << left << setw(18) << "Invoice ID" << setw(12) << "Date" << setw(14) << "Amount (RM)" << setw(14) << "Paid (RM)" << setw(10) << "Status" << endl;
    cout << "---------------------------------------------------------\n";

    bool found = false;
    double totalPaid = 0.0;
    double totalBalance = 0.0;

    for (const auto& p : payments) {
        if (p.studentID == studentID) {
            found = true;
            totalPaid += p.amountPaid;
            double balance = p.amountDue - p.amountPaid;
            if (balance > 0) totalBalance += balance;

            cout << left << setw(18) << p.invoiceID
                 << setw(12) << p.date
                 << "RM" << fixed << setprecision(2) << setw(12) << p.amountDue
                 << "RM" << fixed << setprecision(2) << setw(12) << p.amountPaid
                 << setw(10) << p.status << endl;
        }
    }

    if (!found) {
        cout << "No payment records found.\n";
    } else {
        cout << "---------------------------------------------------------\n";
        cout << "TOTAL AMOUNT PAID      : RM" << fixed << setprecision(2) << totalPaid << endl;
        cout << "TOTAL REMAINING BALANCE: RM" << fixed << setprecision(2) << totalBalance << endl;
    }
    cout << "=========================================================\n";
}

// ==========================================
// REPORTS & ANALYTICS LOGIC
// ==========================================

// 1. Monthly Income Report
void generateMonthlyIncomeReport(const vector<PaymentRecord>& payments) {
    string targetMonth;
    cout << "Enter Month (YYYY-MM): ";
    cin >> targetMonth;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    double totalRevenue = 0.0;
    int count = 0;

    cout << "\n========== MONTHLY INCOME REPORT (" << targetMonth << ") ==========\n";
    cout << left << setw(18) << "Invoice ID" << setw(14) << "Student ID" << setw(14) << "Amount Paid" << endl;
    cout << "---------------------------------------------------\n";

    for (const auto& p : payments) {
        if (p.date.substr(0, 7) == targetMonth && p.amountPaid > 0) {
            cout << left << setw(18) << p.invoiceID
                 << setw(14) << p.studentID
                 << "RM" << fixed << setprecision(2) << p.amountPaid << endl;
            totalRevenue += p.amountPaid;
            count++;
        }
    }
    cout << "---------------------------------------------------\n";
    cout << "Total Transactions : " << count << endl;
    cout << "Total Monthly Income: RM" << fixed << setprecision(2) << totalRevenue << endl;
    cout << "===================================================\n";
}

// 2. Outstanding Payment Report
void generateOutstandingReport(const vector<PaymentRecord>& payments) {
    double totalOutstanding = 0.0;
    cout << "\n================ OUTSTANDING PAYMENTS REPORT ================\n";
    cout << left << setw(18) << "Invoice ID" << setw(14) << "Student ID" << setw(14) << "Due (RM)" << setw(14) << "Paid (RM)" << setw(14) << "Balance (RM)" << endl;
    cout << "-------------------------------------------------------------\n";

    for (const auto& p : payments) {
        if (p.status != "PAID") {
            double bal = p.amountDue - p.amountPaid;
            totalOutstanding += bal;
            cout << left << setw(18) << p.invoiceID
                 << setw(14) << p.studentID
                 << "RM" << fixed << setprecision(2) << setw(12) << p.amountDue
                 << "RM" << fixed << setprecision(2) << setw(12) << p.amountPaid
                 << "RM" << fixed << setprecision(2) << bal << endl;
        }
    }
    cout << "-------------------------------------------------------------\n";
    cout << "TOTAL OUTSTANDING BALANCE: RM" << fixed << setprecision(2) << totalOutstanding << endl;
    cout << "=============================================================\n";
}

// 3. Student & Subject Statistics Report
void generateStudentStatistics(const vector<Student>& students, const vector<Subject>& subjects, const vector<StudentSubject>& enrolList) {
    cout << "\n================ STUDENT & SUBJECT STATISTICS ================\n";
    cout << "Total Registered Students: " << students.size() << endl;

    map<string, int> subjectCounts;
    for (const auto& en : enrolList) {
        subjectCounts[en.subjectCode]++;
    }

    cout << "\nEnrollment Breakdown by Subject:\n";
    string mostPopularCode = "";
    int maxEnrol = -1;

    for (const auto& sub : subjects) {
        int count = subjectCounts[sub.subjectCode];
        cout << "  - " << left << setw(10) << sub.subjectCode << setw(25) << sub.subjectName << ": " << count << " students" << endl;
        if (count > maxEnrol) {
            maxEnrol = count;
            mostPopularCode = sub.subjectCode + " (" + sub.subjectName + ")";
        }
    }

    cout << "-------------------------------------------------------------\n";
    if (maxEnrol > 0) {
        cout << "Most Popular Subject : " << mostPopularCode << " with " << maxEnrol << " enrollments." << endl;
    } else {
        cout << "Most Popular Subject : N/A (No enrollments recorded)" << endl;
    }
    cout << "=============================================================\n";
}

// ==========================================
// MENU DRIVERS
// ==========================================

void studentPaymentMenu(string studentID, const vector<Subject>& subjList, const vector<StudentSubject>& enrolList, vector<PaymentRecord>& payments) {
    int choice;
    do {
        cout << "\n========== STUDENT PAYMENT MENU ==========" << endl;
        cout << "1. View Registered Subjects & Fee Breakdown" << endl;
        cout << "2. View Payment History & Calculate Balance" << endl;
        cout << "3. Pay Invoice" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Enter choice: ";

        choice = readMenuChoice(0, 3);

        switch (choice) {
            case 1:
                viewRegisteredSubjects(studentID, subjList, enrolList);
                break;
            case 2:
                viewPaymentHistory(studentID, payments);
                break;
            case 3:
                processPayment(studentID, payments);
                break;
            case 0:
                cout << "Returning to previous menu...\n";
                break;
        }
    } while (choice != 0);
}

void adminPaymentMenu(const vector<Student>& students, const vector<Subject>& subjList, const vector<StudentSubject>& enrolList, const vector<Scholarship>& scholarships, vector<PaymentRecord>& payments) {
    int choice;
    do {
        cout << "\n========== ADMIN PAYMENT & REPORTS MENU ==========" << endl;
        cout << "1. Generate Invoice" << endl;
        cout << "2. Receive Payment & Generate Receipt" << endl;
        cout << "3. Outstanding Payment Report" << endl;
        cout << "4. Monthly Income Report" << endl;
        cout << "5. Student & Subject Statistics" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Enter option: ";

        choice = readMenuChoice(0, 5);

        switch (choice) {
            case 1: {
                string sID = readNonEmptyLine("Enter Student ID to generate invoice: ");
                generateInvoice(sID, subjList, enrolList, scholarships, payments);
                break;
            }
            case 2: {
                string sID = readNonEmptyLine("Enter Student ID: ");
                processPayment(sID, payments);
                break;
            }
            case 3:
                generateOutstandingReport(payments);
                break;
            case 4:
                generateMonthlyIncomeReport(payments);
                break;
            case 5:
                generateStudentStatistics(students, subjList, enrolList);
                break;
            case 0:
                cout << "Returning to previous menu...\n";
                break;
        }
    } while (choice != 0);
}
