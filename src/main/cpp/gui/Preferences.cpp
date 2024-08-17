module;

#if defined(_MSC_VER) // removal of specific msvc warnings due to FLTK
#  pragma warning(push)
#  pragma warning(disable : 4191 4242 4244 4365 4458 4514 4625 4626 4668 4820 5026 5027 5219 )
#endif  // _MSC_VER

#include <FL/Fl.H> // assert
#include <FL/fl_ask.H> // fl_alert
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Tree.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#endif  // _MSC_VER

export module gui.Preferences;

import gui.Dimensions;
import language.strings;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop ) 

template<typename T>
concept Savable = std::is_same_v<T, double> or std::is_same_v<T, float> or std::is_same_v<T, int> or std::is_same_v<T, const char*>;

/**
 * User preferences. This class encapsulates the FLTK user preference management.
 */
export class [[nodiscard]] Preferences final {
private:
  std::unique_ptr<Fl_Preferences> m_preferences { std::make_unique<Fl_Preferences>(Fl_Preferences::USER,
        APP_NAME_SHORT, APP_NAME_SHORT) };
public:
  enum class PrefName : short {
    mainWindow, reviewerWindow, handHistoDir
  };
  template<Savable T>
  void save(std::string_view key, T value);
  [[nodiscard]] std::tuple<int, int, int, int> getMainWindowXYWH() const;
  [[nodiscard]] std::tuple<int, int, int, int> getGameWindowXYWH() const;
  void saveMainWindowSizeAndPosition(const std::array<int, 4>& xywh);
  void saveGameReviewWindowSizeAndPosition(const std::array<int, 4>& xywh);
  void saveGameHistoryDirs(const std::vector<std::string>& dirs);
  [[nodiscard]] std::vector<std::string> readGameHistoryDirs() /*const*/;
  void savePreviousChosenHistoryDir(std::string_view dir);
  std::string getPreviousChosenHistoryDir() const;
  Preferences() = default;
  Preferences(const Preferences&) = delete;
  ~Preferences() = default;
  Preferences& operator=(const Preferences& t) = delete;
}; // export class Preferences

module : private;

namespace detail {
static constexpr std::string_view MAIN_WINDOW_X = "mainwindowx";
static constexpr std::string_view MAIN_WINDOW_Y = "mainwindowy";
static constexpr std::string_view MAIN_WINDOW_WIDTH = "mainwindoww";
static constexpr std::string_view MAIN_WINDOW_HEIGHT = "mainwindowh";
static constexpr std::string_view GAME_WINDOW_X = "gamewindowx";
static constexpr std::string_view GAME_WINDOW_Y = "gamewindowy";
static constexpr std::string_view GAME_WINDOW_WIDTH = "gamewindoww";
static constexpr std::string_view GAME_WINDOW_HEIGHT = "gamewindowh";
static constexpr std::string_view HISTORY_DIR = "historyDir";
static constexpr std::string_view PREVIOUS_HISTORY_DIR = "previousHistoryDir";

[[nodiscard]] static int getIntWithMin(Fl_Preferences& fltkPreferences, std::string_view key, int minValue) {
  int value;
  fltkPreferences.get(key.data(), value, minValue);
  return (value < minValue) ? minValue : value;
}

[[nodiscard]] static int getIntWithDefault(Fl_Preferences& fltkPreferences, std::string_view key, int defaultValue) {
  int value;
  fltkPreferences.get(key.data(), value, defaultValue);
  return value;
}

[[nodiscard]] static std::string getString(Fl_Preferences& fltkPreferences, std::string_view key) {
  char value[512];
  fltkPreferences.get(key.data(), &value[0], "", std::ssize(value) - 1);
  return value;
}

[[nodiscard]] std::tuple<int, int, int, int> getGenericWindowXYWH(
  Fl_Preferences& fltkPreferences,
  std::string_view xname,
  std::string_view yname,
  std::string_view widthname,
  std::string_view heightname) {
  /* get the previous width and height from preferences, if any */
  auto width { getIntWithMin(fltkPreferences, widthname, dimensions::MAIN_WINDOW_WIDTH) };
  auto height { getIntWithMin(fltkPreferences, heightname, dimensions::MAIN_WINDOW_HEIGHT) };
  /* compute the center position */
  int dummyX, dummyY, screenWidth, screenHeight;
  Fl::screen_xywh(dummyX, dummyY, screenWidth, screenHeight);
  /* get the previous position from preferences. if none, use the center position */
  auto x { getIntWithDefault(fltkPreferences, xname, (screenWidth - width) / 2) };
  auto y { getIntWithDefault(fltkPreferences, yname, (screenHeight - height) / 2) };
  return { x, y, width, height };
}

} // namespace detail

template<Savable T>
void Preferences::save(std::string_view key, T value) {
  if (0 == m_preferences->set(key.data(), value)) {
    const auto str { std::format("Couldn't save '{}' into the preferences repository.", key) };
    fl_alert(str.c_str());
  }
}

void Preferences::saveMainWindowSizeAndPosition(const std::array<int, 4>& xywh) {
  using namespace detail;
  const auto& [x, y, w, h] { xywh };
  save(MAIN_WINDOW_X, x);
  save(MAIN_WINDOW_Y, y);
  save(MAIN_WINDOW_WIDTH, w);
  save(MAIN_WINDOW_HEIGHT, h);
}

void Preferences::saveGameReviewWindowSizeAndPosition(const std::array<int, 4>& xywh) {
  using namespace detail;
  const auto& [x, y, w, h] { xywh };
  save(GAME_WINDOW_X, x);
  save(GAME_WINDOW_Y, y);
  save(GAME_WINDOW_WIDTH, w);
  save(GAME_WINDOW_HEIGHT, h);
}

[[nodiscard]] static inline std::string toKey(std::string_view key, std::size_t i) {
  return std::format("{}|{}", key, i);
}

void Preferences::saveGameHistoryDirs(const std::vector<std::string>& dirs) {
  using namespace detail;
  bool keepGoing = true;
  std::size_t i = 0;
  do {
    const auto key { toKey(HISTORY_DIR, i) };
    if (i < dirs.size()) { save(key, dirs[i].c_str()); }
    else if (m_preferences->entry_exists(key.c_str())) { m_preferences->delete_entry(key.c_str()); }
    else { keepGoing = false; }
    i++;
  } while (keepGoing);
}

[[nodiscard]] std::vector<std::string> Preferences::readGameHistoryDirs() {
  std::vector<std::string> ret;
  using namespace detail;
  bool keepGoing = true;
  std::size_t i = 0;
  do {
    const auto key { toKey(HISTORY_DIR, i) };
    keepGoing = (0 != m_preferences->entry_exists(key.c_str()));
    if (keepGoing) { ret.push_back(getString(*m_preferences, key.c_str())); }
    i++;
  } while (keepGoing);

  return ret;
}

void Preferences::savePreviousChosenHistoryDir(std::string_view dir) {
  save(detail::PREVIOUS_HISTORY_DIR, dir.data());
}

std::string Preferences::getPreviousChosenHistoryDir() const {
  return detail::getString(*m_preferences, detail::PREVIOUS_HISTORY_DIR);
}

[[nodiscard]] std::tuple<int, int, int, int> Preferences::getMainWindowXYWH() const {
  using namespace detail;
  return getGenericWindowXYWH(*m_preferences, MAIN_WINDOW_X, MAIN_WINDOW_Y, MAIN_WINDOW_WIDTH,
                                      MAIN_WINDOW_HEIGHT);
}

[[nodiscard]] std::tuple<int, int, int, int> Preferences::getGameWindowXYWH() const {
  using namespace detail;
  return getGenericWindowXYWH(*m_preferences, GAME_WINDOW_X, GAME_WINDOW_Y, GAME_WINDOW_WIDTH,
                                      GAME_WINDOW_HEIGHT);
}
