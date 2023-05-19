
#include <direct.h>
#include "main-check-env.h"
#include "main-write-files.h"
#include "main-copy-file.h"

#include <windows.h>

void run(std::string& cmd)
{
    std::cout << ">> " << cmd << std::endl;
    auto r = std::system(cmd.c_str());
    if (r != 0) throw exce("构建错误 - build fail");
    cmd.clear();
}

int main()
{
    try {
        SetConsoleOutputCP(CP_UTF8);
        checkEnv();
        unpackSource();
        copyFile();

        std::string buildFlag;
        std::cout << ":: 构建设置 - build flag (default as " + s_buildFlag + "): ";
        auto& s = buildFlag;

        std::getline(std::cin, s);
        if (s.empty()) s = s_buildFlag;

        std::string cmd, cmakeGenPath = outputPath + "../cmake-gen";
        if (!fs::exists(cmakeGenPath)) fs::create_directory(cmakeGenPath);
        _chdir(cmakeGenPath.c_str());

        if (s_cmakePath == "cmake")
            cmd = s_cmakePath + " -G \"" + vsVersionStr + "\" \"../" + outputPath + "\"";
        else
            cmd = "\"" + s_cmakePath + "\"" + " -G \"" + vsVersionStr + "\" \"../" + outputPath + "\"";
        run(cmd);

        cmd = "\"" + s_msbuildPath + "\" smbx-38a-starter.sln " + s_buildFlag;
        run(cmd);

        std::cout << "构建成功 - build success\n";
    }
    catch (std::exception& e) {
        std::cerr << ":: err: " << e.what() << std::endl;
        system("pause");
        return -1;
    }
    system("pause");
    return 0;
}

