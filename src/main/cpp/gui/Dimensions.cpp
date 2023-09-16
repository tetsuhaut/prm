module;

export module gui.Dimensions;

export namespace dimensions {
constexpr int BUTTON_HEIGHT = 25;
constexpr int BUTTON_WIDTH = 120;
}

export struct [[nodiscard]] Dimension final {
  int x;
  int y;
  int w;
  int h;
};