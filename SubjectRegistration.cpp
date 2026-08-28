#include "common.h"

void saveSubjectToFile(const vector<Subject>& subjects)
{
    ofstream outFile("subjects.txt");
    if (!outFile.is_open())
    {
        cout << "Warning: Cannot open subjects.txt to save!\n";
        return;
    }
    for(size_t i=0; i<subjects.size(); i++)
    {
        outFile << subjects[i].subjectCode << "|"
                << subjects[i].subjectName << "|"
                << fixed << setprecision(2) << subjects[i].monthlyFee << endl;
    }
    outFile.close();
}

void loadSubjectFromFile(vector<Subject>& subjects)
{
    ifstream inFile("subjects.txt");
    if (!inFile.is_open())
    {
        cout << "subjects.txt not found, use default sample data.\n";
        return;
    }
    subjects.clear();
    string line;
    while(getline(inFile, line))
    {
        if(line.empty()) continue;
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1+1);
        Subject s;
        s.subjectCode = line.substr(0, pos1);
        s.subjectName = line.substr(pos1+1, pos2-pos1-1);
        s.monthlyFee = stod(line.substr(pos2+1));
        subjects.push_back(s);
    }
    inFile.close();
}

void saveEnrolToFile(const vector<StudentSubject>& enrolList)
{
    ofstream outFile("registrations.txt");
    if(!outFile.is_open())
    {
        cout << "Warning: Cannot open registrations.txt\n";
        return;
    }
    for(auto &rec : enrolList)
    {
        outFile << rec.studentID << "|" << rec.subjectCode << endl;
    }
    outFile.close();
}

void loadEnrolFromFile(vector<StudentSubject>& enrolList)
{
    ifstream inFile("registrations.txt");
    if(!inFile.is_open())
    {
        cout << "registrations.txt not found.\n";
        return;
    }
    enrolList.clear();
    string line;
    while(getline(inFile, line))
    {
        if(line.empty()) continue;
        size_t pos = line.find('|');
        StudentSubject r;
        r.studentID = line.substr(0,pos);
        r.subjectCode = line.substr(pos+1);
        enrolList.push_back(r);
    }
    inFile.close();
}

void viewAvailableSubjects(const vector<Subject>& subjects)
{
    cout << "\n========== AVAILABLE TUITION SUBJECTS ==========" << endl;
    cout << left << setw(10) << "Code"
         << setw(25) << "Subject Name"
         << setw(12) << "Monthly Fee" << endl;
    cout << "-----------------------------------------" << endl;
    for (size_t i = 0; i < subjects.size(); i++)
    {
        cout << left << setw(10) << subjects[i].subjectCode
             << setw(25) << subjects[i].subjectName
             << "RM" << fixed << setprecision(2) << subjects[i].monthlyFee << endl;
    }
    cout << "=========================================\n";
}

bool registerSubject(string studentID, string subjectCode, const vector<Subject>& subjects, vector<StudentSubject>& studentEnrol)
{
    bool subjExist = false;
    for (size_t i = 0; i < subjects.size(); i++)
    {
        if (subjects[i].subjectCode == subjectCode)
        {
            subjExist = true;
            break;
        }
    }
    if (!subjExist)
    {
        cout << "Error: Invalid Subject Code! Please enter a valid code.\n";
        return false;
    }
    for (size_t i = 0; i < studentEnrol.size(); i++)
    {
        if (studentEnrol[i].studentID == studentID && studentEnrol[i].subjectCode == subjectCode)
        {
            cout << "Error: You have already registered this subject!\n";
            return false;
        }
    }
    StudentSubject newRec;
    newRec.studentID = studentID;
    newRec.subjectCode = subjectCode;
    studentEnrol.push_back(newRec);
    cout << "Registration Successful!\n";
    for(auto &s : subjects){
        if(s.subjectCode == subjectCode){
            cout << "You have registered subject: " << s.subjectName << endl;
            break;
        }
    }
    saveEnrolToFile(studentEnrol);
    return true;
}

bool dropSubject(string studentID, string subjectCode, vector<StudentSubject>& studentEnrol)
{
    for (size_t i = 0; i < studentEnrol.size(); i++)
    {
        if (studentEnrol[i].studentID == studentID && studentEnrol[i].subjectCode == subjectCode)
        {
            studentEnrol.erase(studentEnrol.begin() + i);
            cout << "Subject dropped successfully.\n";
            saveEnrolToFile(studentEnrol);
            return true;
        }
    }
    cout << "Error: You are not registered for this subject.\n";
    return false;
}

void viewRegisteredSubjects(string studentID, const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol)
{
    cout << "\n========== YOUR REGISTERED SUBJECTS ==========" << endl;
    bool hasSubject = false;
    double total = 0.0;
    for (size_t e = 0; e < studentEnrol.size(); e++)
    {
        if (studentEnrol[e].studentID == studentID)
        {
            hasSubject = true;
            string code = studentEnrol[e].subjectCode;
            for (size_t s = 0; s < subjects.size(); s++)
            {
                if (subjects[s].subjectCode == code)
                {
                    cout << left << setw(10) << subjects[s].subjectCode
                         << setw(25) << subjects[s].subjectName
                         << "RM" << fixed << setprecision(2) << subjects[s].monthlyFee << endl;
                    total += subjects[s].monthlyFee;
                    break;
                }
            }
        }
    }
    if (!hasSubject)
    {
        cout << "You have not registered any subject.\n";
    }
    else
    {
        cout << "-------------------------------------" << endl;
        cout << "TOTAL MONTHLY FEE: RM" << fixed << setprecision(2) << total << endl;
    }
    cout << "=====================================\n";
}

double calculateMonthlyFee(string studentID, const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol)
{
    double total = 0.0;
    for (size_t e = 0; e < studentEnrol.size(); e++)
    {
        if (studentEnrol[e].studentID == studentID)
        {
            string c = studentEnrol[e].subjectCode;
            for (size_t s = 0; s < subjects.size(); s++)
            {
                if (subjects[s].subjectCode == c)
                {
                    total += subjects[s].monthlyFee;
                    break;
                }
            }
        }
    }
    return total;
}

bool addSubject(vector<Subject>& subjects)
{
    Subject newS;
    cout << "\nEnter New Subject Code: ";
    cin >> newS.subjectCode;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (const auto& s : subjects)
    {
        if (s.subjectCode == newS.subjectCode)
        {
            cout << "Error: Subject code already exists!\n";
            return false;
        }
    }

    cout << "Enter Subject Name: ";
    getline(cin, newS.subjectName);
    if(newS.subjectName.empty())
    {
        cout << "Error: Subject name cannot be empty!\n";
        return false;
    }

    cout << "Enter Monthly Fee: ";
    if (!(cin >> newS.monthlyFee) || newS.monthlyFee <= 0)
    {
        cout << "Error: Invalid monthly fee! Must be positive number.\n";
        clearInputBuffer();
        return false;
    }

    subjects.push_back(newS);
    cout << "New subject added successfully.\n";
    saveSubjectToFile(subjects);
    return true;
}

bool editSubject(vector<Subject>& subjects, string editCode)
{
    for (size_t i = 0; i < subjects.size(); i++)
    {
        if (subjects[i].subjectCode == editCode)
        {
            cout << "\nEditing Subject [" << editCode << "]\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter new subject name: ";
            getline(cin, subjects[i].subjectName);
            if(subjects[i].subjectName.empty())
            {
                cout << "Error: Subject name cannot be empty!\n";
                return false;
            }

            cout << "Enter new monthly fee: ";
            if (!(cin >> subjects[i].monthlyFee) || subjects[i].monthlyFee <= 0)
            {
                cout << "Error: Invalid monthly fee! Must be positive number.\n";
                clearInputBuffer();
                return false;
            }

            cout << "Subject detail updated successfully.\n";
            saveSubjectToFile(subjects);
            return true;
        }
    }
    cout << "Subject code not found.\n";
    return false;
}

bool deleteSubject(vector<Subject>& subjects, string delCode, vector<StudentSubject>& enrolList)
{
    bool found = false;
    for (size_t i = 0; i < subjects.size(); i++)
    {
        if (subjects[i].subjectCode == delCode)
        {
            subjects.erase(subjects.begin() + i);
            found = true;
            break;
        }
    }
    if(!found){
        cout << "Subject code not found.\n";
        return false;
    }
    for(size_t i = enrolList.size(); i > 0 ; i--)
    {
        if(enrolList[i-1].subjectCode == delCode)
        {
            enrolList.erase(enrolList.begin() + (i-1));
        }
    }
    cout << "Subject deleted successfully. Related enrolment records removed.\n";
    saveSubjectToFile(subjects);
    saveEnrolToFile(enrolList);
    return true;
}

void viewAllRegisteredStudents(const vector<Subject>& subjects, const vector<StudentSubject>& studentEnrol)
{
    cout << "\n========== ALL SUBJECT REGISTRATION RECORDS ==========" << endl;
    for(auto &sub : subjects)
    {
        cout << "\nSubject: " << sub.subjectCode << " - " << sub.subjectName << endl;
        bool haveStudent = false;
        for(auto &en : studentEnrol)
        {
            if(en.subjectCode == sub.subjectCode)
            {
                cout << "   Student ID: " << en.studentID << endl;
                haveStudent = true;
            }
        }
        if(!haveStudent){
            cout << "   No student registered for this subject.\n";
        }
    }
    cout << "============================================\n";
}

void studentSubjectMenu(string studentID, vector<Subject>& subjList, vector<StudentSubject>& enrolList)
{
    int choice;
    do
    {
        cout << "\n========== STUDENT SUBJECT MENU ==========" << endl;
        cout << "1. View all available subjects" << endl;
        cout << "2. Register new subject" << endl;
        cout << "3. Drop registered subject" << endl;
        cout << "4. View registered subjects & fee" << endl;
        cout << "0. Back to student profile menu" << endl;
        cout << "Enter your choice: ";

        choice = readMenuChoice(0, 4);

        switch(choice)
        {
            case 1:
                viewAvailableSubjects(subjList);
                break;
            case 2:{
                string code;
                cout << "Enter Subject Code to Register: ";
                cin >> code;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                registerSubject(studentID, code, subjList, enrolList);
                break;
            }
            case 3:{
                string code;
                cout << "Enter subject code to drop: ";
                cin >> code;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                dropSubject(studentID, code, enrolList);
                break;
            }
            case 4:
                viewRegisteredSubjects(studentID, subjList, enrolList);
                break;
            case 0:
                cout << "Return back to student profile menu...\n";
                break;
            default:
                cout << "Invalid choice! Please select again.\n";
        }
    } while (choice != 0);
}

void adminSubjectMenu(vector<Subject>& subjList, vector<StudentSubject>& enrolList)
{
    int opt;
    do
    {
        cout << "\n========== ADMIN SUBJECT MENU ==========" << endl;
        cout << "1. Add new subject" << endl;
        cout << "2. Edit subject detail" << endl;
        cout << "3. Delete subject" << endl;
        cout << "4. View all available subjects" << endl;
        cout << "5. View all student registration records" << endl;
        cout << "0. Back to admin menu" << endl;
        cout << "Enter option: ";

        opt = readMenuChoice(0, 5);
        
        switch(opt)
        {
            case 1:
                addSubject(subjList);
                break;
            case 2:{
                string c;
                cout << "Enter subject code to edit: ";
                cin >> c;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                editSubject(subjList, c);
                break;
            }
            case 3:{
                string c;
                cout << "Enter subject code to delete: ";
                cin >> c;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                deleteSubject(subjList, c, enrolList);
                break;
            }
            case 4:
                viewAvailableSubjects(subjList);
                break;
            case 5:
                viewAllRegisteredStudents(subjList, enrolList);
                break;
            case 0:
                cout << "Return back to admin menu...\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while(opt != 0);
}
