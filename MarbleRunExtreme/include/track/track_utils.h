#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "mesh_entity.h"
#include "physics.h"
#include "track_segment.h"
#include "obstacle_utils.h"

inline TrackSegment buildCurvedSegment(
    PhysicsWorld& physics,
    float arcDeg,
    float drop   = 10.0f,
    float radius = 30.0f,
    float width  = 5.0f,
    float depth  = 3.0f,
    int segU = 240,
    int segV = 60
                                       ) {
    TrackSegment seg;
    
    float arc = glm::radians(arcDeg);
    
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<unsigned int> idx;
    
    glm::mat3 lastBasis;
    
    // --- BUILD GEOMETRY ---
    for (int u = 0; u <= segU; ++u) {
        float t = float(u) / segU;
        float angle = arc * t;
        
        // Downward slope
        float y = -drop * t;
        
        glm::vec3 center(
                         radius * cos(angle) - radius,
                         y,
                         radius * sin(angle)
                         );
        
        // Tangent / forward direction
        glm::vec3 forward(-sin(angle), 0.0f, cos(angle));
        glm::vec3 up(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        
        // Local basis for this slice (x=right, y=up, z=forward)
        glm::mat3 basis(right, up, forward);
        
        lastBasis = basis;
        
        // --- Build cross-section ---
        for (int v = 0; v <= segV; ++v) {
            float s = float(v) / segV;
            
            float x = (s - 0.5f) * (width * 2.0f);
            float yLocal = -depth * cos((s - 0.5f) * glm::pi<float>());
            
            glm::vec3 local(x, yLocal, 0);
            
            verts.push_back(center + basis * local);
            
            glm::vec3 nLocal(0, 1, (x/width) * 0.3f);
            norms.push_back(glm::normalize(basis * nLocal));
        }
    }
    
    // --- TRIANGLES ---
    for (int u = 0; u < segU; ++u) {
        for (int v = 0; v < segV; ++v) {
            int i0 = u * (segV + 1) + v;
            int i1 = i0 + 1;
            int i2 = i0 + (segV + 1);
            int i3 = i2 + 1;
            
            idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
            idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
        }
    }
    
    RenderableMesh mesh;
    mesh.load(verts, norms, idx);
    seg.mesh = mesh;
    
    seg.body = physics.addTriangleMesh(verts, idx, glm::vec3(0), glm::vec3(0));
    
    // Connection
    seg.entryPos = glm::vec3(0, 0, 0);
    seg.entryForward = glm::vec3(0, 0, 1);
    
    float xExit = radius * (cos(arc) - 1.0f);
    float zExit = radius * sin(arc);
    float yExit = -drop;
    
    seg.exitPos = glm::vec3(xExit, yExit, zExit);
    
    glm::vec3 exitForward(-sin(arc), 0.0f, cos(arc));
    seg.exitForward = glm::normalize(exitForward);
    
    seg.exitUp = glm::normalize(lastBasis * glm::vec3(0,1,0));
    
    return seg;
}


inline TrackSegment buildStraightSegment(
    PhysicsWorld& physics,
    float length,
    float pitchDeg,
    float heightOffset = 0.0f,
    float width = 5.0f,
    float depth = 2.0f
) {
    TrackSegment seg;

    float pitchRad = glm::radians(pitchDeg);

    glm::vec3 forward(0, sin(pitchRad), cos(pitchRad));
    forward = glm::normalize(forward);

    glm::vec3 up(0, cos(pitchRad), -sin(pitchRad));
    up = glm::normalize(up);

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    glm::vec3 downShift = glm::vec3(0, -heightOffset, 0);

    glm::vec3 p0 = (-right * width) + (-up * depth) + downShift;
    glm::vec3 p1 = ( right * width) + (-up * depth) + downShift;
    glm::vec3 p2 = p0 + forward * length;
    glm::vec3 p3 = p1 + forward * length;

    std::vector<glm::vec3> verts = { p0, p1, p2, p3 };
    std::vector<glm::vec3> norms = { up, up, up, up };
    std::vector<unsigned int> idx = { 0,2,1, 1,2,3 };

    RenderableMesh m;
    m.load(verts, norms, idx);
    seg.mesh = m;

    seg.body = physics.addTriangleMesh(verts, idx, glm::vec3(0), glm::vec3(0));
    seg.entryPos = glm::vec3(0);
    seg.entryForward = forward;
    seg.exitPos = forward * length;
    seg.exitForward = forward;
    seg.exitUp = up;

    return seg;
}


inline std::vector<Obstacle> generateSlotMachineObstacles(
    PhysicsWorld& physics,
    const TrackSegment& segment,
    float segmentLength,
    float segmentWidth,
    int count
) {
    std::vector<Obstacle> obstacles;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(-segmentWidth + 2.0f, segmentWidth - 2.0f);
    std::uniform_real_distribution<float> distZ(2.0f, segmentLength - 2.0f);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);

    // Segment axes in world space
    glm::vec3 segForward = glm::normalize(glm::vec3(segment.worldTransform * glm::vec4(segment.exitPos - segment.entryPos, 0.0f)));
    glm::vec3 segUp      = glm::normalize(glm::vec3(segment.worldTransform * glm::vec4(segment.exitUp, 0.0f)));
    glm::vec3 segRight   = glm::normalize(glm::cross(segForward, segUp));

    glm::vec3 segOrigin = glm::vec3(segment.worldTransform * glm::vec4(segment.entryPos, 1.0f));

    for (int i = 0; i < count; i++) {
        float w = sizeDist(gen);
        float h = sizeDist(gen) + 1.0f;
        float d = sizeDist(gen);

        float xLocal = distX(gen);
        float zLocal = distZ(gen);

        // Compute obstacle center so its base sits on the track surface
        glm::vec3 worldPos = segOrigin + segRight * xLocal + segForward * zLocal + segUp * (h * 0.5f - 3.0f);

        obstacles.push_back(buildObstacle(physics, worldPos, glm::vec3(w, h, d)));
    }

    return obstacles;
}

inline TrackSegment buildFunnelSegment(
    PhysicsWorld& physics,
    float arcDeg,
    float drop = 10.0f,
    float radius = 30.0f,
    float startWidth = 5.0f,
    float depth = 3.0f,
    float exitWidth = 2.5f,
    int segU = 240,
    int segV = 60
) {
    TrackSegment seg;

    float arc = glm::radians(arcDeg);
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<unsigned int> idx;
    glm::mat3 lastBasis;

    for (int u = 0; u <= segU; ++u) {
        float t = float(u) / segU; // 0 -> 1 along the segment
        float angle = arc * t;
        float y = -drop * t;

        glm::vec3 center(radius * cos(angle) - radius, y, radius * sin(angle));

        glm::vec3 forward(-sin(angle), 0.0f, cos(angle));
        glm::vec3 up(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::mat3 basis(right, up, forward);
        lastBasis = basis;

        for (int v = 0; v <= segV; ++v) {
            float s = float(v) / segV;

            // Linearly interpolate width from startWidth -> exitWidth
            float startX = (s - 0.5f) * startWidth * 2.0f;
            float endX   = (s - 0.5f) * exitWidth  * 2.0f;
            float x = startX * (1.0f - t) + endX * t;

            float yLocal = -depth * cos((s - 0.5f) * glm::pi<float>());
            glm::vec3 local(x, yLocal, 0);
            verts.push_back(center + basis * local);

            glm::vec3 nLocal(0, 1, (x / startWidth) * 0.3f);
            norms.push_back(glm::normalize(basis * nLocal));
        }
    }

    // --- Build triangles ---
    for (int u = 0; u < segU; ++u) {
        for (int v = 0; v < segV; ++v) {
            int i0 = u * (segV + 1) + v;
            int i1 = i0 + 1;
            int i2 = i0 + (segV + 1);
            int i3 = i2 + 1;

            idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
            idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
        }
    }

    // --- Load mesh ---
    RenderableMesh mesh;
    mesh.load(verts, norms, idx);
    seg.mesh = mesh;

    seg.body = physics.addTriangleMesh(verts, idx, glm::vec3(0), glm::vec3(0));

    // --- Connection points ---
    seg.entryPos = glm::vec3(0, 0, 0);
    seg.entryForward = glm::vec3(0, 0, 1);

    float xExit = radius * (cos(arc) - 1.0f);
    float zExit = radius * sin(arc);
    float yExit = -drop;

    seg.exitPos = glm::vec3(xExit, yExit, zExit);
    glm::vec3 exitForward(-sin(arc), 0, cos(arc));
    seg.exitForward = glm::normalize(exitForward);
    seg.exitUp = glm::normalize(lastBasis * glm::vec3(0, 1, 0));

    return seg;
}

