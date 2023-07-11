#include "RasterisingUtils.h"
#include "Scene.h"
#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <CanvasPoint.h>
#include <glm/glm.hpp>
#include "TriangleUtils.h"

namespace {
    /// @brief Converts a point in world space to a canvas point
    CanvasPoint worldToCanvas(Scene &scene, glm::vec3 vertex) {
        float n = scene.camera.near;
        float f = scene.camera.far;
        glm::vec4 clipPos = scene.camera.vp * glm::vec4(vertex, 1.f);
        glm::vec3 ndsPos(clipPos.x/clipPos.w, clipPos.y/clipPos.w, clipPos.z/clipPos.w); // normalised device space
        CanvasPoint screenPos(scene.width/2 * (ndsPos.x + 1), scene.height/2 * (ndsPos.y + 1), ndsPos.z * (f - n)/2 + (f + n)/2);
        return screenPos;
    }

    /// @brief Takes triangles in world space and makes them into triangles with canvas points
    CanvasTriangle makeCanvasTriangle(Scene &scene, ModelTriangle triangle) {
        CanvasTriangle canvasTriangle;
        for (int i=0; i<3; i++) {
            glm::vec3 vertex = triangle.vertices[i];
            CanvasPoint canvasIntersection = worldToCanvas(scene, vertex);
            canvasTriangle.vertices[i] = canvasIntersection;
        }
        return canvasTriangle;
    }
}

namespace RasterisingUtils {
    /// @brief Default rasterised
    void drawFilled(Scene &scene) {
        scene.camera.resetDepthBuffer();
        for (const auto &triangle : scene.triangles) {
            CanvasTriangle canvasTriangle = makeCanvasTriangle(scene, triangle);
            TriangleUtils::drawFilledTriangle(scene, canvasTriangle, triangle.colour);
        }
    }

    /// @brief Wire frame
    void drawStroked(Scene &scene) {
        for (const auto &triangle : scene.triangles) {
            CanvasTriangle canvasTriangle = makeCanvasTriangle(scene, triangle);
            TriangleUtils::drawStrokedTriangle(scene, canvasTriangle);
        }
    }
}

