#include "marble_entity.h"

MarbleEntity::MarbleEntity(const glm::vec3& pos,
                           const glm::vec3& color,
                           float radius,
                           float mass,
                           PhysicsWorld& world)
    : renderable(pos, color, radius) {
    body = world.addSphere(radius, pos, mass);
}

void MarbleEntity::updateFromPhysics(PhysicsWorld& world) {
    renderable.position = world.getObjectPosition(body);
}
