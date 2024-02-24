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

    std::regex versionRegex("MSBuild\\s[\\s\\S]+\\s([0-9]+)\\.([0-9]+)\\.");
    std::smatch versionMatch;

    if (std::regex_search(msbuildCommand, versionMatch, versionRegex))
    {
        int majorVersion = std::stoi(versionMatch[1].str());
        vsVersion = majorVersion;
    }

    // int result = std::system(msbuildCommand.c_str());
    return vsVersion >= 15;
}



void checkEnv()
{
    loadCache();
    std::cout << ":: ------------------------------------------------\n\n";

    while (!isCMakeVersionGreaterThan3_24(s_cmakePath))
    {
        std:: cout << ":: 不正确的 cmake 路径 - incorrect cmake path\n"
        << ":: 请输入 cmake (>= 3.24) 路径 - please input cmake path: ";

        auto& s = s_cmakePath;
        std::getline(std::cin, s);
        formatPath(s);
    }
    saveCache();

    while (!isMSBuildInstalled(s_msbuildPath))
    {
        std::cout << ":: 找不到合适的构建程序 - can't find msbuild (>= vs 16 2019)\n"
        << ":: 请输入合适的构建程序 - please input msbuild path: ";

        auto& s = s_msbuildPath;
        std::getline(std::cin, s);
        formatPath(s);
    }
    saveCache();

    switch (vsVersion) {
        case 15: vsVersionStr = "Visual Studio 15 2017"; break;
        case 16: vsVersionStr = "Visual Studio 16 2019"; break;
        case 17: vsVersionStr = "Visual Studio 17 2022"; break;
        default: throw exce("错误的 vs 版本 - wrong vs version");
    }
    std::cout << ":: 结束环境检查 - env checked\n\n:: ------------------------------------------------\n\n";
}
