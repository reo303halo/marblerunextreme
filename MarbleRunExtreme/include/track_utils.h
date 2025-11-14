#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "mesh_entity.h"
#include "physics.h"

inline std::vector<MeshEntity> buildCurvedMeshTrack(PhysicsWorld& physics) {
    std::vector<MeshEntity> result;

    const int segU = 240;         // more segments along the path
    const int segV = 60;          // smoother cross-section
    const float pathRadius = 30.0f;  // big curve
    const float halfWidth = 5.0f;    // wide half-pipe
    const float depth = 3.0f;        // deeper bowl
    const float drop = 20.0f;        // tall vertical drop
    const float arc = glm::radians(180.0f); // half-circle

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    for (int u = 0; u <= segU; ++u) {
        float t = (float)u / segU;
        float angle = arc * t;

        // center of track along a circular path in XZ
        glm::vec3 center(
            pathRadius * cos(angle),
            6.0f - drop * t,
            pathRadius * sin(angle)
        );

        // direction tangent to the path
        glm::vec3 forward(-sin(angle), 0.0f, cos(angle));
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        // rotation matrix aligning local coords to world path orientation
        glm::mat3 basis(right, up, forward);

        for (int v = 0; v <= segV; ++v) {
            float s = (float)v / segV; // 0..1 across width
            float x = (s - 0.5f) * (halfWidth * 2.0f); // side offset
            float y = -depth * cos((s - 0.5f) * glm::pi<float>()); // bowl shape
            float z = 0.0f;

            glm::vec3 local(x, y, z);
            glm::vec3 world = center + basis * local;
            vertices.push_back(world);

            // compute normal
            glm::vec3 nLocal = glm::normalize(glm::vec3(0.0f, 1.0f, (x / halfWidth) * 0.3f));
            normals.push_back(glm::normalize(basis * nLocal));
        }
    }

    // build indices
    for (int u = 0; u < segU; ++u) {
        for (int v = 0; v < segV; ++v) {
            int i0 = u * (segV + 1) + v;
            int i1 = i0 + 1;
            int i2 = i0 + (segV + 1);
            int i3 = i2 + 1;
            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }

    // upload to GPU
    RenderableMesh mesh;
    mesh.load(vertices, normals, indices);

    // create Bullet triangle mesh
    btRigidBody* body = physics.addTriangleMesh(vertices, indices,glm::vec3(0.0f), glm::vec3(0.0f));

    result.emplace_back(body, mesh);
    return result;
}



