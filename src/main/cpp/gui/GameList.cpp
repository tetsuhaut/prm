module;

#if defined(_MSC_VER) // removal of specific msvc warnings due to FLTK
#  pragma warning(push)
#  pragma warning(disable : 4100 4191 4242 4244 4266 4365 4458 4514 4625 4626 4668 4820 5026 5027 5219 )
#elif defined(__MINGW32__) // removal of specific gcc warnings due to FLTK
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-function-type"
#  pragma GCC diagnostic ignored "-Wsuggest-override"
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Weffc++"
#endif  // _MSC_VER

#include <FL/Fl_Tree.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <cassert> // assert

export module gui.GameList;

import history.WinamaxHistory;
import std;

export class [[nodiscard]] GameList final {
private:
  Fl_Tree m_games;
  std::function<void(const Fl_Tree_Item&)> m_elementSelectionCallback;
  friend static void gameListCb(Fl_Widget* w, void* self);
public:
  GameList(int x, int y, int width, int height);
  ~GameList();
  GameList(const GameList&) = delete;
  GameList(GameList&&) = delete;
  GameList& operator=(const GameList&) = delete;
  GameList& operator=(GameList&&) = delete;
  [[nodiscard]] std::optional<std::filesystem::path> getSelectedGameHistoryFile() /*const*/;
  [[nodiscard]] std::optional<std::string> getSelectedGameHistoryDir() /*const*/;
  void listenToElementSelection(const std::function<void(const Fl_Tree_Item&)>& callback);
  [[nodiscard]] std::vector<std::string> getGameHistoryDirs() /*const*/;
  void setGames(const std::unordered_map<std::string, std::vector<std::string>>& games);
  void addDir(std::string_view dir);
  void removeDir(std::string_view dir);
  bool containsGameHistoryDir(std::string_view dir) const;
}; // class GameList

module : private;

static constexpr std::string_view CHOSE_HAND_HISTORY_DIRECTORY_MSG { "<chose a hand history directory>" };
static constexpr std::string_view GAMES_LIST_LABEL { "Hand History Directories" };

[[nodiscard]] static GameList* THIS(void* self) {
  return static_cast<GameList*>(self);
}

static void gameListCb(Fl_Widget* w, void* self) {
  auto* tree = static_cast<Fl_Tree*>(w);

  if (auto* item = tree->callback_item(); item && THIS(self)->m_elementSelectionCallback) {
    THIS(self)->m_elementSelectionCallback(*item);
  }
}

static void initTree(Fl_Tree& tree) {
  tree.root_label(GAMES_LIST_LABEL.data());
  tree.add(CHOSE_HAND_HISTORY_DIRECTORY_MSG.data());
}

GameList::GameList(int x, int y, int width, int height) 
: m_games(x, y, width, height),
  m_elementSelectionCallback() {
  initTree(m_games);
  m_games.callback(gameListCb, this);
  m_games.deactivate();
}

GameList::~GameList() {}

[[nodiscard]] std::optional<std::filesystem::path> GameList::getSelectedGameHistoryFile() {
    if (const auto item { m_games.first_selected_item() }; item and std::string_view(item->label()).ends_with(".txt")) {
      char pathname[256];
      auto ret { m_games.item_pathname(pathname, sizeof(pathname), item) };
      assert(0 == ret);
      return std::string(pathname).substr(GAMES_LIST_LABEL.size() + 1);
    }
  return {};
}

[[nodiscard]] std::optional<std::string> GameList::getSelectedGameHistoryDir() {
  if (const auto item { m_games.first_selected_item() }; item and !std::string_view(item->label()).ends_with(".txt")) {
    char pathname[256];
    auto ret { m_games.item_pathname(pathname, sizeof(pathname), item) };
    assert(0 == ret);
    return std::string(pathname);
  }
  return {};
}

void GameList::listenToElementSelection(const std::function<void(const Fl_Tree_Item&)>& callback) {
  m_elementSelectionCallback = callback;
}

[[nodiscard]] std::vector<std::string> GameList::getGameHistoryDirs() /*const*/ {
  std::vector<std::string> ret;
  std::string currentDir;
  for (auto item = m_games.first(); item; item = m_games.next(item)) {
    if ((GAMES_LIST_LABEL != item->label()) and CHOSE_HAND_HISTORY_DIRECTORY_MSG != item->label()) {
      if (0 != item->children()) {
        ret.emplace_back(item->label());
      }
    }
  }
  return ret;
}

[[nodiscard]] static std::string toTreeRoot(std::string_view path) {
  std::string ret;
  std::ranges::for_each(path, [&](const auto c) {
    ret += c;
    // double the anti-slashes
    if ('\\' == c) { ret += c; }
  });
  return ret;
}

static void closeDirectories(Fl_Tree& tree) {
  for (auto item { tree.first() }; item; item = tree.next(item)) {
    if (!item->is_root() and 0 != item->children()) {
      item->close();
    }
  }
}

void GameList::setGames(const std::unordered_map<std::string, std::vector<std::string>>& games) {
  if (CHOSE_HAND_HISTORY_DIRECTORY_MSG == m_games.last()->label()) {
    m_games.remove(m_games.last());
    m_games.activate();
  }
  for (const auto& [dir, files] : games) {
    const auto dirNode { dir + "/" };
    std::ranges::for_each(files, [&](const auto& file) {
      m_games.add((toTreeRoot(dirNode + std::filesystem::path(file).filename().string()).c_str()));
    });
  }
  closeDirectories(m_games);
}

/**
 * @param dir must contain a "history" subdir
 */
void GameList::addDir(std::string_view dir) {
  if (CHOSE_HAND_HISTORY_DIRECTORY_MSG == m_games.last()->label()) {
    m_games.remove(m_games.last());
    m_games.activate();
  }
  const std::filesystem::path p { dir };
  if (const auto historyFiles { WinamaxHistory::getFiles(p) }; !historyFiles.empty()) {
    const auto dirNode { (p / "history").lexically_normal() };
    std::ranges::for_each(historyFiles, [this, &dirNode](const auto& file) {
      m_games.add((toTreeRoot(dirNode.string() + "/" + file.filename().string()).c_str()));
    });
    closeDirectories(m_games);
  }
  else {
    m_games.add(toTreeRoot(p.string() + "/").c_str());
  }
}

void GameList::removeDir(std::string_view dir) {
  if (GAMES_LIST_LABEL == dir) { return; }
  const auto dirNode { std::string(dir) + "/" };
  if (auto item { m_games.find_item(dirNode.c_str()) }; item) { m_games.remove(item); }
  if (GAMES_LIST_LABEL == m_games.last()->label()) {
    m_games.add(CHOSE_HAND_HISTORY_DIRECTORY_MSG.data());
    m_games.deactivate();
  }
  m_games.redraw();
}

bool GameList::containsGameHistoryDir(std::string_view dir) const {
  const auto dirNode { std::string(dir) + "/" };
  return nullptr != m_games.find_item(dirNode.c_str());
}