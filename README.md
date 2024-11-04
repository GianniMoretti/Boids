# Boids
An implementation of the artificial life program introduced by Craig Reynolds in 1986, designed to produce realistic simulations of flocking behavior. The project is developed in both sequential and parallel programming, with the goal of studying the speedup achieved through parallelization.

The software is based on interactions between artificial life forms, called boids, that move within a two-dimensional environment. In its simplest form, these artificial agents adjust their trajectories based on three primary rules:

- Separation: A boid steers to avoid local crowding (moving away from nearby boids).
- Alignment: A boid steers to align itself with the flight paths of nearby boids.
- Cohesion: A boid steers to move toward the average position (center of mass) of nearby boids.

This particular implementation follows the rules outlined in the following link: https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html
