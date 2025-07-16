#ifndef COMMAND_HELP_CPP
#define COMMAND_HELP_CPP

#include "../command.cpp"
#include "../color.cpp"

struct Help : Command
{
    Help() : Command("--help") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        Command *cmd = &topLevelCommands;
        int index = 0;
        while (cmd->isTreeCommand() && index < args.size())
        {
            cmd = static_cast<TreeCommand *>(cmd)->getCommand(args[index]);
            if (!cmd)
            {
                std::cout << Color::Red << "Unknown command: " << args[index] << Style::Reset << std::endl;
                return;
            }
            index++;
        }
        if (cmd)
        {
            std::cout << cmd->help() << std::endl;
        }
        else
        {
            std::cout << Color::Red << "No command found." << Style::Reset << std::endl;
        }
    }
    virtual std::string help() const override
    {
        return "Usage: help [command]\n"
               "Displays help information for the specified command.\n"
               "If no command is specified, displays help for all commands.";
    }
    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        topLevelCommands.complete(args, index, completions);
    }
};
#endif