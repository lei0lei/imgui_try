#pragma once

// Command identifiers for the command layer
enum class CommandId {
    None = 0,
    FileNew,
    FileNew2D,
    FileNew3D,
    FileNewNodeGraph,
    FileOpen,
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
    TogglePrimarySidebar,
    TogglePanel,
    ToggleSecondarySidebar
};
