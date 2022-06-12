#include "RMIO.h"
#include <Windows.h>
#include <ShObjIdl_core.h> 
#include <ShlObj.h>
#include <wchar.h>
#include <iostream>
#include <fstream>
#include <direct.h>
//Guides: https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/shell/common-file-dialog.md#sample-usage

namespace RMIO {
    std::string subDirs[] = { "data" };
    std::string appName = "RayMarching";


    std::string GetAppdataPath() {
        PWSTR appdataPath = NULL;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdataPath);
        if (SUCCEEDED(hr)) {
            std::wstring ws(appdataPath);
            std::string str(ws.begin(), ws.end());
            CoTaskMemFree(appdataPath);
            return str;
        }
    }

    bool SetupDirectories() {
        try {
            auto fullPath = GetAppdataPath() + "\\" + appName;
            std::cout << fullPath;
            _mkdir(fullPath.c_str());
            for (const std::string& dir : subDirs)
            {
                _mkdir((fullPath + "\\" + dir).c_str());
            }

            return true;
        }
        catch (const std::exception& e) {
            std::cout << e.what();
            return false;
        }
    }

    bool SaveAppData(std::string path, std::string content) {
        try {
            auto fullPath = GetAppdataPath() + "\\" + appName + "\\" + path;

            std::ofstream file;
            file.open(fullPath);
            file << content;
            file.close();

            return true;
        }
        catch (const std::exception& e) {
            std::cout << e.what();
            return false;
        }
    }

    std::string Load(std::string path) {
        try {
            std::ifstream inFile;
            inFile.open(path);
            if (!inFile.is_open()) {
                std::cout << "failed to open " << path << '\n';
            }
            else {
                std::string tp;
                while (std::getline(inFile, tp)) {}

                inFile.close();

                return tp;
            }
            return NULL;
        }
        catch (const std::exception& e) {
            std::cout << e.what();
            return NULL;
        }
    }

    bool Save(std::string path, std::string content) {
        try {

            std::ofstream file;
            file.open(path);
            file << content;
            file.close();

            return true;
        }
        catch (const std::exception& e) {
            std::cout << e.what();
            return false;
        }
    }
}

namespace RMIO {
    COMDLG_FILTERSPEC ComDlgFS[3] = { {L"RayMarching Project Files (*.rma)", L"*.rma"},{L"All Files",L"*.*"} };

    SaveReturn ExplorerOpenFile()
    {
        SaveReturn s;


        s.selected = false;
        //  CREATE FILE OBJECT INSTANCE
        HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(f_SysHr))
            return s;

        // CREATE FileOpenDialog OBJECT
        IFileOpenDialog* f_FileSystem;
        f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
        f_FileSystem->SetOptions(FOS_FILEMUSTEXIST | FOS_OKBUTTONNEEDSINTERACTION);
        f_FileSystem->SetFileTypes(2, ComDlgFS);
        if (FAILED(f_SysHr)) {
            CoUninitialize();
            return s;
        }

        //  SHOW OPEN FILE DIALOG WINDOW
        f_SysHr = f_FileSystem->Show(NULL);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
        IShellItem* f_Files;
        f_SysHr = f_FileSystem->GetResult(&f_Files);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  STORE AND CONVERT THE FILE NAME
        PWSTR f_Path;
        f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
        if (FAILED(f_SysHr)) {
            f_Files->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  FORMAT AND STORE THE FILE PATH
        std::wstring path(f_Path);
        std::string c(path.begin(), path.end());
        s.path = c;

        //  FORMAT STRING FOR EXECUTABLE NAME
        const size_t slash = s.path.find_last_of("/\\");
        s.file = s.path.substr(slash + 1);

        //  SUCCESS, CLEAN UP
        CoTaskMemFree(f_Path);
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();
        s.selected = true;
        return s;
    }
    SaveReturn ExplorerSaveFile()
    {
        SaveReturn s;

        s.selected = false;
        //  CREATE FILE OBJECT INSTANCE
        HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(f_SysHr))
            return s;

        // CREATE FileOpenDialog OBJECT
        IFileSaveDialog* f_FileSystem;

        f_SysHr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&f_FileSystem));
        f_FileSystem->SetDefaultExtension(L"rma");
        f_FileSystem->SetFileTypes(2, ComDlgFS);
        if (FAILED(f_SysHr)) {
            CoUninitialize();
            return s;
        }

        //  SHOW OPEN FILE DIALOG WINDOW
        f_SysHr = f_FileSystem->Show(NULL);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
        IShellItem* f_Files;
        f_SysHr = f_FileSystem->GetResult(&f_Files);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  STORE AND CONVERT THE FILE NAME
        PWSTR f_Path;
        f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
        if (FAILED(f_SysHr)) {
            f_Files->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return s;
        }

        //  FORMAT AND STORE THE FILE PATH
        std::wstring path(f_Path);
        std::string c(path.begin(), path.end());
        s.path = c;

        //  FORMAT STRING FOR EXECUTABLE NAME
        const size_t slash = s.path.find_last_of("/\\");
        s.file = s.path.substr(slash + 1);

        //  SUCCESS, CLEAN UP
        CoTaskMemFree(f_Path);
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();
        s.selected = true;
        return s;
    }
}

