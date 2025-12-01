#pragma once
#include "box_entity.h"
#include <vector>
#include <random>
#include "track_segment.h"
#include "physics.h"

struct Obstacle {
    BoxEntity* box = nullptr;
};


inline Obstacle buildObstacle(
    PhysicsWorld& physics,
    const glm::vec3& worldPos,
    const glm::vec3& size
) {
    glm::vec3 halfExtents = size * 0.5f;

    btRigidBody* body = physics.addBox(halfExtents, worldPos, glm::vec3(0.0f), true);

    Obstacle o;
    o.box = new BoxEntity(body, halfExtents);

    return o;
}
