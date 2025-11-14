#pragma once
#include "renderable_mesh.h"
#include "physics.h"

class MeshEntity {
public:
    btRigidBody* body;
    RenderableMesh renderable;

    MeshEntity(btRigidBody* rb, const RenderableMesh& mesh)
        : body(rb), renderable(mesh) {}

    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
        btTransform trans;
        body->getMotionState()->getWorldTransform(trans);

        glm::vec3 pos(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
        btQuaternion rot = trans.getRotation();
        glm::quat quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(quat);
        renderable.draw(shaderProgram, model, view, projection);
    }
};
