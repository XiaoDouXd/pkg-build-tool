#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <cstring>
#include <zconf.h>
#include <unzip.h>
#include <windows.h>
#include <array>
#include <cstddef>
#include <span>

#include "popup.h"
#include "RES_ZIP.h"

template<size_t size>
void Write(std::ofstream &f, const std::array<std::span<const uint8_t>, size> data) {
    for (const auto &sp : data) f.write((char*)sp.data(), sp.size());
}

template<size_t size>
void Write(std::ofstream &f, const std::span<const uint8_t, size> data) {
    f.write((char*)data.data(), data.size());
}

namespace fs = std::filesystem;

#define IDI_ICON1 1
#define MAX_FILENAME 512
#define READ_SIZE 8192
#define DIR_IDENTIFY '/'

class exce : public std::exception {
public:
    explicit exce(const std::string &str) : _what(str.c_str()) {}
    explicit exce(const char *str) : _what(str) {}
    [[nodiscard]] const char *what() const noexcept override { return _what; }

private:
    const char *_what;
};

void tryCreateParentDir(const fs::path &p) {
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

int count = 0;
HINSTANCE hInst = nullptr;
std::chrono::time_point<std::chrono::steady_clock> start;
BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam)
{
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == (DWORD)lParam)
    {
        HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        if (++count >= 20)
        {
            Sleep(250);
            return FALSE;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (duration.count() > 8000) return FALSE;
    return TRUE;
}

void createTask(const char *content) {
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    LPCSTR cmd = content;
    CreateProcess(nullptr,
                    (LPSTR) cmd,
                    nullptr, nullptr,
                    FALSE, 0,
                    nullptr,
                    nullptr, &startupInfo,
                    &processInfo);

    Sleep(250);
    start = std::chrono::high_resolution_clock::now();
    while (EnumWindows(FindWindowProc, (LPARAM)processInfo.dwProcessId));
    // WaitForSingleObject(processInfo.hProcess, INFINITE);
    // CloseHandle(processInfo.hProcess);
    // CloseHandle(processInfo.hThread);
}

const auto gameSrcPath = "./!" + gameName + "_data/";
const auto gameRunnerPath = "./!" + gameName + "_data/" + pathToExe;
const auto gameFilePath = "!" + gameName + "_data/" + pathToMain + "";

const auto packTempPath = "./~" + gameName + "_temp";
const auto exePath = fs::absolute(fs::path("."));

void openFile() {
    auto path = fs::absolute(gameFilePath);
    createTask(("\"" + exePath.string() + "\\\\" + gameRunnerPath + "\" \"" + path.string() + "\"").c_str());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        hInst = hInstance;
        if (fs::exists(gameRunnerPath)) {
            openFile();
            return 0;
        }

        CreateProgressDialog(hInstance, "", "Loading...");
        UpdateProgressDialog("Loading...", 0);

        if (fs::exists(gameSrcPath))
            fs::remove_all(gameSrcPath);
        if (fs::exists(packTempPath))
            fs::remove(packTempPath);
        std::ofstream f;
        f.open(packTempPath, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        Write(f, RC::RES_ZIP);
        f.close();

        fs::create_directory(gameSrcPath);

        auto zipfile = unzOpen64(packTempPath.c_str());

        // Get info about the zip file
        unz_global_info global_info;
        if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
            unzClose(zipfile);
            throw exce("could not read file global info");
        }

        // Buffer to hold data read from the zip file.
        char read_buffer[READ_SIZE];

        // Loop to extract all files
        uLong i;
        for (i = 0; i < global_info.number_entry; ++i) {
            // Get info about current file.
            unz_file_info file_info;
            char filename[MAX_FILENAME];
            if (unzGetCurrentFileInfo(
                    zipfile,
                    &file_info,
                    filename,
                    MAX_FILENAME,
                    nullptr, 0, nullptr, 0) != UNZ_OK) {
                unzClose(zipfile);
                throw exce("could not read file info");
            }

            // Check if this entry is a directory or file.
            const size_t filename_length = strlen(filename);
            if (filename[filename_length - 1] == DIR_IDENTIFY) {
                // Entry is a directory, so create it.
                if (!fs::exists(gameSrcPath + filename)) {
                    tryCreateParentDir(gameSrcPath + filename);
                    fs::create_directory(gameSrcPath + filename);
                }
            } else {
                if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                    unzClose(zipfile);
                    throw exce("could not open file");
                }

                // Open a file to write out the data.
                tryCreateParentDir(gameSrcPath + filename);
                f.open(gameSrcPath + filename, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
                if (!f.is_open()) {
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    throw exce("could not open destination file");
                }

                int error = UNZ_OK;
                do {
                    error = unzReadCurrentFile(zipfile, read_buffer, READ_SIZE);
                    if (error < 0) {
                        unzCloseCurrentFile(zipfile);
                        unzClose(zipfile);
                        throw exce("error %d" + std::to_string(error));
                    }

                    // Write data to file.
                    if (error > 0) f.write(read_buffer, error);
                } while (error > 0);

                f.close();
            }

            UpdateProgressDialog("Loading...", (float) i / (float) global_info.number_entry);
            unzCloseCurrentFile(zipfile);
            // Go the the next entry listed in the zip file.
            if ((i + 1) < global_info.number_entry) {
                if (unzGoToNextFile(zipfile) != UNZ_OK) {
                    unzClose(zipfile);
                    throw exce("could not read next file");
                }
            }
        }
        if (WPopup) DestroyWindow(WPopup);
        unzClose(zipfile);
        fs::remove(packTempPath);

        openFile();
        return 0;
    }
    catch (std::exception &e) {
        if (WPopup) DestroyWindow(WPopup);
        MessageBox(nullptr, e.what(), "app throw", MB_ICONERROR | MB_OK);
    }
    return 0;
}