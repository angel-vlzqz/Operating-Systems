#include <iostream>
#include <vector>
#include <iomanip>

struct Task {
  std::string description;
  bool completed;

  Task(const std::string& desc) : description(desc), completed(false) {}
};

class Scheduler {
  private:
    std::vector<Task> tasks;

  public:
    void addTask(const std::string& description) {
      tasks.emplace_back(description);
      std::cout << "Task added successfully!\n";
    }

    void viewTasks() const {
      std::cout << "Tasks:\n";
      std::cout << std::setw(5) << "ID" << std::setw(20) << "Description" << std::setw(15) << "Status\n";
      for (size_t i = 0; i < tasks.size(); ++i) {
        std::cout << std::setw(5) << i + 1 << std::setw(20) << tasks[i].description
          << std::setw(15) << (tasks[i].completed ? "Completed" : "Pending") << "\n";
      }
    }

    void markTaskCompleted(size_t taskId) {
      if (taskId > 0 && taskId <= tasks.size()) {
        tasks[taskId - 1].completed = true;
        std::cout << "Task marked as completed!\n";
      } else {
        std::cout << "Invalid task ID!\n";
      }
    }
};

int main() {
  Scheduler scheduler;
  int choice;

  do {
    std::cout << "\nScheduler Menu:\n";
    std::cout << "1. Add Task\n";
    std::cout << "2. View Tasks\n";
    std::cout << "3. Mark Task as Completed\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice (1-4): ";
    std::cin >> choice;

    switch (choice) {
      case 1: {
                std::string taskDescription;
                std::cout << "Enter task description: ";
                std::cin.ignore(); // Clear the newline character from the buffer
                std::getline(std::cin, taskDescription);
                scheduler.addTask(taskDescription);
                break;
              }
      case 2:
              scheduler.viewTasks();
              break;
      case 3: {
                size_t taskId;
                std::cout << "Enter task ID to mark as completed: ";
                std::cin >> taskId;
                scheduler.markTaskCompleted(taskId);
                break;
              }
      case 4:
              std::cout << "Exiting scheduler. Goodbye!\n";
              break;
      default:
              std::cout << "Invalid choice. Please enter a number between 1 and 4.\n";
    }

  } while (choice != 4);

  return 0;
}

