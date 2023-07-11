#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <Colour.h>
#include <ModelTriangle.h>

class Scene; // pre-declare to avoid circular dependency

namespace TriangleUtils {
    glm::vec3 calculateVertexNormals(ModelTriangle modelTriangle, std::vector<ModelTriangle> modelTriangles, glm::vec3 v);
    glm::vec3 calculateSurfaceNormal(std::array<glm::tvec3<float>, 3> vertices);
    bool isInsideCanvas(DrawingWindow &window, CanvasPoint point);
    void drawPixel(DrawingWindow &window, CanvasPoint point, Colour colour);
    void drawStrokedTriangle(Scene &scene, CanvasTriangle triangle);
    void drawFilledTriangle(Scene &scene, CanvasTriangle triangle, Colour colour);
}