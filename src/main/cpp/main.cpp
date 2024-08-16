import gui.MainWindow;

import std;

[[nodiscard]] int main() {
  MainWindow mw;
  return mw.run();
}

// REVIEW : vérifier que les constexpr sont exportés sinon ça ne compile pas