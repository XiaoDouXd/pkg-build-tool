#pragma once

#include "collect-files.h"
#include "main-check-env.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;
auto o = std::ofstream();

#include "SRC_MAIN_CPP.h"

void unpackSource()
{
    if(!fs::exists(outputPath + "/CMakeLists.txt")) collectStaticFiles();

    auto pathToMain = outputPath + "src/main.cpp";
    auto pathToConfig = outputPath + "Config.cmake";

    char* sBuffer = new char[2048]{};
    std::string name = "*", pathToProgram = "*", pathToGameFile = "*";

    while(name == "*")
    {
        std::cout << ">> 项目名称 - project name (default as " + s_name + "): ";
        std::cin.getline(sBuffer, 2048);
        name = std::string(sBuffer);
        if (name.empty()) break;
        if (!std::regex_match(name, std::regex("[_a-zA-Z][_0-9a-zA-Z]*")))
        {
            std::cout << "illegal name" << std::endl;
            name = "*";
        }
    }
    if (name.empty()) name = "untitled";
    s_name = name;
    saveCache();

    while(pathToProgram == "*")
    {
        std::cout << ">> 项目启动程序 - path to .exe (default as " + s_exePath + "): ";
        std::cin.getline(sBuffer, 2048);
        pathToProgram = std::string(sBuffer);
        if (pathToProgram.empty()) break;
        if (!std::regex_match(pathToProgram,std::regex(R"([^:<>"|*?]+.exe)")))
        {
            std::cout << "illegal path" << std::endl;
            pathToProgram = "*";
        }
    }
    if (pathToProgram.empty()) pathToProgram = "./smbx.exe";
    s_exePath = pathToProgram;
    saveCache();

    while(pathToGameFile == "*")
    {
        std::cout << ">> 项目游戏文件 - path to gameFile (default as " + s_gameFilePath + "): ";
        std::cin.getline(sBuffer, 2048);
        pathToGameFile = std::string(sBuffer);
        if (pathToGameFile.empty()) break;
        if (!std::regex_match(pathToGameFile,std::regex(R"([^:<>"|*?]+)")))
        {
            std::cout << "illegal path" << std::endl;
            pathToGameFile = "*";
        }
    }
    if (pathToGameFile.empty()) pathToGameFile = "./worlds/main.elvl";
    s_gameFilePath = pathToGameFile;
    saveCache();
    system("cls");

    auto content = std::string((const char*)RC::SRC_MAIN_CPP.data());
    auto r = regex_replace(content, std::regex(R"(\$\{name\})"), name);
    r = regex_replace(r, std::regex(R"(\$\{pathToProgram\})"), pathToProgram);
    r = regex_replace(r, std::regex(R"(\$\{pathToGameFile\})"), pathToGameFile);
    o.open(pathToMain, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    o.write(r.data(), (long long)r.size());
    o.close();
}