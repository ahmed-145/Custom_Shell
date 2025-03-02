#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>  // For fork(), chdir()
#include <sys/types.h> // For pid_t
#include <sys/wait.h>  // For wait()
#include <cstdlib>  // For exit(), getenv()
#include <unordered_map>


std::vector <std::string> command_history;
std::unordered_map<std::string, std::string> aliases;
void shell_loop();
std::string read_line();
std::vector<std::string> split_line(std::string line);
int execute_command(std::vector<std::string> args);
void shell_help();

int main() {
    shell_loop();
    return 0;
}

void shell_loop() {
    std::string line;
    std::vector<std::string> args;
    int status;

    do {
        std::cout << "> ";
        line = read_line();
        args = split_line(line);
        if (!line.empty())
        {
        command_history.push_back(line);
        }
        status = execute_command(args);
    } while (status);
}

std::string read_line() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::vector<std::string> split_line(std::string line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

int execute_command(std::vector<std::string> args) {
    if (args.empty()) return 1;

    if (aliases.find(args[0]) != aliases.end()) {
    args = split_line(aliases[args[0]]); // Replace alias with actual command
}

    // Handle built-in commands
    if (args[0] == "exit") return 0;

    if (args[0] == "cd") {
        if (args.size() < 2) {
            std::cerr << "cd: missing argument\n";
        } else if (chdir(args[1].c_str()) != 0) {
            perror("cd");
        }
        else {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::cout << "Directory changed to: " << cwd << std::endl;
        } else {
            perror("getcwd");
        }
        }
        return 1;
    }

    if (args[0] == "help") {
        shell_help();
        return 1;
    }
    if (args[0] == "history")
    {
    for (size_t i = 0; i < command_history.size(); i++)
    {
    std::cout << i + 1 << " " << command_history[i] << std::endl;
    }
    return 1;
    }

    if (args[0] == "alias"){
    if (args.size() == 1)
    {
    for (auto& pair : aliases)
    {
    std::cout << pair.first << " = " << pair.second << std::endl;
    }
    }
    else if (args.size() >= 3 && args[1].find('=') != std::string::npos)
    {
    std::string alias_name = args[1].substr(0, args[1].find('='));
    std::string alias_value = args[1].substr(args[1].find('=') + 1);
    aliases[alias_name] = alias_value;
    } else {
    std::cerr << "Usage: alias name='command'\n";
    }
    return 1;
    }

    if (args[0] == "unalias") {
    if (args.size() != 2) {
        std::cerr << "Usage: unalias name\n";
    } else {
        aliases.erase(args[1]);
    }
    return 1;
}


    // Handle background execution
    bool run_in_background = false;
    if (args.back() == "&") {
        run_in_background = true;
        args.pop_back(); // Remove '&' from arguments
    }

    // Fork and execute external commands
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        std::vector<char*> c_args(args.size() + 1, nullptr);
        for (size_t i = 0; i < args.size(); ++i) {
            c_args[i] = &args[i][0];
        }

        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {  // Fork failed
        perror("fork");
    }
    else {  // Parent process
        if (!run_in_background) {
            int status;
            waitpid(pid, &status, WUNTRACED);
        } else {
            std::cout << "[Background process started]\n";
        }
    }
    return 1;
}

// Help command
void shell_help() {
    std::cout << "Custom Shell\n";
    std::cout << "Built-in commands:\n";
    std::cout << "  cd <directory> - Change directory\n";
    std::cout << "  help - Show this help message\n";
    std::cout << "  exit - Exit the shell\n";
}
