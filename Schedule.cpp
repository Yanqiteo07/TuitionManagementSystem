#include "common.h"

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

        getline(ss, schedule.studentID, '|');
        getline(ss, schedule.subjectCode, '|');
        getline(ss, schedule.teacherName, '|');
        getline(ss, schedule.classroom, '|');
        getline(ss, schedule.day, '|');
        getline(ss, schedule.time, '|');

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
            << schedule.time << endl;
    }

    file.close();
}bool createSchedule(
    Schedule& schedule,
    vector<Schedule>& schedules)
{
    // Check empty input
    if (schedule.studentID.empty() ||
        schedule.subjectCode.empty() ||
        schedule.teacherName.empty() ||
        schedule.classroom.empty() ||
        schedule.day.empty() ||
        schedule.time.empty())
    {
        cout << "Error: All schedule information is required.\n";
        return false;
    }

    // Validate day
    if (schedule.day != "Monday" &&
        schedule.day != "Tuesday" &&
        schedule.day != "Wednesday" &&
        schedule.day != "Thursday" &&
        schedule.day != "Friday" &&
        schedule.day != "Saturday" &&
        schedule.day != "Sunday")
    {
        cout << "Error: Invalid day.\n";
        return false;
    }

    // Check duplicate schedule
    for (const Schedule& existing : schedules)
    {
        if (existing.studentID == schedule.studentID &&
            existing.subjectCode == schedule.subjectCode &&
            existing.day == schedule.day &&
            existing.time == schedule.time)
        {
            cout << "Error: Duplicate schedule.\n";
            return false;
        }
    }

    // Check classroom conflict
    for (const Schedule& existing : schedules)
    {
        if (existing.classroom == schedule.classroom &&
            existing.day == schedule.day &&
            existing.time == schedule.time)
        {
            cout << "Error: Classroom conflict.\n";
            return false;
        }
    }

    // Check teacher conflict
    for (const Schedule& existing : schedules)
    {
        if (existing.teacherName == schedule.teacherName &&
            existing.day == schedule.day &&
            existing.time == schedule.time)
        {
            cout << "Error: Teacher conflict.\n";
            return false;
        }
    }

    schedules.push_back(schedule);

    saveScheduleData(schedules);

    cout << "Schedule Created Successfully!\n";

    return true;
}bool searchSchedule(
    string subjectCode,
    const vector<Schedule>& schedules)
{
    bool found = false;

    for (const Schedule& schedule : schedules)
    {
        if (schedule.subjectCode == subjectCode)
        {
            cout << "\n========== Schedule Found ==========\n";
            cout << "Student ID : " << schedule.studentID << endl;
            cout << "Subject    : " << schedule.subjectCode << endl;
            cout << "Teacher    : " << schedule.teacherName << endl;
            cout << "Classroom  : " << schedule.classroom << endl;
            cout << "Day        : " << schedule.day << endl;
            cout << "Time       : " << schedule.time << endl;

            found = true;
        }
    }

    if (!found)
    {
        cout << "Schedule not found.\n";
    }

    return found;
}bool assignTeacher(
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
            existing.time == schedule.time)
        {
            cout << "Error: Teacher conflict.\n";
            return false;
        }
    }

    schedule.teacherName = teacherName;

    cout << "Teacher assigned successfully.\n";

    return true;
}bool assignClassroom(
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
            existing.time == schedule.time)
        {
            cout << "Error: Classroom conflict.\n";
            return false;
        }
    }

    schedule.classroom = classroom;

    cout << "Classroom assigned successfully.\n";

    return true;
}bool updateSchedule(
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
            existing.time = schedule.time;

            saveScheduleData(schedules);

            cout << "Schedule Updated Successfully!\n";

            return true;
        }
    }

    cout << "Schedule not found.\n";

    return false;
}void displaySchedule(
    const vector<Schedule>& schedules)
{
    if (schedules.empty())
    {
        cout << "No schedule records available.\n";
        return;
    }

    cout << "\n================ CLASS SCHEDULE ================\n";

    cout << left
        << setw(15) << "Student ID"
        << setw(15) << "Subject"
        << setw(20) << "Teacher"
        << setw(15) << "Classroom"
        << setw(15) << "Day"
        << setw(15) << "Time"
        << endl;

    cout << "--------------------------------------------------------------------------\n";

    for (const Schedule& schedule : schedules)
    {
        cout << left
            << setw(15) << schedule.studentID
            << setw(15) << schedule.subjectCode
            << setw(20) << schedule.teacherName
            << setw(15) << schedule.classroom
            << setw(15) << schedule.day
            << setw(15) << schedule.time
            << endl;
    }
}void viewSchedule(
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
            cout << "Day       : " << schedule.day << endl;
            cout << "Time      : " << schedule.time << endl;
            cout << "--------------------------------------\n";

            found = true;
        }
    }

    if (!found)
    {
        cout << "No class schedule found.\n";
    }
}void studentScheduleMenu(
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
}void adminScheduleMenu(vector<Schedule>& schedules)
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

            schedule.studentID =
                readNonEmptyLine("Enter Student ID: ");

            schedule.subjectCode =
                readNonEmptyLine("Enter Subject Code: ");

            schedule.teacherName =
                readNonEmptyLine("Enter Teacher Name: ");

            schedule.classroom =
                readNonEmptyLine("Enter Classroom: ");

            schedule.day =
                readNonEmptyLine("Enter Day: ");

            schedule.time =
                readNonEmptyLine("Enter Time: ");

            createSchedule(schedule, schedules);

            break;
        }

        case 2:
        {
            Schedule schedule;

            schedule.studentID =
                readNonEmptyLine("Enter Student ID: ");

            schedule.subjectCode =
                readNonEmptyLine("Enter Subject Code: ");

            schedule.teacherName =
                readNonEmptyLine("Enter New Teacher: ");

            schedule.classroom =
                readNonEmptyLine("Enter New Classroom: ");

            schedule.day =
                readNonEmptyLine("Enter New Day: ");

            schedule.time =
                readNonEmptyLine("Enter New Time: ");

            updateSchedule(schedule, schedules);

            break;
        }

        case 3:
        {
            string subjectCode =
                readNonEmptyLine(
                    "Enter Subject Code to Search: ");

            searchSchedule(subjectCode, schedules);

            break;
        }

        case 4:
        {
            string studentID =
                readNonEmptyLine("Enter Student ID: ");

            string subjectCode =
                readNonEmptyLine("Enter Subject Code: ");

            string teacherName =
                readNonEmptyLine("Enter Teacher Name: ");

            bool found = false;

            for (Schedule& schedule : schedules)
            {
                if (schedule.studentID == studentID &&
                    schedule.subjectCode == subjectCode)
                {
                    if (assignTeacher(
                        schedule,
                        teacherName,
                        schedules))
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
            string studentID =
                readNonEmptyLine("Enter Student ID: ");

            string subjectCode =
                readNonEmptyLine("Enter Subject Code: ");

            string classroom =
                readNonEmptyLine("Enter Classroom: ");

            bool found = false;

            for (Schedule& schedule : schedules)
            {
                if (schedule.studentID == studentID &&
                    schedule.subjectCode == subjectCode)
                {
                    if (assignClassroom(
                        schedule,
                        classroom,
                        schedules))
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
