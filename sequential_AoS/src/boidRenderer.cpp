#include "../include/boidRenderer.h"
#include <cmath>

// Costruttore: inizializza la forma del boid
BoidRenderer::BoidRenderer(float scale) : shape(18) {
    shape.setPoint(0, sf::Vector2f(0, -10));  
    shape.setPoint(1, sf::Vector2f(-2, -8));  
    shape.setPoint(2, sf::Vector2f(-2, -6));  
    shape.setPoint(3, sf::Vector2f(-4, -4));   
    shape.setPoint(4, sf::Vector2f(-12, 0));
    shape.setPoint(5, sf::Vector2f(-16, 10));
    shape.setPoint(6, sf::Vector2f(-7, 2));
    shape.setPoint(7, sf::Vector2f(-2, 2));       
    shape.setPoint(8, sf::Vector2f(-8, 18)); 
    shape.setPoint(9, sf::Vector2f(0, 10));   
    shape.setPoint(10, sf::Vector2f(8, 18));      
    shape.setPoint(11, sf::Vector2f(2, 2));     
    shape.setPoint(12, sf::Vector2f(7, 2));    
    shape.setPoint(13, sf::Vector2f(16, 10));
    shape.setPoint(14, sf::Vector2f(12, 0));  
    shape.setPoint(15, sf::Vector2f(4, -4));   
    shape.setPoint(16, sf::Vector2f(2, -6));    
    shape.setPoint(17, sf::Vector2f(2, -8));     
    shape.setFillColor(sf::Color::Blue);
    shape.setScale(scale, scale);
}

// Disegna il boid alla posizione specificata in `boidData`
void BoidRenderer::draw(sf::RenderWindow &window, const BoidData &boidData) {
    shape.setPosition(boidData.position);
    updateShapeRotation(boidData.velocity);
    window.draw(shape);
}

// Aggiorna la rotazione della forma per allinearla alla direzione della velocità
void BoidRenderer::updateShapeRotation(const sf::Vector2f &velocity) {
    float angle = std::atan2(velocity.y, velocity.x) * 180 / M_PI;
    shape.setRotation(angle + 90); // +90 per allineare la punta alla direzione
}