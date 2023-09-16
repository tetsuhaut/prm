#include <compare>
import gui.MainWindow;

[[nodiscard]] int main() {
  return MainWindow().run();
}

// TODO : vérifier que les constexpr sont exportés sinon ça ne compile pas