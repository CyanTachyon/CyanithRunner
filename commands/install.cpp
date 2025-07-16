#ifndef COMMAND_INSTALL_CPP
#define COMMAND_INSTALL_CPP

#include "../command.cpp"
#include "../config.cpp"
#include "../color.cpp"
#include <functional>

extern std::string currentFile;

namespace {
    template <size_t N>
    std::string operator|(const char (&str)[N], const std::function<std::string(const std::string &)> &func)
    {
        return func(std::string(str));
    }
    std::string operator|(const std::string& str, const std::function<std::string(const std::string &)> &func)
    {
        return func(str);
    }

    const std::function<std::string(const std::string &)> trimIndent = [](const std::string &str) -> std::string
    {
        std::istringstream stream(str);
        std::string line;
        size_t minIndent = std::string::npos;
        while (std::getline(stream, line))
        {
            size_t indent = line.find_first_not_of(' ');
            if (indent != std::string::npos && indent < minIndent) minIndent = indent;
        }
        if (minIndent == std::string::npos || minIndent == 0) return str;
        std::string result;
        stream.clear();
        stream.seekg(0);
        while (std::getline(stream, line))
        {
            if (line.size() >= minIndent) result += line.substr(minIndent) + "\n";
            else result += "\n";
        }
        return result;
    };

    const std::function<std::string(const std::string &)> trimBlankLines = [](const std::string &str) -> std::string
    {
        auto start = str.find_first_not_of('\n');
        auto end = str.find_last_not_of('\n');
        if (start == std::string::npos) start = 0;
        if (end == std::string::npos) end = str.size() - 1;
        return str.substr(start, end - start + 1);
    };
}

struct InstallInfo
{
    std::map<std::string, std::string> completions;
    const std::string installPath = "/usr/bin/cyanith-runner";

    InstallInfo()
    {
        const auto bashCompletion = R"FLAG(
            _cyanith_runner_completions() { 
                local cur=${COMP_WORDS[COMP_CWORD]} 
                local completions=$(cyanith-runner ---complete ${COMP_CWORD} ${COMP_WORDS[@]}) 
                COMPREPLY=(compgen -W "$completions" -- ${cur})
            } && 
            complete -F _cyanith_runner_completions cyanith-runner 
        )FLAG" | trimIndent | trimBlankLines;

        const auto zshCompletion = R"FLAG(
            #compdef cyanith-runner
            _cyanith_runner() {
                local -a tokens=(${words})
                local CURRENT=$((CURRENT - 1))
                local completions=($(cyanith-runner ---complete $CURRENT "${tokens[@]}"))
                if (( ${#completions} )); then
                    _describe 'completions' completions
                else
                    _message 'No completions found'
                fi
            }
            _cyanith_runner
        )FLAG" | trimIndent | trimBlankLines;
        const auto fishCompletion = R"FLAG(
            function __cyanith_runner_completions
                set -l tokens (commandline -p --tokens-raw)
                set -l token_index (count (commandline -pc --tokens-raw))
                set -l completions (cyanith-runner ---complete $token_index $tokens)
                for comp in $completions
                    echo $comp
                end
            end
            complete -f -c cyanith-runner -a "(__cyanith_runner_completions)"
        )FLAG" | trimIndent | trimBlankLines;

        completions["/usr/share/bash-completion/completions/cyanith-runner"] = bashCompletion;
        completions["/usr/local/share/bash-completion/completions/cyanith-runner"] = bashCompletion;
        completions["/etc/bash_completion.d/cyanith-runner"] = bashCompletion;

        completions["/usr/share/zsh/site-functions/_cyanith-runner"] = zshCompletion;
        completions["/usr/local/share/zsh/site-functions/_cyanith-runner"] = zshCompletion;

        completions["/usr/share/fish/vendor_completions.d/cyanith-runner.fish"] = fishCompletion;
    }
};

struct Install : Command, private InstallInfo
{
    Install() : Command("--install") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 0)
        {
            std::cout << Color::Red << "Usage: install" << Style::Reset << std::endl;
            return;
        }
        std::cout << Color::Green << "Installing cyanith-runner..." << Style::Reset << std::endl;
        std::filesystem::path sourcePath(currentFile);
        std::filesystem::path targetPath(installPath);
        try
        {
            if (sourcePath.parent_path() != targetPath.parent_path())
            {
                std::filesystem::copy_file(sourcePath, targetPath, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::remove(sourcePath);
            }
            std::cout << Color::Green << "Installed cyanith-runner to " << installPath << Style::Reset << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << Color::Red << "Failed to install cyanith-runner: " << e.what() << Style::Reset << std::endl;
        }

        for (const auto &[path, content] : completions) try
        {
            std::filesystem::path completionPath(path);
            if (!std::filesystem::exists(completionPath.parent_path()))
            {
                std::cout << Color::Yellow << "Could not find directory for completion file, skipping: " << path << Style::Reset << std::endl;
                continue;
            }
            std::ofstream outFile(path);
            if (!outFile)
            {
                throw std::runtime_error("Could not open completion file for writing: " + path);
            }
            outFile << content;
            outFile.close();
            std::cout << Color::Green << "Installed shell completions to " << path << Style::Reset << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << Color::Red << "Failed to install shell completions: " << e.what() << Style::Reset << std::endl;
        }
    }

    virtual std::string help() const override
    {
        return "Usage: install\n"
               "Installs the cyanith-runner to system.\n";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        // No completions for this command
    }
};

struct Uninstall : Command, private InstallInfo
{
    Uninstall() : Command("--uninstall") {}

    virtual void execute(const std::vector<std::string> &args) const override
    {
        if (args.size() != 0)
        {
            std::cout << Color::Red << "Usage: uninstall" << Style::Reset << std::endl;
            return;
        }
        std::cout << Color::Green << "Uninstalling cyanith-runner..." << Style::Reset << std::endl;
        try
        {
            std::filesystem::remove(installPath);
            std::cout << Color::Green << "Uninstalled cyanith-runner from " << installPath << Style::Reset << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << Color::Red << "Failed to uninstall cyanith-runner: " << e.what() << Style::Reset << std::endl;
        }
        for (const auto &[path, content] : completions) try
        {
            std::filesystem::path completionPath(path);
            if (!std::filesystem::exists(completionPath))
            {
                std::cout << Color::Yellow << "Completion file does not exist, skipping: " << path << Style::Reset << std::endl;
                continue;
            }
            std::filesystem::remove(completionPath);
            std::cout << Color::Green << "Removed completion file: " << path << Style::Reset << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << Color::Red << "Failed to remove completion file: " << e.what() << Style::Reset << std::endl;
        }

        std::cout << Color::Green << "Uninstallation complete." << Style::Reset << std::endl;
        std::cout << Color::Yellow << "You may need to remove the configs in " << getConfigFolder() << ", if you want to clean up completely." << Style::Reset << std::endl;
    }

    virtual std::string help() const override
    {
        return "Usage: uninstall\n"
               "Uninstalls the cyanith-runner from system.\n";
    }

    virtual void complete(const std::vector<std::string> &args, const int &index, std::vector<std::string> &completions) const override
    {
        // No completions for this command
    }
};

#endif