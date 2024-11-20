#ifndef BOID_DATA_H
#define BOID_DATA_H

struct BoidDataList {
    std::vector<float> xPos;
    std::vector<float> yPos;
    std::vector<float> xVelocity;
    std::vector<float> yVelocity;
    std::vector<float> biasvals;
    std::vector<int> scoutGroup;
    int numBoid = 0;

    void addBoid(float x, float y, float xv, float yv, float biasval, int scoutgroup) {
        xPos.push_back(x);
        yPos.push_back(y);
        xVelocity.push_back(xv);
        yVelocity.push_back(yv);
        biasvals.push_back(biasval);
        scoutGroup.push_back(scoutgroup);
        numBoid++;
    }

    void reserve(int num){
        xPos.reserve(num);
        yPos.reserve(num);
        xVelocity.reserve(num);
        yVelocity.reserve(num);
        biasvals.reserve(num);
        numBoid++;
    }

    int size(){
        return numBoid;
    }

    void clear() {
        // Clear all vectors to deallocate memory
        xPos.clear();
        yPos.clear();
        xVelocity.clear();
        yVelocity.clear();
        biasvals.clear();
        scoutGroup.clear();

        // Optionally shrink the capacity to zero to free up memory
        std::vector<float>().swap(xPos);
        std::vector<float>().swap(yPos);
        std::vector<float>().swap(xVelocity);
        std::vector<float>().swap(yVelocity);
        std::vector<float>().swap(biasvals);
        std::vector<int>().swap(scoutGroup);

        // Reset the boid count
        numBoid = 0;
    }
};

#endif // BOID_DATA_H

// shape.setPoint(0, sf::Vector2f(0, -10));   // Punta (testa)
// shape.setPoint(1, sf::Vector2f(-8, 8));    // Ala sinistra
// shape.setPoint(2, sf::Vector2f(-4, 4));    // Corpo sinistro
// shape.setPoint(3, sf::Vector2f(4, 4));     // Corpo destro
// shape.setPoint(4, sf::Vector2f(8, 8));     // Ala destra