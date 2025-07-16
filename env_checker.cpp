#ifndef ENV_CHECKER_CPP
#define ENV_CHECKER_CPP

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include "color.cpp"

void checkEnvironment()
{
    std::vector<std::string> requiredCommands = {"screen", "java"};
    for (const auto &command : requiredCommands)
    {
        if (std::system((command + " --version > /dev/null 2>&1").c_str()) != 0)
        {
            std::cerr << Color::Red << Style::Bold << "Error: Required command '" << command << "' is not installed or not found in PATH." << Style::Reset << std::endl;
            std::cerr << Color::Yellow << "Please install it before running the application." << Style::Reset << std::endl;
            throw std::runtime_error("Missing required command: " + command);
        }
    }
}

#endif