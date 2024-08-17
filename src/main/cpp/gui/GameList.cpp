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

#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Tree.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <cassert> // assert

export module gui.GameList;

import history.WinamaxHistory;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop ) 

export class [[nodiscard]] GameList : public Fl_Tree {
private:
  std::function<void(const Fl_Tree_Item&)> m_elementSelectionCallback;
  std::string_view getItemPathName(const Fl_Tree_Item* pItem) const;
  friend static void gameListCb(Fl_Widget* w, void* self);
public:
  GameList(int x, int y, int width, int height);
  ~GameList();
  GameList(const GameList&) = delete;
  GameList(GameList&&) = delete;
  GameList& operator=(const GameList&) = delete;
  GameList& operator=(GameList&&) = delete;
  int handle(int event); /*[[override]]*/
  [[nodiscard]] std::optional<std::filesystem::path> getSelectedGameHistoryFile() /*const*/;
  [[nodiscard]] std::optional<std::string> getSelectedGameHistoryDir() /*const*/;
  void listenToElementSelection(const std::function<void(const Fl_Tree_Item&)>& callback);
  [[nodiscard]] std::vector<std::string> getGameHistoryDirs() /*const*/;
  void addDir(std::string_view dir);
  void removeDir(std::string_view dir);
  bool containsGameHistoryDir(std::string_view dir) const;
}; // class GameList

module : private;

static constexpr std::string_view CHOSE_HAND_HISTORY_DIRECTORY_MSG { "<chose a hand history directory>" };
static constexpr std::string_view GAMES_LIST_LABEL { "Hand History Directories" };

static void gameListCb(Fl_Widget* w, void* self) {
  auto* pTree = static_cast<Fl_Tree*>(w);
  auto* pThis { static_cast<GameList*>(self) };

  if (auto* item = pTree->callback_item(); item && pThis->m_elementSelectionCallback) {
    pThis->m_elementSelectionCallback(*item);
  }
}

static void initTree(Fl_Tree& tree) {
  tree.root_label(GAMES_LIST_LABEL.data());
  tree.add(CHOSE_HAND_HISTORY_DIRECTORY_MSG.data());
}

GameList::GameList(int x, int y, int width, int height)
: Fl_Tree(x, y, width, height),
  m_elementSelectionCallback() {
  initTree(*this);
  this->callback(gameListCb, this);
  this->deactivate();
}

GameList::~GameList() {}

int GameList::handle(int event) {
  if ((FL_PUSH == event) and (FL_RIGHT_MOUSE == Fl::event_button())) {
    const auto width { 100 };
    auto* popup { new Fl_Menu_Button(Fl::event_x(), Fl::event_y() - width, width, 100, "Menu")};
    popup->add("This|is|a popup|menu");
    popup->popup();
    return 1;
  }
  return Fl_Tree::handle(event);
}

std::string_view GameList::getItemPathName(const Fl_Tree_Item* pItem) const {
  char pathname[512] { '\0' };
  auto ret { this->item_pathname(pathname, sizeof(pathname), pItem) };
  assert(0 == ret);
  return pathname;
}

[[nodiscard]] std::optional<std::filesystem::path> GameList::getSelectedGameHistoryFile() {
    if (const auto* pItem { this->first_selected_item() }; pItem and std::string_view(pItem->label()).ends_with(".txt")) {
      const auto pathname { getItemPathName(pItem) };
      return pathname.substr(GAMES_LIST_LABEL.size() + 1);
    }
  return {};
}

[[nodiscard]] std::optional<std::string> GameList::getSelectedGameHistoryDir() {
  if (const auto* pItem { this->first_selected_item() }; pItem and !std::string_view(pItem->label()).ends_with(".txt")) {
    return std::string(getItemPathName(pItem));
  }
  return {};
}

void GameList::listenToElementSelection(const std::function<void(const Fl_Tree_Item&)>& callback) {
  m_elementSelectionCallback = callback;
}

[[nodiscard]] std::vector<std::string> GameList::getGameHistoryDirs() /*const*/ {
  std::vector<std::string> ret;
  std::string currentDir;
  for (auto item = this->first(); item; item = this->next(item)) {
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

/**
 * @param dir must contain a "history" subdir
 */
void GameList::addDir(std::string_view dir) {
  if (CHOSE_HAND_HISTORY_DIRECTORY_MSG == this->last()->label()) {
    this->remove(this->last());
    this->activate();
  }
  const std::filesystem::path p { dir.ends_with("history") ? dir.substr(0, dir.length() - std::size("history")) : dir };
  if (const auto historyFiles { WinamaxHistory::getFiles(p) }; !historyFiles.empty()) {
    const auto dirNode { (p / "history").lexically_normal() };
    std::ranges::for_each(historyFiles, [this, &dirNode](const auto& file) {
      this->add((toTreeRoot(dirNode.string() + "/" + file.filename().string()).c_str()));
    });
    closeDirectories(*this);
  }
  else {
    this->add(toTreeRoot(p.string() + "/").c_str());
  }
}

void GameList::removeDir(std::string_view dir) {
  if (GAMES_LIST_LABEL == dir) { return; }
  const auto dirNode { std::string(dir) + "/" };
  if (auto item { this->find_item(dirNode.c_str()) }; item) { this->remove(item); }
  if (GAMES_LIST_LABEL == this->last()->label()) {
    this->add(CHOSE_HAND_HISTORY_DIRECTORY_MSG.data());
    this->deactivate();
  }
  this->redraw();
}

bool GameList::containsGameHistoryDir(std::string_view dir) const {
  const auto dirNode { std::format("{}/", dir) };
  return nullptr != this->find_item(dirNode.c_str());
}