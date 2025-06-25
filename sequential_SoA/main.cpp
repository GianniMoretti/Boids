#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <chrono>

#include "include/boid.h"
#include "include/boidRenderer.h"

using namespace std::chrono;

//Time profiling setting
int prof_cycle = 20;
int count_cycle = 0;
float sum_time = 0.0f;

//BOIDS      Time     
//30000      2187.31

int windows_width = 1200;
int windows_height = 1000;
float boids_scale = 0.5f;
int boids_number = 30000;                               //Con define forse va meglio
int windows_frame_rate = 60;
bool isGraphicsOn = false;

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
    BoidDataList boidDataList;
    boidDataList.reserve(boids_number);

    // Boids creation
    for (int i = 0; i < boids_number; ++i) {
        float x = static_cast<float>((rand() % (windows_width - Margin)) + Margin / 2);
        float y = static_cast<float>((rand() % (windows_height - Margin)) + Margin / 2);
        float vx = static_cast<float>(rand() % 3 - 1);
        float vy = static_cast<float>(rand() % 3 - 1);
        int scG = rand() % 3;
        boidDataList.addBoid(i, x, y, vx, vy, biasincrement, scG);
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

        auto start_time = high_resolution_clock::now();

        // Buffer temporanei per le nuove proprietà (array raw)
        int N = boidDataList.size();
        float* new_xPos = new float[N];
        float* new_yPos = new float[N];
        float* new_xVelocity = new float[N];
        float* new_yVelocity = new float[N];
        float* new_biasvals = new float[N];

        // Calcolo nuove velocità, bias e posizioni
        for (int i = 0; i < N; i++) {
            float tmp_pos_x = boidDataList.xPos[i];
            float tmp_pos_y = boidDataList.yPos[i];
            float tmp_vel_x = boidDataList.xVelocity[i];
            float tmp_vel_y = boidDataList.yVelocity[i];
            float tmp_biasval = boidDataList.biasvals[i];
            int scoutGroup = boidDataList.scoutGroup[i];

            float boidState[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            // Ottimizzazione: raggruppa le letture in variabili locali per migliorare locality e prefetch
            for (int j = 0; j < N; j++) {
                if (i == j) continue;
                float xj = boidDataList.xPos[j];
                float yj = boidDataList.yPos[j];
                float vxj = boidDataList.xVelocity[j];
                float vyj = boidDataList.yVelocity[j];

                float dx = tmp_pos_x - xj;
                float dy = tmp_pos_y - yj;
                float distance = std::sqrt(dx * dx + dy * dy);
                if(distance < protectedRange){
                    boidState[5] += dx;
                    boidState[6] += dy;
                }
                else if (distance < visualRange){
                    boidState[0] += xj;
                    boidState[1] += yj;
                    boidState[2] += vxj;
                    boidState[3] += vyj;
                    boidState[4] += 1;
                }
            }

            if(boidState[4] > 0){
                boidState[0] /= boidState[4];
                boidState[1] /= boidState[4];
                boidState[2] /= boidState[4];
                boidState[3] /= boidState[4];
                tmp_vel_x = tmp_vel_x + (boidState[0] - tmp_pos_x) * centeringfactor + (boidState[2] - tmp_vel_x) * matchingfactor;
                tmp_vel_y = tmp_vel_y + (boidState[1] - tmp_pos_y) * centeringfactor + (boidState[3] - tmp_vel_y) * matchingfactor;
            }

            tmp_vel_x += (boidState[5] * avoidfactor);
            tmp_vel_y += (boidState[6] * avoidfactor);

            if(tmp_pos_x < Margin)
                tmp_vel_x += turnfactorx;
            if(tmp_pos_x > windows_width - Margin)
                tmp_vel_x -= turnfactorx;
            if(tmp_pos_y < Margin)
                tmp_vel_y += turnfactory;
            if(tmp_pos_y > windows_height - Margin)
                tmp_vel_y -= turnfactory;

            // Bias update
            if(scoutGroup == 1){
                if(tmp_vel_x > 0){
                    tmp_biasval = std::max(maxbias, tmp_biasval + biasincrement);
                } else {
                    tmp_biasval = std::min(biasincrement, tmp_biasval - biasincrement);
                }
                tmp_vel_x = (1 - tmp_biasval)* tmp_vel_x + (tmp_biasval * 1);
            }
            if(scoutGroup == 2){
                if(tmp_vel_x < 0){
                    tmp_biasval = std::max(maxbias, tmp_biasval + biasincrement);
                } else {
                    tmp_biasval = std::min(biasincrement, tmp_biasval - biasincrement);
                }
                tmp_vel_x = (1 - tmp_biasval)* tmp_vel_x + (tmp_biasval * (-1));
            }

            float refBoidSpeed = std::sqrt(tmp_vel_x * tmp_vel_x + tmp_vel_y * tmp_vel_y);
            if (refBoidSpeed < minspeed){
                tmp_vel_x = (tmp_vel_x / refBoidSpeed) * minspeed;
                tmp_vel_y = (tmp_vel_y / refBoidSpeed) * minspeed;
            }
            else if (refBoidSpeed > maxspeed){
                tmp_vel_x = (tmp_vel_x / refBoidSpeed) * maxspeed;
                tmp_vel_y = (tmp_vel_y / refBoidSpeed) * maxspeed;
            }

            new_xVelocity[i] = tmp_vel_x;
            new_yVelocity[i] = tmp_vel_y;
            new_biasvals[i] = tmp_biasval;

            float new_pos_x = tmp_pos_x + tmp_vel_x;
            float new_pos_y = tmp_pos_y + tmp_vel_y;

            if(new_pos_x < 0)
                new_pos_x = 0;
            else if(new_pos_x > windows_width)
                new_pos_x = windows_width;
            if (new_pos_y < 0)
                new_pos_y = 0;
            else if (new_pos_y > windows_height)
                new_pos_y = windows_height;

            new_xPos[i] = new_pos_x;
            new_yPos[i] = new_pos_y;
        }

        // Copia i buffer temporanei negli array SoA originali
        #pragma omp simd
        for (int i = 0; i < N; i++) {
            boidDataList.xPos[i] = new_xPos[i];
            boidDataList.yPos[i] = new_yPos[i];
            boidDataList.xVelocity[i] = new_xVelocity[i];
            boidDataList.yVelocity[i] = new_yVelocity[i];
            boidDataList.biasvals[i] = new_biasvals[i];
            // scoutGroup non cambia
        }

        delete[] new_xPos;
        delete[] new_yPos;
        delete[] new_xVelocity;
        delete[] new_yVelocity;
        delete[] new_biasvals;

        auto end_time = high_resolution_clock::now();
        sum_time += duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
        std::cout << "parz: " << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << "iter: " << count_cycle <<std::endl;

        if (count_cycle >= prof_cycle){
            window.close();
        }

        if (isGraphicsOn){
            window.clear();
            for (int i = 0; i < boidDataList.size(); i++){
                renderer.draw(window, boidDataList.xPos[i], boidDataList.yPos[i], boidDataList.xVelocity[i], boidDataList.yVelocity[i]);
            }
            window.display();
        }
        count_cycle++;
    }
    std::cout << "Mean time: " << (sum_time / (prof_cycle + 1)) << " ms" << std::endl;
    return 0;
}