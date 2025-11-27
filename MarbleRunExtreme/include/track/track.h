#pragma once
#include <vector>
#include "track_segment.h"

class Track {
public:
    std::vector<TrackSegment> segments;

    void addSegment(TrackSegment seg) {
        if (segments.empty()) {
            seg.setWorldTransform(glm::mat4(1.0f));
            segments.push_back(seg);
            return;
        }

        TrackSegment& prev = segments.back();
        glm::mat4 T = computeAttachmentTransform(prev, seg);

        seg.setWorldTransform(T);
        segments.push_back(seg);
    }

private:
    glm::mat4 computeAttachmentTransform(
        const TrackSegment& prev,
        const TrackSegment& next
    ) {
        // --- 1. Compute previous segment's end frame in world space ---
        glm::vec3 P = glm::vec3(prev.worldTransform * glm::vec4(prev.exitPos, 1.0f));
        glm::vec3 F = glm::normalize(glm::vec3(prev.worldTransform * glm::vec4(prev.exitForward, 0.0f)));
        glm::vec3 U = glm::normalize(glm::vec3(prev.worldTransform * glm::vec4(prev.exitUp, 0.0f)));
        glm::vec3 R = glm::normalize(glm::cross(F, U));

        glm::mat4 endBasis(
            glm::vec4(R,0),
            glm::vec4(U,0),
            glm::vec4(F,0),
            glm::vec4(P,1)
        );

        // --- 2. Remove NEXT segment’s entry offset ---
        glm::mat4 removeEntry = glm::translate(glm::mat4(1.0f), -next.entryPos);

        return endBasis * removeEntry;
    }
};
