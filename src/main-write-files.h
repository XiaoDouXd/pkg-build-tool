#pragma once

#include "main-def.h"

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

    std::string name = "*", pathToProgram = "*", pathToGameFile = "*";

    while(name == "*")
    {
        std::cout << ">> 项目名称 - project name (default as " + s_name + "): ";
        auto& s = name;

        std::getline(std::cin, s);
        if (s.empty()) break;
        if (!std::regex_match(s, std::regex("[_a-zA-Z][_0-9a-zA-Z]*")))
        {
            std::cout << ":: 非法路径 - illegal name" << std::endl;
            s = "*";
        }
    }
    if (name.empty()) name = "untitled";
    s_name = name;

    while(pathToProgram == "*")
    {
        std::cout << ">> 项目启动程序 - path to .exe (default as " + s_exePath + "): ";
        auto& s = pathToProgram;

        std::getline(std::cin, s);
        if (s.empty()) break;
        if (!std::regex_match(s,std::regex(R"([^:<>"|*?]+.exe)")))
        {
            std::cout << ":: 非法路径 - illegal path" << std::endl;
            s = "*";
        }
    }
    if (pathToProgram.empty()) pathToProgram = "./smbx.exe";
    s_exePath = pathToProgram;

    while(pathToGameFile == "*")
    {
        std::cout << ">> 项目游戏文件 - path to gameFile (default as " + s_gameFilePath + "): ";
        auto& s = pathToGameFile;

        std::getline(std::cin, s);
        if (s.empty()) break;
        if (!std::regex_match(s,std::regex(R"([^:<>"|*?]+)")))
        {
            std::cout << ":: 非法路径 - illegal path" << std::endl;
            s = "*";
        }
    }
    if (pathToGameFile.empty()) pathToGameFile = "./worlds/main.elvl";
    s_gameFilePath = pathToGameFile;


    saveCache();
    auto content = std::string((const char*)RC::SRC_MAIN_CPP.data());
    auto r = regex_replace(content, std::regex(R"(\$\{name\})"), name);
    r = regex_replace(r, std::regex(R"(\$\{pathToProgram\})"), pathToProgram);
    r = regex_replace(r, std::regex(R"(\$\{pathToGameFile\})"), pathToGameFile);
    o.open(pathToMain, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    o.write(r.data(), (long long)r.size());
    o.close();

    std::cout << ":: 生成设置完毕 - generating set success\n";
    system("pause");
    system("cls");
}