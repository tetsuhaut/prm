module;

export module gui.Labels;

import std;

export namespace labels {
  constexpr std::string_view OPEN_THE_REVIEW_LABEL { "Open the review" };
  constexpr std::string_view PREVIOUS_HAND_LABEL { "@|<" };
  constexpr std::string_view PREVIOUS_ACTION_LABEL { "@<<" };
  constexpr std::string_view NEXT_HAND_LABEL { "@>|" };
  constexpr std::string_view NEXT_ACTION_LABEL { "@>>" };
  constexpr std::string_view PAUSE_LABEL { "||" };
  constexpr std::string_view PLAY_LABEL { "@>" };
} // namespace labels