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
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/FL_ask.H> // fl_alert

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <cassert> // assert

export module gui.ReviewerWindow;

import entities.Card;
import entities.Game; // Game, CashGame, Tournament
import entities.Hand;
import entities.Seat; // tableSeat::*
import gui.dimensions; // button size
import gui.Labels;
import gui.Preferences;
import language.Map;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop ) 

export class [[nodiscard]] ReviewerWindow final {
  friend static void reviewerWindowCb(Fl_Widget*, void* self);
  friend static void nextHandCallback(Fl_Widget*, void* self);
private:
  Fl_Double_Window m_window;
  std::function<void()> m_closeNotifier;
  std::vector<const Hand*> m_hands;
  std::string m_hero;
  Preferences& m_preferences;
  const Hand* m_pCurrentHand;

public:
  ReviewerWindow(Preferences& p, std::string_view label, std::function<void()> closeNotifier,
                 std::string_view hero, const std::vector<const Hand*>& hands);
  ReviewerWindow(const ReviewerWindow&) = delete;
  ReviewerWindow& operator=(const ReviewerWindow& t) = delete;
  ~ReviewerWindow();
  bool nextHand();
}; // export class ReviewerWindow

module : private;

Fl_Double_Window buildWindow(const Preferences& preferences,
  std::string_view label) {
  const auto [localX, localY, width, height] { preferences.getGameWindowXYWH() };
  return Fl_Double_Window(localX, localY, width, height, label.data());
}

static void reviewerWindowCb(Fl_Widget*, void* self) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() and FL_Escape == Fl::event_key()) { return; }

  static_cast<ReviewerWindow*>(self)->m_closeNotifier();
}

[[nodiscard]] std::string getSystemError() {
  char ret[512] { '\0' };
  strerror_s(ret, std::size(ret), errno);
  return ret;
}

[[nodiscard]] Fl_GIF_Image* loadImage(std::string_view imageName,
    std::tuple<unsigned char*, unsigned int> imageData) {
  const auto [data, size] {imageData};
  auto image { new Fl_GIF_Image(imageName.data(), data, size) };

  switch (image->fail()) {
    case Fl_Image::ERR_NO_IMAGE:
      fl_alert(std::format("{}: couldn't find image data from {}",
        getSystemError(), imageName).c_str());
      return nullptr;

    case Fl_Image::ERR_FILE_ACCESS:
      fl_alert(std::format("{}: couldn't access image data from {}",
                           getSystemError(), imageName).c_str());
      return nullptr;

    case Fl_Image::ERR_FORMAT:
      fl_alert(std::format("{}: couldn't decode image", getSystemError()).c_str());
      return nullptr;

    case Fl_Image::ERR_MEMORY_ACCESS:
      fl_alert(std::format("{}: image decoder tried to access memory outside of given memory block",
        getSystemError()).c_str());
      return nullptr;

    case 0:
      return image;
  }
  std::unreachable();
}

[[nodiscard]] Fl_Box* toCardBox(Card card) {
  auto image { loadImage(toString(card), cardImages::getImage(card))};
  auto box { new Fl_Box(0, 0, image->w(), image->h()) };
  box->image(image);
  return box;
}

using Point = std::pair<int, int>;

static constexpr auto ZERO { std::make_pair<double, double>(-1, -1) };
// +----> x
// |
// v
// y
// 
// TODO: mauvaises positions pour 3
/* The position of seats, if the window size is 1 x 1 */
static constexpr auto NB_SEATS_TO_COEFF = language::Map<Seat, std::array<std::pair<double, double>, 10>, 9> { {{
  { Seat::seatTwo,  { std::make_pair<double, double>(0.5, 0),        std::make_pair<double, double>(0.5, 1.0),      ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatThree,  { std::make_pair<double, double>(0.3333, 0.3333), std::make_pair<double, double>(0.6666, 0.3333), std::make_pair<double, double>(0.5, 1),      ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatFour,  { std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.75, 0.25),    std::make_pair<double, double>(0.75, 0.75),  std::make_pair<double, double>(0.25, 0.75),  ZERO, ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatFive,  { std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.75, 0.25),    std::make_pair<double, double>(0.75, 0.75),  std::make_pair<double, double>(0.5, 1),      std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatSix,  { std::make_pair<double, double>(0, 0.25),    std::make_pair<double, double>(0.5, 0),        std::make_pair<double, double>(1, 0.25),  std::make_pair<double, double>(1, 0.75),  std::make_pair<double, double>(0.5, 1),     std::make_pair<double, double>(0, 0.75), ZERO, ZERO, ZERO, ZERO } },
  { Seat::seatSeven,  { std::make_pair<double, double>(0, 0.5),        std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.75, 0.25),  std::make_pair<double, double>(1, 0.5),     std::make_pair<double, double>(0.75, 0.75), std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO, ZERO } },
  { Seat::seatEight,  { std::make_pair<double, double>(0, 0.5),        std::make_pair<double, double>(0.25, 0.25),    std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.75, 0.25),  std::make_pair<double, double>(1, 0.5),     std::make_pair<double, double>(0.75, 0.75), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.25, 0.75), ZERO, ZERO } },
  { Seat::seatNine,  { std::make_pair<double, double>(0.125, 0.35),   std::make_pair<double, double>(0.35, 0.125),   std::make_pair<double, double>(0.625, 0.125), std::make_pair<double, double>(0.85, 0.35),  std::make_pair<double, double>(0.85, 0.635), std::make_pair<double, double>(0.625, 0.85), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.35, 0.85), std::make_pair<double, double>(0.125, 0.635), ZERO } },
  { Seat::seatTen, { std::make_pair<double, double>(0.125, 0.35),   std::make_pair<double, double>(0.35, 0.125),   std::make_pair<double, double>(0.5, 0),      std::make_pair<double, double>(0.625, 0.125), std::make_pair<double, double>(0.85, 0.35), std::make_pair<double, double>(0.85, 0.635), std::make_pair<double, double>(0.625, 0.85), std::make_pair<double, double>(0.5, 1), std::make_pair<double, double>(0.35, 0.85), std::make_pair<double, double>(0.125, 0.635) } }
}}};

[[nodiscard]] Point getCardsPosition(int w, int h, Seat seat, Seat tableMaxSeats) {
  const auto positions { NB_SEATS_TO_COEFF.at(tableMaxSeats) };
  const auto [coefX, coefY] { positions.at(tableSeat::toArrayIndex(seat)) };
  assert(coefX != -1 and coefY != -1);
  return { static_cast<int>(coefX* w * 0.85), static_cast<int>(coefY* h * 0.75) };
}

[[nodiscard]] std::pair<Card, Card> getCards(std::string_view player, const Hand& hand,
                                   std::string_view hero) {
  if (player.empty()) { return std::make_pair(Card::none, Card::none); }

  if (player == hero) { return std::make_pair(hand.getHeroCard1(), hand.getHeroCard2()); }

  return std::make_pair(Card::back, Card::back);
}

// +----> x
// |
// |
// v
// y
static void drawCards(int w, int h, const Hand& hand, std::string_view hero) {
  static constexpr std::array seats {
         Seat::seatOne, Seat::seatTwo, Seat::seatThree, Seat::seatFour, Seat::seatFive, Seat::seatSix,
         Seat::seatSeven,
         Seat::seatEight, Seat::seatNine, Seat::seatTen
  };
  const auto& seatPlayers { hand.getSeats() };

  std::ranges::for_each(seats, [&](const auto seat) {
    if (const auto& entry { seatPlayers.find(seat) }; entry != seatPlayers.end()) {
      const auto& player { entry->second };
      const auto [card1, card2] { getCards(player, hand, hero) };
      auto box1 { toCardBox(card1) };
      auto box2 { toCardBox(card2) };
      const auto [card1X, card1Y] { getCardsPosition(w, h, seat, hand.getMaxSeats()) };
      box1->position(card1X, card1Y);
      box2->position(box1->x() + box1->w(), box1->y());
    }
    });
}

static void drawTable() {
  // TODO: dessiner un genre d'ovale
}

static void previousHandCallback(Fl_Widget*, void* self) {

}

static void previousActionCallback(Fl_Widget*, void* self) {

}

static void playCallback(Fl_Widget*, void* self) {

}

static void pauseCallback(Fl_Widget*, void* self) {

}

static void nextActionCallback(Fl_Widget*, void* self) {

}

struct Data {
  int w;
  int h;
  const Hand* currentHand;
  std::string hero;
};

static void nextHandCallback(Fl_Widget*, void* self) {
  if (auto pThis { static_cast<ReviewerWindow*>(self) }; pThis->nextHand()) {
    auto awaitCb = [](void* hiddenData) {
      auto data { std::unique_ptr<Data>(static_cast<Data*>(hiddenData)) };
      drawTable();
      drawCards(data->w, data->h, *data->currentHand, data->hero);
    };
    auto data = new Data();
    data->w = pThis->m_window.w();
    data->h = pThis->m_window.h();
    data->currentHand = pThis->m_pCurrentHand;
    data->hero = pThis->m_hero;
    Fl::awake(awaitCb, data);
  }
}

static inline Fl_Button* buildButton(int x, int y, std::string_view label, Fl_Callback cb, void* cbData) {
  using namespace dimensions;
  auto ret { new Fl_Button(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, label.data()) };
  ret->callback(cb, cbData);
  return ret;
}

// +----> x
// | cards/bets
// | |</<</play/pause/>>/>|
// |
// v
// y
void drawPlayButtonBar(const int bottom, const int width, ReviewerWindow* self) {
  using namespace dimensions;
  const auto y { bottom - BUTTON_HEIGHT - SPACE };
  const auto x { static_cast<int>(0.5 * (static_cast<double>(width) - 6.0 * static_cast<double>(BUTTON_WIDTH))) };
  auto previousHand { buildButton(x, y, labels::PREVIOUS_HAND_LABEL, previousHandCallback, self) };
  auto previousAction { buildButton(previousHand->x() + BUTTON_WIDTH, y, labels::PREVIOUS_ACTION_LABEL, previousActionCallback, self) };
  auto play { buildButton(previousAction->x() + BUTTON_WIDTH, y, labels::PLAY_LABEL, playCallback, self) };
  auto pause { buildButton(play->x() + BUTTON_WIDTH, y, labels::PAUSE_LABEL, pauseCallback, self) };
  auto nextAction { buildButton(pause->x() + BUTTON_WIDTH, y, labels::NEXT_ACTION_LABEL, nextActionCallback, self) };
  auto nextHand { buildButton(nextAction->x() + BUTTON_WIDTH, y, labels::NEXT_HAND_LABEL, nextHandCallback, self) };
  std::ignore = nextHand;
}

// +----> x
// | cards/bets
// | |</<</play/pause/>>/>|
// |
// v
// y
ReviewerWindow::ReviewerWindow(Preferences& p, std::string_view label,
                               std::function<void()> closeNotifier,
                               std::string_view hero,
                               const std::vector<const Hand*>& hands)
  : m_window { buildWindow(p, label) },
    m_closeNotifier { closeNotifier },
    m_hands { hands },
    m_hero { hero },
    m_preferences { p },
  m_pCurrentHand { m_hands[0] } {
  m_window.callback(reviewerWindowCb, this);
  drawTable();
  drawCards(m_window.w(), m_window.h(), *m_pCurrentHand, m_hero);
  drawPlayButtonBar(m_window.h(), m_window.w(), this);
  m_window.end();
  m_window.show();
}

ReviewerWindow::~ReviewerWindow() {
  const std::array xywh {m_window.x(), m_window.y(), m_window.w(), m_window.h()};
  m_preferences.saveGameReviewWindowSizeAndPosition(xywh);
}

// return false if there is no other hand, else return true and make m_currentHand point to the next hand
bool ReviewerWindow::nextHand() {
  const auto nextHandExists { m_pCurrentHand != m_hands.back() };
  m_pCurrentHand = nextHandExists ? m_pCurrentHand + 1 : nullptr;
  return nextHandExists;
}