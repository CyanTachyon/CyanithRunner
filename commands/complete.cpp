#ifndef COMMAND_COMPLETE_CPP
#define COMMAND_COMPLETE_CPP

#include "../command.cpp"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

struct Complete : Command
{
    Complete() : Command("---complete") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() < 2) return;
        const auto index = std::stoi(args[0]);
        std::vector<std::string> completions;
        topLevelCommands.complete(std::vector<std::string>(args.begin() + 2, args.end()), index - 1, completions);
        for (const auto &completion : completions)
        {
            std::cout << completion << std::endl;
        }
    }

    virtual std::string help() const override
    {
        return "";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        // This command does not provide any specific completions.
    }
};

#endif