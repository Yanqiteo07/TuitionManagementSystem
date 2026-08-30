#include "common.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

const double SST = 0.00;
const double MIN_PAYMENT = 10.00;


// Load payment records from payment.txt when program starts
void loadPaymentsFromFile(vector<Payment>& payments) {
    ifstream inFile("payment.txt");
    if (!inFile.is_open()) return;

    payments.clear();
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string item;
        Payment p;

        getline(ss, p.invoiceID, '|');
        getline(ss, p.studentID, '|');
        getline(ss, p.date, '|');
        getline(ss, item, '|'); p.amountDue = stod(item);
        getline(ss, item, '|'); p.amountPaid = stod(item);
        getline(ss, item, '|'); p.balance = stod(item);
        getline(ss, p.status, '|');

        payments.push_back(p);
    }
    inFile.close();
}

// Save updated payment records to payment.txt
void savePaymentsToFile(const vector<Payment>& payments) {
    ofstream outFile("payment.txt");
    if (!outFile.is_open()) return;

    for (const auto& p : payments) {
        outFile << p.invoiceID << "|"
                << p.studentID << "|"
                << p.date << "|"
                << fixed << setprecision(2) << p.amountDue << "|"
                << p.amountPaid << "|"
                << p.balance << "|"
                << p.status << endl;
    }
    outFile.close();
}

// 1. Returns true if a payment record exists for the given Student ID
bool searchPayment(string studentID, const vector<Payment>& payments) {
    for (const auto& p : payments) {
        if (p.studentID == studentID) return true;
    }
    return false;
}

// 2. Updates payment status to "Paid", "Partial", or "Unpaid"
void updatePaymentStatus(Payment &payment) {
    payment.balance = payment.amountDue - payment.amountPaid;
    
    if (payment.balance <= 0.0001) {
        payment.status = "Paid";
        payment.balance = 0.00; // Normalizes floating-point precision issues
    } else if (payment.amountPaid > 0) {
        payment.status = "Partial";
    } else {
        payment.status = "Unpaid";
    }
}

// 3. Calculates total fees including SST and builds the invoice record
void generateInvoice(Invoice &invoice, double totalFee) {
    double finalAmount = totalFee + (totalFee * SST);
    
    invoice.amountDue = finalAmount;
    invoice.amountPaid = 0.00;
    invoice.balance = finalAmount;
    
    updatePaymentStatus(invoice);
}

// 4. Deducts payment amount from balance and updates payment history
void makePayment(Payment &payment, double paymentAmount) {
    payment.amountPaid += paymentAmount;
    updatePaymentStatus(payment);
}

// 5. Formats and prints a transaction summary receipt
void generateReceipt(const Payment &payment) {
    cout << "\n===============================================\n";
    cout << "                   RECEIPT                       \n";
    cout << "===============================================\n";
    cout << "Invoice ID      : " << payment.invoiceID << endl;
    cout << "Student ID      : " << payment.studentID << endl;
    cout << "Date            : " << payment.date << endl;
    cout << "Total Amount Due: RM" << fixed << setprecision(2) << payment.amountDue << endl;
    cout << "Amount Paid     : RM" << fixed << setprecision(2) << payment.amountPaid << endl;
    cout << "Remaining Bal   : RM" << fixed << setprecision(2) << payment.balance << endl;
    cout << "Payment Status  : " << payment.status << endl;
    cout << "===============================================\n";
}

// 6. Aggregates all recorded transactions to calculate total revenue
void paymentReport(const vector<Payment> &payments) {
    double totalRevenue = 0.00;
    double totalOutstanding = 0.00;

    cout << "\n=========================================================================\n";
    cout << "                               INVOICE                                      \n";
    cout << "=========================================================================\n";
    cout << left << setw(18) << "Invoice ID" 
         << setw(14) << "Student ID" 
         << setw(12) << "Due (RM)" 
         << setw(12) << "Paid (RM)" 
         << setw(12) << "Bal (RM)" 
         << setw(10) << "Status" << endl;
    cout << "-------------------------------------------------------------------------\n";

    for (const auto& p : payments) {
        cout << left << setw(18) << p.invoiceID
             << setw(14) << p.studentID
             << "RM" << fixed << setprecision(2) << setw(10) << p.amountDue
             << "RM" << fixed << setprecision(2) << setw(10) << p.amountPaid
             << "RM" << fixed << setprecision(2) << setw(10) << p.balance
             << setw(10) << p.status << endl;

        totalRevenue += p.amountPaid;
        totalOutstanding += p.balance;
    }

    cout << "-------------------------------------------------------------------------\n";
    cout << "TOTAL REVENUE COLLECTED : RM" << fixed << setprecision(2) << totalRevenue << endl;
    cout << "TOTAL OUTSTANDING BAL   : RM" << fixed << setprecision(2) << totalOutstanding << endl;
    cout << "=========================================================================\n";
}

// ==========================================
// USER INTERFACE DRIVERS
// ==========================================

// Student UI: View Tuition Balance, Make Payment, View Receipts
void studentPaymentMenu(string studentID, const vector<Subject>& subjList, const vector<StudentSubject>& enrolList, vector<Payment>& payments) {
    int choice;
    do {
        cout << "\n========== STUDENT PAYMENT MENU ==========\n";
        cout << "1. View Tuition Fee Balance\n";
        cout << "2. Make Tuition Payment\n";
        cout << "3. View Receipts\n";
        cout << "0. Back\n";
        cout << "Enter choice: ";
        choice = readMenuChoice(0, 3);

        if (choice == 1) {
            cout << "\n--- YOUR TUITION FEE BALANCE ---\n";
            if (!searchPayment(studentID, payments)) {
                cout << "No payment records found for your ID.\n";
            } else {
                for (const auto& p : payments) {
                    if (p.studentID == studentID) {
                        cout << "Invoice ID : " << p.invoiceID << endl;
                        cout << "Total Due  : RM" << fixed << setprecision(2) << p.amountDue << endl;
                        cout << "Total Paid : RM" << fixed << setprecision(2) << p.amountPaid << endl;
                        cout << "Balance    : RM" << fixed << setprecision(2) << p.balance << endl;
                        cout << "Status     : " << p.status << endl;
                        cout << "-----------------------------------\n";
                    }
                }
            }
        }
        else if (choice == 2) {
            vector<int> pendingIndices;
            cout << "\n--- SELECT INVOICE TO PAY ---\n";
            for (size_t i = 0; i < payments.size(); i++) {
                if (payments[i].studentID == studentID && payments[i].status != "Paid") {
                    pendingIndices.push_back(i);
                    cout << pendingIndices.size() << ". " << payments[i].invoiceID 
                         << " (Outstanding Balance: RM" << fixed << setprecision(2) << payments[i].balance << ")\n";
                }
            }

            if (pendingIndices.empty()) {
                cout << "You have no outstanding balance to pay!\n";
                continue;
            }

            cout << "Select invoice index: ";
            int idxChoice; 
            if (!(cin >> idxChoice) || idxChoice < 1 || idxChoice > static_cast<int>(pendingIndices.size())) {
                cout << "Invalid selection.\n";
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            int targetIdx = pendingIndices[idxChoice - 1];
            double payAmt;
            cout << "Enter Payment Amount (Min RM" << fixed << setprecision(2) << MIN_PAYMENT << "): RM";
            if (!(cin >> payAmt)) {
                cout << "Invalid input.\n";
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            // Guard Rules explicitly checking payment boundaries
            if (payAmt <= 0) {
                cout << "Error: System rejects negative or zero payment amounts!\n";
            } else if (payAmt < MIN_PAYMENT) {
                cout << "Error: Payment amount rejected! Minimum payment is RM" << fixed << setprecision(2) << MIN_PAYMENT << ".\n";
            } else if (payAmt > payments[targetIdx].balance) {
                cout << "Error: Payment amount rejected! Payment is larger than outstanding balance of RM" 
                     << fixed << setprecision(2) << payments[targetIdx].balance << ".\n";
            } else {
                makePayment(payments[targetIdx], payAmt);
                savePaymentsToFile(payments);
                cout << "\nPayment successful!\n";
                generateReceipt(payments[targetIdx]);
            }
        }
        else if (choice == 3) {
            if (!searchPayment(studentID, payments)) {
                cout << "No receipts available.\n";
            } else {
                for (const auto& p : payments) {
                    if (p.studentID == studentID) {
                        generateReceipt(p);
                    }
                }
            }
        }
    } while (choice != 0);
}

// Administrator UI: Generate Invoice, View Financial Payment Report
void adminPaymentMenu(const vector<Student>& students, const vector<Subject>& subjList, const vector<StudentSubject>& enrolList, const vector<Scholarship>& scholarships, vector<Payment>& payments) {
    int choice;
    do {
        cout << "\n========== ADMIN PAYMENT & REPORTS MENU ==========\n";
        cout << "1. Generate Invoice Record\n";
        cout << "2. Generate Financial Payment Report\n";
        cout << "0. Back\n";
        cout << "Enter choice: ";
        choice = readMenuChoice(0, 2);

        if (choice == 1) {
            string sID = readNonEmptyLine("Enter Student ID: ");
            
            // Calculates tuition dynamically from Subject and Scholarship modules
            double rawFee = calculateMonthlyFee(sID, subjList, enrolList);
            if (rawFee <= 0) {
                cout << "Error: Student has no registered subjects!\n";
                continue;
            }

            double discountRate = 0.0;
            int schIdx;
            if (scholarshipRecordExists(scholarships, sID, schIdx)) {
                if (scholarships[schIdx].applicationStatus == "Approved") {
                    discountRate = scholarships[schIdx].discountRate;
                }
            }

            double totalFee = rawFee * (1.0 - discountRate);

            Payment newInv;
            newInv.studentID = sID;
            newInv.date = readNonEmptyLine("Enter Invoice Date (YYYY-MM-DD): ");
            newInv.invoiceID = "INV-" + sID + "-" + newInv.date.substr(0, 7);

            // Calls technical function generateInvoice
            generateInvoice(newInv, totalFee);

            payments.push_back(newInv);
            savePaymentsToFile(payments);
            
            cout << "\nInvoice generated successfully!\n";
            generateReceipt(newInv);
        }
        else if (choice == 2) {
            paymentReport(payments);
        }
    } while (choice != 0);
}

            Payment newInv;
            newInv.studentID = sID;
            newInv.date = readNonEmptyLine("Enter Invoice Date (YYYY-MM-DD): ");
            newInv.invoiceID = "INV-" + sID + "-" + newInv.date.substr(0, 7);

            // Call technical function generateInvoice
            generateInvoice(newInv, totalFee);

            payments.push_back(newInv);
            savePaymentsToFile(payments);
            cout << "Invoice generated successfully!\n";
            generateReceipt(newInv);
        }
        else if (choice == 2) {
            paymentReport(payments);
        }
    } while (choice != 0);
}
