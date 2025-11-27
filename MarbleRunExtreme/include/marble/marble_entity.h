#pragma once
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include "marble.h"
#include "physics.h"

class MarbleEntity {
public:
    Marble renderable;
    btRigidBody* body;

    MarbleEntity(const glm::vec3& pos,
                 const glm::vec3& color,
                 float radius,
                 float mass,
                 PhysicsWorld& world);

    void updateFromPhysics(PhysicsWorld& world);
};
