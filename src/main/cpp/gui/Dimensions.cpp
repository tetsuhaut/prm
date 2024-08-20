module;

export module gui.Dimensions;

// +----> x
// | menu
// | hand history directory
// | open the review button
// |
// v
// y

export namespace dimensions {
constexpr int MAIN_WINDOW_WIDTH = 800;
constexpr int MAIN_WINDOW_HEIGHT = 600;
constexpr int SPACE = 5;
constexpr int BUTTON_HEIGHT = 25;
constexpr int BUTTON_WIDTH = 120;
constexpr int MENUBAR_X = 0;
constexpr int MENUBAR_Y = 0;
constexpr int MENUBAR_HEIGHT = BUTTON_HEIGHT;
constexpr int HISTORY_DIRECTORY_TXT_HEIGHT = BUTTON_HEIGHT;
constexpr int HISTORY_FILES_TREE_X = SPACE;
constexpr int HISTORY_FILES_TREE_Y = SPACE;
constexpr int GAME_LIST_X = SPACE;
constexpr int GAME_LIST_Y = MENUBAR_HEIGHT + SPACE;
constexpr int GAME_LIST_HEIGHT = MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT - BUTTON_HEIGHT - 3*SPACE;
} // namespace dimensions

export struct [[nodiscard]] Dimension final {
  int x;
  int y;
  int w;
  int h;
};