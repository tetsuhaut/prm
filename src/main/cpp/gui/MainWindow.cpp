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

#include <cassert> // assert

export module gui.MainWindow;

import entities.Card;
import entities.Game; // CashGame, Tournament
import entities.Site;
import gui.Dimensions; // button size
import gui.GameList;
import gui.Labels;
import gui.Preferences;
import gui.ReviewerWindow;
import history.WinamaxGameHistory;
import history.WinamaxHistory;

import std;

export class [[nodiscard]] MainWindow final {
private:
  Preferences m_preferences = Preferences();
  std::unique_ptr<Fl_Double_Window> m_mainWindow = nullptr;
  std::unique_ptr<ReviewerWindow> m_reviewerWindow = nullptr;
  std::unique_ptr<GameList> m_games = nullptr;

public:
  void exit();
  void toggleGameWindow();
  [[nodiscard]] int run();
  void chooseHandHistoryDirectory();
  void removeHandHistoryDirectory();
  void newGameWindow();
  void addHistoryDirectoryToList(std::string_view dir);
}; // export class MainWindow

module : private;

namespace fs = std::filesystem;
static MainWindow* pThis { nullptr };
static Fl_Button* pReviewerButton { nullptr };
static constexpr std::string_view CLOSE_THE_REVIEW_LABEL { "Close the review" };

//[[nodiscard]] static constexpr int toInt(std::size_t value) {
//  constexpr int kIntMax { std::numeric_limits<int>::max() };
//  return std::cmp_greater(value, kIntMax) ? kIntMax : static_cast<int>(value);
//}

//[[nodiscard]] static constexpr int toInt(auto) = delete; // forbid other types

[[nodiscard]] static std::unique_ptr<Fl_Native_File_Chooser> buildHandHistoryDirectoryChooser(Preferences& p) {
  auto pHistoryChoser { std::make_unique<Fl_Native_File_Chooser>() };
  pHistoryChoser->title("Choisissez un répertoire d'historiques de mains");
  pHistoryChoser->type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
  auto prev { p.getPreviousChosenHistoryDir() };
  pHistoryChoser->directory(prev.c_str());
  return pHistoryChoser;
}

/**
  * Called by the event loop when the user chosed a valid history file.
  * Starts the import process.
  */
void MainWindow::newGameWindow() {
  const auto oHistoryFile { m_games->getSelectedGameHistoryFile() };
  assert(oHistoryFile.has_value());
  const auto site { WinamaxGameHistory::parseGameHistory(oHistoryFile.value()) };
  const auto cashGames { site->viewCashGames() };

  if (cashGames.empty()) {
    fl_alert("Pas de cashgame détecté dans cet historique");
    pReviewerButton->label(labels::OPEN_THE_REVIEW_LABEL.data());
    return;
  }

  assert(1 == cashGames.size());
  const auto [localX, localY, width, height] { m_preferences.getGameWindowXYWH() };

  const auto deleteReviewerWindow = [this]() {
    // destroy the existing game window
    m_reviewerWindow.reset();
    // tell the user the review button will open a new game window
    pReviewerButton->label(labels::OPEN_THE_REVIEW_LABEL.data());
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

template<typename T>
[[nodiscard]] static std::unique_ptr<T> toUniquePtr(void* value) {
  return std::unique_ptr<T>(static_cast<T*>(value));
}

void MainWindow::addHistoryDirectoryToList(std::string_view dir) {
  m_games->addDir(dir);
  m_preferences.savePreviousChosenHistoryDir(dir);
}

struct Data {
  MainWindow* pThis { nullptr };
  std::string dir;
};

static void addHistoryDirectoryToListAwakeCb(void* hiddenData) {
  auto data { toUniquePtr<Data>(hiddenData) };
  data->pThis->addHistoryDirectoryToList(data->dir);
}

void MainWindow::chooseHandHistoryDirectory() {
  auto dirChoser { buildHandHistoryDirectoryChooser(m_preferences) };

  switch (FileChoiceStatus(dirChoser->show())) {
  case FileChoiceStatus::ok: {
    const std::filesystem::path p { dirChoser->filename() };

    if (!std::filesystem::is_directory(p)) { fl_alert("Veuillez choisir un répertoire"); }
    else if (!m_games->containsGameHistoryDir(dirChoser->filename())) {
        auto data { std::make_unique<Data>() };
        data->pThis = this;
        data->dir = dirChoser->filename();
        Fl::awake(addHistoryDirectoryToListAwakeCb, data.release());
      }    
  } break;

  case FileChoiceStatus::error: [[unlikely]] {
    fl_alert(dirChoser->errmsg());
    } break;

  case FileChoiceStatus::cancel: [[fallthrough]];

  default: /* nothing to do */ break;
  }
}

void MainWindow::removeHandHistoryDirectory() {
  if (const auto oDir { m_games->getSelectedGameHistoryDir() }; oDir.has_value()) {
    m_games->removeDir(oDir.value());
  }
}

/**
  * Called when the user clicks on the 'exit' menu or the X.
  */
void MainWindow::exit() {
  /* remember the windows position when closing them */
  if (m_mainWindow) {
    std::array xywh {m_mainWindow->x(), m_mainWindow->y(), m_mainWindow->w(), m_mainWindow->h()};
    m_preferences.saveMainWindowSizeAndPosition(xywh);
    const auto dirs { m_games->getGameHistoryDirs() };
    m_preferences.saveGameHistoryDirs(dirs);
  }

  /* hide all windows: this will terminate the MainWindow */
  while (Fl::first_window()) { Fl::first_window()->hide(); }
}

void MainWindow::toggleGameWindow() {
  if (nullptr != m_reviewerWindow) {
    // destroy the existing game window
    m_reviewerWindow.reset();
    // tell the user the review button will open a new game window
    pReviewerButton->label(labels::OPEN_THE_REVIEW_LABEL.data());
  } else {
    // tell the user the review button will close the game window
    pReviewerButton->label(CLOSE_THE_REVIEW_LABEL.data());
    // open a new game window and populate it
    // we need to use the same flavor of Fl::awake() in the whole program
    // see https://www.fltk.org/doc-1.4/advanced.html#advanced_multithreading
    Fl::awake([](void*) { pThis->newGameWindow(); }, nullptr);
  }
}

static void reviewCallback(Fl_Widget*, void*) {
  pThis->toggleGameWindow();
}

[[nodiscard]] static Fl_Button* buildReviewButton(int bottom) {
  auto pReviewButton = new Fl_Button(dimensions::BUTTON_X, bottom - dimensions::BUTTON_HEIGHT - 5, dimensions::BUTTON_WIDTH,
    dimensions::BUTTON_HEIGHT, labels::OPEN_THE_REVIEW_LABEL.data());
  pReviewButton->callback(reviewCallback);
  pReviewButton->deactivate();
  return pReviewButton;
}

[[nodiscard]] static std::unique_ptr<GameList> buildGameList(int x, int y, int width, int height, Preferences& prefs) {
  auto gameList { std::make_unique<GameList>(x, y, width, height) };
  gameList->listenToElementSelection([](const auto& item) {
    if ((0 == item.children()) and std::string(item.label()).ends_with(".txt")) {
      pReviewerButton->activate();
    } else {
      pReviewerButton->deactivate();
    }
  });
  gameList->setReviewCallback(reviewCallback);
  const auto dirs { prefs.readGameHistoryDirs() };

  for (const auto& dir : dirs) { gameList->addDir(dir); }
  return gameList;
}

[[nodiscard]] static std::unique_ptr<Fl_Menu_Bar> buildMenuBar(int x, int y, int width, int height) {
  auto pMenuBar = std::make_unique<Fl_Menu_Bar>(x, y, width, height);
  pMenuBar->add("&File/Add hand history directory", 0, [](Fl_Widget*, void*) { pThis->chooseHandHistoryDirectory(); });
  pMenuBar->add("&File/Remove hand history directory", 0, [](Fl_Widget*, void*) { pThis->removeHandHistoryDirectory(); });
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
  pMainWindow->callback([](auto) {
    // we dont't want the Esc key to close the program
    if (FL_SHORTCUT != Fl::event() or FL_Escape != Fl::event_key()) { pThis->exit(); }
  });
  return pMainWindow;
}

[[nodiscard]] int MainWindow::run() {
  pThis = this;
  const auto [localX, localY, width, height] { m_preferences.getMainWindowXYWH() };
  m_mainWindow = buildMainWindow(localX, localY, width, height);
  [[maybe_unused]]
  const auto menuBar = buildMenuBar(dimensions::MENUBAR_X, dimensions::MENUBAR_Y, width, dimensions::MENUBAR_HEIGHT);
  m_games = buildGameList(dimensions::GAME_LIST_X, dimensions::GAME_LIST_Y, width - 10, dimensions::GAME_LIST_HEIGHT, m_preferences);
  pReviewerButton = buildReviewButton(height);
  m_mainWindow->end();
  Fl::lock(); /* "start" the FLTK lock mechanism */
  m_mainWindow->show();
  auto ret { Fl::run() };
  pThis = nullptr;
  return ret;
}
