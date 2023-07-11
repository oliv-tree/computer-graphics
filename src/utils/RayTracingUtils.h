#pragma once

#include <glm/glm.hpp>
#include <RayTriangleIntersection.h>
#include <Ray.h>

class Scene; // pre-declare to avoid circular dependency

namespace RayTracingUtils {
    glm::vec3 calculatePointNormal(ModelTriangle triangle, glm::vec3 point);
    RayTriangleIntersection findClosestTriangle(Scene &scene, Ray ray, bool mirror, int k);
    glm::vec3 canSeeLight(Scene &scene, const RayTriangleIntersection &closestTriangle);
    void draw(Scene &scene);
}