#ifndef VERSION_COMMAND_CPP
#define VERSION_COMMAND_CPP

#include "../command.cpp"
#include "../color.cpp"
#include <iostream>
#include <vector>
#include <string>

#define logo \
std::string(                                                                                             \
    "      ⣀⣤⣴⣦⣤                           ⢠⣤⣤⣤⣤⣤⣤⡄                  ⢀⣤\n"                               \
    "    ⢀⣼⠟⠉ ⠈⠛⠃ ⢀⡀   ⣀    ⢀⣀⡀⢀⡀  ⣀ ⢀⣀⡀     ⠈⣽⡏      ⣀⣀ ⣀    ⢀⣀⣀⡀   ⢸⡟⢀⣀⡀    ⣀   ⢀⡀    ⣀⣀⡀   ⢀⡀ ⣀⣀\n"   \
    "    ⣼⡏       ⣼⡇  ⢠⣿  ⢀⣴⠟⠛⢻⣾⠃  ⣿⡷⠛⠛⢿⡇    ⢀⣿⠁    ⣠⡾⠛⠛⣷⡟  ⢀⣾⠟⠛⠛⠻⠇  ⣿⡷⠛⠛⢿⡇  ⢠⣿   ⣼⡇  ⣰⡿⠛⠛⢿⡆  ⢸⣿⠞⠛⠻⣿⡀\n" \
    "   ⢰⣿        ⣿   ⢸⡇  ⣾⠏  ⢸⡿  ⢠⣿⠁  ⣸⡇    ⢸⡿    ⢰⡿⠁  ⣿⠇ ⢀⣾⠇      ⢠⣿⠁  ⣸⡇  ⢸⡇   ⣿  ⢰⡿   ⢸⡟  ⣼⡏  ⢀⣿⠃\n" \
    "   ⠘⣿⡄      ⢸⣿  ⣠⣿⠇ ⢰⣿  ⢀⣾⡇  ⢸⡟   ⣿⠃    ⣼⡇    ⣿⡇  ⣰⣿  ⢸⣿⡀      ⢸⡟   ⣿⠃  ⣿⡇ ⢀⣼⡿  ⣾⡇   ⣾⠇  ⣿⠃  ⢸⡟\n"  \
    "    ⠹⢿⣦⣤⣴⡶  ⠈⠻⠿⠾⢫⣿  ⠘⣿⣦⣴⠟⢹⡇  ⣾⠇  ⢰⡿     ⣿⠁    ⢻⣷⣤⡾⠋⣿   ⠻⣷⣦⣴⡾⠂  ⣿⠇  ⢰⡿   ⠙⠿⠿⠟⣽⡇  ⠘⢿⣦⣤⡾⠋  ⢸⡿   ⣾⠇\n"  \
    "      ⠈⠁   ⢀⣤   ⣼⡏    ⠉                        ⠈⠁                      ⣠⡄  ⢠⣿     ⠈\n"              \
    "           ⠈⠻⠿⠶⠾⠛                                                      ⠙⠿⠶⠶⠟⠁\n")

struct Version : Command
{
    Version() : Command("--version") {}

    virtual std::string help() const override
    {
        return "Usage: version\n"
               "Displays the version information of the application.";
    }

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (!args.empty())
        {
            std::cout << Color::Red << "Usage: version" << Style::Reset << std::endl;
            return;
        }
        std::cout << Color::Cyan << logo << Style::Reset << std::endl;
        std::cout << "Version: " << VERSION_INFO.version << std::endl;
        std::cout << "Website: " << VERSION_INFO.website << std::endl;
        std::cout << "Description: " << VERSION_INFO.description << std::endl;
        std::cout << "Author: " << VERSION_INFO.author << std::endl;
        std::cout << "Author Website: " << VERSION_INFO.authorWebsite << std::endl;
        std::cout << "Author Email: " << VERSION_INFO.authorEmail << std::endl;
        std::cout << "License: " << VERSION_INFO.license << std::endl;
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        // No completions for this command
    }
};

#endif