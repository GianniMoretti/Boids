#ifndef BOID_RENDERER_H
#define BOID_RENDERER_H

#include <SFML/Graphics.hpp>
#include "boid.h"

class BoidRenderer {
public:
    BoidRenderer(float scale = 1.0f);
    void draw(sf::RenderWindow &window, const BoidData &boidData);

private:
    sf::ConvexShape shape;
    void updateShapeRotation(const sf::Vector2f &velocity);
};

#endif // BOID_RENDERER_H