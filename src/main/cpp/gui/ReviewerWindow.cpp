module;

#if defined(_MSC_VER) // removal of specific msvc warnings due to FLTK
#  pragma warning(push)
#  pragma warning(disable : 4100 4191 4242 4244 4365 4458 4514 4625 4626 4668 4820 5026 5027 )
#elif defined(__MINGW32__) // removal of specific gcc warnings due to FLTK
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-function-type"
#  pragma GCC diagnostic ignored "-Wsuggest-override"
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Weffc++"
#endif  // _MSC_VER

#include <FL/Fl.H> // Fl::event()
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/FL_ask.H> // fl_alert

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <cassert> // assert
#include <format>
#include <functional> // std::function
#include <memory> // std::make_unique
#include <utility> // std::pair
#include <string_view>


export module gui.ReviewerWindow;

import entities.Card;
import entities.Game; // Game, CashGame, Tournament
import entities.Hand;
import entities.Seat; // tableSeat::*
import gui.Preferences;
import language.Map;

export class [[nodiscard]] ReviewerWindow final {
  friend void reviewerWindowCb(Fl_Widget* /*menuBar*/, void* self);
private:
  Preferences& m_preferences;
  std::unique_ptr<Fl_Double_Window> m_window;
  std::function<void()> m_closeNotifier;

public:
  ReviewerWindow(Preferences& p, std::string_view label, std::function<void()> closeNotifier,
                 std::string_view hero, const Hand& hand);
  ReviewerWindow(const ReviewerWindow&) = delete;
  ReviewerWindow& operator=(const ReviewerWindow& t) = delete;
  ~ReviewerWindow();
}; // export class ReviewerWindow

module : private;

[[nodiscard]] ReviewerWindow* REVIEWER_WINDOW(void* self) {
  return static_cast<ReviewerWindow*>(self);
}

std::unique_ptr<Fl_Double_Window> buildWindow(const Preferences& preferences,
    std::string_view label) {
  const auto [localX, localY, width, height] { preferences.getGameWindowXYWH() };
  return std::make_unique<Fl_Double_Window>(
           localX, localY, width, height, label.data());
}

void reviewerWindowCb(Fl_Widget* /*menuBar*/, void* self) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  REVIEWER_WINDOW(self)->m_closeNotifier();
}

[[nodiscard]] std::string getSystemError() {
  char ret[256] { '\0' };
  strerror_s(ret, std::size(ret), errno);
  return ret;
}

[[nodiscard]] std::unique_ptr<Fl_GIF_Image> loadImage(std::string_view imageName,
    std::tuple<unsigned char*, unsigned int> imageData) {
  auto [data, size] {imageData};
  auto image { std::make_unique<Fl_GIF_Image>(imageName.data(), data, size)};

  switch (image->fail()) {
    case Fl_Image::ERR_NO_IMAGE:
      [[fallthrough]];

    case Fl_Image::ERR_FILE_ACCESS:
      fl_alert(std::format("{}: couldn't access image data from {}",
                           getSystemError().c_str(), imageName).c_str());
      return nullptr;

    case Fl_Image::ERR_FORMAT:
      fl_alert("%s: couldn't decode image", getSystemError().c_str());
      return nullptr;

    default:
      return image;
  }
}

[[nodiscard]] std::unique_ptr<Fl_Box> toCardBox(Card card) {
  auto image { loadImage(toString(card), cardImages::getImage(card))};
  auto box { std::make_unique<Fl_Box>(0, 0, image->w(), image->h()) };
  box->image(image.release());
  return box;
}

using PrivateHand = std::pair<Card, Card>;
using Point = std::pair<int, int>;

static constexpr auto ZERO { std::make_pair<double, double>(-1, -1) };

// TODO: mauvaises positions pour 3
/* The position of seats, if the window size is 1 x 1 */
static constexpr auto NB_SEATS_TO_COEFF = language::Map<Seat, std::array<std::pair<double, double>, 10>, 9> { {{
  { Seat::seatTwo,  { std::make_pair<double, double>(0.5, 0),        std::make_pair<double, double>(0.5, 1.0),      ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatThree,  { std::make_pair<double, double>(0.3333, 0.3333), std::make_pair<double, double>(0.6666, 0.3333), std::make_pair<double, double>(0.5, 1),      ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatFour,  { std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.75, 0.25),    std::make_pair<double, double>(0.75, 0.75),  std::make_pair<double, double>(0.25, 0.75),  ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatFive,  { std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.75, 0.25),    std::make_pair<double, double>(0.75, 0.75),  std::make_pair<double, double>(0.5, 1),      std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatSix,  { std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.5, 0),        std::make_pair<double, double>(0.75, 0.25),  std::make_pair<double, double>(0.75, 0.75),  std::make_pair<double, double>(0.5, 1),     std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatSeven,  { std::make_pair<double, double>(0, 0.5),        std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.75, 0.25),  std::make_pair<double, double>(1, 0.5),     std::make_pair<double, double>(0.75, 0.75), std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO, ZERO } },
  { Seat::seatEight,  { std::make_pair<double, double>(0, 0.5),        std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.75, 0.25),  std::make_pair<double, double>(1, 0.5),     std::make_pair<double, double>(0.75, 0.75), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO } },
  { Seat::seatNine,  { std::make_pair<double, double>(0.125, 0.35),   std::make_pair<double, double>(0.35, 0.125),   std::make_pair<double, double>(0.625, 0.125), std::make_pair<double, double>(0.85, 0.35),  std::make_pair<double, double>(0.85, 0.635), std::make_pair<double, double>(0.625, 0.85), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.35, 0.85), std::make_pair<double, double>(0.125, 0.635), ZERO } },
  { Seat::seatTen, { std::make_pair<double, double>(0.125, 0.35),   std::make_pair<double, double>(0.35, 0.125),   std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.625, 0.125), std::make_pair<double, double>(0.85, 0.35), std::make_pair<double, double>(0.85, 0.635), std::make_pair<double, double>(0.625, 0.85), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.35, 0.85), std::make_pair<double, double>(0.125, 0.635) } }
}}};

[[nodiscard]] Point getCardsPosition(const Point& wh, Seat seat, Seat tableMaxSeats) {
  const auto& [w, h] { wh };
  const auto positions { NB_SEATS_TO_COEFF.at(tableMaxSeats) };
  const auto [coefX, coefY] { positions.at(tableSeat::toArrayIndex(seat)) };
  assert(coefX != -1 && coefY != -1);
  return { static_cast<int>(coefX* w * 0.85), static_cast<int>(coefY* h * 0.75) };
}

[[nodiscard]] PrivateHand getCards(std::string_view player, const Hand& hand,
                                   std::string_view hero) {
  if (player.empty()) { return std::make_pair(Card::none, Card::none); }

  if (player == hero) { return std::make_pair(hand.getHeroCard1(), hand.getHeroCard2()); }

  return std::make_pair(Card::back, Card::back);
}

void drawPlayButtonBar(const Point& wh) {
  // TODO
}

// +----> x
// |
// |
// v
// y
void drawCards(const Point& wh, const Hand& hand, std::string_view hero) {
  const auto& seatPlayers { hand.getSeats() };

  for (Seat seat : {
         Seat::seatOne, Seat::seatTwo, Seat::seatThree, Seat::seatFour, Seat::seatFive, Seat::seatSix,
         Seat::seatSeven,
         Seat::seatEight, Seat::seatNine, Seat::seatTen
       }) {
    if (const auto & entry { seatPlayers.find(seat) }; entry != seatPlayers.end()) {
      const auto& player { entry->second };
      const auto [card1, card2] { getCards(player, hand, hero) };
      const auto [card1X, card1Y] { getCardsPosition(wh, seat, hand.getMaxSeats()) };
      auto box1 = toCardBox(card1).release();
      box1->position(card1X, card1Y);
      auto box2 = toCardBox(card2).release();
      box2->position(box1->x() + box1->w(), box1->y());
    }
  }
}

void drawTable() {
  // TODO: dessiner un genre d'ovale
}

ReviewerWindow::ReviewerWindow(Preferences& p, std::string_view label,
                               std::function<void()> closeNotifier,
                               std::string_view hero,
                               const Hand& hand)
  : m_preferences { p },
    m_window { buildWindow(p, label) },
    m_closeNotifier { closeNotifier } {
  m_window->callback(reviewerWindowCb, this);
  const auto wh { std::make_pair(m_window->w(), m_window->h())};
  drawTable();
  drawCards(wh, hand, hero);
  drawPlayButtonBar(wh);
  m_window->end();
  m_window->show();
}

ReviewerWindow::~ReviewerWindow() {
  std::array xywh {m_window->x(), m_window->y(), m_window->w(), m_window->h()};
  m_preferences.saveSizeAndPosition(xywh, Preferences::PrefName::reviewerWindow);
}