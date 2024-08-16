module;

#include <gsl/gsl>

#include <cassert> // assert

export module system.TextFile;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop ) 

/**
 * A text file reader.
 */
export class [[nodiscard]] TextFile final {
private:
  std::filesystem::path m_file;
  std::string m_line {};
  int m_lineNb { 0 };
  std::stringstream m_content;

public:
  explicit TextFile(const std::filesystem::path& file);
  // use only std::filesystem::path
  TextFile(auto file) = delete;
  // non copyable
  TextFile(const TextFile&) = delete;
  TextFile(TextFile&&) = delete;
  TextFile& operator=(const TextFile&) = delete;
  TextFile& operator=(TextFile&&) = delete;
  ~TextFile() = default;

  /**
   * Reads the next line of text in the file.
   * @returns true if there is another line to read, false if not.
   */
  /*[[nodiscard]]*/ bool next();

  /**
   * @returns the current line index, the first line being 0.
   */
  [[nodiscard]] int getLineIndex() const noexcept;
  [[nodiscard]] std::string getFileName() const;

  /**
   * @returns the filename without extension.
   */
  [[nodiscard]] std::string getFileStem() const;

  /**
   * @returns the current line of text.
   */
  [[nodiscard]] std::string getLine() const noexcept;
  [[nodiscard]] std::size_t find(std::string_view s) const noexcept;
  [[nodiscard]] std::size_t find(char c) const noexcept;
  [[nodiscard]] bool lineIsEmpty() const noexcept;
  [[nodiscard]] bool startsWith(char c) const noexcept;
  [[nodiscard]] bool startsWith(std::string_view s) const noexcept;
  [[nodiscard]] bool endsWith(std::string_view s) const noexcept;
  [[nodiscard]] bool endsWith(char c) const noexcept;
  [[nodiscard]] bool contains(std::string_view s) const noexcept { return std::string_view::npos != find(s); }
  [[nodiscard]] bool contains(char c) const noexcept { return std::string::npos != find(c); }
  [[nodiscard]] bool containsExact(std::string_view s) const noexcept;
  [[nodiscard]] bool containsOneOf(std::span<const std::string_view> patterns) const;
  /*[[nodiscard]]*/ TextFile& trim(); // can be discarded
}; // class TextFile

module : private;

template<typename T> requires(std::same_as<T, std::filesystem::path>)
[[nodiscard]] inline bool isDir(const T& p) noexcept {
  std::error_code ec;
  return std::filesystem::is_directory(p, ec) and 0 == ec.value();
}

template<typename T> requires(std::same_as<T, std::filesystem::path>)
[[nodiscard]] inline bool isFile(const T& p) noexcept {
  std::error_code ec;
  return std::filesystem::is_regular_file(p, ec) and 0 == ec.value();
}

// use std::filesystem::path as std needs it
std::string readToString(const std::filesystem::path& p) {
  assert((!isDir(p)) and "given a dir instead of a file");
  std::println("{}", p.string());
  assert((isFile(p)) and "given a non existing file");
  std::ifstream in { p };
  // decltype(std::ifstream::gcount()) is std::streamsize, which is signed.
  // std::string constructor takes a std::string::size_type, which is unsigned.
  // we know that std::ifstream::gcount() is always positive
  in.ignore(std::numeric_limits<std::streamsize>::max());
  std::string result(gsl::narrow_cast<std::string::size_type>(in.gcount()), '\0');
  in.seekg(0);
  in.read(result.data(), gsl::narrow_cast<std::streamsize>(result.size()));
  return result;
}

TextFile::TextFile(const std::filesystem::path& file)
  : m_file { file },
    m_content { readToString(file) } {}

bool TextFile::next() {
  const auto ret { !std::getline(m_content, m_line).fail() };

  if (ret) { ++m_lineNb; }

  return ret;
}

template<typename CONTAINER, typename PREDICATE>
[[nodiscard]] constexpr bool containsIf(const CONTAINER& c, PREDICATE p) {
  return std::end(c) != std::find_if(std::begin(c), std::end(c), p);
}

bool TextFile::containsOneOf(std::span<const std::string_view> patterns) const {
  return containsIf(patterns, [this](const auto & s) noexcept { return containsExact(s); });
}

bool isSpace(char c) { return ' ' == c; }

std::string_view myTrim(std::string_view s) {
  s.remove_prefix(gsl::narrow_cast<std::string_view::size_type>(std::distance(s.cbegin(),
                  std::find_if_not(s.cbegin(), s.cend(), isSpace))));
  s.remove_suffix(gsl::narrow_cast<std::string_view::size_type>(std::distance(s.crbegin(),
                  std::find_if_not(s.crbegin(), s.crend(), isSpace))));
  return s;
}

TextFile& TextFile::trim() {
  m_line = myTrim(m_line);
  return *this;
}

bool TextFile::containsExact(std::string_view s) const noexcept {
  const auto pos { m_line.find(s) };

  if (std::string_view::npos == pos) { return false; }

  const auto offset { pos + s.size() };

  // if the token found is at the end of the string then match is always exact
  if (m_line.size() <= offset) { return true; }

  // if the user passed a token ending by a space then match is always exact
  if (' ' == m_line.at(offset - 1)) { return true; }

  // look at the character just after the substring we found
  const auto c { m_line.at(offset) };
  return ('A' > c) or (c > 'z');
}

int TextFile::getLineIndex() const noexcept { return m_lineNb; }
std::string TextFile::getFileName() const { return m_file.string(); }
std::string TextFile::getFileStem() const { return m_file.stem().string(); }
std::string TextFile::getLine() const noexcept { return m_line; }
std::size_t TextFile::find(std::string_view s) const noexcept { return m_line.find(s); }
std::size_t TextFile::find(char c) const noexcept { return m_line.find(c); }
bool TextFile::lineIsEmpty() const noexcept { return m_line.empty(); }
bool TextFile::startsWith(char c) const noexcept { return m_line.starts_with(c); }
bool TextFile::startsWith(std::string_view s) const noexcept { return m_line.starts_with(s); }
bool TextFile::endsWith(std::string_view s) const noexcept { return m_line.ends_with(s); }
bool TextFile::endsWith(char c) const noexcept  { return m_line.ends_with(c); }