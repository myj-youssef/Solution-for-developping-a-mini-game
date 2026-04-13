#include <exception>
#include <iostream>

#include "Player.hpp"
int main() {
  try {
    Game game;
    game.run();
  } catch (const std::exception& e) {
    std::cerr << "Erreur fatale: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}