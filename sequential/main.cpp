#include <SFML/Graphics.hpp>
#include "include/boid.h"
#include "include/boidRenderer.h"
#include <vector>
#include <cstdlib>

int windows_width = 1000;
int windows_height = 800;
float boids_scale = 0.5f;
int boids_number = 50;
int windows_frame_rate = 60;

int main() {
    sf::RenderWindow window(sf::VideoMode(windows_width, windows_height), "Boids Simulation");
    window.setFramerateLimit(windows_frame_rate);

    // Vettore di dati dei boids
    std::vector<BoidData> boidDataList;
    BoidRenderer renderer(boids_scale);

    // Inizializza i boids
    for (int i = 0; i < boids_number; ++i) {
        float x = static_cast<float>(rand() % windows_width);
        float y = static_cast<float>(rand() % windows_height);
        float vx = static_cast<float>(rand() % 3 - 1);
        float vy = static_cast<float>(rand() % 3 - 1);
        boidDataList.emplace_back(x, y, vx, vy);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Aggiornamento dei dati di posizione (logica semplificata)
        for (auto &boid : boidDataList) {
            boid.position += boid.velocity;
            
            // Controlla i bordi per un semplice rimbalzo
            if (boid.position.x < 0 || boid.position.x > windows_width) 
                boid.velocity.x = -boid.velocity.x;
            if (boid.position.y < 0 || boid.position.y > windows_height) 
                boid.velocity.y = -boid.velocity.y;
        }

        window.clear();
        for (const auto &boid : boidDataList) {
            renderer.draw(window, boid); // Disegna ogni boid con il renderer
        }
        window.display();

    }

    return 0;
}