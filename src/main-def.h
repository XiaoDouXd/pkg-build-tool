#pragma once

#include <string>
#include "main-def-extern.h"

const std::string outputPath = "source-gen/";
std::string s_cmakePath = "cmake";
std::string s_msbuildPath = "msbuild";
std::string s_name = "untitled";
std::string s_exePath = "./main.exe";
std::string s_gameFilePath = "./main.lvl";
std::string s_srcFilePath = "./src.zip";
std::string s_iconFilePath = "./icon.ico";
std::string vsVersionStr = "Visual Studio 17 2022";
std::string s_buildFlag = "/p:Configuration=Release /p:Platform=x64";
int vsVersion = 0;

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


#include <filesystem>
namespace fs = std::filesystem;

const auto iniName = "envCache.ini";
std::unique_ptr<INIReader> p_reader;

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
    o_ini << "icon=" << s_iconFilePath << std::endl;

    // build
    o_ini << "[build]\n";
    o_ini << "flag=" << s_buildFlag << std::endl;
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
            throw exce("无法创建或读取设置 - can't create or read config");
        }
    }

    const auto& reader = *p_reader;
    s_cmakePath = reader.Get("env", "cmake", s_cmakePath);
    s_msbuildPath = reader.Get("env", "msbuild", s_msbuildPath);
    s_name = reader.Get("src", "name", s_name);
    s_exePath = reader.Get("src", "exe", s_exePath);
    s_gameFilePath = reader.Get("src", "game", s_gameFilePath);
    s_srcFilePath = reader.Get("game", "src", s_srcFilePath);
    s_iconFilePath = reader.Get("game", "icon", s_iconFilePath);
    s_buildFlag = reader.Get("build", "flag", s_buildFlag);
}

std::string trimString(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    return str.substr(start, end - start);
}

void formatPath(std::string& s)
{
    if (s.length() <= 1) return;
    s = trimString(s);
    if (s.starts_with('"')) s = s.substr(1);
    if (s.ends_with('"')) s.pop_back();
}