#include "command.cpp"

#include "commands/install.cpp"
#include "commands/complete.cpp"

#include "commands/help.cpp"

#include "commands/create.cpp"
#include "commands/remove.cpp"
#include "commands/list.cpp"
#include "commands/rename.cpp"

#include "commands/core.cpp"

#include "commands/version.cpp"

std::string currentFile;

void initializeCommands();
int main(int argc, char *argv[])
{
    currentFile = std::string(argv[0]);
    initializeCommands();
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    try
    {
        topLevelCommands.execute(args);
    }
    catch (const std::exception &e)
    {
        std::cerr << Color::Red << "Error: " << e.what() << Style::Reset << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << Color::Red << "An unknown error occurred." << Style::Reset << std::endl;
        return 1;
    }
    return 0;
}

void initializeCommands()
{
    topLevelCommands.addCommand(new Install());
    topLevelCommands.addCommand(new Uninstall());
    topLevelCommands.addCommand(new Complete());

    topLevelCommands.addCommand(new Help());

    topLevelCommands.addCommand(new Create());
    topLevelCommands.addCommand(new Remove());
    topLevelCommands.addCommand(new List());
    topLevelCommands.addCommand(new Rename());

    topLevelCommands.addCommand(new Start());
    topLevelCommands.addCommand(new Stop());
    topLevelCommands.addCommand(new Restart());
    topLevelCommands.addCommand(new Status());
    topLevelCommands.addCommand(new Enter());

    topLevelCommands.addCommand(new Version());
}