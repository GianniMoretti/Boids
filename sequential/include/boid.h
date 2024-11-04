#ifndef BOID_DATA_H
#define BOID_DATA_H

#include <SFML/System.hpp>

struct BoidData {
    sf::Vector2f position;
    sf::Vector2f velocity;

    BoidData(float x, float y, float vx, float vy) : position(x, y), velocity(vx, vy) {}
};

#endif // BOID_DATA_H

// shape.setPoint(0, sf::Vector2f(0, -10));   // Punta (testa)
// shape.setPoint(1, sf::Vector2f(-8, 8));    // Ala sinistra
// shape.setPoint(2, sf::Vector2f(-4, 4));    // Corpo sinistro
// shape.setPoint(3, sf::Vector2f(4, 4));     // Corpo destro
// shape.setPoint(4, sf::Vector2f(8, 8));     // Ala destra