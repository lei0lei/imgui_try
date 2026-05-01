#pragma once

#include <string>

namespace UI {

// Native folder picker (Windows: SHBrowseForFolder). Returns empty string if canceled.
std::string PickFolderPathFromDialog();

} // namespace UI
