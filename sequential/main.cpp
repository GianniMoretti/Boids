#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "include/boid.h"
#include "include/boidRenderer.h"

int windows_width = 1200;
int windows_height = 1000;
float boids_scale = 0.5f;
int boids_number = 50;
int windows_frame_rate = 10;

//Potrebbero essere delle costanti?
//Tunneble parameters
float visualRange = 40;
float protectedRange = 10;
float avoidfactor = 0.15f;
float matchingfactor = 0.1f;
float centeringfactor = 0.003f;
int Margin = 300;                 // For now all the margin are equal
float turnfactorx = 0.25f;
float turnfactory = 0.15f;
float maxspeed = 6;
float minspeed = 3;
//Biasval (comment for constant bias version)
float maxbias = 0.0005f;
float biasincrement = 0.00004f;
//uncomment for constant bias version
// float maxbias = 0.001f;
// float biasincrement = 0.001f;

//Crea la versione che vede del cibo e lo segue

int main() {
    sf::RenderWindow window(sf::VideoMode(windows_width, windows_height), "Boids Simulation");
    window.setFramerateLimit(windows_frame_rate);

    // Vettore di dati dei boids
    std::vector<BoidData> boidDataList;  //Potrebbe avere senso farlo diventare dei vettori per ogni variabile
    BoidRenderer renderer(boids_scale);

    // Boids creation
    for (int i = 0; i < boids_number; ++i) {
        float x = static_cast<float>(rand() % (windows_width - Margin));
        float y = static_cast<float>(rand() % (windows_height - Margin));
        float vx = static_cast<float>(rand() % 3 - 1);
        float vy = static_cast<float>(rand() % 3 - 1);
        int scG = rand() % 3;
        boidDataList.emplace_back(x, y, vx, vy, biasincrement, scG);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Boids Logic
        //Loop over all the boids
        for (int i = 0; i < boidDataList.size(); i++) {       
            //Reference Boid
            BoidData refBoid = boidDataList[i];
            //Vector of the boid's state
            //The variable are:
            //0:xpos_avg, 1:ypos_avg, 2:xvel_avg, 3:yvel_avg, 4:neighboring_boids, 5:close_dx, 6:close_dy
            float boidState[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            //Loop on all otherbirds less the reference boids
            for (int j = 0; j < boidDataList.size(); j++) {
                if (i == j) {
                    continue;  // Skip if we're considering the same boid
                }
                BoidData boid = boidDataList[j];

                //Calculate euclidean distance
                float dx = refBoid.position.x - boid.position.x;
                float dy = refBoid.position.y - boid.position.y;
                float distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
                if(distance < protectedRange){
                    boidState[5] += dx;
                    boidState[6] += dy;
                }
                else if (distance < visualRange){
                    boidState[0] += boid.position.x;
                    boidState[1] += boid.position.y;
                    boidState[2] += boid.velocity.x;
                    boidState[3] += boid.velocity.y;
                    boidState[4] += 1;
                }
            }

            if(boidState[4] > 0){   // if there are neighboar boids

                // Mean of the comulated neighbor boids
                boidState[0] /= boidState[4];
                boidState[1] /= boidState[4];
                boidState[2] /= boidState[4];
                boidState[3] /= boidState[4];

                //Centering/Matching contibution contribution
                refBoid.velocity.x = refBoid.velocity.x + (boidState[0] - refBoid.position.x) * centeringfactor + (boidState[2] - refBoid.velocity.x) * matchingfactor;
                refBoid.velocity.y = refBoid.velocity.y + (boidState[1] - refBoid.position.y) * centeringfactor + (boidState[3] - refBoid.velocity.y) * matchingfactor;
            }

            //Add the avoidance contribution to velocity
            refBoid.velocity.x += (boidState[5] * avoidfactor);
            refBoid.velocity.y += (boidState[6] * avoidfactor);

            //Windows margin controll
            if(refBoid.position.x < Margin)
                refBoid.velocity.x += turnfactorx;
            if(refBoid.position.x > windows_width - Margin)
                refBoid.velocity.x -= turnfactorx;
            if(refBoid.position.y < Margin)
                refBoid.velocity.y += turnfactory;
            if(refBoid.position.y > windows_height - Margin)
                refBoid.velocity.y -= turnfactory;

            //update the biasval
            if(refBoid.scoutGroup == 1){  //he wants to go to the right side
                if(refBoid.velocity.x > 0){
                    refBoid.biasval = std::max(maxbias, refBoid.biasval + biasincrement);
                }
                else{
                    refBoid.biasval = std::min(biasincrement, refBoid.biasval - biasincrement);
                }
                refBoid.velocity.x = (1 - refBoid.biasval)* refBoid.velocity.x + (refBoid.biasval * 1);
            }
            if(refBoid.scoutGroup == 2){  //he wants to go to the left side   
                if(refBoid.velocity.x < 0){
                    refBoid.biasval = std::max(maxbias, refBoid.biasval + biasincrement);
                }
                else{
                    refBoid.biasval = std::min(biasincrement, refBoid.biasval - biasincrement);
                }
                refBoid.velocity.x = (1 - refBoid.biasval)* refBoid.velocity.x + (refBoid.biasval * (-1));
            }
            
            float refBoidSpeed = std::sqrt(std::pow(refBoid.velocity.x, 2) + std::pow(refBoid.velocity.y, 2));

            if (refBoidSpeed < minspeed){
                refBoid.velocity.x = (refBoid.velocity.x / refBoidSpeed) * minspeed;
                refBoid.velocity.y = (refBoid.velocity.y / refBoidSpeed) * minspeed;
            }
            else if (refBoidSpeed > maxspeed){
                refBoid.velocity.x = (refBoid.velocity.x / refBoidSpeed) * maxspeed;
                refBoid.velocity.y = (refBoid.velocity.y / refBoidSpeed) * maxspeed;
            }
            refBoid.position.x += refBoid.velocity.x;
            refBoid.position.y += refBoid.velocity.y;

            // Border limit controll
            if(refBoid.position.x < 0)
                refBoid.position.x = 0;
            else if(refBoid.position.x > windows_width) 
                refBoid.position.x = windows_width;
            
            if (refBoid.position.y < 0)
                refBoid.position.y = 0;
            else if (refBoid.position.y > windows_height) 
                refBoid.position.y = windows_height;

            boidDataList[i] = refBoid;
        }

        window.clear();
        for (const auto &boid : boidDataList) {
            renderer.draw(window, boid); // Disegna ogni boid con il renderer
        }
        window.display();
    }
    return 0;
}