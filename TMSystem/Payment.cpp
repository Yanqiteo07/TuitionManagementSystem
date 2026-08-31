#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include <ctime>

void loadPaymentsFromFile(vector<Payment>& payments)
{
    ifstream file("payments.txt");
    if (!file.is_open())
    {
        return;
    }
    payments.clear();
    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        Payment p;
        string invID, sid, dt, disPctStr, disAmtStr, dueStr, paidStr, balStr, statStr;
        getline(ss, invID, '|');
        getline(ss, sid, '|');
        getline(ss, dt, '|');
        getline(ss, disPctStr, '|');
        getline(ss, disAmtStr, '|');
        getline(ss, dueStr, '|');
        getline(ss, paidStr, '|');
        getline(ss, balStr, '|');
        getline(ss, statStr, '|');
        p.invoiceID = invID;
        p.studentID = sid;
        p.date = dt;
        p.discountPercent = stod(disPctStr);
        p.discountAmount = stod(disAmtStr);
        p.amountDue = stod(dueStr);
        p.amountPaid = stod(paidStr);
        p.balance = stod(balStr);
        p.status = statStr;
        payments.push_back(p);
    }
    file.close();
}

void savePaymentsToFile(vector<Payment>& payments)
{
    ofstream file("payments.txt");
    if (!file.is_open())
    {
        cout << "Unable to open payments file.\n";
        return;
    }
    for (auto& p : payments)
    {
        file << p.invoiceID << "|"
            << p.studentID << "|"
            << p.date << "|"
            << fixed << setprecision(2) << p.discountPercent << "|"
            << fixed << setprecision(2) << p.discountAmount << "|"
            << fixed << setprecision(2) << p.amountDue << "|"
            << fixed << setprecision(2) << p.amountPaid << "|"
            << fixed << setprecision(2) << p.balance << "|"
            << p.status << endl;
    }
    file.close();
}

void generateInvoice(vector<Payment>& payments,
    const vector<Student>& students,
    const vector<Subject>& subjectList,
    const vector<StudentSubject>& enrolRecords,
    string targetStudentID)
{
    bool invoiceExist = false;
    for (auto& p : payments)
    {
        if (p.studentID == targetStudentID && p.status != "Paid")
        {
            cout << "\nError: Unpaid invoice already exists for this student. Cannot generate duplicate invoice.\n";
            invoiceExist = true;
            break;
        }
    }
    if (invoiceExist)
    {
        return;
    }
    Payment newPayment;
    newPayment.studentID = targetStudentID;
    double originalTotal = calculateMonthlyFee(targetStudentID, enrolRecords, subjectList);
    newPayment.discountPercent = 0.0;
    newPayment.discountAmount = 0.0;
    newPayment.amountDue = originalTotal;
    newPayment.amountPaid = 0.0;
    newPayment.balance = originalTotal;
    newPayment.status = "Unpaid";
    time_t now = time(nullptr);
    newPayment.invoiceID = "INV-" + to_string(now);
    newPayment.date = ctime(&now);
    payments.push_back(newPayment);
    savePaymentsToFile(payments);
    cout << "Invoice generated successfully. Invoice ID: " << newPayment.invoiceID << "\n";
}

void studentPaymentMenu(string studentID,
    vector<Payment>& payments,
    const vector<Student>& students,
    const vector<Subject>& subjectList,
    const vector<StudentSubject>& enrolRecords,
    const vector<ScholarshipApplication>& scholarshipRecords,
    const vector<ScholarshipEligibility>& eligibilityList)
{
    int choice;
    do
    {
        cout << "\n===== STUDENT PAYMENT MENU =====\n";
        cout << "1. Make Payment\n";
        cout << "2. View My Invoices\n";
        cout << "0. Back to Student Menu\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 2);
        switch (choice)
        {
        case 1:
        {
            int targetIdx = -1;
            for (int i = 0; i < payments.size(); i++)
            {
                if (payments[i].studentID == studentID && payments[i].status == "Unpaid")
                {
                    targetIdx = i;
                    break;
                }
            }
            if (targetIdx == -1)
            {
                cout << "No unpaid invoice found. Please wait admin generate invoice.\n";
                break;
            }
            Payment& payRec = payments[targetIdx];

            // 实时读取学生最新奖学金状态
            int schIdx = -1;
            double discountPercent = 0.0;
            double discountAmount = 0.0;
            bool hasApprovedDiscount = false;
            double originalTotal = calculateMonthlyFee(studentID, enrolRecords, subjectList);

            if (scholarshipRecordExists(scholarshipRecords, studentID, schIdx))
            {
                if (scholarshipRecords[schIdx].applicationStatus == "Approved")
                {
                    hasApprovedDiscount = true;
                    string appliedCriteria = scholarshipRecords[schIdx].criteriaName;
                    discountPercent = getDiscountByCriteriaName(appliedCriteria, eligibilityList);
                    discountAmount = originalTotal * discountPercent;

                    // 更新当前invoice的折扣（付款时刻动态套用最新奖学金）
                    payRec.discountPercent = discountPercent;
                    payRec.discountAmount = discountAmount;
                }
            }

            // ====== 没有获批奖学金 → 弹出确认提示 ======
            if (!hasApprovedDiscount)
            {
                char confirmPay;
                cout << "\nNotice: You do not have an approved scholarship discount.\nProceed to pay without discount? (Y/N): ";
                cin >> confirmPay;
                clearInputBuffer();
                if (toupper(confirmPay) != 'Y')
                {
                    cout << "Payment cancelled, return to payment menu.\n";
                    break;
                }
            }

            double finalPayable = originalTotal - discountAmount;
            // balance = 应付总额 - 已经付过的钱（保留历史已付金额，不会重置amountPaid，修复原先bug）
            payRec.balance = finalPayable - payRec.amountPaid;

            cout << "\n==== Fee Breakdown ====\n";
            cout << "Original Monthly Fee: RM " << fixed << setprecision(2) << originalTotal << endl;
            cout << "Discount: " << fixed << setprecision(2) << discountPercent * 100 << "% | RM " << discountAmount << endl;
            cout << "Total Payable after discount: RM " << finalPayable << endl;
            cout << "Already Paid: RM " << fixed << setprecision(2) << payRec.amountPaid << endl;
            cout << "Current Remaining Balance: RM " << fixed << setprecision(2) << payRec.balance << endl;

            double payAmount;
            cout << "\nEnter payment amount(RM): ";
            if (!(cin >> payAmount) || payAmount <= 0)
            {
                cout << "Invalid payment amount.\n";
                clearInputBuffer();
                break;
            }
            clearInputBuffer();

            payRec.amountPaid += payAmount;
            payRec.balance = finalPayable - payRec.amountPaid;

            if (payRec.balance <= 0)
            {
                payRec.status = "Paid";
                payRec.balance = 0;
            }
            savePaymentsToFile(payments);
            cout << "Payment processed successfully.\n";
            if (payRec.status == "Paid")
            {
                cout << "Invoice fully paid.\n";
            }
            else
            {
                cout << "Remaining balance RM " << fixed << setprecision(2) << payRec.balance << "\n";
            }
            break;
        }

        case 2:
        {
            cout << "\n========== MY INVOICES ==========\n";
            bool foundInvoice = false;
            for (const auto& p : payments)
            {
                if (p.studentID == studentID)
                {
                    cout << "Invoice ID: " << p.invoiceID << endl;
                    cout << "Date: " << p.date;
                    cout << "Original Amount: RM " << fixed << setprecision(2) << p.amountDue << endl;
                    cout << "Discount: " << fixed << setprecision(2) << p.discountPercent * 100 << "% RM " << p.discountAmount << endl;
                    cout << "Paid: RM " << fixed << setprecision(2) << p.amountPaid << endl;
                    cout << "Balance: RM " << fixed << setprecision(2) << p.balance << endl;
                    cout << "Status: " << p.status << "\n---------------------------------\n";
                    foundInvoice = true;
                }
            }
            if (!foundInvoice)
            {
                cout << "No invoice records.\n";
            }
            break;
        }
        case 0:
            cout << "Returning to Student Menu.\n";
            break;
        default:
            cout << "Invalid menu choice.\n";
        }
    } while (choice != 0);
}

void adminPaymentMenu(vector<Payment>& payments,
    const vector<Student>& students,
    const vector<Subject>& subjectList,
    const vector<StudentSubject>& enrolRecords)
{
    int choice;
    do
    {
        cout << "\n===== ADMIN PAYMENT MENU =====\n";
        cout << "1. Generate Invoice\n";
        cout << "2. View Payment Report\n";
        cout << "0. Back to Admin Menu\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 2);
        switch (choice)
        {
        case 1:
        {
            string sid = readNonEmptyLine("Enter student ID: ");
            generateInvoice(payments, students, subjectList, enrolRecords, sid);
            break;
        }
        case 2:
        {
            cout << "\n============================== PAYMENT REPORT ==============================\n";
            if (payments.empty())
            {
                cout << "No payment records.\n";
                break;
            }
            cout << left
                << setw(12) << "InvoiceID"
                << setw(12) << "StudentID"
                << setw(12) << "Due(RM)"
                << setw(12) << "Paid(RM)"
                << setw(12) << "Balance(RM)"
                << setw(10) << "Status" << endl;
            cout << "----------------------------------------------------------------------------\n";
            for (const auto& p : payments)
            {
                cout << left
                    << setw(12) << p.invoiceID
                    << setw(12) << p.studentID
                    << setw(12) << fixed << setprecision(2) << p.amountDue
                    << setw(12) << fixed << setprecision(2) << p.amountPaid
                    << setw(12) << fixed << setprecision(2) << p.balance
                    << setw(10) << p.status << endl;
            }
            break;
        }
        case 0:
            cout << "Returning to Admin Menu.\n";
            break;
        default:
            cout << "Invalid menu choice.\n";
        }
    } while (choice != 0);
}
