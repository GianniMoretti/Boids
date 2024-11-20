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
int boids_number = 300;
int windows_frame_rate = 60;
bool isGraphicsOn = true;

//Potrebbero essere delle costanti?
//Tunneble parameters
float visualRange = 40;
float protectedRange = 10;
float avoidfactor = 0.15f;
float matchingfactor = 0.1f;
float centeringfactor = 0.003f;
int Margin = 300;                       // Same margin for all side
float turnfactorx = 0.15f;
float turnfactory = 0.15f;
float maxspeed = 6;
float minspeed = 3;
//Biasval
float maxbias = 0.001f;
float biasincrement = 0.00004f;

int main() {
    //Data Boid Vector
    BoidDataList boidDataList;  //SoA vs AoS?
    boidDataList.reserve(boids_number);
    //Data tmp Boid vector
    BoidDataList boidDataList_tmp;
    boidDataList_tmp.reserve(boids_number);

    // Boids creation
    for (int i = 0; i < boids_number; ++i) {
        float x = static_cast<float>((rand() % (windows_width - Margin)) + Margin / 2);
        float y = static_cast<float>((rand() % (windows_height - Margin)) + Margin / 2);
        float vx = static_cast<float>(rand() % 3 - 1);
        float vy = static_cast<float>(rand() % 3 - 1);
        int scG = rand() % 3;
        boidDataList.addBoid(x, y, vx, vy, biasincrement, scG);
    }

    BoidRenderer renderer(boids_scale);

    sf::RenderWindow window(sf::VideoMode(windows_width, windows_height), "Boids Simulation");
    window.setFramerateLimit(windows_frame_rate);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Boids Logic
        //Loop over all the boids
        for (int i = 0; i < boidDataList.size(); i++) {
            float tmp_pos_x = boidDataList.xPos[i];
            float tmp_pos_y = boidDataList.yPos[i];
            float tmp_vel_x = boidDataList.xVelocity[i];
            float tmp_vel_y = boidDataList.yVelocity[i];
            float tmp_biasval = boidDataList.biasvals[i];

            //Vector of the boid's state
            //The variable are:
            //0:xpos_avg, 1:ypos_avg, 2:xvel_avg, 3:yvel_avg, 4:neighboring_boids, 5:close_dx, 6:close_dy
            float boidState[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            //Loop on all otherbirds less the reference boids
            for (int j = 0; j < boidDataList.size(); j++) {
                if (i == j) {
                    continue;  // Skip if we're considering the same boid
                }
                //Calculate euclidean distance
                float dx = tmp_pos_x - boidDataList.xPos[j];
                float dy = tmp_pos_y - boidDataList.yPos[j];
                float distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
                if(distance < protectedRange){
                    boidState[5] += dx;
                    boidState[6] += dy;
                }
                else if (distance < visualRange){
                    boidState[0] += boidDataList.xPos[j];
                    boidState[1] += boidDataList.yPos[j];
                    boidState[2] += boidDataList.xVelocity[j];
                    boidState[3] += boidDataList.yVelocity[j];
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
                tmp_vel_x = tmp_vel_x + (boidState[0] - tmp_pos_x) * centeringfactor + (boidState[2] - tmp_vel_x) * matchingfactor;
                tmp_vel_y = tmp_vel_y + (boidState[1] - tmp_pos_y) * centeringfactor + (boidState[3] - tmp_vel_y) * matchingfactor;
            }

            //Add the avoidance contribution to velocity
            tmp_vel_x += (boidState[5] * avoidfactor);
            tmp_vel_y += (boidState[6] * avoidfactor);

            //Windows margin controll
            if(tmp_pos_x < Margin)
                tmp_pos_x += turnfactorx;
            if(tmp_pos_x > windows_width - Margin)
                tmp_pos_x -= turnfactorx;
            if(tmp_pos_y < Margin)
                tmp_pos_y += turnfactory;
            if(tmp_pos_y > windows_height - Margin)
                tmp_pos_y -= turnfactory;

            //update the biasval
            if(boidDataList.scoutGroup[i] == 1){  //he wants to go to the right side
                if(tmp_vel_x > 0){
                    tmp_biasval = std::max(maxbias, tmp_biasval + biasincrement);
                }
                else{
                    tmp_biasval = std::min(biasincrement, tmp_biasval - biasincrement);
                }
                tmp_vel_x = (1 - tmp_biasval)* tmp_vel_x + (tmp_biasval * 1);
                
            }
            if(boidDataList.scoutGroup[i] == 2){  //he wants to go to the left side   
                if(tmp_vel_x < 0){
                    tmp_biasval = std::max(maxbias, tmp_biasval + biasincrement);
                }
                else{
                    tmp_biasval = std::min(biasincrement, tmp_biasval - biasincrement);
                }
                tmp_vel_x = (1 - tmp_biasval)* tmp_vel_x + (tmp_biasval * (-1));
            }

            float refBoidSpeed = std::sqrt(std::pow(tmp_vel_x, 2) + std::pow(tmp_vel_y, 2));

            if (refBoidSpeed < minspeed){
                tmp_vel_x = (tmp_vel_x / refBoidSpeed) * minspeed;
                tmp_vel_y = (tmp_vel_y / refBoidSpeed) * minspeed;
            }
            else if (refBoidSpeed > maxspeed){
                tmp_vel_x = (tmp_vel_x / refBoidSpeed) * maxspeed;
                tmp_vel_y = (tmp_vel_y / refBoidSpeed) * maxspeed;
            }
            tmp_pos_x += tmp_vel_x;
            tmp_pos_y += tmp_vel_y;

            // Border limit controll
            if(tmp_pos_x < 0)
                tmp_pos_x = 0;
            else if(tmp_pos_x > windows_width) 
                tmp_pos_x = windows_width;
            
            if (tmp_pos_y < 0)
                tmp_pos_y = 0;
            else if (tmp_pos_y > windows_height) 
                tmp_pos_y = windows_height;
            
            boidDataList_tmp.addBoid(tmp_pos_x, tmp_pos_y, tmp_vel_x, tmp_vel_y, tmp_biasval, boidDataList.scoutGroup[i]);
        }

        //New position, I use std::move() to avoid copying values
        boidDataList = std::move(boidDataList_tmp);
        boidDataList_tmp.clear();

        if (isGraphicsOn){
            //Rendering
            window.clear();
            for (int i = 0; i < boidDataList.size(); i++){
                renderer.draw(window, boidDataList.xPos[i], boidDataList.yPos[i], boidDataList.xVelocity[i], boidDataList.yVelocity[i]);
            }
            window.display();
        }
    }
    return 0;
}