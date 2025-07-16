#ifndef COMMAND_LIST_CPP
#define COMMAND_LIST_CPP

#include "../command.cpp"
#include "../config.cpp"
#include "../color.cpp"

struct List : Command
{
    List() : Command("list") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 0)
        {
            std::cout << Color::Red << "Usage: list" << Style::Reset << std::endl;
            return;
        }
        const auto configIds = getConfigIds();
        if (configIds.empty())
        {
            std::cout << Color::Yellow << "No configs found." << Style::Reset << std::endl;
            return;
        }
        std::cout << Color::Green << "Configs found:" << Style::Reset << std::endl;
        for (const auto &id : configIds)
        {
            std::cout << " - " << id << std::endl;
        }
    }

    virtual std::string help() const override
    {
        return "Usage: list\n"
               "Lists all available configs.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        // No completions for this command
    }
};

#endif