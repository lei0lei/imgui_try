#pragma once

// Command identifiers for the command layer
enum class CommandId {
    None = 0,
    FileNew,
    FileOpen,
    FileOpenFolder,
    FileOpenProject,
    FileSave,
    FileExit,
    EditUndo,
    EditRedo,
    ViewExplorer,
    ViewConsole,
    HelpAbout,
    WindowMinimize,
    WindowMaximize,
    WindowClose,
    TogglePrimarySidebar
};
