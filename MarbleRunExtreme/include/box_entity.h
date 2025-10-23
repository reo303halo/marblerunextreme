#pragma once
#include "physics.h"
#include "renderable_box.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BoxEntity {
public:
    btRigidBody* body;
    RenderableBox renderable;

    BoxEntity(btRigidBody* b, const glm::vec3& halfExtents)
        : body(b), renderable(halfExtents) {}

    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
        btTransform trans;
        body->getMotionState()->getWorldTransform(trans);

        glm::vec3 pos(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
        btQuaternion rot = trans.getRotation();
        glm::quat quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model *= glm::mat4_cast(quat);

        renderable.draw(shaderProgram, model, view, projection);
    }
};
