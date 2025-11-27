#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "mesh_entity.h"
#include "physics.h"
#include "track_segment.h"

inline TrackSegment buildCurvedSegment(
    PhysicsWorld& physics,
    float arcDeg,
    float radius = 30.0f,
    float width  = 5.0f,
    float depth  = 3.0f,
    float drop   = 10.0f,
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
        
        // Curve center
        glm::vec3 center(
                         radius * cos(angle) - radius,
                         y,
                         radius * sin(angle)
                         );
        
        // Tangent / forward direction
        glm::vec3 forward(-sin(angle), 0.0f, cos(angle));
        
        // World up for generating local frame
        glm::vec3 up(0, 1, 0);
        
        // Local right vector
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        
        // Local basis for this slice (x=right, y=up, z=forward)
        glm::mat3 basis(right, up, forward);
        
        lastBasis = basis; // <-- store the last orientation
        
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
    
    // --- Render + Physics ---
    RenderableMesh mesh;
    mesh.load(verts, norms, idx);
    seg.mesh = mesh;
    
    seg.body = physics.addTriangleMesh(verts, idx, glm::vec3(0), glm::vec3(0));
    
    // ----------------------------
    //   CONNECTIVITY INFORMATION
    // ----------------------------
    seg.entryPos = glm::vec3(0, 0, 0);
    seg.entryForward = glm::vec3(0, 0, 1);
    
    // Exit position (curve endpoint + slope)
    float xExit = radius * (cos(arc) - 1.0f);
    float zExit = radius * sin(arc);
    float yExit = -drop;
    
    seg.exitPos = glm::vec3(xExit, yExit, zExit);
    
    // Exit forward direction
    glm::vec3 exitForward(-sin(arc), 0.0f, cos(arc));
    seg.exitForward = glm::normalize(exitForward);
    
    // EXIT UP — uses the *last* basis of the curved segment
    // This is the critical fix!
    seg.exitUp = glm::normalize(lastBasis * glm::vec3(0,1,0));
    
    return seg;
}

inline TrackSegment buildStraightSegment(PhysicsWorld& physics, float length, float width=5.0f, float depth=2.0f) {
    TrackSegment seg;
    
    // vertices local
    std::vector<glm::vec3> verts = {
        {-width, -depth, 0}, { width, -depth, 0},
        {-width, -depth, length}, { width, -depth, length}
    };
    std::vector<glm::vec3> norms = {
        {0,1,0},{0,1,0},{0,1,0},{0,1,0}
    };
    std::vector<unsigned int> idx = {0,2,1,1,2,3};
    
    RenderableMesh m;
    m.load(verts,norms,idx);
    seg.mesh = m;
    
    seg.body = physics.addTriangleMesh(verts, idx, glm::vec3(0), glm::vec3(0));
    
    seg.entryPos = glm::vec3(0,0,0);
    seg.entryForward = glm::vec3(0,0,1);
    seg.exitPos = glm::vec3(0,0,length);
    seg.exitForward = glm::vec3(0,0,1);
    
    return seg;
}
