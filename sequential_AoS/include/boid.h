#ifndef BOID_DATA_H
#define BOID_DATA_H

#include <SFML/System.hpp>

struct BoidData {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float biasval = 0.0f;
    int scoutGroup = 0;

    BoidData(float x, float y, float vx, float vy, float biasval, int scoutGroup = 0) : position(x, y), velocity(vx, vy), biasval(biasval), scoutGroup(scoutGroup) {}
};

#endif // BOID_DATA_H

// shape.setPoint(0, sf::Vector2f(0, -10));   // Punta (testa)
// shape.setPoint(1, sf::Vector2f(-8, 8));    // Ala sinistra
// shape.setPoint(2, sf::Vector2f(-4, 4));    // Corpo sinistro
// shape.setPoint(3, sf::Vector2f(4, 4));     // Corpo destro
// shape.setPoint(4, sf::Vector2f(8, 8));     // Ala destra