#ifndef COMMAND_REMOVE_CPP
#define COMMAND_REMOVE_CPP

#include "../command.cpp"
#include "../config.cpp"
#include "../color.cpp"

struct Remove : Command
{
    Remove() : Command("remove") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: remove <name>" << Style::Reset << std::endl;
            return;
        }
        std::string name = args[0];
        if (!hasConfig(name))
        {
            std::cout << Color::Red << "No config found with name '" << name << "'." << Style::Reset << std::endl;
            return;
        }
        if (load(name).isRunning())
        {
            std::cout << Color::Red << "Cannot remove config '" << name << "' because it is currently running." << Style::Reset << std::endl;
            return;
        }
        removeConfig(name);
        std::cout << Color::Green << "Removed config for " << name << Style::Reset << std::endl;
    }

    virtual std::string help() const override
    {
        return "Usage: remove <name>\n"
               "Removes the config file for the specified name.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0) for (const auto &id : getConfigIds()) completions.push_back(id);
    }
};

#endif