#ifndef BOID_DATA_BENCHMARK_H
#define BOID_DATA_BENCHMARK_H

// Versione semplificata di BoidData senza dipendenze SFML per il benchmark

struct Vector2f {
    float x, y;
    Vector2f(float x = 0, float y = 0) : x(x), y(y) {}
};

struct BoidData {
    Vector2f position;
    Vector2f velocity;
    float biasval = 0.0f;
    int scoutGroup = 0;

    BoidData(float x, float y, float vx, float vy, float biasval, int scoutGroup = 0) 
        : position(x, y), velocity(vx, vy), biasval(biasval), scoutGroup(scoutGroup) {}
};

// Structure of Arrays per SoA e Parallel
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
    }

    void reset(int size) {
        deallocate();
        allocate(size);
    }
};

#endif // BOID_DATA_BENCHMARK_H
