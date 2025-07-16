#ifndef COMMAND_RENAME_CPP
#define COMMAND_RENAME_CPP

#include "../command.cpp"
#include "../config.cpp"
#include "../color.cpp"
#include "../env_checker.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <ext/stdio_filebuf.h>
#include <signal.h>

struct Rename : Command
{
    Rename() : Command("rename") {}

    virtual std::string help() const override
    {
        return "Usage: rename <oldName> <newName>\n"
               "Renames an existing config file.";
    }

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 2)
        {
            std::cout << Color::Red << "Usage: rename <oldName> <newName>" << Style::Reset << std::endl;
            return;
        }
        std::string oldName = args[0];
        std::string newName = args[1];
        if (!hasConfig(oldName))
        {
            std::cout << Color::Red << "No config found with name '" << oldName << "'." << Style::Reset << std::endl;
            return;
        }
        if (hasConfig(newName))
        {
            std::cout << Color::Red << "Config with name '" << newName << "' already exists." << Style::Reset << std::endl;
            return;
        }
        if (load(oldName).isRunning())
        {
            std::cout << Color::Red << "Cannot rename config '" << oldName << "' because it is currently running." << Style::Reset << std::endl;
            return;
        }
        renameConfig(oldName, newName);
        std::cout << Color::Green << "Renamed config from " << oldName << " to " << newName << Style::Reset << std::endl;
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0) for (const auto &id : getConfigIds()) completions.push_back(id);
    }
};

#endif