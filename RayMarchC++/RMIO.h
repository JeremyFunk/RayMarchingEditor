#include <string>

//Guides: https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/shell/common-file-dialog.md#sample-usage

namespace RMIO {
    struct SaveReturn {
        std::string file, path;
        bool selected;
    };

    std::string GetAppdataPath();
    bool SetupDirectories();
    bool SaveAppData(std::string path, std::string content);
    std::string Load(std::string path);
    bool Save(std::string path, std::string content);
    SaveReturn ExplorerOpenFile();
    SaveReturn ExplorerSaveFile();
}

