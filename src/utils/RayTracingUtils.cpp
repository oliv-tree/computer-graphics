#include "RayTracingUtils.h"
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <ModelTriangle.h>
#include "Scene.h"
#include "TriangleUtils.h"
#include "LightingUtils.h"
#include <cmath>

namespace {
    /// @brief Gets the absolute distance along ray (t), and proportional distances along triangle edges (u, v)
    glm::vec3 calculateRawIntersection(glm::vec3 from, ModelTriangle triangle, glm::mat3 DEMatrix) {
        glm::vec3 SPVector = from - triangle.vertices[0];
        glm::vec3 rawIntersection = glm::inverse(DEMatrix) * SPVector;
        return rawIntersection;
    }

    /// @brief Ensures the distances are on the triangle edges, and the distance along the ray is positive
    bool validateRawIntersection(glm::vec3 rawIntersection) {
        float t = rawIntersection.x;
        float u = rawIntersection.y;
        float v = rawIntersection.z;
        if (u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && u + v <= 1.0 && t > 0) {
            return true;
        }
        return false;
    }

    /// @brief Finds the relevant 3D point on the intersecting triangle
    glm::vec3 calculateIntersection(glm::mat3 DEMatrix, ModelTriangle triangle, glm::vec3 rawIntersection) {
        glm::vec3 e0 = DEMatrix[1];
        glm::vec3 e1 = DEMatrix[2];
        glm::vec3 intersectionPoint = triangle.vertices[0] + rawIntersection.y * e0 + rawIntersection.z * e1;
        return intersectionPoint;
    }

    glm::mat3 calculateDEMatrix(ModelTriangle triangle, glm::vec3 rayDirection) {
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        return DEMatrix;
    }

    /// @brief Converts a canvas point from ([0, width], [0, height]) to ([-1, 1], [-1, 1])
    glm::vec2 normaliseCanvasPoint(Scene &scene, CanvasPoint canvasPoint) {
        float normalisedX = canvasPoint.x * 2 / scene.width - 1;
        float normalisedY = canvasPoint.y * 2 / scene.height - 1;
        return {normalisedX, normalisedY};
    }

    /// @brief Gets the ray from origin camera (near plane viewpoint) to canvas point in 3D
    Ray calculateRayFromCamera(Scene &scene, CanvasPoint canvasPoint) {
        float n = scene.camera.near;
        float f = scene.camera.far;
        glm::vec2 normalisedCanvasPoint = normaliseCanvasPoint(scene, canvasPoint);
        glm::vec4 nearPos(normalisedCanvasPoint.x, normalisedCanvasPoint.y, -1.0, 1.0); // canvas point on near plane
        glm::vec4 origin(glm::inverse(scene.camera.vp) * nearPos * n); // adjust near pos to world
        glm::vec4 farPos(normalisedCanvasPoint.x * (f - n), normalisedCanvasPoint.y * (f - n), f + n, f - n); // canvas point on far plane
        glm::vec4 direction(glm::normalize(glm::inverse(scene.camera.vp) * farPos)); // adjust far pos to world and normalise
        return {glm::vec3(origin), glm::vec3(direction)};
    }
}

namespace RayTracingUtils {
    /// @brief Uses barycentric coords to figure out normal of point within triangle, given vertex normals
    glm::vec3 calculatePointNormal(ModelTriangle triangle, glm::vec3 point) {
        std::array<glm::vec3, 3> vertices = triangle.vertices;
        glm::vec3 v0 = vertices[1] - vertices[0];
        glm::vec3 v1 = vertices[2] - vertices[0];
        glm::vec3 v2 = point - vertices[0];
        float dotv0v0 = glm::dot(v0, v0);
        float dotv0v1 = glm::dot(v0, v1);
        float dotv1v1 = glm::dot(v1, v1);
        float dotv2v0 = glm::dot(v2, v0);
        float dotv2v1 = glm::dot(v2, v1);
        float denominator = dotv0v0 * dotv1v1 - dotv0v1 * dotv0v1;
        float w1 = (dotv1v1 * dotv2v0 - dotv0v1 * dotv2v1) / denominator;
        float w2 = (dotv0v0 * dotv2v1 - dotv0v1 * dotv2v0) / denominator;
        float w0 = 1.f - w1 - w2;
        return glm::normalize(w0 * triangle.vertexNormals[0] + w1 * triangle.vertexNormals[1] + w2 * triangle.vertexNormals[2]);
    }

    /// @brief Returns the triangle that the given ray intersects with first, using given ray (camera or light source)
    RayTriangleIntersection findClosestTriangle(Scene &scene, Ray ray, bool mirror, int k) {
        RayTriangleIntersection closestTriangle;
        closestTriangle.distanceFromCamera = FLT_MAX;
        for (size_t i=0; i<scene.triangles.size(); i++) {
            ModelTriangle triangle = scene.triangles[i];
            glm::mat3 DEMatrix = calculateDEMatrix(triangle, ray.direction);
            if (glm::determinant(DEMatrix) == 0) continue; // would be dividing by 0
            glm::vec3 rawIntersection = calculateRawIntersection(ray.origin, triangle, DEMatrix);
            if (!validateRawIntersection(rawIntersection)) continue;
            if (rawIntersection.x >= closestTriangle.distanceFromCamera || closestTriangle.distanceFromCamera <= 0) continue; // something closer already exists
            if (mirror && i == k) continue; // same triangle we are reflecting from
            glm::vec3 intersectionPoint = calculateIntersection(DEMatrix, triangle, rawIntersection);
            closestTriangle = {intersectionPoint, rawIntersection.x, triangle, i};
        }
        return closestTriangle;
    }

    /// @brief Checks if the point we want to draw on an intersecting triangle is able to see the light source
    glm::vec3 canSeeLight(Scene &scene, const RayTriangleIntersection &closestTriangle) {
        // ray using intersection and light source
        glm::vec3 direction = glm::normalize(closestTriangle.intersectionPoint - scene.light.position);
        glm::vec3 origin = scene.light.position;
        Ray ray(origin, direction);
        // get triangle closest to light source
        RayTriangleIntersection newClosestTriangle = findClosestTriangle(scene, ray, false, -1);
        // if triangle we are trying to draw is the triangle closest to the light source
        // then that means it can see the light. otherwise, it cannot as it's behind another one
        if (closestTriangle.triangleIndex == newClosestTriangle.triangleIndex) {
            return {-1.f, -1.f, -1.f}; // true
        }
        return newClosestTriangle.intersectionPoint;
    }

    void draw(Scene &scene) {
        for (int x=0; x<scene.width; x++) {
            for (int y=0; y<scene.height; y++) {
                CanvasPoint canvasPoint((float) x, (float) y);
                if (!TriangleUtils::isInsideCanvas(scene.window, canvasPoint)) continue;
                Ray ray = calculateRayFromCamera(scene, canvasPoint);
                RayTriangleIntersection closestTriangle = findClosestTriangle(scene, ray, false, -1);
                if (closestTriangle.distanceFromCamera == FLT_MAX) {
                    continue; // no triangle intersection found
                }
                Colour colour;
                glm::vec3 pointNormal = calculatePointNormal(closestTriangle.intersectedTriangle, closestTriangle.intersectionPoint);
                if (scene.mirror && LightingUtils::isMirror(closestTriangle.intersectedTriangle.colour)) {
                    colour = LightingUtils::applyMirror(scene, closestTriangle);
                } else {
                    colour = LightingUtils::applyLighting(scene, closestTriangle, pointNormal);
                }
                TriangleUtils::drawPixel(scene.window, canvasPoint, colour);
            }
        }
    }
}