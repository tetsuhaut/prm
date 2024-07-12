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

#include <FL/Fl.H> // Fl::*
#include <FL/FL_ask.H> // fl_alert
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tree.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <algorithm> // std::ranges
#include <array> // std::array
#include <cctype> // std::isdigit
#include <cassert>
#include <cerrno>
#include <cstring> // std::strerror_s
#include <filesystem> // std::filesystem::path
#include <memory> // std::make_unique
#include <print> // std::format, std::print
#include <span>
#include <string> // std::string
#include <utility> // std::pair
#include <vector>

export module gui.MainWindow;

import entities.Card;
import entities.Game; // CashGame, Tournament
import entities.Site;
import gui.Dimensions; // button size
import gui.Preferences;
import gui.ReviewerWindow;
import history.WinamaxGameHistory;
import history.WinamaxHistory;

export class [[nodiscard]] MainWindow final {
private:
  Preferences m_preferences = Preferences();
  std::unique_ptr<Fl_Double_Window> m_mainWindow = nullptr;
  std::unique_ptr<ReviewerWindow> m_reviewerWindow = nullptr;
  std::unique_ptr<Fl_Tree> m_games = nullptr;
  bool handHistoryDirIsUnknown(std::string_view dir);

public:
  void exit();
  void toggleGameWindow();
  [[nodiscard]] int run();
  void chooseHandHistoryDirectory();
  void newGameWindow();
  void addHistoryDirectoryToList(std::string_view dir, const std::vector<std::filesystem::path>& historyFiles);
}; // export class MainWindow

module : private;

namespace fs = std::filesystem;
static MainWindow* pThis { nullptr };
static Fl_Button* pReviewerButton { nullptr };
static constexpr std::string_view CHOSE_HAND_HISTORY_DIRECTORY_MSG { "<chose a hand history directory>" };
static constexpr std::string_view GAMES_LIST_LABEL { "Hand History Directories" };
static constexpr std::string_view OPEN_THE_REVIEW_LABEL { "Open the review" };
static constexpr std::string_view CLOSE_THE_REVIEW_LABEL { "Close the review" };

[[nodiscard]] static constexpr int toInt(std::size_t value) {
  constexpr int kIntMax { std::numeric_limits<int>::max() };
  return (value > static_cast<std::size_t>(kIntMax)) ? kIntMax : static_cast<int>(value);
}

[[nodiscard]] static constexpr int toInt(auto) = delete; // forbid other types

static void mainWindowCb(Fl_Widget*) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  pThis->exit();
}

[[nodiscard]] static fs::path getFilenameFromItem(const Fl_Tree& tree, const Fl_Tree_Item& item) {
  char pathname[256];
  tree.item_pathname(pathname, sizeof(pathname), &item);
  return std::string(pathname).substr(GAMES_LIST_LABEL.size() + 1);
}

[[nodiscard]] static bool isItemHistoryFile(const Fl_Tree& tree, const Fl_Tree_Item& item) {
  char pathname[256];
  tree.item_pathname(pathname, sizeof(pathname), &item);
  return std::string(pathname).starts_with(GAMES_LIST_LABEL) and std::string(pathname).ends_with(".txt");
}

static void gameListCb(Fl_Widget* w) {
  auto* tree = static_cast<Fl_Tree*>(w);
  auto* item = tree->callback_item();
  if (item and (0 == item->children()) and isItemHistoryFile(*tree, *item)) {
    pReviewerButton->activate();
  } else {
    pReviewerButton->deactivate();
  }
}

[[nodiscard]] static Fl_Tree_Item* getSelectedItem(Fl_Tree& tree) {
  for (auto* item = tree.first(); item; item = tree.next(item)) {
    if (item->is_selected()) {
      return item;
    }
  }
  return nullptr;
}

[[nodiscard]] static std::unique_ptr<Fl_Native_File_Chooser> buildHandHistoryDirectoryChooser() {
  auto pHistoryChoser { std::make_unique<Fl_Native_File_Chooser>() };
  pHistoryChoser->title("Choisissez un répertoire d'historiques de mains");
  pHistoryChoser->type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
  return pHistoryChoser;
}

/**
  * Called by the event loop when the user chosed a valid history file.
  * Starts the import process.
  */
void MainWindow::newGameWindow() {
  const auto selectedItem { getSelectedItem(*m_games) };
  const auto historyFile { getFilenameFromItem(*m_games, *selectedItem) };
  const auto site { WinamaxGameHistory::parseGameHistory(historyFile) };
  const auto cashGames { site->viewCashGames() };

  if (cashGames.empty()) {
    fl_alert("Pas de cashgame détecté dans cet historique");
    pReviewerButton->label(OPEN_THE_REVIEW_LABEL.data());
    return;
  }

  assert(1 == cashGames.size());
  const auto [localX, localY, width, height] { m_preferences.getGameWindowXYWH() };

  const auto deleteReviewerWindow = [this]() {
    // destroy the existing game window
    m_reviewerWindow.reset();
    // tell the user the review button will open a new game window
    pReviewerButton->label(OPEN_THE_REVIEW_LABEL.data());
    };
  m_reviewerWindow = std::make_unique<ReviewerWindow>(m_preferences,
    cashGames[0]->getId(),
    deleteReviewerWindow,
    site->whoIsHero(),
    *cashGames[0]->viewHands()[0]);
}

/**
 * The call to int Fl_Native_File_Chooser::show () returns:
 * 0  – user picked a file
 * 1  – user cancelled
 * -1 – failed; errmsg() has reason
 *
 * @see https://www.fltk.org/doc-1.4/classFl__Native__File__Chooser.html#ac3c1724eea8f9f74a257cc198d69deb4
*/
enum class [[nodiscard]] FileChoiceStatus : short { ok = 0, error = -1, cancel = 1 };

[[nodiscard]] static constexpr bool startsLikeAHistoryFile(std::string_view s) {
  return (s.length() > 11)
    and std::isdigit(s[0])
    and std::isdigit(s[1])
    and std::isdigit(s[2])
    and std::isdigit(s[3])
    and std::isdigit(s[4])
    and std::isdigit(s[5])
    and std::isdigit(s[6])
    and std::isdigit(s[7])
    and '_' == s[8];
}

/**
 * @returns true if the given path
 * - is a directory
 * - that contains only files
 * - that contains files beginning by 8 digits and _
 * - that contains files ending with .txt
*/

[[nodiscard]] static std::vector<fs::path> getHistoryFiles(const fs::path& histoDir) {
  std::vector<fs::path> ret;  

  if (!fs::is_directory(histoDir)) {
    return {};
  }
 
  for (const auto& dirEntry : fs::directory_iterator(histoDir)) {
    const auto& entryPath { dirEntry.path() };
    if (dirEntry.is_regular_file()
      and startsLikeAHistoryFile(entryPath.filename().string())
      and entryPath.filename().string().starts_with("20") // like the year 2022
      and entryPath.string().ends_with(".txt")
      and !entryPath.string().ends_with("_summary.txt")) {
      ret.push_back(entryPath);
    }
  }
  return ret;
}

struct Data {
  std::vector<fs::path> historyFiles;
  std::string dir;
};

template<typename T>
[[nodiscard]] static std::unique_ptr<T> toUniquePtr(void* value) {
  return std::unique_ptr<T>(static_cast<T*>(value));
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

void MainWindow::addHistoryDirectoryToList(std::string_view dir, const std::vector<fs::path>& historyFiles) {
  if (CHOSE_HAND_HISTORY_DIRECTORY_MSG == m_games->last()->label()) {
    m_games->remove(m_games->last());
    m_games->activate();
  }
  const auto dirNode { std::format("{}/", dir) };
  std::ranges::for_each(historyFiles, [&](const auto& file) {
    m_games->add((toTreeRoot(dirNode + file.filename().string()).c_str()));
  });
  // tentative de fermeture du dirNode
  for (auto node = m_games->first(); node; node = m_games->next(node)) {
    if (!node->is_root() && 0 != node->children()) {
      node->close();
    }
  }
  m_games->resize(m_games->x(), m_games->y(), m_games->w(), dimensions::GAME_LIST_HEIGHT * toInt(historyFiles.size()));
  m_games->redraw();
}

bool MainWindow::handHistoryDirIsUnknown(std::string_view dir) {
  for (auto item { m_games->first() }; nullptr != item; item = m_games->next(item)) {
    if (dir == item->label()) { return false; }
  }
  return true;
}

void MainWindow::chooseHandHistoryDirectory() {
  auto dirChoser { buildHandHistoryDirectoryChooser() };

  switch (FileChoiceStatus(dirChoser->show())) {
  case FileChoiceStatus::ok: {
    if (handHistoryDirIsUnknown(dirChoser->filename())) {
      if (auto historyFiles = getHistoryFiles(dirChoser->filename()); !historyFiles.empty()) {
        auto data { std::make_unique<Data>() };
        data->historyFiles = historyFiles;
        data->dir = dirChoser->filename();
        const auto addHistoryDirectoryToListAwakeCb = [](void* hiddenData) {
          auto data { toUniquePtr<Data>(hiddenData) };
          pThis->addHistoryDirectoryToList(data->dir, data->historyFiles);
          };
        Fl::awake(addHistoryDirectoryToListAwakeCb, data.release());
      }
    }
  } break;

  case FileChoiceStatus::error: [[unlikely]] {
    fl_alert(dirChoser->errmsg());
    } break;

  case FileChoiceStatus::cancel: [[fallthrough]];

  default: /* nothing to do */ break;
  }
}

/**
  * Called when the user clicks on the 'exit' menu or the X.
  */
void MainWindow::exit() {
  /* remember the windows position when closing them */
  if (m_mainWindow) {
    std::array xywh {m_mainWindow->x(), m_mainWindow->y(), m_mainWindow->w(), m_mainWindow->h()};
    m_preferences.saveSizeAndPosition(xywh, Preferences::PrefName::mainWindow);
    m_preferences.saveHistoryDirs(*m_games);
  }

  /* hide all windows: this will terminate the MainWindow */
  while (Fl::first_window()) { Fl::first_window()->hide(); }
}

void MainWindow::toggleGameWindow() {
  if (nullptr != m_reviewerWindow) {
    // destroy the existing game window
    m_reviewerWindow.reset();
    // tell the user the review button will open a new game window
    pReviewerButton->label(OPEN_THE_REVIEW_LABEL.data());
  } else {
    // tell the user the review button will close the game window
    pReviewerButton->label(CLOSE_THE_REVIEW_LABEL.data());
    // open a new game window and populate it
    // we need to use the same flavor of Fl::awake() in the whole program
    // see https://www.fltk.org/doc-1.4/advanced.html#advanced_multithreading
    Fl::awake([](void*) { pThis->newGameWindow(); }, nullptr); // TODO
  }
}

[[nodiscard]] static Fl_Button* buildReviewButton(int bottom) {
  auto pReviewButton = new Fl_Button(dimensions::BUTTON_X, bottom - dimensions::BUTTON_HEIGHT - 5, dimensions::BUTTON_WIDTH,
    dimensions::BUTTON_HEIGHT, OPEN_THE_REVIEW_LABEL.data());
  pReviewButton->callback([](Fl_Widget*) { pThis->toggleGameWindow(); });
  pReviewButton->deactivate();
  return pReviewButton;
}

void populateGames(Fl_Tree& /*tree*/, std::span<const std::string> /*histoDirList*/) {
  // TODO
  //data->historyFiles = historyFiles;
  //data->dir = dirChoser->filename();
  //const auto addHistoryDirectoryToListAwakeCb = [](void* hiddenData) {
  //  auto data { toUniquePtr<Data>(hiddenData) };
  //  pThis->addHistoryDirectoryToList(data->dir, data->historyFiles);
  //  };
  //Fl::awake(addHistoryDirectoryToListAwakeCb, data.release());
}
[[nodiscard]] static std::unique_ptr<Fl_Tree> buildGameList(int x, int y, int width, int height, std::span<const std::string> histoDirList) {
  auto tree { std::make_unique<Fl_Tree>(x, y, width, height)};
  tree->root_label(GAMES_LIST_LABEL.data());
  tree->add(CHOSE_HAND_HISTORY_DIRECTORY_MSG.data());
  tree->callback(gameListCb);
  tree->deactivate();
  populateGames(*tree, histoDirList);
  return tree;
}

[[nodiscard]] static std::unique_ptr<Fl_Menu_Bar> buildMenuBar(int x, int y, int width, int height) {
  auto pMenuBar = std::make_unique<Fl_Menu_Bar>(x, y, width, height);
  pMenuBar->add("&File/Add hand history directory", 0, [](Fl_Widget*, void*) { pThis->chooseHandHistoryDirectory(); });
  pMenuBar->add("&File/E&xit", 0, [](Fl_Widget*, void*) { pThis->exit(); });
  return pMenuBar;
}

// +----> x
// | menu
// | hand history directory
// | open the review button
// |
// v
// y
[[nodiscard]] static std::unique_ptr<Fl_Double_Window> buildMainWindow(int localX, int localY, int width, int height) {
  auto pMainWindow = std::make_unique<Fl_Double_Window>(localX, localY, width, height,
    "Poker Reviewer Modulaire");
  pMainWindow->callback(mainWindowCb);
  return pMainWindow;
}

[[nodiscard]] int MainWindow::run() {
  pThis = this;
  const auto [localX, localY, width, height] { m_preferences.getMainWindowXYWH() };
  m_mainWindow = buildMainWindow(localX, localY, width, height);
  [[maybe_unused]]
  const auto menuBar = buildMenuBar(dimensions::MENUBAR_X, dimensions::MENUBAR_Y, width, dimensions::MENUBAR_HEIGHT);
  const auto list { m_preferences.getHandHistoryDirList() };
  m_games = buildGameList(dimensions::EMPTY_GAME_LIST_X, dimensions::EMPTY_GAME_LIST_Y, width-10, dimensions::EMPTY_GAME_LIST_HEIGHT, list);
  pReviewerButton = buildReviewButton(height);
  m_mainWindow->end();
  Fl::lock(); /* "start" the FLTK lock mechanism */
  m_mainWindow->show();
  auto ret { Fl::run() };
  pThis = nullptr;
  return ret;
}
