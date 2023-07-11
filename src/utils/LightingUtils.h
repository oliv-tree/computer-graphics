#pragma once

#include <RayTriangleIntersection.h>

class Scene;

namespace LightingUtils {
    Colour applyLighting(Scene &scene, RayTriangleIntersection &closestTriangle, glm::vec3 pointNormal);
    bool isMirror(Colour &colour);
    Colour applyMirror(Scene &scene, RayTriangleIntersection &closestTriangle);
}