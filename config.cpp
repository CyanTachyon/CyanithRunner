#ifndef CONFIG_CPP
#define CONFIG_CPP

#include <string>
#include <vector>
#include "json.hpp"
#include <fstream>
#include <iostream>

const std::string SESSION_PREFIX = "__CYANITH_RUNNER_";

struct Config
{
    std::string id;
    std::string jarPath;
    std::string mainClassName;
    std::string workingDirectory;
    std::vector<std::string> args;

    bool isRunning()
    {
        return isRunning(id);
    }

    static bool isRunning(const std::string &id)
    {
        std::string command = "screen -S ";
        command += SESSION_PREFIX + id + " -Q select > /dev/null";
        return system(command.c_str()) == 0;
    }
};

std::string getConfigFolder()
{
    const char *configDir = std::getenv("");
    if (configDir)
    {
        std::filesystem::create_directories(configDir);
        return std::string(configDir);
    }
    else if (const char *homeDir = std::getenv("HOME"))
    {
        const auto dir = std::filesystem::path(homeDir) / ".config" / "cyanith-runner";
        std::filesystem::create_directories(dir);
        return dir.string();
    }
    else
    {
        throw std::runtime_error("Could not determine config folder");
    }
}

Config loadFromConfigFile(const std::string &filePath)
{
    if (!std::filesystem::exists(filePath))
    {
        throw std::runtime_error("Config file does not exist: " + filePath);
    }
    if (!std::filesystem::is_regular_file(filePath))
    {
        throw std::runtime_error("Config file is not a regular file: " + filePath);
    }
    if (filePath.substr(filePath.find_last_of(".") + 1) != "json")
    {
        throw std::runtime_error("Config file is not a .json file: " + filePath);
    }
    const auto fileName = std::filesystem::path(filePath).filename().string();
    const auto id = fileName.substr(0, fileName.find_last_of('.'));
    Config config;
    config.id = id;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open config file: " + filePath);
    }
    nlohmann::json jsonConfig = nlohmann::json::parse(file);
    if (!jsonConfig.contains("jarPath") || !jsonConfig.contains("args") || !jsonConfig.contains("mainClassName") || !jsonConfig.contains("workingDirectory"))
    {
        throw std::runtime_error("Invalid config file format: " + filePath);
    }
    if (!jsonConfig["jarPath"].is_string() || !jsonConfig["args"].is_array() || !jsonConfig["mainClassName"].is_string() || !jsonConfig["workingDirectory"].is_string())
    {
        throw std::runtime_error("Invalid config file format: " + filePath);
    }
    config.jarPath = jsonConfig["jarPath"].get<std::string>();
    config.workingDirectory = jsonConfig["workingDirectory"].get<std::string>();
    config.mainClassName = jsonConfig["mainClassName"].get<std::string>();
    for (const auto &arg : jsonConfig["args"])
    {
        if (!arg.is_string())
        {
            throw std::runtime_error("Invalid config file format: " + filePath);
        }
        config.args.push_back(arg.get<std::string>());
    }
    return config;
}

std::vector<std::string> getConfigFiles()
{
    std::vector<std::string> configFiles;
    std::string configFolder = getConfigFolder();
    for (const auto &entry : std::filesystem::directory_iterator(configFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            configFiles.push_back(entry.path().string());
        }
    }
    return configFiles;
}

std::vector<std::string> getConfigIds()
{
    const auto configFiles = getConfigFiles();
    std::vector<std::string> configIds;
    for (const auto &filePath : configFiles)
    {
        const auto fileName = std::filesystem::path(filePath).filename().string();
        const auto id = fileName.substr(0, fileName.find_last_of('.'));
        configIds.push_back(id);
    }
    return configIds;
}

std::vector<Config> loadAllConfigs()
{
    std::vector<Config> configs;
    std::vector<std::string> configFiles = getConfigFiles();
    for (const auto &filePath : configFiles)
    {
        try
        {
            configs.push_back(loadFromConfigFile(filePath));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading config file " << filePath << ": " << e.what() << std::endl;
        }
    }
    return configs;
}

Config load(const std::string &id)
{
    auto folder = getConfigFolder();
    auto filePath = folder + "/" + id + ".json";
    if (!std::filesystem::exists(filePath))
    {
        throw std::runtime_error("Config file does not exist: " + filePath);
    }
    return loadFromConfigFile(filePath);
}

void save(const Config &config, const std::string &id)
{
    auto folder = getConfigFolder();
    std::filesystem::create_directories(folder);
    auto filePath = folder + "/" + id + ".json";
    nlohmann::json jsonConfig;
    jsonConfig["jarPath"] = config.jarPath;
    jsonConfig["args"] = config.args;
    jsonConfig["mainClassName"] = config.mainClassName;
    jsonConfig["workingDirectory"] = config.workingDirectory;
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open config file for writing: " + filePath);
    }
    file << jsonConfig.dump(4);
}

void removeConfig(const std::string &id)
{
    auto folder = getConfigFolder();
    auto filePath = folder + "/" + id + ".json";
    if (std::filesystem::exists(filePath))
    {
        std::filesystem::remove(filePath);
    }
}

bool hasConfig(const std::string &id)
{
    auto folder = getConfigFolder();
    auto filePath = folder + "/" + id + ".json";
    return std::filesystem::exists(filePath);
}

void renameConfig(const std::string &oldId, const std::string &newId)
{
    if (!hasConfig(oldId))
    {
        throw std::runtime_error("Config with ID '" + oldId + "' does not exist.");
    }
    if (hasConfig(newId))
    {
        throw std::runtime_error("Config with ID '" + newId + "' already exists.");
    }
    auto folder = getConfigFolder();
    auto oldFilePath = folder + "/" + oldId + ".json";
    auto newFilePath = folder + "/" + newId + ".json";
    std::filesystem::rename(oldFilePath, newFilePath);
}

#endif