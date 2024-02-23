#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// Function to execute a single command
void executeCommand(const string& command) {
  // Tokenize command based on spaces
  vector<string> args;
  char* token = strtok(const_cast<char*>(command.c_str()), " ");
  while (token != NULL) {
    args.push_back(token);
    token = strtok(NULL, " ");
  }

  // Convert vector of strings to array of C-style strings
  char* argv[args.size() + 1];
  for (vector<string>::size_type i = 0; i < args.size(); ++i) {
    argv[i] = const_cast<char*>((args[i]).c_str());
  }
  argv[args.size()] = NULL;

  // Fork a child process
  pid_t pid = fork();

  if (pid < 0) {
    cerr << "Error: Fork failed." << endl;
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Child process
    // Execute the command
    execvp(argv[0], argv);
    // If execvp returns, an error occurred
    cerr << "Error: Execution failed." << endl;
    exit(EXIT_FAILURE);
  }
}

// Function to wait for all child processes to finish
void waitForChildProcesses(const vector<pid_t>& childPids) {
  for (vector<pid_t>::const_iterator it = childPids.begin(); it != childPids.end(); ++it) {
    int status;
    waitpid(*it, &status, 0);
  }
}

// Function to end execution
void endExecution() {
  cout << "Execution ended." << endl;
  exit(EXIT_SUCCESS);
}

// Function to exit the shell
void exitShell() {
  cout << "Exiting shell." << endl;
  exit(EXIT_SUCCESS);
}

// Function to handle batch mode
void batchMode(const char* batchFileName) {
  ifstream batchFile(batchFileName);
  if (!batchFile.is_open()) {
    cerr << "Error: Unable to open batch file." << endl;
    exit(EXIT_FAILURE);
  }

  string line;
  vector<pid_t> childPids;
  while (getline(batchFile, line)) {
    cout << "csh> " << line << endl; // Display the command from the batch file
    executeCommand(line);
  }

  batchFile.close();

  // Wait for all child processes to finish
  waitForChildProcesses(childPids);
}

// Function to handle interactive mode
void interactiveMode() {
  string input;
  while (true) {
    cout << "csh> "; // Define your own prompt
    getline(cin, input); // Get input from user

    if (input == "exit") {
      exitShell();
    } else if (input == "end") {
      endExecution();
    }

    // Tokenize input based on semicolons
    vector<string> commands;
    size_t pos = 0;
    string token;
    while ((pos = input.find(';')) != string::npos) {
      token = input.substr(0, pos);
      commands.push_back(token);
      input.erase(0, pos + 1);
    }
    if (!input.empty()) {
      commands.push_back(input);
    }

    // Execute each command
    vector<pid_t> childPids;
    for (vector<string>::iterator it = commands.begin(); it != commands.end(); ++it) {
      executeCommand(*it);
    }

    // Wait for all child processes to finish
    waitForChildProcesses(childPids);
  }
}

int main(int argc, char *argv[]) {
  cout << "CSH (CITRO's SHELL) 2024 Copyright All Rights Reserved Â©" << endl;

  if (argc == 2) {
    // Batch mode: input provided via command line arguments
    batchMode(argv[1]);
  } else {
    // Interactive mode
    interactiveMode();
  }

  return 0;
}
