#ifndef COMMAND_CORE_CPP
#define COMMAND_CORE_CPP

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

#define sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#define ESCAPE_STRING(str) ({  \
    std::string escaped;       \
    for (char c : str)         \
    {                          \
        if (c == '\"')         \
            escaped += "\\\""; \
        else if (c == '\\')    \
            escaped += "\\\\"; \
        else                   \
            escaped += c;      \
    }                          \
    escaped;                   \
})

#define start_session(configId)                                                                                            \
    do                                                                                                                     \
    {                                                                                                                      \
        if (!hasConfig(configId))                                                                                          \
        {                                                                                                                  \
            std::cout << Color::Red << "Config with ID '" << configId << "' does not exist." << Style::Reset << std::endl; \
            return;                                                                                                        \
        }                                                                                                                  \
        Config config = load(configId);                                                                                    \
        system("screen -wipe > /dev/null 2>&1");                                                                           \
        if (config.isRunning())                                                                                            \
        {                                                                                                                  \
            std::cout << Color::Yellow << "Config " << configId << " is already running." << Style::Reset << std::endl;    \
            return;                                                                                                        \
        }                                                                                                                  \
        std::cout << Color::Green << "Starting " << configId << "..." << Style::Reset << std::endl;                        \
        std::string command = "screen -dmS \"" + SESSION_PREFIX + configId + "\" bash -c \"";                              \
        std::string tCmd;                                                                                                  \
        if (!config.workingDirectory.empty())                                                                              \
        {                                                                                                                  \
            tCmd += "cd '" + config.workingDirectory + "' && ";                                                            \
            tCmd += "java";                                                                                                \
        }                                                                                                                  \
        if (!config.mainClassName.empty())                                                                                 \
        {                                                                                                                  \
            tCmd += " -classpath \"" + ESCAPE_STRING(config.jarPath) + "\"";                                               \
            tCmd += " \"" + ESCAPE_STRING(config.mainClassName) + "\"";                                                    \
        }                                                                                                                  \
        else                                                                                                               \
            tCmd += " -jar \"" + ESCAPE_STRING(config.jarPath) + "\"";                                                     \
        for (const auto &arg : config.args)                                                                                \
            tCmd += " \"" + ESCAPE_STRING(arg) + "\"";                                                                     \
        tCmd += " \"-__CYANITH_RUNNER_ID=" + ESCAPE_STRING(SESSION_PREFIX + configId) + "\"";                              \
        tCmd += "; exit";                                                                                                  \
        command += ESCAPE_STRING(tCmd);                                                                                    \
        command += "\"";                                                                                                   \
        int result = system(command.c_str());                                                                              \
        if (result == 0)                                                                                                   \
            std::cout << Color::Green << "Started " << configId << Style::Reset << std::endl;                              \
        else                                                                                                               \
            std::cerr << Color::Red << "Failed to start " << configId << Style::Reset << std::endl;                        \
    } while (0)

#define stop_session(configId)                                                                                             \
    do                                                                                                                     \
    {                                                                                                                      \
        if (!hasConfig(configId))                                                                                          \
        {                                                                                                                  \
            std::cout << Color::Red << "Config with ID '" << configId << "' does not exist." << Style::Reset << std::endl; \
            break;                                                                                                         \
        }                                                                                                                  \
        Config config = load(configId);                                                                                    \
        system("screen -wipe > /dev/null 2>&1");                                                                           \
        if (!config.isRunning())                                                                                           \
        {                                                                                                                  \
            std::cout << Color::Yellow << "Config " << configId << " is not running." << Style::Reset << std::endl;        \
            break;                                                                                                         \
        }                                                                                                                  \
        std::cout << Color::Green << "Stopping " << configId << "..." << Style::Reset << std::endl;                        \
        check_stop(configId);                                                                                              \
        system("screen -wipe > /dev/null 2>&1");                                                                           \
        std::cout << Color::Green << "Stopped " << configId << Style::Reset << std::endl;                                  \
    } while (0)

#define getPids(pids, configId)                                                                       \
    std::vector<unsigned long long> pids;                                                             \
    do                                                                                                \
    {                                                                                                 \
        std::string cmd = "pgrep -f \"";                                                              \
        auto id = SESSION_PREFIX + configId;                                                          \
        id = std::string() + "[" + id[0] + "]" + id.substr(1);                                        \
        cmd += ESCAPE_STRING(id) + "\"";                                                              \
        FILE *fp = popen(cmd.c_str(), "r");                                                           \
        if (fp == nullptr)                                                                            \
        {                                                                                             \
            std::cerr << Color::Red << "Failed to run command: " << cmd << Style::Reset << std::endl; \
            break;                                                                                    \
        }                                                                                             \
        __gnu_cxx::stdio_filebuf<char> filebuf(fp, std::ios::in);                                     \
        std::istream is(&filebuf);                                                                    \
        unsigned long long pid;                                                                       \
        while (is >> pid)                                                                             \
            pids.push_back(pid);                                                                      \
        pclose(fp);                                                                                   \
    } while (0)

#define check_stop(configId)                                                                                                         \
    do                                                                                                                               \
    {                                                                                                                                \
        for (int i = 0; i < 10; i++)                                                                                                 \
        {                                                                                                                            \
            std::string command = "screen -S \"" + ESCAPE_STRING(SESSION_PREFIX + configId) + "\" -X stuff '\003' > /dev/null 2>&1"; \
            system(command.c_str());                                                                                                 \
            sleep(100);                                                                                                              \
            command = "screen -S \"" + ESCAPE_STRING(SESSION_PREFIX + configId) + "\" -X stuff 'stop\n' > /dev/null 2>&1";           \
            system(command.c_str());                                                                                                 \
            getPids(pids, configId);                                                                                                 \
            if (!pids.empty())                                                                                                       \
            {                                                                                                                        \
                std::cout << Color::Yellow << "Waiting for processes to stop..." << std::endl;                                       \
                sleep(1000);                                                                                                         \
            }                                                                                                                        \
            else                                                                                                                     \
            {                                                                                                                        \
                std::cout << Color::Green << "All processes stopped." << Style::Reset << std::endl;                                  \
                break;                                                                                                               \
            }                                                                                                                        \
        }                                                                                                                            \
        getPids(pids, configId);                                                                                                     \
        if (pids.empty())                                                                                                            \
            break;                                                                                                                   \
        std::cout << Color::Red << "Processes still running after 5 attempts." << Style::Reset << std::endl;                         \
        std::cout << Color::Red << "Killing processes..." << Style::Reset << std::endl;                                              \
        for (const auto &pid : pids)                                                                                                 \
            kill(pid, 9);                                                                                                            \
    } while (0)

struct Start : Command
{
    Start() : Command("start") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: start <configId>" << Style::Reset << std::endl;
            return;
        }
        checkEnvironment();
        const std::string &configId = args[0];
        start_session(configId);
    }

    virtual std::string help() const override
    {
        return "Usage: start <configId>\n"
               "Starts the specified configuration.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            completions = getConfigIds();
        }
    }
};

struct Stop : Command
{
    Stop() : Command("stop") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: stop <configId>" << Style::Reset << std::endl;
            return;
        }
        checkEnvironment();
        const std::string &configId = args[0];
        stop_session(configId);
    }

    virtual std::string help() const override
    {
        return "Usage: stop <configId>\n"
               "Stops the specified configuration.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            completions = getConfigIds();
        }
    }
};

struct Restart : Command
{
    Restart() : Command("restart") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: restart <configId>" << Style::Reset << std::endl;
            return;
        }
        checkEnvironment();
        const std::string &configId = args[0];
        stop_session(configId);
        sleep(100);
        start_session(configId);
    }

    virtual std::string help() const override
    {
        return "Usage: restart <configId>\n"
               "Restarts the specified configuration.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            completions = getConfigIds();
        }
    }
};

struct Status : Command
{
    Status() : Command("status") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: status <configId>" << Style::Reset << std::endl;
            return;
        }
        checkEnvironment();
        const std::string &configId = args[0];
        if (!hasConfig(configId))
        {
            std::cout << Color::Cyan << "Config " << configId << " does " << Color::Red << Style::Bold << "[not exist]" << Style::Reset << Color::Cyan << "." << Style::Reset << std::endl;
            return;
        }
        if (Config::isRunning(configId))
        {
            std::cout
                << Color::Cyan << "Config " << configId << " is "
                << Color::Green << Style::Bold << "[running]"
                << Style::Reset << Color::Cyan << "."
                << Style::Reset << std::endl;
        }
        else
        {
            std::cout
                << Color::Cyan << "Config " << configId << " is "
                << Color::Yellow << Style::Bold << "[stopped]"
                << Style::Reset << Color::Cyan << "."
                << Style::Reset << std::endl;
        }
    }

    virtual std::string help() const override
    {
        return "Usage: status <configId>\n"
               "Checks the status of the specified configuration.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            completions = getConfigIds();
        }
    }
};

struct Enter : Command
{
    Enter() : Command("enter") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 1)
        {
            std::cout << Color::Red << "Usage: enter <configId>" << Style::Reset << std::endl;
            return;
        }
        checkEnvironment();
        const std::string &configId = args[0];
        if (!hasConfig(configId))
        {
            std::cout << Color::Red << "Config " << configId << " does not exist." << Style::Reset << std::endl;
            return;
        }
        Config config = load(configId);
        if (!config.isRunning())
        {
            std::cout << Color::Red << "Config " << configId << " is not running." << Style::Reset << std::endl;
            return;
        }
        std::string command = "screen -r \"" + SESSION_PREFIX + configId + "\"";
        std::cout << Color::Yellow << Style::Bold << "Warning: \nPress Ctrl+A then D to detach from the screen session." << Style::Reset << std::endl;
        std::cout << Color::Green << "Press Enter to continue..." << Style::Reset << std::endl;
        std::cin.get();
        system(command.c_str());
    }

    virtual std::string help() const override
    {
        return "Usage: enter <configId>\n"
               "Enters the specified configuration's screen session.";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        if (index == 0)
        {
            completions = getConfigIds();
        }
    }
};

#undef sleep
#undef ESCAPE_STRING
#undef start_session
#undef stop_session
#undef getPids
#undef check_stop
#endif
