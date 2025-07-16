#ifndef COMMAND_CREATE_CPP
#define COMMAND_CREATE_CPP

#include "../command.cpp"
#include "../config.cpp"
#include "../color.cpp"

struct Create : Command
{
    Create() : Command("create") {}

    virtual std::string help() const override
    {
        return "Usage: create <name> <jarPath> [--main <mainClassName>] [--working <workingDirectory>] [<args>]\n"
               "Creates a new config file for the specified name.";
    }

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() < 2)
        {
            std::cout << Color::Red << "Usage: create <name> <jarPath> [--main <mainClassName>] [--working <workingDirectory>] [<args>]" << Style::Reset << std::endl;
            return;
        }
        std::string name = args[0];
        if (hasConfig(name))
        {
            std::cout << Color::Red << "Config with name '" << name << "' already exists." << Style::Reset << std::endl;
            return;
        }
        std::string jarPath = args[1];
        if (name.find('/') != std::string::npos || name.find('\\') != std::string::npos)
        {
            std::cout << Color::Red << "Invalid config name: " << name << Style::Reset << std::endl;
            return;
        }
        std::string mainClassName;
        std::string workingDirectory;
        std::vector<std::string> commandArgs;
        for (size_t i = 2; i < args.size(); ++i)
        {
            if (args[i] == "--main" && i + 1 < args.size()) mainClassName = args[++i];
            else if (args[i] == "--working" && i + 1 < args.size()) workingDirectory = args[++i];
            else commandArgs.push_back(args[i]);
        }
        auto jarFile = std::filesystem::path(jarPath);
        if (!std::filesystem::exists(jarFile) || !std::filesystem::is_regular_file(jarFile) || jarFile.extension() != ".jar")
        {
            std::cout << Color::Red << "Invalid JAR file path: " << jarPath << Style::Reset << std::endl;
            return;
        }
        if (!workingDirectory.empty())
        {
            auto workingDirPath = std::filesystem::path(workingDirectory);
            if (!std::filesystem::exists(workingDirPath) || !std::filesystem::is_directory(workingDirPath))
            {
                std::cout << Color::Red << "Invalid working directory: " << workingDirectory << Style::Reset << std::endl;
                return;
            }
        }
        Config config;
        config.jarPath = std::filesystem::absolute(jarFile).string();
        config.args = commandArgs;
        config.mainClassName = mainClassName;
        config.workingDirectory = workingDirectory.empty() ? "" : std::filesystem::absolute(workingDirectory).string();
        save(config, name);
        std::cout << Color::Green << "Created config for " << name << Style::Reset << std::endl;
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        std::string current = "";
        if (index < 0 || index >= args.size()) current = "";
        else current = args[index];
        if (index == 2 || ( index > 2 && !(args[index - 1] == "--main" || args[index - 1] == "--working") ))
        {
            completions.push_back("--main");
            completions.push_back("--working");
        }
        else if (index == 0)
        {
            // name
            // do nothing
        }
        else if (index == 1)
        {
            // jarPath
            completeFilePath(current, completions);
        }
        else if (index > 2 && args[index - 1] == "--main")
        {
            // main class name
        }
        else if (index > 2 && args[index - 1] == "--working")
        {
            // working directory
            completeFilePath(current, completions);
        }
    }
};

#endif