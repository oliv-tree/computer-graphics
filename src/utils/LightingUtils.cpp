#include "LightingUtils.h"
#include "Scene.h"
#include <cmath>
#include "RayTracingUtils.h"

namespace {
    Colour vectorToColour(glm::vec3 colour) {
        return {(int) fmin(colour.x, 255.f), (int) fmin(colour.y, 255.f), (int) fmin(colour.z, 255.f)};
    }

    /// @brief Gets brightness based on distance from light source
    float calculateProximityIntensity(float distance) {
        float maxDistance = sqrt(12); // corner to corner of our room
        float radius = maxDistance * 0.7f;
        float falloff = 2.f;
        float normalisedDistance = distance/radius;
        if (normalisedDistance >= 1.f) return 0.f;
        //return 1.f * pow(1 - pow(normalisedDistance, 2), 2) / (1 + falloff * normalisedDistance);
        return 1.f * pow(1 - pow(normalisedDistance, 2), 2) / (1 + falloff * pow(normalisedDistance, 2));
    }

    float calculateIncidenceAngle(Scene &scene, RayTriangleIntersection &closestTriangle, glm::vec3 lightDir, glm::vec3 pointNormal) {
        float incidenceAngle = glm::dot(pointNormal, lightDir);
        return glm::max(incidenceAngle, 0.f);
    }

    float calculateSpecularIntensity(Scene &scene, RayTriangleIntersection &closestTriangle, glm::vec3 lightDir, float incidenceAngle, glm::vec3 pointNormal) {
        if (incidenceAngle <= 0.f) return 0.f;
        float specularIntensity = 0.5f;
        float specularFactor = 16.f;
        glm::vec3 reflectDir = glm::reflect(-lightDir, pointNormal);
        glm::vec3 viewDir = glm::normalize(scene.camera.position - closestTriangle.intersectionPoint);
        float specularAngle = glm::max(glm::dot(reflectDir, viewDir), 0.05f);
        return pow(specularAngle, specularFactor) * specularIntensity;
    }

//    Colour applyProximityLighting(Scene &scene, RayTriangleIntersection &closestTriangle) {
//        float distance = glm::length(scene.light.position - closestTriangle.intersectionPoint);
//        float proximityIntensity = calculateProximityIntensity(distance);
//        glm::vec3 diffuseColour = {closestTriangle.intersectedTriangle.colour.red, closestTriangle.intersectedTriangle.colour.green, closestTriangle.intersectedTriangle.colour.blue};
//        glm::vec3 colour(
//            (diffuseColour * proximityIntensity) +
//            (diffuseColour * scene.light.ambientIntensity)
//        );
//        return vectorToColour(colour);
//    }

    Colour applyPhongLighting(Scene &scene, RayTriangleIntersection &closestTriangle, glm::vec3 pointNormal) {
        float distance = glm::length(scene.light.position - closestTriangle.intersectionPoint);
        float proximityIntensity = calculateProximityIntensity(distance);
        glm::vec3 diffuseColour = {closestTriangle.intersectedTriangle.colour.red, closestTriangle.intersectedTriangle.colour.green, closestTriangle.intersectedTriangle.colour.blue};
        glm::vec3 lightDir = glm::normalize(scene.light.position - closestTriangle.intersectionPoint);

        float incidenceAngle = calculateIncidenceAngle(scene, closestTriangle, lightDir, pointNormal);
        float specularIntensity = calculateSpecularIntensity(scene, closestTriangle, lightDir, incidenceAngle, pointNormal);
        float shadowIntensity = 1.f;
        glm::vec3 shadowIntersection = RayTracingUtils::canSeeLight(scene, closestTriangle);
        if (shadowIntersection != glm::vec3(-1.f, -1.f, -1.f)) {
            if (scene.light.softShadows) {
                float dist = glm::length(shadowIntersection - closestTriangle.intersectionPoint);
                shadowIntensity = glm::clamp(dist/2.8f + 0.5f, 0.f, 1.f);
            }
            incidenceAngle = 0.f;
            specularIntensity = 0.f;
        }
        glm::vec3 colour(
            (diffuseColour * proximityIntensity * incidenceAngle) +
            (scene.light.colour * proximityIntensity * specularIntensity) +
            (diffuseColour * scene.light.ambientIntensity * shadowIntensity)
        );
        return vectorToColour(colour);
    }
}

namespace LightingUtils {
    Colour applyLighting(Scene &scene, RayTriangleIntersection &closestTriangle, glm::vec3 pointNormal) {
        Colour colour;
        switch (scene.light.mode) {
            case Light::DEFAULT:
                colour = closestTriangle.intersectedTriangle.colour;
                break;
            case Light::PHONG:
                colour = applyPhongLighting(scene, closestTriangle, pointNormal);
                break;
            default:
                break;
        }
        return colour;
    }

    /// @brief Checks if the provided colour is equal to the object colour we have set to be a mirror
    bool isMirror(Colour &colour) {
        Colour mirrorColour(178, 178, 178);
        if (colour == mirrorColour) return true;
        return false;
    }

    Colour applyMirror(Scene &scene, RayTriangleIntersection &closestTriangle) {
        if (!isMirror(closestTriangle.intersectedTriangle.colour)) return closestTriangle.intersectedTriangle.colour;
        glm::vec3 viewDir = glm::normalize(scene.camera.position - closestTriangle.intersectionPoint);
        glm::vec3 reflectDir = glm::reflect(-viewDir, glm::normalize(closestTriangle.intersectedTriangle.surfaceNormal));
        Ray ray(closestTriangle.intersectionPoint, reflectDir);
        RayTriangleIntersection newClosestTriangle = RayTracingUtils::findClosestTriangle(scene, ray, true, closestTriangle.triangleIndex);
        if (newClosestTriangle.distanceFromCamera == FLT_MAX) {
            return {0, 0, 0}; // looking out into the ether
        }
        glm::vec3 pointNormal = RayTracingUtils::calculatePointNormal(newClosestTriangle.intersectedTriangle, newClosestTriangle.intersectionPoint);
        Colour colour = LightingUtils::applyLighting(scene, newClosestTriangle, pointNormal);
        float modifier = 0.85; // bit darker to make it more realistic
        // FIXME: can make this smarter by accounting for total distance (light -> triangle + triangle -> mirror + mirror -> eye)
        colour.red *= modifier;
        colour.green *= modifier;
        colour.blue *= modifier;
        return colour;
    }
}