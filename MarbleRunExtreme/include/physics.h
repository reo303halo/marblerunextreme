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
    btRigidBody* addInclinedPlane(const glm::vec3& normal, float constant, const glm::vec3& position, const glm::vec3& rotation);
    btRigidBody* addBox(const glm::vec3& halfExtents, const glm::vec3& position, const glm::vec3& rotation, bool isStatic = true);
    glm::vec3 getObjectPosition(btRigidBody* body) const;
    void addRigidBody(btRigidBody* body);
    
private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    std::vector<btCollisionShape*> collisionShapes;
};
