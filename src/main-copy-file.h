#pragma once

#include "main-def-extern.h"
#include "main-def.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <memory>
#include <Windows.h>
#include <filesystem>

#include "ICON_ICON_ICO.h"

void copyFile()
{
    std::string icon = "*", pack = "*";

    while (icon == "*")
    {
        std::cout << ":: 图标路径 - icon path (default as " + s_iconFilePath + "): ";
        auto& s = icon;

        std::getline(std::cin, s);
        formatPath(s);
        if (s.empty()) break;
        if (!std::regex_match(s, std::regex(R"([^<>"|*?]+.ico)")))
        {
            std::cout << ":: 非法的文件或路径 - illegal file or path" << std::endl;
            s = "*";
        }
    }
    if (icon.empty()) icon = s_iconFilePath;
    s_iconFilePath = icon;
    if (!fs::exists(s_iconFilePath))
    {
        std::cout << ":: 警告! 找不到指定图标, 将使用默认图标 - warning! can't find target icon, will use default icon\n";
        o.open(outputPath + "icon/icon.ico", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        o.write((const char*)RC::ICON_ICON_ICO.data(), RC::ICON_ICON_ICO.size());
        o.close();
    } else
    {
        if (fs::exists(outputPath + "icon/icon.ico")) fs::remove(outputPath + "icon/icon.ico");
        fs::copy_file(s_iconFilePath, outputPath + "icon/icon.ico");
    }

    while (pack == "*")
    {
        std::cout << ":: 游戏源文件路径 - game pack path (default as " + s_srcFilePath + "): ";
        auto& s = pack;

        std::getline(std::cin, s);
        formatPath(s);
        if (s.empty()) s = s_srcFilePath;
        if (!std::regex_match(s, std::regex(R"([^<>"|*?]+.(zip|ZIP|Zip|zIP|zIp|ziP|ZIp|ZiP))")))
        {
            std::cout << ":: 非法的文件或路径 - illegal file or path" << std::endl;
            s = "*";
        }
        if (!fs::exists(s))
        {
            std::cout << ":: 文件不存在 - file not exist" << std::endl;
            s = "*";
        }
    }
    s_srcFilePath = pack;
    if (!fs::exists(outputPath + "rc/res")) fs::create_directory(outputPath + "rc/res");
    if (fs::exists(outputPath + "rc/res/ZIP.zip")) fs::remove(outputPath + "rc/res/ZIP.zip");
    fs::copy_file(s_srcFilePath, outputPath + "rc/res/ZIP.zip");

    std::string cmd = fs::current_path().string() + "/" + outputPath + "binary-generator.exe \"./" + outputPath + "/rc/res/ZIP.zip\"";
    std::cout << ">> " << cmd << std::endl;
    std::string cmdOp;
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (pipe)
    {
        char buffer[1024];
        while (!feof(pipe))
        {
            if (fgets(buffer, 1024, pipe) != nullptr)
                cmdOp += buffer;
        }
        _pclose(pipe);
    }
    std::cout << ">> " << cmdOp << std::endl;
    saveCache();

    std::cout << ":: 源文件收集完毕 - file collect success\n\n:: ------------------------------------------------\n\n";
}
