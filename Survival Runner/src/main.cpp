#include <SFML/Graphics.hpp>  // Inclusion de l'interface SFML

int main() {
  // 1. Création de la fenêtre (Instance de la classe sf::RenderWindow)
  sf::RenderWindow window(sf::VideoMode(800, 600),
                          "Survival Runner - Milestone 1");

  // 2. La Game Loop
  while (window.isOpen()) {
    // Gestion des événements (Input)
    sf::Event event;
    while (window.pollEvent(event)) {
      // Si on clique sur la croix, on ferme la fenêtre
      if (event.type == sf::Event::Closed) window.close();
    }

    // 3. Rendu (Dessin)
    window.clear(sf::Color::Black);  // On efface l'écran avec du noir

    // C'est ici que nous dessinerons nos objets plus tard

    window.display();  // On affiche ce qui a été dessiné
  }

  return 0;  // Succès
}