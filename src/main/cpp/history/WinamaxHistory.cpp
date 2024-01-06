module;


#if defined(_MSC_VER) // removal of specific msvc warnings due to stlab
#  pragma warning(push)
#  pragma warning(disable : 4355 4868 4996 5204 )
#endif  // _MSC_VER

#include <stlab/concurrency/utility.hpp> // stlab::blocking_get
#include <stlab/concurrency/future.hpp> // stlab::async, std::forward
#include <stlab/concurrency/default_executor.hpp>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#endif  // _MSC_VER

#include <algorithm>
#include <expected>
#include <filesystem>
#include <format>
#include <functional> // std::function
#include <iostream> // std::cerr TODO use std::print when available
#include <ranges>
#include <span>
#include <vector>

export module history.WinamaxHistory;

import entities.Action;
import entities.Card;
import entities.Game;
import entities.Player;
import entities.Site;
import history.WinamaxGameHistory;
import language.strings;
import system.filesystem;


using FunctionVoid = std::function<void()>;
using FunctionInt = std::function<void(std::size_t)>;

export class [[nodiscard]] WinamaxHistory final {
private:
  struct Implementation;
  std::unique_ptr<Implementation> m_pImpl;
public:
  WinamaxHistory() noexcept;
  WinamaxHistory(const WinamaxHistory&) = delete;
  WinamaxHistory(WinamaxHistory&&) = delete;
  WinamaxHistory& operator=(const WinamaxHistory&) = delete;
  WinamaxHistory& operator=(WinamaxHistory&&) = delete;
  ~WinamaxHistory();
  /**
   * @returns a Site containing all the games which history files are located in
   * the given <historyDir>/history directory.
   */
  [[nodiscard]] std::unique_ptr<Site> load(const std::filesystem::path& historyDir,
      FunctionVoid incrementCb,
      FunctionInt setNbFilesCb);
  std::unique_ptr<Site> load(auto, FunctionVoid, FunctionInt) = delete;

  [[nodiscard]] static std::unique_ptr<Site> importGame(const std::filesystem::path& historyDir);
  std::unique_ptr<Site> importGame(auto) = delete;

  void stopGameImporting();

  [[nodiscard]] std::unique_ptr<Site> reloadFile(const std::filesystem::path& winamaxHistoryFile);
  std::unique_ptr<Site> reloadFile(auto) = delete;

  [[nodiscard]] static bool isValidHistoryDir(const std::filesystem::path& historyDir);
  static bool isValidHistoryDir(auto) = delete;

  [[nodiscard]] static bool isValidHistoryFile(const std::filesystem::path& historyFile);
  static bool isValidHistoryFile(auto) = delete;

  [[nodiscard]] std::string_view getTableNameFromTableWindowTitle(std::string_view tableWindowTitle)
  const;

  [[nodiscard]] std::filesystem::path getHistoryFileFromTableWindowTitle(
    const std::filesystem::path& historyDir,
    std::string_view tableWindowTitle) const;
  std::filesystem::path getHistoryFileFromTableWindowTitle(auto, std::string_view) const = delete;
}; // class WinamaxHistory

module : private;

struct [[nodiscard]] WinamaxHistory::Implementation final {
  std::vector<stlab::future<Site*>> m_tasks {};
  std::atomic_bool m_stop { true };
}; // struct WinamaxHistory::Implementation

WinamaxHistory::WinamaxHistory() noexcept : m_pImpl { std::make_unique<Implementation>() }  {}

WinamaxHistory::~WinamaxHistory() {
  try {
    stopGameImporting();
  } catch (...) {
    std::exit(4);
  }
}

[[nodiscard]] std::expected<std::vector<std::filesystem::path>, std::string>
getErrorMessageOrHistoryFiles(
  const std::filesystem::path& dir, const std::filesystem::path& histoDir) {
  if (!prm::system::filesystem::isDir(histoDir)) {
    return std::unexpected(
             std::format("The chosen directory '{}' should contain a 'history' directory", dir.string()));
  }

  if (!prm::system::filesystem::isDir(dir / "data" / "buddy")) {
    return std::unexpected(
             std::format("The chosen directory '{}' should contain a 'data/buddy' directory", dir.string()));
  }

  if (!prm::system::filesystem::isDir(dir / "data" / "players")) {
    return std::unexpected(
             std::format("The chosen directory '{}' should contain a 'data/players' directory", dir.string()));
  }

  if (!prm::system::filesystem::listSubDirs(histoDir).empty()) {
    return std::unexpected(
             std::format("The chosen directory '{}' should contain a 'history' directory that contains only files",
                         dir.string()));
  }

  if (const auto & allFilesAndDirs { prm::system::filesystem::listFilesAndDirs(histoDir) };
      !allFilesAndDirs.empty()) {
    return allFilesAndDirs;
  }

  return std::unexpected(
           std::format("The chosen directory '{}' should contain a non empty 'history' directory ",
                       dir.string()));
}


[[nodiscard]] bool containsAFileEndingWith(std::span<const std::filesystem::path> files,
    std::string_view str) {
  return std::ranges::any_of(files, [str](const auto & p) { return p.string().ends_with(str); });
}

/**
 * @return true if the given dir is an existing dir, contains a 'history' subdir which only contains txt files,
 * and if it contains a 'data' subdir, beside 'history', which contain 'buddy' and 'players' subdirs.
 */
bool WinamaxHistory::isValidHistoryDir(const std::filesystem::path& dir) {
  // have to use std::filesystem::path.append() to produce consistent result on all compilers
  const auto& histoDir { (dir / "history").lexically_normal() };
  const auto& expected { getErrorMessageOrHistoryFiles(dir, histoDir) };

  if (!expected.has_value()) {
    return false; // do not throw as functionnaly correct
  }

  const auto& allFilesAndDirs { expected.value() };
  return containsAFileEndingWith(allFilesAndDirs, "winamax_positioning_file.dat")
         and containsAFileEndingWith(allFilesAndDirs, "_summary.txt");
}

std::vector<std::filesystem::path> getFiles(const std::filesystem::path& historyDir) {
  if (WinamaxHistory::isValidHistoryDir(historyDir)) { return prm::system::filesystem::listTxtFilesInDir(historyDir / "history"); }

  return {};
}

bool WinamaxHistory::isValidHistoryFile(const std::filesystem::path& historyFile) {
  std::error_code ec;
  return
    (std::filesystem::is_regular_file(historyFile, ec) and 0 == ec.value()) and
    (historyFile.extension() == ".txt") and
    !historyFile.filename().string().ends_with("_summary.txt");
}

[[nodiscard]] std::vector<std::filesystem::path> getFiles(auto) =
  delete; // use only std::filesystem::path

// using auto&& enhances performances by inlining std::function's logic
[[nodiscard]] std::vector<std::filesystem::path> getFilesAndNotify(const std::filesystem::path&
    historyDir,
    auto&& setNbFilesCb) {
  const auto& files { getFiles(historyDir) };

  if (setNbFilesCb) {
    const auto fileSize{ files.size() };

    if (!files.empty()) { std::forward<decltype(setNbFilesCb)>(setNbFilesCb)(fileSize); }
  }

  return files;
}
std::vector<std::filesystem::path> getFilesAndNotify(auto, auto) = delete;

std::vector<stlab::future<Site*>> parseFilesAsync(std::span<const std::filesystem::path> files,
std::atomic_bool& stop, const auto& incrementCb) {
  std::vector<stlab::future<Site*>> ret;
  ret.reserve(files.size());
  std::transform(std::begin(files), std::end(files), std::back_inserter(ret), [&incrementCb,
  &stop](const auto & file) {
    if (!stop) {
      return stlab::async(stlab::default_executor, [&file, &incrementCb, &stop]() {
        Site* pSite { nullptr };

        try {
          if (!stop) { pSite = WinamaxGameHistory::parseGameHistory(file).release(); }
        } catch (const std::exception& e) {
          std::cerr << std::format("Exception loading the file {}: {}\n", file.filename().string(), e.what());
        } catch (const char* str) {
          std::cerr << std::format("Exception loading the file {}: {}\n", file.filename().string(), str);
        }

        if (!stop and incrementCb) { incrementCb(); }

        return pSite;
      });
    } else { return stlab::future<Site*>(); }
  });
  return ret;
}

constexpr std::string_view WINAMAX_SITE_NAME = "Winamax";

std::unique_ptr<Site> WinamaxHistory::load(const std::filesystem::path& winamaxHistoryDir,
    FunctionVoid incrementCb,
    FunctionInt setNbFilesCb) {
  m_pImpl->m_stop = false;

  try {
    const auto& files { getFilesAndNotify(winamaxHistoryDir, setNbFilesCb) };
    auto ret { std::make_unique<Site>(WINAMAX_SITE_NAME) };

    if (files.empty()) {
      return ret;
    }

    m_pImpl->m_tasks = parseFilesAsync(files, m_pImpl->m_stop, incrementCb);
    std::ranges::for_each(m_pImpl->m_tasks, [&ret, this](auto & task) {
      if (task.valid()) {
        std::unique_ptr<Site> s { stlab::blocking_get(task) };

        if (!m_pImpl->m_stop and s) { ret->merge(*s); }
      }
    });
    m_pImpl->m_tasks.clear();
    return ret;
  } catch (const std::exception& e) {
    std::cerr << std::format("Exception au chargement de {} : {}\n", winamaxHistoryDir.string(),
                             e.what());
    return std::make_unique<Site>(WINAMAX_SITE_NAME);
  }
}

/* [[nodicard]] static */ std::unique_ptr<Site> WinamaxHistory::importGame(
  const std::filesystem::path& historyDir) {
  WinamaxHistory wh;
  return wh.load(historyDir, nullptr, nullptr);
}

void WinamaxHistory::stopGameImporting() {
  m_pImpl->m_stop = true;
  std::size_t nbTasksFinished { 0 };

  while (nbTasksFinished != m_pImpl->m_tasks.size()) {
    std::ranges::for_each(m_pImpl->m_tasks, [&nbTasksFinished](auto & task) {
      if (task.is_ready()) {
        task.reset(); // it won't be ready anymore
        nbTasksFinished++;
      }
    });
  }
}

std::unique_ptr<Site> WinamaxHistory::reloadFile(const std::filesystem::path& file) {
  std::unique_ptr<Site> ret { nullptr };

  try {
    ret = WinamaxGameHistory::parseGameHistory(file);
  } catch (const std::exception& e) {
    std::cerr << std::format("Exception loading the file {}: {}\n", file.string(), e.what());
  }

  return ret;
}

[[nodiscard]] inline bool notFound(std::string_view::size_type st) { return std::string_view::npos == st; }

std::string_view WinamaxHistory::getTableNameFromTableWindowTitle(std::string_view tableWindowTitle)
const {
  const auto pos { tableWindowTitle.find("#") };
  return (notFound(pos)) ? tableWindowTitle.substr(0,
         tableWindowTitle.find(" / ")) : tableWindowTitle.substr(0, pos);
}

static constexpr bool isReal(std::string_view tableWindowTitle) noexcept { return !tableWindowTitle.ends_with("fictif"); }

std::filesystem::path WinamaxHistory::getHistoryFileFromTableWindowTitle(
  const std::filesystem::path& historyDir,
  std::string_view tableWindowTitle) const {
  const auto tableName { getTableNameFromTableWindowTitle(tableWindowTitle) };
  const auto& reality { isReal(tableWindowTitle) ? "real" : "play" };
  const auto& game { tableWindowTitle.contains("NL Holdem") ? "holdem_no-limit" : "omaha5_pot-limit" };
  const auto& postfix { std::format("_{}_{}_{}.txt", tableName, reality, game)};
  auto files { prm::system::filesystem::listFilesInDir(historyDir / "history", postfix) };

  if (files.empty()) {
    return "";
  }

  if (1 == files.size()) { return files.front(); }

  std::ranges::sort(files, [](const std::filesystem::path & a,
  const std::filesystem::path & b) { return a.string() < b.string(); });
  return files.back();
}