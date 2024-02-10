#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void executeCommand(const vector<string>& args) {
  pid_t pid = fork();

  if (pid == -1) {
    cerr << "Error forking process." << endl;
    exit(EXIT_FAILURE);
  } else if (pid == 0) { // Child process
                         // Convert vector of strings to array of C-style strings
    vector<char*> cArgs;
    for (const auto& arg : args) {
      cArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    cArgs.push_back(nullptr); // Null-terminate the array

    // Execute the command
    execvp(cArgs[0], cArgs.data());

    // If execvp fails
    cerr << "Error executing command: " << args[0] << endl;
    exit(EXIT_FAILURE);
  } else { // Parent process
    int status;
    waitpid(pid, &status, 0);
  }
}

int main() {
  string input;

  while (true) {
    // Display prompt
    cout << "SimpleShell> ";

    // Get user input
    getline(cin, input);

    // Exit if the user enters 'exit'
    if (input == "quit") {
      break;
    }

    // Tokenize the input
    istringstream iss(input);
    vector<string> args{
      istream_iterator<string>(iss),
        istream_iterator<string>()
    };

    // Execute the command
    if (!args.empty()) {
      executeCommand(args);
    }
  }

  return 0;
}

