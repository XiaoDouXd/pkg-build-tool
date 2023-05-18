#include "exce.h"
#include "main-check-env.h"
#include "main-write-files.h"
#include "main-copy-file.h"

void cmakeGen(const std::string& vsVersion)
{
    std::string cmakeCommand = "cmake -S projPath -B projBuild -G \""+ vsVersion +"\"";
    std::cout << ">> " << cmakeCommand << std::endl;
    int result = std::system(cmakeCommand.c_str());
    if (result != 0) throw exce("CMake configuration failed.\n");
}

int main()
{
    try {
        std::system("chcp 65001");
        checkEnv();
        unpackSource();
        copyFile();

        // build
        // 查询当前系统的 Visual Studio 版本
        const char* command = R"(reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /s)";
        FILE* stream = _popen(command, "r");
        if (stream) {
            const int bufferSize = 2048;
            char output[bufferSize];

            while (fgets(output, bufferSize, stream) != nullptr) {
                // 检查输出是否包含 Visual Studio 版本号
                if (strstr(output, "15.0") != nullptr) {
                    std::cout << ":: Detected Visual Studio 2017\n";
                    cmakeGen("Visual Studio 15 2017");
                    break;
                }
                else if (strstr(output, "16.0") != nullptr) {
                    std::cout << "Detected Visual Studio 2019\n";
                    cmakeGen("Visual Studio 16 2019");
                    break;
                }
                else if (strstr(output, "17.0") != nullptr) {
                    // Visual Studio 2019
                    std::cout << "Detected Visual Studio 2022\n";
                    cmakeGen("Visual Studio 17 2022");
                    break;
                }
                throw exce("不支持的 vs 版本 - unsupported version of vs");
            }
            _pclose(stream);
        }
    }
    catch (std::exception& e) {
        std::cerr << ":: err: " << e.what() << std::endl;
        system("pause");
        return -1;
    }
    system("pause");
    return 0;
}

