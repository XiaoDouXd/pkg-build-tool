#include <string>

// game name, which determines the name of data folder generated
const std::string gameName = "${name}";

// path to game exe (start from ./!$gameName$_data)
const std::string pathToExe = "${pathToProgram}";

// path to game file (start from ./!$gameName$_data)
const std::string pathToMain = "${pathToGameFile}";

#include "private/main-content.h"