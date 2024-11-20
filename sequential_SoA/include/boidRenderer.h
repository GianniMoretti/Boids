#ifndef BOID_RENDERER_H
#define BOID_RENDERER_H

#include <SFML/Graphics.hpp>
#include "boid.h"

class BoidRenderer {
public:
    BoidRenderer(float scale = 1.0f);
    void draw(sf::RenderWindow &window, float posx, float posy, float velx, float vely);

private:
    sf::ConvexShape shape;
};

#endif // BOID_RENDERER_H