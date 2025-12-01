#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
#include "physics.h"
#include "marble_entity.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "track.h"

class FinishTrigger {
public:
    FinishTrigger(PhysicsWorld& world, TrackSegment& segment);
    ~FinishTrigger();

    MarbleEntity* checkForWinner(const std::vector<MarbleEntity>& marbles) const;

private:
    PhysicsWorld* m_world;
    btPairCachingGhostObject* m_ghost = nullptr;
};
