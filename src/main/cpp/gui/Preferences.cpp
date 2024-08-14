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

#include <array>
#include <cassert>
#include <cctype>
#include <filesystem>
#include <format>
#include <memory> // std::unique_ptr
#include <print>
#include <string_view>
#include <tuple>


export module gui.Preferences;

import language.strings;

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
  void saveWindowX(PrefName, int x);
  void saveWindowY(PrefName, int y);
  void saveWindowWidth(PrefName, int width);
  void saveWindowHeight(PrefName, int height);
  void loadGameList(Fl_Tree& tree) const;
  [[nodiscard]] std::tuple<int, int, int, int> getMainWindowXYWH() const;
  [[nodiscard]] std::tuple<int, int, int, int> getGameWindowXYWH() const;
  void saveSizeAndPosition(const std::array<int, 4>& xywh, PrefName name);
  void saveGameList(Fl_Tree& tree);
  void savePreviousHistoryDir(std::string_view dir);
  int getHistoryFilesSize() const;
  std::string getPreviousHistoryDir() const;
  Preferences() = default;
  Preferences(const Preferences&) = delete;
  ~Preferences() = default;
  Preferences& operator=(const Preferences& t) = delete;
}; // export class Preferences

module : private;

namespace detail {
static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;
static constexpr std::string_view MAIN_WINDOW_X = "mainwindowx";
static constexpr std::string_view MAIN_WINDOW_Y = "mainwindowy";
static constexpr std::string_view MAIN_WINDOW_WIDTH = "mainwindoww";
static constexpr std::string_view MAIN_WINDOW_HEIGHT = "mainwindowh";
static constexpr std::string_view GAME_WINDOW_X = "gamewindowx";
static constexpr std::string_view GAME_WINDOW_Y = "gamewindowy";
static constexpr std::string_view GAME_WINDOW_WIDTH = "gamewindoww";
static constexpr std::string_view GAME_WINDOW_HEIGHT = "gamewindowh";
static constexpr std::string_view PREVIOUS_HISTORY_DIR = "previousHistoryDir";
static constexpr std::string_view TREE = "tree";
static constexpr std::string_view TREE_X = "treex";
static constexpr std::string_view TREE_Y = "treey";
static constexpr std::string_view TREE_W = "treew";
static constexpr std::string_view TREE_H = "treeh";

static void save(Fl_Preferences& fltkPreferences, std::string_view key, auto&& value) {
  if (0 == fltkPreferences.set(key.data(), std::forward<decltype(value)>(value))) {
    fl_alert(std::format("Couldn't save '{}' into the preferences repository.", key).c_str());
  }
}

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
  auto width { getIntWithMin(fltkPreferences, widthname, DEFAULT_WIDTH) };
  auto height { getIntWithMin(fltkPreferences, heightname, DEFAULT_HEIGHT) };
  /* compute the center position */
  int dummyX, dummyY, screenWidth, screenHeight;
  Fl::screen_xywh(dummyX, dummyY, screenWidth, screenHeight);
  /* get the previous position from preferences. if none, use the center position */
  auto x { getIntWithDefault(fltkPreferences, xname, (screenWidth - width) / 2) };
  auto y { getIntWithDefault(fltkPreferences, yname, (screenHeight - height) / 2) };
  return { x, y, width, height };
}

} // namespace detail

void Preferences::saveWindowX(PrefName p, int x) {
  using namespace detail;
  auto key = (PrefName::mainWindow == p) ? MAIN_WINDOW_X : GAME_WINDOW_X;
  detail::save(*m_preferences, key, x);
}

void Preferences::saveWindowY(PrefName p, int y) {
  using namespace detail;
  auto key = (PrefName::mainWindow == p) ? MAIN_WINDOW_Y : GAME_WINDOW_Y;
  detail::save(*m_preferences, key, y);
}

void Preferences::saveWindowWidth(PrefName p, int width) {
  using namespace detail;
  auto key = (PrefName::mainWindow == p) ? MAIN_WINDOW_WIDTH : GAME_WINDOW_WIDTH;
  detail::save(*m_preferences, key, width);
}

void Preferences::saveWindowHeight(PrefName p, int height) {
  using namespace detail;
  auto key = (PrefName::mainWindow == p) ? MAIN_WINDOW_HEIGHT : GAME_WINDOW_HEIGHT;
  detail::save(*m_preferences, key, height);
}

void Preferences::saveSizeAndPosition(const std::array<int, 4>& xywh, Preferences::PrefName name) {
  const auto& [x, y, w, h] { xywh };
  saveWindowX(name, x);
  saveWindowY(name, y);
  saveWindowWidth(name, w);
  saveWindowHeight(name, h);
}

void Preferences::saveGameList(Fl_Tree& tree) {
  using namespace detail;
  save(*m_preferences, TREE_X, tree.x());
  save(*m_preferences, TREE_Y, tree.y());
  save(*m_preferences, TREE_W, tree.w());
  save(*m_preferences, TREE_H, tree.h());
  int i = 0;
  for (auto pItem = tree.first(); pItem; pItem = tree.next(pItem)) {
    save(*m_preferences, std::format("{}{}", TREE, i), pItem->label());
    ++i;
  }
}

void Preferences::savePreviousHistoryDir(std::string_view dir) {
  detail::save(*m_preferences, detail::PREVIOUS_HISTORY_DIR, dir.data());
}

std::string Preferences::getPreviousHistoryDir() const {
  return detail::getString(*m_preferences, detail::PREVIOUS_HISTORY_DIR);
}

void Preferences::loadGameList(Fl_Tree& tree) const {
  using namespace detail;
  
  std::string s;
  int i = 0;
  do {
    if (s = getString(*m_preferences, std::format("{}{}", TREE, i)); !s.empty()) {
      i++;
      tree.add(s.c_str());
    }
  } while (!s.empty());
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

[[nodiscard]] int Preferences::getHistoryFilesSize() const {
  return m_preferences->entries();
}