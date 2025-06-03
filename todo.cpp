#include <iostream>
#include <vector>
#include <string>

struct Task {
    std::string description;
    bool done;

    Task(const std::string& desc) : description(desc), done(false) {}
};

class TodoList {
    std::vector<Task> tasks;

public:
    void add(const std::string& task) {
        tasks.emplace_back(task);
        std::cout << "Added: " << task << "\n";
    }

    void list() const {
        if (tasks.empty()) {
            std::cout << "No tasks in the list.\n";
            return;
        }
        for (size_t i = 0; i < tasks.size(); i++) {
            std::cout << i + 1 << ". [" << (tasks[i].done ? 'x' : ' ') << "] " << tasks[i].description << "\n";
        }
    }

    void mark_done(size_t index) {
        if (index == 0 || index > tasks.size()) {
            std::cout << "Invalid task number.\n";
            return;
        }
        tasks[index - 1].done = true;
        std::cout << "Task " << index << " marked as done.\n";
    }
};

int main() {
    TodoList todo;
    std::string command;

    std::cout << "Simple Todo List\nCommands: add <task>, list, done <task number>, quit\n";

    while (true) {
        std::cout << "> ";
        getline(std::cin, command);

        if (command == "quit") break;
        else if (command.substr(0, 4) == "add ") {
            todo.add(command.substr(4));
        }
        else if (command == "list") {
            todo.list();
        }
        else if (command.substr(0, 5) == "done ") {
            size_t index = std::stoi(command.substr(5));
            todo.mark_done(index);
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
