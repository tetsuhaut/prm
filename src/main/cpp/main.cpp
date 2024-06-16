#include <compare>
import gui.MainWindow;

[[nodiscard]] int main() {
  MainWindow mw;
  return mw.run();
}

// TODO : vérifier que les constexpr sont exportés sinon ça ne compile pas