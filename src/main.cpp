#include <exception>
#include <string>

class exce : public std::exception {
public:
    explicit exce(const std::string &str) : _what(str.c_str()) {}
    explicit exce(const char *str) : _what(str) {}
    [[nodiscard]] const char *what() const noexcept override { return _what; }

private:
    const char *_what;
};

#include "collect-files.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;
auto o = std::ofstream();

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

#include "SRC_MAIN_CPP.h"

void unpackSource()
{
    if(!fs::exists(outputPath + "/CMakeLists.txt")) collectStaticFiles();

    auto pathToMain = outputPath + "src/main.cpp";
    auto pathToConfig = outputPath + "Config.cmake";

    system("chcp 65001");

    char* sBuffer = new char[2048]{};
    std::string name = "*", pathToProgram = "*", pathToGameFile = "*";

    while(name == "*")
    {
        std::cout << ">> 项目名称 - project name (default as untitled): ";
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

    while(pathToProgram == "*")
    {
        std::cout << ">> 项目启动程序 - path to .exe (default as ./smbx.exe): ";
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

    while(pathToGameFile == "*")
    {
        std::cout << ">> 项目游戏文件 - path to gameFile (default as ./worlds/main.elvl): ";
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
    system("cls");

    auto content = std::string((const char*)RC::SRC_MAIN_CPP.data());
    auto r = regex_replace(content, std::regex(R"(\$\{name\})"), name);
    r = regex_replace(r, std::regex(R"(\$\{pathToProgram\})"), pathToProgram);
    r = regex_replace(r, std::regex(R"(\$\{pathToGameFile\})"), pathToGameFile);
    o.open(pathToMain, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    o.write(r.data(), (long long)r.size());
    o.close();
}

int main()
{
    try {
        unpackSource();

    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

