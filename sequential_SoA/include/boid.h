#ifndef BOID_DATA_H
#define BOID_DATA_H

#include <iostream>
#include <cstring> // for memset

struct BoidDataList {
    float* xPos = nullptr;
    float* yPos = nullptr;
    float* xVelocity = nullptr;
    float* yVelocity = nullptr;
    float* biasvals = nullptr;
    int* scoutGroup = nullptr;
    int numBoid = 0;
    int capacity = 0;

    BoidDataList() = default;

    ~BoidDataList() {
        deallocate();
    }

    void allocate(int num) {
        deallocate();
        xPos = new float[num];
        yPos = new float[num];
        xVelocity = new float[num];
        yVelocity = new float[num];
        biasvals = new float[num];
        scoutGroup = new int[num];
        capacity = num;
        numBoid = 0;
    }

    void deallocate() {
        delete[] xPos;
        delete[] yPos;
        delete[] xVelocity;
        delete[] yVelocity;
        delete[] biasvals;
        delete[] scoutGroup;
        xPos = yPos = xVelocity = yVelocity = biasvals = nullptr;
        scoutGroup = nullptr;
        capacity = 0;
        numBoid = 0;
    }

    void addBoid(int index, float x, float y, float xv, float yv, float biasval, int scoutgroup) {
        if (index >= capacity) return;
        xPos[index] = x;
        yPos[index] = y;
        xVelocity[index] = xv;
        yVelocity[index] = yv;
        biasvals[index] = biasval;
        scoutGroup[index] = scoutgroup;
        if (index >= numBoid) numBoid = index + 1;
    }

    void reserve(int num) {
        allocate(num);
    }

    int size() const {
        return numBoid;
    }

    void clear() {
        numBoid = 0;
        // Optionally, zero memory (not strictly needed)
        // if (capacity > 0) memset(xPos, 0, capacity * sizeof(float));
    }

    void reset(int size) {
        deallocate();
        allocate(size);
    }
};

#endif // BOID_DATA_H

// shape.setPoint(0, sf::Vector2f(0, -10));   // Punta (testa)
// shape.setPoint(1, sf::Vector2f(-8, 8));    // Ala sinistra
// shape.setPoint(2, sf::Vector2f(-4, 4));    // Corpo sinistro
// shape.setPoint(3, sf::Vector2f(4, 4));     // Corpo destro
// shape.setPoint(4, sf::Vector2f(8, 8));     // Ala destra