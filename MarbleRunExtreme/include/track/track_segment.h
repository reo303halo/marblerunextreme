#pragma once
#include <glm/glm.hpp>
#include "mesh_entity.h"
#include "physics.h"

class TrackSegment {
public:
    RenderableMesh mesh;
    btRigidBody* body = nullptr;

    glm::vec3 entryPos = glm::vec3(0.0f);
    glm::vec3 entryForward = glm::vec3(0.0f,0.0f,1.0f);

    glm::vec3 exitPos = glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 exitForward = glm::vec3(0.0f,0.0f,1.0f);

    glm::mat4 worldTransform = glm::mat4(1.0f);
    
    glm::vec3 exitUp = glm::vec3(0,1,0);

    void setWorldTransform(const glm::mat4& t) {
        worldTransform = t;

        if(body) {
            btTransform bt;
            bt.setFromOpenGLMatrix(glm::value_ptr(t));
            body->setWorldTransform(bt);
        }
    }
};

