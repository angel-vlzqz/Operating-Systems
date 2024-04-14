#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include <pthread.h>
#include <random>               // For std::random_device and std::mt19937
#include "FileSystem.h" // Include the FileSystem header file

using namespace std;

FileSystem fs; // Create an instance of the FileSystem class

enum ProcessState
{
    NEW,       // Newly created process
    READY,     // Ready to run
    RUNNING,   // Currently running
    WAITING,   // Waiting for I/O or external OS request
    TERMINATED // Process has finished execution
};

struct Process
{
    int id;
    string command;
    pthread_t threadId;
    ProcessState state;
    bool requiresIO;
    int timeLimit;
    int elapsedTime;
    int workDuration;
    int priority; // Added for process priority
};

vector<Process> processes;
int nextProcessId = 1;

// Global process queue
queue<Process *> processQueue;

// Time quantum in seconds
const int TIME_QUANTUM = 2;

// Comparator function for sorting processes by ID
bool compareProcessById(const Process &a, const Process &b)
{
    return a.id < b.id;
}

// Condition object for finding a process by ID
struct FindProcessById
{
    int id;
    FindProcessById(int id) : id(id) {}
    bool operator()(const Process &process) const
    {
        return process.id == id;
    }
};

void displayProcesses(bool detailed, bool sortById = false)
{
    if (sortById)
    {
        std::sort(processes.begin(), processes.end(), compareProcessById);
    }

    for (std::vector<Process>::const_iterator it = processes.begin(); it != processes.end(); ++it)
    {
        const Process &process = *it;
        cout << "ID: " << process.id << ", State: ";
        switch (process.state)
        {
        case READY:
            cout << "READY";
            break;
        case RUNNING:
            cout << "RUNNING";
            break;
        case WAITING:
            cout << "WAITING";
            break;
        case TERMINATED:
            cout << "TERMINATED";
            break;
        default:
            cout << "UNKNOWN";
            break;
        }

        if (detailed)
        {
            cout << ", Command: " << process.command
                 << ", Time Limit: " << process.timeLimit
                 << ", Elapsed Time: " << process.elapsedTime
                 << ", Work Duration: " << process.workDuration
                 << ", Priority: " << process.priority;
        }

        cout << endl;
    }
}

void displayProcessInfo(int processId, bool detailed)
{
    std::vector<Process>::iterator it = std::find_if(processes.begin(), processes.end(), FindProcessById(processId));
    if (it != processes.end())
    {
        // Use iterator to access and display the process
        // The display logic is similar to what's inside the loop of displayProcesses
    }
    else
    {
        cout << "Process ID " << processId << " not found." << endl;
    }
}

void modifyProcessPriority(int processId, int newPriority)
{
    std::vector<Process>::iterator it = std::find_if(processes.begin(), processes.end(), FindProcessById(processId));
    if (it != processes.end())
    {
        it->priority = newPriority;
        cout << "Priority of process ID " << processId << " set to " << newPriority << "." << endl;
    }
    else
    {
        cout << "Process ID " << processId << " not found." << endl;
    }
}

void updateProcessState(Process &process)
{
    switch (process.state)
    {
    case RUNNING:
        if (process.requiresIO)
        {
            process.state = WAITING;
        }
        else if (process.elapsedTime >= process.timeLimit)
        {
            // Simulate time limit reached
            process.state = READY;
            process.elapsedTime = 0; // Reset elapsed time
        }
        break;
    case WAITING:
        // Simulate I/O completion
        process.state = READY;
        break;
    case READY:
        process.state = RUNNING;
        break;
    case TERMINATED:
        // Process will be removed from the list, no state update needed
        break;
    default:
        // New or unhandled states
        break;
    }
}

// Trims leading and trailing spaces from a string
void trim(std::string &s)
{
    size_t start = s.find_first_not_of(" \t\n\r\f\v"); // Including other common whitespace characters
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start == std::string::npos)
    {
        s.clear(); // String is all whitespace
    }
    else
    {
        s = s.substr(start, (end - start + 1));
    }
}

void *executeCommand(void *arg)
{
    Process *process = (Process *)arg;
    process->state = RUNNING;

    int quantumElapsed = 0;
    while (process->state != TERMINATED && quantumElapsed < TIME_QUANTUM)
    {
        sleep(1);
        process->elapsedTime++;
        quantumElapsed++;

        // Simulate process completion
        if (process->elapsedTime >= process->workDuration)
        {
            process->state = TERMINATED;
            cout << "Process " << process->id << " has completed its work.\n";
            break; // Exit the loop if work is completed
        }

        updateProcessState(*process);
    }

    // Check again after loop, in case process completes right at the quantum boundary
    if (process->state != TERMINATED)
    {
        process->state = READY;
        cout << "Process " << process->id << " has been requeued.\n";
        processQueue.push(process); // Re-queue only if not terminated
    }

    pthread_exit(NULL);
}

bool isTerminated(const Process &process)
{
    return process.state == TERMINATED;
}

void cleanupTerminatedProcesses()
{
    processes.erase(
        std::remove_if(processes.begin(), processes.end(), isTerminated),
        processes.end());
}

void roundRobinScheduler()
{
    while (!processQueue.empty())
    {
        Process *currentProcess = processQueue.front();
        processQueue.pop();

        // If process is ready, execute it
        if (currentProcess->state == READY)
        {
            pthread_create(&currentProcess->threadId, NULL, executeCommand, currentProcess);
            pthread_join(currentProcess->threadId, NULL); // Wait for the quantum to expire or process to complete
        }
    }

    // Clean up terminated processes
    cleanupTerminatedProcesses();
}

void displayAboutInfo()
{
    cout << "System Information:" << endl;
    cout << "Quantum Time: " << TIME_QUANTUM << " seconds" << endl;
    cout << "Scheduling Type: Round-Robin" << endl;
    cout << "Author: Nathan D. & Angel V." << endl;
    cout << "Description: This system emulates a simple round-robin scheduler for educational purposes." << endl;
    // cout << "Additional Details: Add more details here as needed." << endl;
}

void displayHelp()
{
    cout << "Available Commands:" << endl;
    cout << "about - Display system information" << endl;
    cout << "queue - Add a new process to the queue" << endl;
    cout << "start - Begin round-robin execution of queued processes" << endl;
    cout << "procs - Display list of processes" << endl;
    cout << "procs -a - Display list of processes with additional details" << endl;
    cout << "procs -a -si - Display list of processes with additional details and sort by ID" << endl;
    cout << "modpri <id> <priority> - Modify priority of a process" << endl;
    cout << "edit <path> -rw OR -w OR -r - Edit file permissions" << endl;
    cout << "help - Display available commands" << endl;
    cout << "exit - Exit the program" << endl;
}

void doCommand(string input)
{
    if (input == "start")
    {
        // Begin round-robin execution of queued processes
        roundRobinScheduler();
        // Clear the queue after execution to be ready for next commands
        while (!processQueue.empty())
        {
            delete processQueue.front(); // Clean up dynamically allocated memory
            processQueue.pop();
        }
    }
    else if (input == "procs")
    {
        displayProcesses(false);
    }
    else if (input == "procs -a")
    {
        displayProcesses(true);
    }
    else if (input == "procs -a -si")
    {
        displayProcesses(true, true);
    }
    else if (input.substr(0, 7) == "modpri ")
    {
        istringstream iss(input.substr(7)); // Skip 'modpri '
        int id, priority;
        iss >> id >> priority;
        modifyProcessPriority(id, priority);
    }
    else if (input == "queue")
    {
        Process *process = new Process(); // Dynamic allocation to persist across function calls
        process->id = nextProcessId++;
        // process->command = command;
        process->state = READY;  // Mark as ready to run
        process->timeLimit = 20; // Example time limit for forcefully terminating a process

        // Random workDuration between 5 and 15
        std::random_device rd;                        // Obtain a random number from hardware
        std::mt19937 eng(rd());                       // Seed the generator
        std::uniform_int_distribution<> distr(5, 15); // Define the range

        process->workDuration = distr(eng); // Assign a random workDuration within the range

        processQueue.push(process);
        processes.push_back(*process); // Store the process in the vector

        cout << "Process " << process->id << " enqueued with work duration " << process->workDuration << " seconds.\n";
    }
    else if (input.substr(0, 6) == "mkdir ")
    {
        string path = input.substr(6);
        bool newFile = fs.createFile(path, FileType_DIRECTORY, "rw-");
    }
    else if (input.substr(0, 3) == "rm ")
    {
        string path = input.substr(3);
        fs.deleteFile(path);
    }
    else if (input.substr(0, 7) == "rename ")
    {
        string restOfInput = input.substr(7);      // Get the substring after "rename "
        size_t spaceIndex = restOfInput.find(' '); // Find the space separating old path and new name

        if (spaceIndex == string::npos)
        {
            cout << "Invalid rename command format. Usage: rename [old path] [new name]" << endl;
        }
        else
        {
            string oldPath = restOfInput.substr(0, spaceIndex);
            string newName = restOfInput.substr(spaceIndex + 1);

            if (!fs.renameFile(oldPath, newName))
            {
                cout << "Failed to rename file." << endl;
            }
        }
    }
    else if (input.substr(0, 3) == "ls ")
    {
        string path = input.substr(3);
        fs.listFiles(path);
    }
    else if (input.substr(0, 6) == "touch ")
    {
        string params = input.substr(6);
        size_t spaceIndex = params.find(' ');
        string path = params.substr(0, spaceIndex);
        unsigned long size = 0; // Default size
        bool randomSize = false;

        // Check if there's a special flag for random size
        if (params.find("--size") != string::npos)
        {
            randomSize = true;
        }

        if (randomSize)
        {
            // Generate a random size between a reasonable range, say 1KB to 1MB
            size = 1024 + (rand() % (1024 * 1000)); // Random size in bytes
        }

        fs.createFile(path, FileType_FILE, "rw-", size);
    }
    else if (input.substr(0, 6) == "ls ")
    {
        string path = "/" + input.substr(6);
        fs.createFile("/test", FileType_DIRECTORY, "rw-");
        fs.listFiles(path);
    }
    else if (input.substr(0, 4) == "tree")
    {
        string path = input.substr(4); // Remove the "tree" command part

        if (path.empty())
        {
            // No additional input, display tree from root
            fs.displayDirectoryTree();
        }
        else
        {
            fs.displayDirectoryTree(path);
        }
    }
    else if (input.substr(0, 5) == "move ")
    {
        string restOfInput = input.substr(5);
        size_t spaceIndex = restOfInput.find(' ');
        if (spaceIndex == string::npos)
        {
            cout << "Invalid move command. Correct format: move [oldPath] [newPath]" << endl;
        }
        else
        {
            string oldPath = restOfInput.substr(0, spaceIndex);
            string newPath = restOfInput.substr(spaceIndex + 1);
            if (!fs.moveFile(oldPath, newPath))
            {
                cout << "Failed to move file from '" << oldPath << "' to '" << newPath << "'." << endl;
            }
        }
    }
    else if (input.substr(0, 8) == "dupfile ")
    {
        string restOfInput = input.substr(8);
        size_t spaceIndex = restOfInput.find(' ');
        if (spaceIndex == string::npos)
        {
            cout << "Invalid duplicate file command. Correct format: dupfile [sourcePath] [targetPath]" << endl;
        }
        else
        {
            string sourcePath = restOfInput.substr(0, spaceIndex);
            string targetPath = restOfInput.substr(spaceIndex + 1);
            if (!fs.duplicateFile(sourcePath, targetPath))
            {
                cout << "Failed to duplicate file from '" << sourcePath << "' to '" << targetPath << "'." << endl;
            }
        }
    }
    else if (input.substr(0, 7) == "dupdir ")
    {
        string restOfInput = input.substr(7);
        size_t spaceIndex = restOfInput.find(' ');
        if (spaceIndex == string::npos)
        {
            cout << "Invalid duplicate directory command. Correct format: dupdir [sourceDirPath] [targetDirPath]" << endl;
        }
        else
        {
            string sourceDirPath = restOfInput.substr(0, spaceIndex);
            string targetDirPath = restOfInput.substr(spaceIndex + 1);
            if (!fs.duplicateDirectory(sourceDirPath, targetDirPath))
            {
                cout << "Failed to duplicate directory from '" << sourceDirPath << "' to '" << targetDirPath << "'." << endl;
            }
        }
    }
    else if (input.substr(0, 6) == "search")
    {
        // Trim leading and trailing whitespace from the input command
        string restOfInput = input.substr(6);
        trim(restOfInput); // Trim the rest of the input after "search"

        size_t firstSpace = restOfInput.find(' ');
        string fileName, startPath = "/"; // Default start path is the root directory

        if (firstSpace == std::string::npos)
        {
            fileName = restOfInput; // Only filename is provided, search from root
        }
        else
        {
            fileName = restOfInput.substr(0, firstSpace);
            startPath = restOfInput.substr(firstSpace + 1);
            trim(startPath); // Trim whitespace after splitting to get a clean path
        }

        if (fileName.empty())
        {
            std::cout << "Invalid command format. Correct usage: search [fileName] [optional startPath]" << std::endl;
        }
        else
        {
            std::cout << "Searching for '" << fileName << "' starting from '" << startPath << "'..." << std::endl;
            if (!fs.searchFile(fileName, startPath))
            {
                std::cout << "File '" << fileName << "' not found starting from '" << startPath << "'." << std::endl;
            }
        }
    }
    else if (input.substr(0, 5) == "info ")
    {
        // Strip the "info " prefix from the input to analyze the remainder
        std::string remainingInput = input.substr(5);
        std::string path;
        bool detailed = false;

        // Check if the remaining input starts with "-d" indicating detailed info is required
        if (remainingInput.substr(0, 3) == "-d ")
        {
            path = remainingInput.substr(3); // Skip "-d " to get the path
            detailed = true;
        }
        else
        {
            path = remainingInput; // The whole remaining input is the path
        }

        // Trim the path to remove any leading or trailing whitespace
        trim(path);

        // Display information based on the type of request
        if (detailed)
        {
            std::cout << "Displaying detailed information for: " << path << std::endl;
            fs.displayDetailedInfo(path);
        }
        else
        {
            std::cout << "Displaying basic information for: " << path << std::endl;
            fs.displayBasicInfo(path);
        }
    }
    else if (input.substr(0, 5) == "edit ")
    {
        string restOfInput = input.substr(5);
        size_t spaceIndex = restOfInput.find(' ');
        if (spaceIndex == string::npos)
        {
            cout << "Invalid edit command. Correct format: edit [path] [newPermissions]" << endl;
        }
        else
        {
            string path = restOfInput.substr(0, spaceIndex);
            string newPermissions = restOfInput.substr(spaceIndex + 1);
            if (!fs.editFileDetails(path, newPermissions))
            {
                cout << "Failed to edit details for: '" << path << "'." << endl;
            }
        }
    }
    else if (input == "about")
    {
        displayAboutInfo();
    }
    else if (input == "help")
    {
        displayHelp();
    }
    else
    {
        cout << "Invalid command. Please try again. (enter 'help' for a list of commands)\n";
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        string batchFilePath = argv[1];
        ifstream batchFile(batchFilePath);

        if (!batchFile.is_open())
        {
            cerr << "Failed to open batch file: " << batchFilePath << endl;
            return 1;
        }

        string command;
        while (getline(batchFile, command))
        {
            if (!command.empty())
            {
                doCommand(command);
            }
        }
        batchFile.close();
    }
    else
    {
        cout << "Welcome to citroSh © 2024 All rights reserved." << endl;
        cout << "Enter 'help' to see a list of available commands." << endl;

        string input;
        while (true)
        {
            cout << "\n$citroSh >>>";
            getline(cin, input);

            if (input == "exit")
            {
                cout << "Exiting citroSh. Goodbye! =]\n";
                break;
            }
            else
            {
                doCommand(input);
            }
        }
    }

    // Before exiting the program, ensure to clean up any dynamically allocated Process instances
    while (!processQueue.empty())
    {
        delete processQueue.front(); // Clean up
        processQueue.pop();
    }

    return 0;
}