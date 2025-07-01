#include <fstream>
#include <filesystem>
#include <string>

#include "main-def-extern.h"

namespace fs = std::filesystem;
auto o_static = std::ofstream();
auto pCache = std::string();

static void tryCreateParentDirWhileCollectingFiles(const fs::path &p) {
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

static void writeFile(const uint8_t* data, size_t size, const char* path)
{
    pCache += outputPath;
    pCache += path;
    tryCreateParentDirWhileCollectingFiles(pCache);
    o_static.open(pCache, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    pCache.clear();

    o_static.write((const char*)data, (long long)size);
    o_static.close();
}

// ---------------------------- minizip
#include "_3RD_MINIZIP_CRYPT_H.h"
#include "_3RD_MINIZIP_IOAPI_C.h"
#include "_3RD_MINIZIP_IOAPI_H.h"
#include "_3RD_MINIZIP_UNZIP_C.h"
#include "_3RD_MINIZIP_UNZIP_H.h"
#include "_3RD_MINIZIP_ZIP_C.h"
#include "_3RD_MINIZIP_ZIP_H.h"
// ---------------------------- minizip

// ---------------------------- zlib
#include "_3RD_ZLIB_ADLER32_C.h"
#include "_3RD_ZLIB_COMPRESS_C.h"
#include "_3RD_ZLIB_CRC32_C.h"
#include "_3RD_ZLIB_CRC32_H.h"
#include "_3RD_ZLIB_DEFLATE_C.h"
#include "_3RD_ZLIB_DEFLATE_H.h"
#include "_3RD_ZLIB_GZCLOSE_C.h"
#include "_3RD_ZLIB_GZGUTS_H.h"
#include "_3RD_ZLIB_GZLIB_C.h"
#include "_3RD_ZLIB_GZREAD_C.h"
#include "_3RD_ZLIB_GZWRITE_C.h"
#include "_3RD_ZLIB_INFBACK_C.h"
#include "_3RD_ZLIB_INFFAST_C.h"
#include "_3RD_ZLIB_INFFAST_H.h"
#include "_3RD_ZLIB_INFFIXED_H.h"
#include "_3RD_ZLIB_INFLATE_C.h"
#include "_3RD_ZLIB_INFLATE_H.h"
#include "_3RD_ZLIB_INFTREES_C.h"
#include "_3RD_ZLIB_INFTREES_H.h"
#include "_3RD_ZLIB_LICENSE.h"
#include "_3RD_ZLIB_TREES_C.h"
#include "_3RD_ZLIB_TREES_H.h"
#include "_3RD_ZLIB_UNCOMPR_C.h"
#include "_3RD_ZLIB_ZCONF_H.h"
#include "_3RD_ZLIB_ZLIB_H.h"
#include "_3RD_ZLIB_ZUTIL_C.h"
#include "_3RD_ZLIB_ZUTIL_H.h"
// ---------------------------- zlib

// ---------------------------- RC
#include "RC_CMAKELISTS_TXT.h"
// ---------------------------- RC

// ---------------------------- ICON
#include "ICON_ICON_RC_IN.h"
// ---------------------------- ICON

// ---------------------------- src
#include "SRC_PRIVATE_EXTERN_INSTANTIATION_CPP.h"
#include "SRC_PRIVATE_MAIN_CONTENT_H.h"
#include "SRC_PRIVATE_POPUP_CPP.h"
#include "SRC_PRIVATE_POPUP_H.h"
// ---------------------------- src

// ----------------------------
#include "LICENSE.h"
#include "CMAKELISTS_TXT.h"
#include "BINARY_GENERATOR_EXE.h"
// ----------------------------

void collectStaticFiles()
{
    writeFile(RC::LICENSE.data(), RC::LICENSE.size(), "LICENSE");
    writeFile(RC::CMAKELISTS_TXT.data(), RC::CMAKELISTS_TXT.size(), "CMakeLists.txt");
    writeFile(RC::BINARY_GENERATOR_EXE.data(), RC::BINARY_GENERATOR_EXE.size(), "binary-generator.exe");

    // ---------------------------- rc
    writeFile(RC::RC_CMAKELISTS_TXT.data(), RC::RC_CMAKELISTS_TXT.size(), "rc/CMakeLists.txt");
    // ---------------------------- rc

    // ---------------------------- icon
    writeFile(RC::ICON_ICON_RC_IN.data(), RC::ICON_ICON_RC_IN.size(), "icon/icon.rc.in");
    // ---------------------------- icon

    // ---------------------------- src
    writeFile(RC::SRC_PRIVATE_EXTERN_INSTANTIATION_CPP.data(), RC::SRC_PRIVATE_EXTERN_INSTANTIATION_CPP.size(), "src/private/extern_instantiation.cpp");
    writeFile(RC::SRC_PRIVATE_MAIN_CONTENT_H.data(), RC::SRC_PRIVATE_MAIN_CONTENT_H.size(), "src/private/main-content.h");
    writeFile(RC::SRC_PRIVATE_POPUP_CPP.data(), RC::SRC_PRIVATE_POPUP_CPP.size(), "src/private/popup.cpp");
    writeFile(RC::SRC_PRIVATE_POPUP_H.data(), RC::SRC_PRIVATE_POPUP_H.size(), "src/private/popup.h");
    // ---------------------------- src

    // ---------------------------- minizip
    writeFile(RC::_3RD_MINIZIP_CRYPT_H.data(), RC::_3RD_MINIZIP_CRYPT_H.size(), "_3rd/minizip/crypt.h");
    writeFile(RC::_3RD_MINIZIP_IOAPI_C.data(), RC::_3RD_MINIZIP_IOAPI_C.size(), "_3rd/minizip/ioapi.c");
    writeFile(RC::_3RD_MINIZIP_IOAPI_H.data(), RC::_3RD_MINIZIP_IOAPI_H.size(), "_3rd/minizip/ioapi.h");
    writeFile(RC::_3RD_MINIZIP_UNZIP_C.data(), RC::_3RD_MINIZIP_UNZIP_C.size(), "_3rd/minizip/unzip.c");
    writeFile(RC::_3RD_MINIZIP_UNZIP_H.data(), RC::_3RD_MINIZIP_UNZIP_H.size(), "_3rd/minizip/unzip.h");
    writeFile(RC::_3RD_MINIZIP_ZIP_C.data(), RC::_3RD_MINIZIP_ZIP_C.size(), "_3rd/minizip/zip.c");
    writeFile(RC::_3RD_MINIZIP_ZIP_H.data(), RC::_3RD_MINIZIP_ZIP_H.size(), "_3rd/minizip/zip.h");
    // ---------------------------- minizip

    // ---------------------------- zlib
    writeFile(RC::_3RD_ZLIB_ADLER32_C.data(), RC::_3RD_ZLIB_ADLER32_C.size(), "_3rd/zlib/adler32.c");
    writeFile(RC::_3RD_ZLIB_COMPRESS_C.data(), RC::_3RD_ZLIB_COMPRESS_C.size(), "_3rd/zlib/compress.c");
    writeFile(RC::_3RD_ZLIB_CRC32_C.data(), RC::_3RD_ZLIB_CRC32_C.size(), "_3rd/zlib/crc32.c");
    writeFile(RC::_3RD_ZLIB_CRC32_H.data(), RC::_3RD_ZLIB_CRC32_H.size(), "_3rd/zlib/crc32.h");
    writeFile(RC::_3RD_ZLIB_DEFLATE_C.data(), RC::_3RD_ZLIB_DEFLATE_C.size(), "_3rd/zlib/deflate.c");
    writeFile(RC::_3RD_ZLIB_DEFLATE_H.data(), RC::_3RD_ZLIB_DEFLATE_H.size(), "_3rd/zlib/deflate.h");
    writeFile(RC::_3RD_ZLIB_GZCLOSE_C.data(), RC::_3RD_ZLIB_GZCLOSE_C.size(), "_3rd/zlib/gzclose.c");
    writeFile(RC::_3RD_ZLIB_GZGUTS_H.data(), RC::_3RD_ZLIB_GZGUTS_H.size(), "_3rd/zlib/gzguts.h");
    writeFile(RC::_3RD_ZLIB_GZLIB_C.data(), RC::_3RD_ZLIB_GZLIB_C.size(), "_3rd/zlib/gzlib.c");
    writeFile(RC::_3RD_ZLIB_GZREAD_C.data(), RC::_3RD_ZLIB_GZREAD_C.size(), "_3rd/zlib/gzread.c");
    writeFile(RC::_3RD_ZLIB_GZWRITE_C.data(), RC::_3RD_ZLIB_GZWRITE_C.size(), "_3rd/zlib/gzwrite.c");
    writeFile(RC::_3RD_ZLIB_INFBACK_C.data(), RC::_3RD_ZLIB_INFBACK_C.size(), "_3rd/zlib/infback.c");
    writeFile(RC::_3RD_ZLIB_INFFAST_C.data(), RC::_3RD_ZLIB_INFFAST_C.size(), "_3rd/zlib/inffast.c");
    writeFile(RC::_3RD_ZLIB_INFFAST_H.data(), RC::_3RD_ZLIB_INFFAST_H.size(), "_3rd/zlib/inffast.h");
    writeFile(RC::_3RD_ZLIB_INFFIXED_H.data(), RC::_3RD_ZLIB_INFFIXED_H.size(), "_3rd/zlib/inffixed.h");
    writeFile(RC::_3RD_ZLIB_INFLATE_C.data(), RC::_3RD_ZLIB_INFLATE_C.size(), "_3rd/zlib/inflate.c");
    writeFile(RC::_3RD_ZLIB_INFLATE_H.data(), RC::_3RD_ZLIB_INFLATE_H.size(), "_3rd/zlib/inflate.h");
    writeFile(RC::_3RD_ZLIB_INFTREES_C.data(), RC::_3RD_ZLIB_INFTREES_C.size(), "_3rd/zlib/inftrees.c");
    writeFile(RC::_3RD_ZLIB_INFTREES_H.data(), RC::_3RD_ZLIB_INFTREES_H.size(), "_3rd/zlib/inftrees.h");
    writeFile(RC::_3RD_ZLIB_TREES_C.data(), RC::_3RD_ZLIB_TREES_C.size(), "_3rd/zlib/trees.c");
    writeFile(RC::_3RD_ZLIB_TREES_H.data(), RC::_3RD_ZLIB_TREES_H.size(), "_3rd/zlib/trees.h");
    writeFile(RC::_3RD_ZLIB_UNCOMPR_C.data(), RC::_3RD_ZLIB_UNCOMPR_C.size(), "_3rd/zlib/uncompr.c");
    writeFile(RC::_3RD_ZLIB_ZCONF_H.data(), RC::_3RD_ZLIB_ZCONF_H.size(), "_3rd/zlib/zconf.h");
    writeFile(RC::_3RD_ZLIB_ZLIB_H.data(), RC::_3RD_ZLIB_ZLIB_H.size(), "_3rd/zlib/zlib.h");
    writeFile(RC::_3RD_ZLIB_ZUTIL_C.data(), RC::_3RD_ZLIB_ZUTIL_C.size(), "_3rd/zlib/zutil.c");
    writeFile(RC::_3RD_ZLIB_ZUTIL_H.data(), RC::_3RD_ZLIB_ZUTIL_H.size(), "_3rd/zlib/zutil.h");
    writeFile(RC::_3RD_ZLIB_LICENSE.data(), RC::_3RD_ZLIB_LICENSE.size(), "_3rd/zlib/LICENSE");
    // ---------------------------- zlib
}