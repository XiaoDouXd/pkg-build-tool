#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <memory>
#include <filesystem>

#include "inireader.h"
#include "main-def.h"

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
    return vsVersion >= 15;
}

std::string trimString(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    return str.substr(start, end - start);
}

void checkEnv()
{
    loadCache();

    while (!isCMakeVersionGreaterThan3_24(s_cmakePath))
    {
        std:: cout << ":: 不正确的 cmake 路径 - incorrect cmake path\n"
        << ":: 请输入 cmake (>= 3.24) 路径 - please input cmake path: ";

        auto& s = s_cmakePath;
        std::getline(std::cin, s);
        s = trimString(s);
        if (s.starts_with('"')) s = s.substr(1);
        if (s.ends_with('"')) s.pop_back();
    }
    saveCache();

    while (!isMSBuildInstalled(s_msbuildPath))
    {
        std::cout << ":: 找不到合适的编译器 - can't find msvc (>= vs 15 2017) compiler\n"
        << ":: 请输入合适的编译器路径 - please input msvc compiler path: ";

        auto& s = s_msbuildPath;
        std::getline(std::cin, s);
        s = trimString(s);
        if (s.starts_with('"')) s = s.substr(1);
        if (s.ends_with('"')) s.pop_back();
    }
    saveCache();

    switch (vsVersion) {
        case 15: vsVersionStr = "Visual Studio 15 2017"; break;
        case 16: vsVersionStr = "Visual Studio 16 2019"; break;
        case 17: vsVersionStr = "Visual Studio 17 2022"; break;
        default: throw exce("错误的 vs 版本 - wrong vs version");
    }
    std::cout << ":: 结束检查 - env checked" << std::endl;

    system("pause");
    system("cls");
}
