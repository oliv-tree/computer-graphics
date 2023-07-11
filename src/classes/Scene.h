#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include <DrawingWindow.h>
#include <ModelTriangle.h>
#include <Light.h>

class Scene {
private:
    void modelToWorld();
    void calculateNormals();
public:
    enum RenderMode {
        WIRE_FRAME,
        RASTERISED,
        RAY_TRACED
    };
    float width;
    float height;
    const bool show;
    bool mirror;
    RenderMode renderMode;
    Light light;
    std::vector<ModelTriangle> triangles;
    Camera camera;
    DrawingWindow window;
    Scene(float width, float height, bool show, bool mirror, RenderMode renderMode, Light light, std::vector<ModelTriangle> triangles, Camera camera);
    void moveLight(Camera::Axis axis, float sign);
    void draw();
};