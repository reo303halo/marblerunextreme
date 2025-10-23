#include "physics.h"

PhysicsWorld::PhysicsWorld() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

PhysicsWorld::~PhysicsWorld() {
    // Clean up Bullet objects
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    for (auto shape : collisionShapes)
        delete shape;

    delete dynamicsWorld;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfiguration;
}

void PhysicsWorld::step(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime, 10);
}

void PhysicsWorld::addGround() {
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    collisionShapes.push_back(groundShape);

    btDefaultMotionState* groundMotion =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                             btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundCI(0, groundMotion, groundShape);
    btRigidBody* groundBody = new btRigidBody(groundCI);
    dynamicsWorld->addRigidBody(groundBody);
}

btRigidBody* PhysicsWorld::addSphere(float radius, const glm::vec3& startPos, float mass) {
    btCollisionShape* sphereShape = new btSphereShape(radius);
    collisionShapes.push_back(sphereShape);

    btDefaultMotionState* sphereMotion =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
                                             btVector3(startPos.x, startPos.y, startPos.z)));

    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f)
        sphereShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo sphereCI(mass, sphereMotion, sphereShape, inertia);
    btRigidBody* body = new btRigidBody(sphereCI);
    dynamicsWorld->addRigidBody(body);

    return body;
}

btRigidBody* PhysicsWorld::addInclinedPlane(const glm::vec3& normal, float constant,
                                            const glm::vec3& position, const glm::vec3& rotation) {
    // Create plane
    btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), constant);
    collisionShapes.push_back(planeShape);

    // Create transform
    btQuaternion quat;
    quat.setEuler(rotation.y, rotation.x, rotation.z); // yaw, pitch, roll
    btTransform transform(quat, btVector3(position.x, position.y, position.z));

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo planeCI(0.0f, motionState, planeShape);
    btRigidBody* body = new btRigidBody(planeCI);

    dynamicsWorld->addRigidBody(body);
    return body;
}

btRigidBody* PhysicsWorld::addBox(const glm::vec3& halfExtents, const glm::vec3& position,
                                  const glm::vec3& rotation, bool isStatic) {
    btCollisionShape* boxShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    collisionShapes.push_back(boxShape);

    btQuaternion quat;
    quat.setEuler(rotation.y, rotation.x, rotation.z); // yaw, pitch, roll
    btTransform transform(quat, btVector3(position.x, position.y, position.z));

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

    float mass = isStatic ? 0.0f : 1.0f;
    btVector3 inertia(0, 0, 0);
    if (mass > 0.0f)
        boxShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo boxCI(mass, motionState, boxShape, inertia);
    btRigidBody* body = new btRigidBody(boxCI);
    dynamicsWorld->addRigidBody(body);

    return body;
}

glm::vec3 PhysicsWorld::getObjectPosition(btRigidBody* body) const {
    btTransform trans;
    body->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

void PhysicsWorld::addRigidBody(btRigidBody* body) {
    dynamicsWorld->addRigidBody(body);
}

