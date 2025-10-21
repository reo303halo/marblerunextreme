#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <bullet/btBulletDynamicsCommon.h>

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float deltaTime);
    void addGround();
    btRigidBody* addSphere(float radius, const glm::vec3& startPos, float mass = 1.0f);
    glm::vec3 getObjectPosition(btRigidBody* body) const;

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    std::vector<btCollisionShape*> collisionShapes;
};
