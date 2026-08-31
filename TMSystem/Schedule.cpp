#include "common.h"
#include <cctype>

string formatTime12Hour(int hour24, int minute)
{
    int h = hour24;
    string ampm = "a.m.";
    if (h >= 12)
    {
        ampm = "p.m.";
        if (h > 12)
            h -= 12;
    }
    ostringstream oss;
    oss << setw(2) << setfill('0') << h << ":"
        << setw(2) << setfill('0') << minute << " " << ampm;
    return oss.str();
}

string getDayNameFromNumber(int dayNum)
{
    switch (dayNum)
    {
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    case 7: return "Sunday";
    default: return "Unknown";
    }
}

bool validateClassroomFormat(const string& room)
{
    if (room.size() != 6)
        return false;
    for (int i = 0; i < 3; i++)
    {
        if (!isupper(static_cast<unsigned char>(room[i])))
            return false;
    }
    for (int i = 3; i < 6; i++)
    {
        if (!isdigit(static_cast<unsigned char>(room[i])))
            return false;
    }
    return true;
}
//====================================================

void loadScheduleData(vector<Schedule>& schedules)
{
    ifstream file("schedule.txt");
    if (!file.is_open())
    {
        return;
    }
    schedules.clear();
    string line;
    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        stringstream ss(line);
        Schedule schedule;
        string dayStr, hourStr, minStr;

        getline(ss, schedule.studentID, '|');
        getline(ss, schedule.subjectCode, '|');
        getline(ss, schedule.teacherName, '|');
        getline(ss, schedule.classroom, '|');
        getline(ss, dayStr, '|');
        getline(ss, hourStr, '|');
        getline(ss, minStr, '|');

        schedule.day = stoi(dayStr);
        schedule.hour = stoi(hourStr);
        schedule.minute = stoi(minStr);

        schedules.push_back(schedule);
    }
    file.close();
}

void saveScheduleData(const vector<Schedule>& schedules)
{
    ofstream file("schedule.txt");
    if (!file.is_open())
    {
        cout << "Unable to open schedule file.\n";
        return;
    }
    for (const Schedule& schedule : schedules)
    {
        file << schedule.studentID << "|"
            << schedule.subjectCode << "|"
            << schedule.teacherName << "|"
            << schedule.classroom << "|"
            << schedule.day << "|"
            << schedule.hour << "|"
            << schedule.minute << endl;
    }
    file.close();
}

bool createSchedule(
    Schedule& schedule,
    vector<Schedule>& schedules)
{
    if (schedule.studentID.empty() ||
        schedule.subjectCode.empty() ||
        schedule.teacherName.empty() ||
        schedule.classroom.empty())
    {
        cout << "Error: All schedule information is required.\n";
        return false;
    }

    // Validate day range
    if (schedule.day < 1 || schedule.day > 7)
    {
        cout << "Error: Invalid day number.\n";
        return false;
    }

    // Time restriction check
    if (schedule.hour < 8 || schedule.hour >= 20)
    {
        cout << "Time restriction: Schedule must between 08:00‑20:00.\n";
        return false;
    }
    if (schedule.day >= 1 && schedule.day <= 5)
    {
        if (schedule.hour < 14)
        {
            cout << "Monday-Friday schedule can only start from 14:00 onwards.\n";
            return false;
        }
    }

    // Duplicate schedule check
    for (const Schedule& existing : schedules)
    {
        if (existing.studentID == schedule.studentID &&
            existing.subjectCode == schedule.subjectCode &&
            existing.day == schedule.day &&
            existing.hour == schedule.hour &&
            existing.minute == schedule.minute)
        {
            cout << "Error: Duplicate schedule.\n";
            return false;
        }
    }

    // Classroom conflict
    for (const Schedule& existing : schedules)
    {
        if (existing.classroom == schedule.classroom &&
            existing.day == schedule.day &&
            existing.hour == schedule.hour &&
            existing.minute == schedule.minute)
        {
            cout << "Error: Classroom conflict.\n";
            return false;
        }
    }

    // Teacher conflict
    for (const Schedule& existing : schedules)
    {
        if (existing.teacherName == schedule.teacherName &&
            existing.day == schedule.day &&
            existing.hour == schedule.hour &&
            existing.minute == schedule.minute)
        {
            cout << "Error: Teacher conflict.\n";
            return false;
        }
    }

    schedules.push_back(schedule);
    saveScheduleData(schedules);
    cout << "Schedule Created Successfully!\n";
    return true;
}

bool searchSchedule(
    string subjectCode,
    const vector<Schedule>& schedules)
{
    bool found = false;
    for (const Schedule& schedule : schedules)
    {
        if (schedule.subjectCode == subjectCode)
        {
            cout << "\n==================== Schedule Found ====================\n";
            cout << "Student ID : " << schedule.studentID << endl;
            cout << "Subject    : " << schedule.subjectCode << endl;
            cout << "Teacher    : " << schedule.teacherName << endl;
            cout << "Classroom  : " << schedule.classroom << endl;
            cout << "Day        : " << getDayNameFromNumber(schedule.day) << endl;
            cout << "Time       : " << formatTime12Hour(schedule.hour, schedule.minute) << endl;
            found = true;
        }
    }
    if (!found)
    {
        cout << "Schedule not found.\n";
    }
    return found;
}

bool assignTeacher(
    Schedule& schedule,
    string teacherName,
    const vector<Schedule>& schedules)
{
    if (teacherName.empty())
    {
        cout << "Teacher name cannot be empty.\n";
        return false;
    }
    for (const Schedule& existing : schedules)
    {
        if (&existing != &schedule &&
            existing.teacherName == teacherName &&
            existing.day == schedule.day &&
            existing.hour == schedule.hour &&
            existing.minute == schedule.minute)
        {
            cout << "Error: Teacher conflict.\n";
            return false;
        }
    }
    schedule.teacherName = teacherName;
    cout << "Teacher assigned successfully.\n";
    return true;
}

bool assignClassroom(
    Schedule& schedule,
    string classroom,
    const vector<Schedule>& schedules)
{
    if (classroom.empty())
    {
        cout << "Classroom cannot be empty.\n";
        return false;
    }
    for (const Schedule& existing : schedules)
    {
        if (&existing != &schedule &&
            existing.classroom == classroom &&
            existing.day == schedule.day &&
            existing.hour == schedule.hour &&
            existing.minute == schedule.minute)
        {
            cout << "Error: Classroom conflict.\n";
            return false;
        }
    }
    schedule.classroom = classroom;
    cout << "Classroom assigned successfully.\n";
    return true;
}

bool updateSchedule(
    Schedule& schedule,
    vector<Schedule>& schedules)
{
    for (Schedule& existing : schedules)
    {
        if (existing.studentID == schedule.studentID &&
            existing.subjectCode == schedule.subjectCode)
        {
            existing.teacherName = schedule.teacherName;
            existing.classroom = schedule.classroom;
            existing.day = schedule.day;
            existing.hour = schedule.hour;
            existing.minute = schedule.minute;
            saveScheduleData(schedules);
            cout << "Schedule Updated Successfully!\n";
            return true;
        }
    }
    cout << "Schedule not found.\n";
    return false;
}

void displaySchedule(
    const vector<Schedule>& schedules)
{
    if (schedules.empty())
    {
        cout << "No schedule records available.\n";
        return;
    }
    cout << "\n========================================= CLASS SCHEDULE =========================================\n";
    cout << left
        << setw(15) << "Student ID"
        << setw(15) << "Subject"
        << setw(20) << "Teacher"
        << setw(15) << "Classroom"
        << setw(15) << "Day"
        << setw(20) << "Time"
        << endl;
    cout << "---------------------------------------------------------------------------------------------------\n";
    for (const Schedule& schedule : schedules)
    {
        cout << left
            << setw(15) << schedule.studentID
            << setw(15) << schedule.subjectCode
            << setw(20) << schedule.teacherName
            << setw(15) << schedule.classroom
            << setw(15) << getDayNameFromNumber(schedule.day)
            << setw(20) << formatTime12Hour(schedule.hour, schedule.minute)
            << endl;
    }
}

void viewSchedule(
    string studentID,
    const vector<Schedule>& schedules)
{
    bool found = false;
    cout << "\n========== MY CLASS SCHEDULE ==========\n";
    for (const Schedule& schedule : schedules)
    {
        if (schedule.studentID == studentID)
        {
            cout << "Subject   : " << schedule.subjectCode << endl;
            cout << "Teacher   : " << schedule.teacherName << endl;
            cout << "Classroom : " << schedule.classroom << endl;
            cout << "Day       : " << getDayNameFromNumber(schedule.day) << endl;
            cout << "Time      : " << formatTime12Hour(schedule.hour, schedule.minute) << endl;
            cout << "--------------------------------------\n";
            found = true;
        }
    }
    if (!found)
    {
        cout << "No class schedule found.\n";
    }
}

void studentScheduleMenu(
    string studentID,
    const vector<Schedule>& schedules)
{
    int choice;
    do
    {
        cout << "\n===== STUDENT SCHEDULE =====\n";
        cout << "1. View Class Schedule\n";
        cout << "0. Back\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 1);
        switch (choice)
        {
        case 1:
            viewSchedule(studentID, schedules);
            break;
        case 0:
            break;
        default:
            cout << "Invalid menu choice.\n";
        }
    } while (choice != 0);
}

void adminScheduleMenu(vector<Schedule>& schedules)
{
    int choice;
    do
    {
        cout << "\n===== ADMIN SCHEDULE MANAGEMENT =====\n";
        cout << "1. Create Class Schedule\n";
        cout << "2. Update Timetable\n";
        cout << "3. Search Timetable\n";
        cout << "4. Assign Teacher\n";
        cout << "5. Assign Classroom\n";
        cout << "6. Display All Schedules\n";
        cout << "0. Back\n";
        cout << "Enter your choice: ";
        choice = readMenuChoice(0, 6);
        switch (choice)
        {
        case 1:
        {
            Schedule schedule;
            schedule.studentID = readNonEmptyLine("Enter Student ID: ");
            schedule.subjectCode = readNonEmptyLine("Enter Subject Code: ");
            schedule.teacherName = readNonEmptyLine("Enter Teacher Name: ");

            // Classroom input with format check
            string roomInput;
            while (true)
            {
                cout << "Enter classroom (format XXX000 e.g ABC001): ";
                getline(cin, roomInput);
                if (validateClassroomFormat(roomInput))
                {
                    schedule.classroom = roomInput;
                    break;
                }
                cout << "Classroom format invalid! Must be 3 uppercase letters + 3 digits, example TUT001\n";
            }

            // Day number selection
            int dayNum;
            while (true)
            {
                cout << "\nSelect Day:\n";
                cout << "1-Monday\n2-Tuesday\n3-Wednesday\n4-Thursday\n5-Friday\n6-Saturday\n7-Sunday\n";
                cout << "Enter day number (1-7): ";
                if (!(cin >> dayNum) || dayNum < 1 || dayNum > 7)
                {
                    cout << "Invalid day number, please input 1-7.\n";
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();
                break;
            }
            schedule.day = dayNum;

            // Hour minute input
            cout << "Enter start hour (24-hour format 0-23): ";
            cin >> schedule.hour;
            cout << "Enter start minute (0-59): ";
            cin >> schedule.minute;
            clearInputBuffer();

            createSchedule(schedule, schedules);
            break;
        }
        case 2:
        {
            Schedule schedule;
            schedule.studentID = readNonEmptyLine("Enter Student ID: ");
            schedule.subjectCode = readNonEmptyLine("Enter Subject Code: ");
            schedule.teacherName = readNonEmptyLine("Enter New Teacher: ");

            string roomInput;
            while (true)
            {
                cout << "Enter new classroom (format XXX000 e.g ABC001): ";
                getline(cin, roomInput);
                if (validateClassroomFormat(roomInput))
                {
                    schedule.classroom = roomInput;
                    break;
                }
                cout << "Classroom format invalid! Must be 3 uppercase letters + 3 digits, example TUT001\n";
            }

            int dayNum;
            while (true)
            {
                cout << "\nSelect New Day:\n";
                cout << "1‑Monday\n2‑Tuesday\n3‑Wednesday\n4‑Thursday\n5‑Friday\n6‑Saturday\n7‑Sunday\n";
                cout << "Enter day number (1‑7): ";
                if (!(cin >> dayNum) || dayNum < 1 || dayNum > 7)
                {
                    cout << "Invalid day number, please input 1‑7.\n";
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();
                break;
            }
            schedule.day = dayNum;

            cout << "Enter new start hour (24‑hour format 0‑23): ";
            cin >> schedule.hour;
            cout << "Enter new start minute (0‑59): ";
            cin >> schedule.minute;
            clearInputBuffer();

            updateSchedule(schedule, schedules);
            break;
        }
        case 3:
        {
            string subjectCode = readNonEmptyLine("Enter Subject Code to Search: ");
            searchSchedule(subjectCode, schedules);
            break;
        }
        case 4:
        {
            string studentID = readNonEmptyLine("Enter Student ID: ");
            string subjectCode = readNonEmptyLine("Enter Subject Code: ");
            string teacherName = readNonEmptyLine("Enter Teacher Name: ");
            bool found = false;
            for (Schedule& schedule : schedules)
            {
                if (schedule.studentID == studentID &&
                    schedule.subjectCode == subjectCode)
                {
                    if (assignTeacher(schedule, teacherName, schedules))
                    {
                        saveScheduleData(schedules);
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "Schedule not found.\n";
            }
            break;
        }
        case 5:
        {
            string studentID = readNonEmptyLine("Enter Student ID: ");
            string subjectCode = readNonEmptyLine("Enter Subject Code: ");

            string classroom;
            while (true)
            {
                cout << "Enter new classroom (format XXX000 e.g ABC001): ";
                getline(cin, classroom);
                if (validateClassroomFormat(classroom))
                    break;
                cout << "Classroom format invalid! Must be 3 uppercase letters + 3 digits, example TUT001\n";
            }

            bool found = false;
            for (Schedule& schedule : schedules)
            {
                if (schedule.studentID == studentID &&
                    schedule.subjectCode == subjectCode)
                {
                    if (assignClassroom(schedule, classroom, schedules))
                    {
                        saveScheduleData(schedules);
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "Schedule not found.\n";
            }
            break;
        }
        case 6:
            displaySchedule(schedules);
            break;
        case 0:
            cout << "Returning to Admin Menu...\n";
            break;
        default:
            cout << "Invalid menu choice.\n";
        }
    } while (choice != 0);
}
