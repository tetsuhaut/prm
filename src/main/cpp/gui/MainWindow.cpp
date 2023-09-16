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
#include <cassert>
#include <cerrno>
#include <cstring> // std::strerror_s
#include <filesystem> // std::path
#include <format> // std::format
#include <memory> // std::make_unique
#include <string> // std::string
#include <utility> // std::pair

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
  Preferences m_preferences;
  std::unique_ptr<Fl_Double_Window> m_mainWindow;
  std::unique_ptr<ReviewerWindow> m_reviewerWindow;
  std::unique_ptr<Fl_Menu_Bar> m_menuBar;
  std::unique_ptr<Fl_Text_Display> m_historyFileText;
  std::unique_ptr<std::filesystem::path> m_gameHistory;
  // widgets
  std::unique_ptr<Fl_Tree> m_games;
  std::unique_ptr<Fl_Button> m_reviewButton;

  void buildReviewButton();
public:
  ~MainWindow() = default;
  void exit();
  void toggleGameWindow();
  [[nodiscard]] int run();
  void chooseHandHistory();
  void newGameWindow();
}; // export class MainWindow

module : private;

[[nodiscard]] MainWindow* MAIN_WINDOW(void* self) {
  return static_cast<MainWindow*>(self);
}

void MainWindow::buildReviewButton() {
  m_reviewButton = std::make_unique<Fl_Button>(50, 200, dimensions::BUTTON_WIDTH,
                   dimensions::BUTTON_HEIGHT, "Open the review");
  m_reviewButton->callback([](Fl_Widget*, void* hidden) {
    // disabling closing the window on hitting escape won't work here
    MAIN_WINDOW(hidden)->toggleGameWindow();
  }, this);
}

void MainWindow::newGameWindow() {
  assert(nullptr != m_gameHistory);
  const auto site { WinamaxGameHistory::parseGameHistory(*m_gameHistory) };
  const auto cashGames { site->viewCashGames() };

  if (cashGames.empty()) {
    fl_alert("Pas de cashgame détecté dans cet historique");
    toggleGameWindow();
    return;
  }

  assert(1 == cashGames.size());
  const auto [localX, localY, width, height] { m_preferences.getGameWindowXYWH() };
  assert(nullptr != m_historyFileText->buffer()->text());
  m_reviewerWindow = std::make_unique<ReviewerWindow>(m_preferences,
                     m_historyFileText->buffer()->text(),
  [this]() { toggleGameWindow(); },
  site->whoIsHero(),
  *cashGames[0]->viewHands()[0]);
}

/**
  * Called by the event loop when the user chosed a valid history file.
  * Starts the import process.
  */
void newGameWindowAwakeCb(void* mainWindow) {
  MAIN_WINDOW(mainWindow)->newGameWindow();
}

bool wasHandlingChosenFileOk(const std::filesystem::path& file, Preferences& preferences,
                             Fl_Text_Display& text) {
  preferences.saveHistoryDir(file.parent_path());

  if (WinamaxHistory::isValidHistoryFile(file)) {
    text.buffer()->text(file.filename().string().c_str());
    return true;
  }

  fl_alert(std::format("The chosen file {} is not a valid poker history",
                       file.filename().string()).c_str());
  return false;
}

[[nodiscard]] std::unique_ptr<Fl_Native_File_Chooser> buildDirectoryChooser(
  Preferences& preferences) {
  auto pHistoryChoser { std::make_unique<Fl_Native_File_Chooser>() };
  pHistoryChoser->filter("*.txt");
  pHistoryChoser->title("Choisissez un historique de mains");
  pHistoryChoser->type(Fl_Native_File_Chooser::BROWSE_FILE);
  pHistoryChoser->directory(preferences.getHandHistoryDir().c_str());
  return pHistoryChoser;
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

void MainWindow::chooseHandHistory() {
  auto dirChoser { buildDirectoryChooser(m_preferences) };

  switch (FileChoiceStatus(dirChoser->show())) {
    case FileChoiceStatus::ok: {
      m_gameHistory = std::make_unique<std::filesystem::path>(dirChoser->filename());

      if (wasHandlingChosenFileOk(*m_gameHistory, m_preferences, *m_historyFileText)) {
        m_reviewButton->activate();
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
* Called by the button component when the user clicks on the 'choose hand history'
* button.
*/
void chooseHandHistoryCb(Fl_Widget*, void* mainWindow) {
  MAIN_WINDOW(mainWindow)->chooseHandHistory();
}

/**
  * Called when the user clicks on the 'exit' menu or the X.
  */
void MainWindow::exit() {
  /* remember the windows position when closing them */
  if (m_mainWindow) {
    std::array xywh {m_mainWindow->x(), m_mainWindow->y(), m_mainWindow->w(), m_mainWindow->h()};
    m_preferences.saveSizeAndPosition(xywh, Preferences::PrefName::mainWindow);
  }

  /* hide all windows: this will terminate the MainWindow */
  while (Fl::first_window()) { Fl::first_window()->hide(); }
}

void exitCb(Fl_Widget*, void* mainWindow) {
  MAIN_WINDOW(mainWindow)->exit();
}

void mainWindowCb(Fl_Widget*, void* mainWindow) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  MAIN_WINDOW(mainWindow)->exit();
}

void MainWindow::toggleGameWindow() {
  if (nullptr != m_reviewerWindow) {
    // destroy the existing game window
    m_reviewerWindow.reset();
    // tell the user the review button will open a new game window
    m_reviewButton->label("Open the review");
  } else {
    // tell the user the review button will close the game window
    m_reviewButton->label("Close the review");
    // open a new game window and populate it
    Fl::awake(newGameWindowAwakeCb, this);
  }
}

void reviewButtonCb(Fl_Widget*, void* mainWindow) {
  MAIN_WINDOW(mainWindow)->toggleGameWindow();
}

// +----> x
// |
// |
// v
// y
[[nodiscard]] int MainWindow::run() {
  const auto [localX, localY, width, height] { m_preferences.getMainWindowXYWH() };
  m_mainWindow = std::make_unique<Fl_Double_Window>(localX, localY, width, height,
                 "Poker Reviewer Modulaire");
  m_mainWindow->callback(mainWindowCb, this);
  m_menuBar = std::make_unique<Fl_Menu_Bar>(0, 0, width, dimensions::BUTTON_HEIGHT);
  m_menuBar->add("&File/O&pen hand history", 0, chooseHandHistoryCb, this);
  m_menuBar->add("&File/E&xit", 0, exitCb, this);
  m_historyFileText = std::make_unique<Fl_Text_Display>(0, m_menuBar->h(), width, 25);
  m_historyFileText->buffer(new Fl_Text_Buffer());
  m_reviewButton = std::make_unique<Fl_Button>(50, 200, dimensions::BUTTON_WIDTH,
                   dimensions::BUTTON_HEIGHT, "Open the review");
  m_reviewButton->callback(reviewButtonCb, this);
  m_reviewButton->deactivate();
  m_mainWindow->end();
  m_mainWindow->show();
  return Fl::run();
}
