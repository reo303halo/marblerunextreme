#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include "finish_trigger.h"

FinishTrigger::FinishTrigger(PhysicsWorld& world, TrackSegment& segment)
    : m_world(&world)
{
    if (!segment.body) {
        std::cerr << "FinishTrigger: segment has no rigid body!\n";
        return;
    }

    // Get segment's half extents for the ghost shape
    btBoxShape* box = dynamic_cast<btBoxShape*>(segment.body->getCollisionShape());
    btVector3 halfExtents = box ? box->getHalfExtentsWithMargin() : btVector3(1,1,1);

    // Create ghost object
    m_ghost = new btPairCachingGhostObject();

    // Create a new box shape for the ghost (option 2)
    btBoxShape* ghostShape = new btBoxShape(halfExtents);
    m_ghost->setCollisionShape(ghostShape);

    // Position the ghost at the segment
    m_ghost->setWorldTransform(segment.body->getWorldTransform());

    // Make it a sensor (no contact response)
    m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    // Add to physics world as a trigger
    if (m_world) {
        m_world->getWorld()->addCollisionObject(
            m_ghost,
            btBroadphaseProxy::SensorTrigger,
            btBroadphaseProxy::AllFilter
        );
    }
}

FinishTrigger::~FinishTrigger() {
    if (m_world && m_ghost) {
        m_world->getWorld()->removeCollisionObject(m_ghost);

        // Delete the ghost's shape too
        delete m_ghost->getCollisionShape();
    }
    delete m_ghost;
}

MarbleEntity* FinishTrigger::checkForWinner(const std::vector<MarbleEntity>& marbles) const {
    if (!m_ghost) return nullptr;

    int num = m_ghost->getNumOverlappingObjects();
    for (int i = 0; i < num; ++i) {
        btCollisionObject* obj = m_ghost->getOverlappingObject(i);
        btRigidBody* rb = btRigidBody::upcast(obj);
        if (!rb) continue;

        for (auto& m : marbles) {
            if (m.body == rb)
                return const_cast<MarbleEntity*>(&m);
        }
    }
    return nullptr;
}
