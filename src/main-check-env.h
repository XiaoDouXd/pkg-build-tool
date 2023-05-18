#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <memory>
#include <filesystem>

#include "inireader.h"
#include "collect-files.h"


const auto iniName = "envCache.ini";
std::string s_cmakePath = "cmake";
std::string s_msbuildPath = "msbuild";
std::string s_name = "untitled";
std::string s_exePath = "./smbx.exe";
std::string s_gameFilePath = "./worlds/m.elvl";
std::string s_srcFilePath = "./src.zip";
std::string s_iconFilePath = "./icon.ico";

int vsVersion = 0;
std::unique_ptr<INIReader> p_reader;

namespace fs = std::filesystem;
static void tryCreateParentDir(const fs::path &p) {
    if (fs::exists(p)) return;

    const fs::path del("/");

    auto temp = fs::path();
    for (auto i = p.begin(); i != p.end(); i++) {
        temp += *i;
        temp += del;
        auto j = i;
        if (++j == p.end()) return;
        if (!fs::exists(temp))
            fs::create_directory(temp);
    }
}

void saveCache()
{
    auto iniPath = outputPath + iniName;
    tryCreateParentDir(iniPath);
    std::ofstream o_ini(iniPath, std::ios_base::out | std::ios_base::trunc);
    if (!o_ini.is_open()) return;
    // env
    o_ini << "[env]\n";
    o_ini << "cmake=" << s_cmakePath << std::endl;
    o_ini << "msbuild=" << s_msbuildPath << std::endl;

    // src
    o_ini << "[src]\n";
    o_ini << "name=" << s_name << std::endl;
    o_ini << "exe=" << s_exePath << std::endl;
    o_ini << "game=" << s_gameFilePath << std::endl;

    // game
    o_ini << "[game]\n";
    o_ini << "src=" << s_srcFilePath << std::endl;
    o_ini << "icon=" << s_iconFilePath;
    o_ini.close();
}

void loadCache()
{
    auto iniPath = outputPath + iniName;
    p_reader = std::make_unique<INIReader>(iniPath);
    if (p_reader->ParseError() < 0)
    {
        saveCache();
        p_reader.reset();
        p_reader = std::make_unique<INIReader>(iniPath);
        if (p_reader->ParseError() < 0)
        {
            p_reader.reset();
            throw exce(":: 无法创建或读取设置 - can't create or read config");
        }
    }

    const auto& reader = *p_reader;
    s_cmakePath = reader.Get("env", "cmake", s_cmakePath);
    s_msbuildPath = reader.Get("env", "msbuild", s_msbuildPath);
    s_name = reader.Get("src", "name", s_name);
    s_exePath = reader.Get("src", "exe", s_exePath);
    s_gameFilePath = reader.Get("src", "game", s_gameFilePath);
}

bool isCMakeVersionGreaterThan3_24(const std::string& cmakePath)
{
    std::string versionCommand = "\"" + cmakePath + "\"" + " --version";
    std::cout << ">> " << versionCommand << std::endl;
    std::string cmakeOutput;

    FILE* pipe = _popen(versionCommand.c_str(), "r");
    if (pipe)
    {
        char buffer[1024];
        while (!feof(pipe))
        {
            if (fgets(buffer, 1024, pipe) != nullptr)
                cmakeOutput += buffer;
        }
        _pclose(pipe);
    }

    std::regex versionRegex(R"(cmake version ([0-9]+)\.([0-9]+)\.([0-9]+))");
    std::smatch versionMatch;
    if (std::regex_search(cmakeOutput, versionMatch, versionRegex))
    {
        int majorVersion = std::stoi(versionMatch[1].str());
        int minorVersion = std::stoi(versionMatch[2].str());
        // int patchVersion = std::stoi(versionMatch[3].str());

        if (majorVersion > 3 || (majorVersion == 3 && minorVersion > 24))
            return true;
    }

    return false;
}

bool isMSBuildInstalled(const std::string& msbuildPath)
{
    std::string msbuildCommand = "\"" + msbuildPath + "\"" + " /version";
    std::cout << ">> " << msbuildCommand << std::endl;

    FILE* pipe = _popen(msbuildCommand.c_str(), "r");
    if (pipe)
    {
        msbuildCommand.clear();
        char buffer[1024];
        while (!feof(pipe))
        {
            if (fgets(buffer, 1024, pipe) != nullptr)
                msbuildCommand += buffer;
        }
        _pclose(pipe);
    }

    std::regex versionRegex(R"(MSBuild version ([0-9]+)\.([0-9]+)\.)");
    std::smatch versionMatch;
    if (std::regex_search(msbuildCommand, versionMatch, versionRegex))
    {
        int majorVersion = std::stoi(versionMatch[1].str());
        vsVersion = majorVersion;
    }

    // int result = std::system(msbuildCommand.c_str());
    return vsVersion;
}

std::string trimString(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    return str.substr(start, end - start);
}

std::string extractVisualStudioVersion(const std::string& msbuildPath) {
    // 提取路径中的目录部分
    std::filesystem::path directory = std::filesystem::path(msbuildPath).parent_path();

    // 使用正则表达式匹配 Visual Studio 版本号
    std::regex versionRegex(R"((\d{2})(\.\d)?)");
    std::smatch match;
    std::string version;

    // 在目录中搜索匹配的版本号
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::string filename = entry.path().filename().string();
        if (std::regex_search(filename, match, versionRegex)) {
            version = match[0];
            break;
        }
    }

    return version;
}

void checkEnv()
{
    loadCache();

    while (!isCMakeVersionGreaterThan3_24(s_cmakePath))
    {
        std:: cout << ":: 不正确的 cmake 路径 - incorrect cmake path\n"
        << ":: 请输入 cmake (version >= 3.24) 路径 - please input cmake path: ";

        auto& s = s_cmakePath;
        std::getline(std::cin, s);
        s = trimString(s);
        if (s.starts_with('"')) s = s.substr(1);
        if (s.ends_with('"')) s.pop_back();
    }
    saveCache();

    while (!isMSBuildInstalled(s_msbuildPath))
    {
        std::cout << ":: 找不到编译器 - can't find msvc compiler"
        << ":: 请输入编译器路径 - please input msvc compiler path: ";

        auto& s = s_msbuildPath;
        std::getline(std::cin, s);
        s = trimString(s);
        if (s.starts_with('"')) s = s.substr(1);
        if (s.ends_with('"')) s.pop_back();
    }
    saveCache();
    system("pause");
    system("cls");
}
