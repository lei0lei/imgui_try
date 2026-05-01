#include "folder_dialog.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace UI {

std::string PickFolderPathFromDialog()
{
#ifdef _WIN32
    BROWSEINFOA bi{};
    bi.lpszTitle = "Select Folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (!pidl) {
        return {};
    }

    char selected_path[MAX_PATH] = {};
    const bool ok = SHGetPathFromIDListA(pidl, selected_path) == TRUE;
    CoTaskMemFree(pidl);
    return ok ? std::string(selected_path) : std::string();
#else
    (void)0;
    return {};
#endif
}

} // namespace UI
