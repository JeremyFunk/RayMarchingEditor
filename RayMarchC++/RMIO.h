#pragma once
#include <string>
#include <vector>
//Guides: https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/shell/common-file-dialog.md#sample-usage

namespace RMIO {
    struct SaveReturn {
        std::string file, path;
        bool selected;
    };


    std::vector<std::string> GetFilesInDir(std::string path);
    std::string GetAppdataPath();
    bool SetupDirectories();
    std::string PathGetFilePart(std::string path);
    std::string PathGetDirectoryPart(std::string path);
    std::string PathGetFilenamePart(std::string path);
    bool SetupProjectDirectories(std::string path);
    bool SaveAppData(std::string path, std::string content);
    std::string Load(std::string path);
    bool Save(std::string path, std::string content);
    SaveReturn ExplorerOpenFile();
    SaveReturn ExplorerSaveFile();
}

