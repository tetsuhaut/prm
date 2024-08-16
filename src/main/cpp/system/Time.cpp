module;

#include <ctime> // std::tm

export module system.Time;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop )

export class [[nodiscard]] Time final {
private:
  std::unique_ptr<std::tm> m_pTimeData;
public:
  struct [[nodiscard]] Params final { std::string_view strTime; std::string_view format; };

  explicit Time(const Params& args);
  Time(const Time& other) noexcept;
  explicit Time(Time&& other) noexcept;
  ~Time() = default;
  [[nodiscard]] Time& operator=(const Time& other) noexcept;
  [[nodiscard]] Time& operator=(Time&& other) noexcept;
  [[nodiscard]] bool operator==(const Time& other) const noexcept;
  [[nodiscard]] std::string toSqliteDate() const;
}; // class Time

static constexpr std::string_view SQLITE_DATE_FORMAT { "%Y-%m-%d %H:%M:%S" };

module : private;

[[nodiscard]] std::tm toTm(const Time::Params& args) {
  std::tm when {.tm_sec = 0, .tm_min = 0, .tm_hour = 0, .tm_mday = 0,
                .tm_mon = 0, .tm_year = 0, .tm_wday = 0, .tm_yday = 0,
                .tm_isdst = 0 };
  std::istringstream iss { args.strTime.data() };
  iss >> std::get_time(&when, args.format.data());

  if (iss.fail()) {
    throw std::format("The string '{}' is not a valid time.", args.strTime);
  }

  return when;
}

Time::Time(const Params& args)
  : m_pTimeData { std::make_unique<std::tm>(toTm(args)) } {
  // nothing to do
}

Time::Time(const Time& other) noexcept
  : m_pTimeData { std::make_unique<std::tm>() } {
  *m_pTimeData = *other.m_pTimeData;
}

Time::Time(Time&& other) noexcept
  : m_pTimeData { std::exchange(other.m_pTimeData, {}) } {
  // nothing to do
}

[[nodiscard]] Time& Time::operator=(const Time& other) noexcept {
  if (this != &other) {
    m_pTimeData = std::make_unique<std::tm>();
    *m_pTimeData = *other.m_pTimeData;
  }

  return *this;
}

[[nodiscard]] Time& Time::operator=(Time&& other) noexcept {
  if (this != &other) {
    m_pTimeData = std::exchange(other.m_pTimeData, {});
  }

  return *this;
}

[[nodiscard]] bool Time::operator==(const Time& other) const noexcept {
  return this == &other ? true :
         m_pTimeData->tm_sec == other.m_pTimeData->tm_sec
         and m_pTimeData->tm_min == other.m_pTimeData->tm_min
         and m_pTimeData->tm_hour == other.m_pTimeData->tm_hour
         and m_pTimeData->tm_mday == other.m_pTimeData->tm_mday
         and m_pTimeData->tm_mon == other.m_pTimeData->tm_mon
         and m_pTimeData->tm_year == other.m_pTimeData->tm_year
         and m_pTimeData->tm_wday == other.m_pTimeData->tm_wday
         and m_pTimeData->tm_yday == other.m_pTimeData->tm_yday
         and m_pTimeData->tm_isdst == other.m_pTimeData->tm_isdst;
}

[[nodiscard]] std::string Time::toSqliteDate() const {
  // "2014-10-31 00:45:01"
  std::ostringstream oss;
  oss << std::put_time(m_pTimeData.get(), SQLITE_DATE_FORMAT.data());
  return oss.str();
}