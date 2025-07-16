#ifndef COMMAND_CPP
#define COMMAND_CPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

struct 
{
    typedef const std::string str;
    str version = "1.0.0";
    str author = "CyanTachyon";
    str description = "A simple command line tool to manage and run CyanTachyon's applications.";
    str license = "AGPL-3.0";
    str website = "https://github.com/CyanTachyon/CyanithRunner";
    str authorWebsite = "https://www.tachyon.moe";
    str authorEmail = "cyan@tachyon.moe";
} VERSION_INFO;

struct Command
{
    std::string name;
    Command(const std::string &name)
    {
        this->name = name;
    }
    virtual void execute(const std::vector<std::string> &args) const = 0;
    virtual std::string help() const = 0;
    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const = 0;
    virtual bool isTreeCommand() const
    {
        return false;
    }
};

struct TreeCommand: public Command
{
    TreeCommand() = delete;
    TreeCommand(const std::string &name) : Command(name) {}
    virtual bool isTreeCommand() const override
    {
        return true;
    }

    std::map<std::string, Command *> commands;
    template <typename... Commands>
    void addCommand(Command * cmd)
    {
        commands[cmd->name] = cmd;
    }
    Command *getCommand(const std::string &name) const
    {
        auto it = commands.find(name);
        if (it != commands.end())
        {
            return it->second;
        }
        return nullptr;
    }
    virtual std::string help() const override
    {
        std::string help_text = "Available commands:\n";
        if (commands.empty())
        {
            help_text += "  No commands available.";
            return help_text;
        }
        for (const auto &pair : commands)
        {
            if (pair.first.find("---") == 0)
                continue;
            const std::string subHelp = pair.second->help();
            if (!subHelp.empty())
            {
                std::vector<std::string> lines;
                std::stringstream iss(subHelp);
                std::string line;
                while (std::getline(iss, line))
                {
                    lines.push_back(line);
                }
                help_text += "  " + pair.first + "\n";
                for (const auto &subLine : lines)
                {
                    help_text += "    " + subLine + "\n";
                }
            }
            else
            {
                help_text += "  " + pair.first + "\n";
            }
        }
        if (help_text.back() == '\n')
            help_text.pop_back();
        return help_text;
    }
    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            for (const auto &pair : commands)
            {
                if (pair.first.find("---") == 0)
                    continue;
                completions.push_back(pair.first);
            }
        }
        else if (index > 0)
        {
            Command *cmd = getCommand(args[0]);
            if (cmd)
            {
                const std::vector<std::string> &subArgs = std::vector<std::string>(args.begin() + 1, args.end());
                cmd->complete(subArgs, index - 1, completions);
            }
        }
    }
    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.empty())
        {
            std::cout << help() << std::endl;
            return;
        }
        Command *cmd = getCommand(args[0]);
        if (cmd)
        {
            std::vector<std::string> subArgs(args.begin() + 1, args.end());
            cmd->execute(subArgs);
        }
        else
        {
            std::cout << "Unknown command: " << args[0] << "\n" << help() << std::endl;
        }
    }
};

TreeCommand topLevelCommands("root");

void completeFilePath(const std::string &prefix, std::vector<std::string>&completions)
{
    if (prefix.empty()) 
    {
        // 当前目录的子目录和文件
        for (const auto &entry : std::filesystem::directory_iterator("."))
        {
            if (entry.is_regular_file() || entry.is_directory())
            {
                completions.push_back(entry.path().filename().string());
            }
        }
        return;
    }
    std::string folder = prefix.substr(0, prefix.find_last_of("/\\") + 1);
    std::string filePrefix = prefix.substr(prefix.find_last_of("/\\") + 1);
    if (folder.empty()) folder = ".";
    if (std::filesystem::exists(folder)) for (const auto &entry : std::filesystem::directory_iterator(folder))
    {
        if (entry.is_regular_file() || entry.is_directory())
        {
            std::string fileName = entry.path().filename().string();
            fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
            fileName = folder + fileName;
            if (entry.is_directory())
                fileName += "/";
            completions.push_back(fileName);
        }
    }
    return;
}

#endif // COMMAND_CPP