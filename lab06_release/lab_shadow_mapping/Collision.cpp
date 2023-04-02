#include "Collision.h"
#include "Box.h"
#include "Sphere.h"
using namespace glm;

void handleBoxSphereCollision( Sphere& sphere);
bool checkForBoxSphereCollision(glm::vec3& pos, const float& r, glm::vec3& n);
void handleGravitationalPull(Sphere& planet, Sphere& commet, float g);

void handleBoxSphereCollision( Sphere& sphere) {
    vec3 n;
    if (checkForBoxSphereCollision(sphere.x, sphere.r, n)) {
        // Task 2b: define the velocity of the sphere after the collision
        sphere.v = sphere.v - n * glm::dot(sphere.v, n) * 1.96f;  //*1.0f for elastic collision
        sphere.P = sphere.m * sphere.v;
    }
}

bool checkForBoxSphereCollision(vec3& pos, const float& r, vec3& n) {

    const float size = 25;
    if (pos.x - r <= 0) {
        //correction
        float dis = -(pos.x - r);
        pos = pos + vec3(dis, 0, 0);

        n = vec3(-1, 0, 0);
    }
    else if (pos.x + r >= size) {
        //correction
        float dis = size - (pos.x + r);
        pos = pos + vec3(dis, 0, 0);

        n = vec3(1, 0, 0);
    }
    else if (pos.y - r <= 0) {
        //correction
        float dis = -(pos.y - r);
        pos = pos + vec3(0, dis, 0);

        n = vec3(0, -1, 0);
    }
    else if (pos.y + r >= size) {
        //correction
        float dis = size - (pos.y + r);
        pos = pos + vec3(0, dis, 0);

        n = vec3(0, 1, 0);
    }
    else if (pos.z - r <= 0) {
        //correction
        float dis = -(pos.z - r);
        pos = pos + vec3(0, 0, dis);

        n = vec3(0, 0, -1);
    }
    else if (pos.z + r >= size) {
        //correction
        float dis = size - (pos.z + r);
        pos = pos + vec3(0, 0, dis);

        n = vec3(0, 0, 1);
    }
    else {
        return false;
    }

    return true;
}


void handleGravitationalPull(Sphere& planet, Sphere& commet, float g) {
    float a = g * planet.m * commet.m / ((distance(commet.x, planet.x)) * (distance(commet.x, planet.x)));
    commet.v = commet.v + a * (1 / 60);
    commet.x = commet.x + commet.v * vec3(1/60,1/60,1/ 60);
}