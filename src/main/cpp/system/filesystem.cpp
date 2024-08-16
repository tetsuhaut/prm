module;

export module system.filesystem;

import std;

export namespace prm::system::filesystem {
[[nodiscard]] bool isFile(const std::filesystem::path& p) noexcept;

[[nodiscard]] bool isDir(const std::filesystem::path& p) noexcept;

[[nodiscard]] std::vector<std::filesystem::path> listFilesAndDirs(
  const std::filesystem::path& dir);

[[nodiscard]] std::vector<std::filesystem::path> listTxtFilesInDir(
  const std::filesystem::path& dir);

[[nodiscard]] std::vector<std::filesystem::path> listFilesInDir(
  const std::filesystem::path& dir, std::string_view postFix);

/**
 * Returns the list of directories contained in @param dir.
 */
[[nodiscard]] std::vector<std::filesystem::path> listSubDirs(const std::filesystem::path&
    dir);
} // namespace prm::system::filesystem

module : private;

bool prm::system::filesystem::isFile(const std::filesystem::path& p) noexcept {
  std::error_code ec;
  return std::filesystem::is_regular_file(p, ec) and 0 == ec.value();
}

bool prm::system::filesystem::isDir(const std::filesystem::path& p) noexcept {
  std::error_code ec;
  return std::filesystem::is_directory(p, ec) and 0 == ec.value();
}

template<typename Iterator>
class [[nodiscard]] FilesInDir final {
private:
  std::filesystem::path m_startDir;

public:
  template<typename T> requires(
    std::same_as<T, std::filesystem::path>) // use only std::filesystem::path
  explicit FilesInDir(const T& startDir) : m_startDir{ startDir } {}
  //FilesInDir(auto) = delete; // use only std::filesystem::path
  [[nodiscard]] Iterator begin() const { return Iterator(m_startDir); }
  [[nodiscard]] Iterator end() const noexcept { return Iterator(); }
  [[nodiscard]] Iterator begin() { return Iterator(m_startDir); }
  [[nodiscard]] Iterator end() noexcept { return Iterator(); }
}; // class FilesInDir

[[nodiscard]] std::vector<std::filesystem::path> prm::system::filesystem::listFilesAndDirs(
  const std::filesystem::path&
  dir) {
  std::vector<std::filesystem::path> ret;

  if (!isDir(dir)) { return ret; }

  for (const auto& dirEntry : FilesInDir<std::filesystem::directory_iterator>(dir)) { ret.push_back(dirEntry.path()); }

  return ret;
}

/**
 * Returns the list of directories contained in @param dir.
 */
std::vector<std::filesystem::path> prm::system::filesystem::listSubDirs(const std::filesystem::path&
    dir) {
  auto allFilesAndDirs { listFilesAndDirs(dir) };
  std::erase_if(allFilesAndDirs, [](const auto & p) noexcept { return isFile(p); });
  return allFilesAndDirs;
}

std::vector<std::filesystem::path> prm::system::filesystem::listTxtFilesInDir(
  const std::filesystem::path& dir) {
  auto allFilesAndDirs { listFilesAndDirs(dir) };
  std::erase_if(allFilesAndDirs, [](const auto & p) {
    const auto& pstr { p.string() };
    return !isFile(p) or !pstr.ends_with(".txt") or pstr.ends_with("_summary.txt");
  });
  return allFilesAndDirs;
}

[[nodiscard]] std::vector<std::filesystem::path> prm::system::filesystem::listFilesInDir(
  const std::filesystem::path& dir,
  std::string_view postFix) {
  std::vector<std::filesystem::path> ret;

  if (!isDir(dir)) { return ret; }

  std::ranges::for_each(std::filesystem::directory_iterator(dir), [&](const auto & dirEntry) {
    const auto& entryPath { dirEntry.path() };

    if (entryPath.filename().string().starts_with("20") // like the year 2022
        and entryPath.string().ends_with(postFix)
        and dirEntry.is_regular_file()) {
      ret.push_back(entryPath);
    }
  });
  return ret;
}
