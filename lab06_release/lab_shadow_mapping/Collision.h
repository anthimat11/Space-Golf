#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

class Box;
class Sphere;
void handleBoxSphereCollision( Sphere& sphere);
//void handleSphereSphereCollision(Sphere& sphere, Sphere& sphere2);
void handleGravitationalPull(Sphere& planet, Sphere& commet, float g);
#endif
