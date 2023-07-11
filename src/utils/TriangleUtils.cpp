#include "TriangleUtils.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include "Scene.h"

namespace {
    /// @brief Uses barycentric coords to figure out depth of point within triangle
    float calculatePointDepth(CanvasTriangle triangle, CanvasPoint point) {
        float denominator = (triangle.v1().y-triangle.v2().y)*(triangle.v0().x-triangle.v2().x)+(triangle.v2().x-triangle.v1().x)*(triangle.v0().y-triangle.v2().y);
        float w0 = ((triangle.v1().y-triangle.v2().y)*(point.x-triangle.v2().x)+(triangle.v2().x-triangle.v1().x)*(point.y-triangle.v2().y))/denominator;
        float w1 = ((triangle.v2().y-triangle.v0().y)*(point.x-triangle.v2().x)+(triangle.v0().x-triangle.v2().x)*(point.y-triangle.v2().y))/denominator;
        float w2 = 1.0f - w0 - w1;
        if (w0 < 0 || w1 < 0 || w2 < 0) {
            return 0; // point is not inside the triangle
        }
        return 1/((w0 * triangle.v0().depth) + (w1 * triangle.v1().depth) + (w2 * triangle.v2().depth));
    }

    /// @brief Makes the smallest possible rectangle around a given triangle
    std::vector<float> boundingBox(CanvasTriangle triangle) {
        float minX = std::min({triangle.v0().x, triangle.v1().x, triangle.v2().x});
        float maxX = std::max({triangle.v0().x, triangle.v1().x, triangle.v2().x});
        float minY = std::min({triangle.v0().y, triangle.v1().y, triangle.v2().y});
        float maxY = std::max({triangle.v0().y, triangle.v1().y, triangle.v2().y});
        return {minX, maxX, minY, maxY};
    }

    /// @brief Draws a coloured line on canvas from given point to another
    void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
        float xDiff = to.x - from.x;
        float yDiff = to.y - from.y;
        float numSteps = std::max(abs(xDiff), abs(yDiff));
        float xStepSize = xDiff / numSteps;
        float yStepSize = yDiff / numSteps;
        for (float i=0.0; i<numSteps; i++) {
            float x = from.x + (xStepSize * i);
            float y = from.y + (yStepSize * i);
            CanvasPoint canvasPoint(x, y);
            if (!TriangleUtils::isInsideCanvas(window, canvasPoint)) continue;
            TriangleUtils::drawPixel(window, canvasPoint, colour);
        }
    }
}

namespace TriangleUtils {
    glm::vec3 calculateVertexNormals(ModelTriangle modelTriangle, std::vector<ModelTriangle> modelTriangles, glm::vec3 v) {
        glm::vec3 vertexNormal = modelTriangle.surfaceNormal;
        for (int j=0; j<modelTriangles.size(); j++) {
            if (v == modelTriangles[j].vertices[0] || v == modelTriangles[j].vertices[1] || v == modelTriangles[j].vertices[2]) {
                vertexNormal += modelTriangles[j].surfaceNormal;
            }
        }
        return vertexNormal;
    }

    glm::vec3 calculateSurfaceNormal(std::array<glm::tvec3<float>, 3> vertices) {
        return glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
    }

    /// @brief Checks if given point is outside of canvas bounds
    bool isInsideCanvas(DrawingWindow &window, CanvasPoint point) {
        if (point.x < 0 || point.x >= window.width || point.y < 0 || point.y >= window.height) {
            return false; // point is outside frame
        }
        return true;
    }

    void drawPixel(DrawingWindow &window, CanvasPoint point, Colour colour) {
        uint32_t colourCode = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
        window.setPixelColour(point.x, point.y, colourCode);
    }

    void drawStrokedTriangle(Scene &scene, CanvasTriangle triangle) {
        Colour colour(255, 255, 255);
        drawLine(scene.window, triangle.v0(), triangle.v1(), colour);
        drawLine(scene.window, triangle.v1(), triangle.v2(), colour);
        drawLine(scene.window, triangle.v2(), triangle.v0(), colour);
    }

    void drawFilledTriangle(Scene &scene, CanvasTriangle triangle, Colour colour) {
        // we should be getting the smallest possible bounding box for the triangle rather than iterating over all of them
        std::vector<float> boundedBy = boundingBox(triangle);
        for (int x=boundedBy[0]; x<boundedBy[1]; x++) {
            for (int y=boundedBy[2]; y<boundedBy[3]; y++) {
                CanvasPoint canvasPoint(x, y);
                float depth = calculatePointDepth(triangle, canvasPoint);
                if (depth == 0) continue; // point is outside triangle
                if (!isInsideCanvas(scene.window, canvasPoint)) continue;
                if (depth < scene.camera.depthBuffer[x][y]) {
                    continue; // something in front of our pixel has already been placed
                }
                drawPixel(scene.window, canvasPoint, colour);
                scene.camera.depthBuffer[x][y] = depth;
            }
        }
    }
}