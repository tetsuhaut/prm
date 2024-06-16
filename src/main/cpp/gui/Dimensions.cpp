module;

export module gui.Dimensions;

export namespace dimensions {
constexpr int BUTTON_HEIGHT = 25;
constexpr int BUTTON_WIDTH = 120;
constexpr int MENUBAR_X = 0;
constexpr int MENUBAR_Y = 0;
constexpr int MENUBAR_HEIGHT = BUTTON_HEIGHT;
constexpr int TXT_HEIGHT = 25;
constexpr int HISTORY_DIRECTORY_TXT_X = 5;
constexpr int HISTORY_DIRECTORY_TXT_Y = BUTTON_HEIGHT;
constexpr int HISTORY_DIRECTORY_TXT_HEIGHT = BUTTON_HEIGHT;
constexpr int HISTORY_FILES_TREE_X = 5;
constexpr int HISTORY_FILES_TREE_Y = 5;
constexpr int GAME_LIST_X = 5;
constexpr int GAME_LIST_Y = 2* BUTTON_HEIGHT;
constexpr int GAME_LIST_HEIGHT = BUTTON_HEIGHT;
constexpr int EMPTY_GAME_LIST_X = 5;
constexpr int EMPTY_GAME_LIST_Y = 2 * BUTTON_HEIGHT;
constexpr int EMPTY_GAME_LIST_HEIGHT = 2*BUTTON_HEIGHT;
} // namespace dimensions

export struct [[nodiscard]] Dimension final {
  int x;
  int y;
  int w;
  int h;
};