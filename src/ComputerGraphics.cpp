#include <CanvasPoint.h>
#include <ModelTriangle.h>
#include <glm/glm.hpp>
#include <vector>
#include "Camera.h"
#include "Scene.h"
#include "FilesUtils.h"
#include "EventUtils.h"
#include "RenderUtils.h"

#define WIDTH 480
#define HEIGHT 480

void printInstructions() {
    std::cout <<
    "=================================" << std::endl <<
    "RENDER MODES: " << std::endl <<
    "1: Wire Frame" << std::endl <<
    "2: Rasterised" << std::endl <<
    "3: Ray Traced" << std::endl <<
    std::endl <<
    "LIGHTING MODES (Ray Traced): " << std::endl <<
    "4: Default" << std::endl <<
    "5: Phong" << std::endl <<
    std::endl <<
    "LIGHT POSITION OPERATIONS: " << std::endl <<
    "H: Translate (y, positive)" << std::endl <<
    "N: Translate (y, negative)" << std::endl <<
    "B: Translate (x, negative)" << std::endl <<
    "M: Translate (x, positive)" << std::endl <<
    "J: Translate (z, positive)" << std::endl <<
    "K: Translate (z, negative)" << std::endl <<
    std::endl <<
    "CAMERA POSITION OPERATIONS: " << std::endl <<
    "L: Look At (0, 0, 0)" << std::endl <<
    "O: Orbit (y, positive)" << std::endl <<
    "W: Translate (y, positive)" << std::endl <<
    "S: Translate (y, negative)" << std::endl <<
    "A: Translate (x, negative)" << std::endl <<
    "D: Translate (x, positive)" << std::endl <<
    "-: Translate (z, positive)" << std::endl <<
    "=: Translate (z, negative)" << std::endl <<
    "UP: Rotate (x, positive)" << std::endl <<
    "DOWN: Rotate (x, negative)" << std::endl <<
    "LEFT: Rotate (y, positive)" << std::endl <<
    "RIGHT: Rotate (y, negative)" << std::endl <<
    std::endl <<
    "MISC: " << std::endl <<
    "R: Toggle mirror" << std::endl <<
    "X: Toggle soft shadows" << std::endl <<
    "F: Save as file" << std::endl <<
    "=================================" << std::endl;
}

Scene initScene(bool show, bool mirror, Scene::RenderMode renderMode, Light light, glm::vec3 initialPosition) {
    float w = WIDTH;
    float h = HEIGHT;
    if (!show) w = 480, h = 480;
    std::string objFileName = "cornell-box.obj";
    //std::string objFileName = "sphere.obj";
    std::string mtlFileName = "cornell-box.mtl";
    Camera camera(w, h, initialPosition, false);
    std::vector<ModelTriangle> triangles = FilesUtils::loadOBJ(objFileName, mtlFileName);
    Scene scene(w, h, show, mirror, renderMode, light, triangles, camera);
    return scene;
}

void showScene(Scene &scene) {
    SDL_Event event;
    scene.draw();
    while (true) {
        if (scene.show) {
            if (scene.window.pollForInputEvents(event)) EventUtils::handleEvent(event, scene);
            if (scene.camera.orbit) {
                scene.camera.rotate(Camera::Axis::y, 1.f);
                scene.draw();
            }
        }
        scene.window.renderFrame();
    }
}

void run(bool show) {
    Scene::RenderMode renderMode = Scene::RAY_TRACED;
    glm::vec3 lightColour = {255.f, 255.f, 255.f};
    float ambientIntensity = 0.15f;
    Light::Mode lightMode = Light::PHONG;
    glm::vec3 lightSource(0.3f,0.6f,1.3f);
    //glm::vec3 lightSource(0.f, 0.5f, 0.3f);
    //glm::vec3 lightSource(0.9f, 0.4f, -0.3f);
    RenderUtils::Sequence sequence = RenderUtils::RASTERISED_NAVIGATION;
    //glm::vec3 lightSource(0.8, 0.8, -0.8);
    //glm::vec3 lightSource(0.0, 0.55, 0.7);
    bool enableMirror = false;
    Light light(lightSource, lightMode, ambientIntensity, lightColour);
    light.softShadows = false;
    glm::vec3 initialPosition(0.f, 0.f, 4.f);
    //glm::vec3 initialPosition(-0.03f,0.39f,2.29f);
    //glm::vec3 initialPosition(0.f, 0.35f, 3.1f);
    Scene scene = initScene(show, enableMirror, renderMode, light, initialPosition);
    printInstructions();
    if (scene.show) {
        showScene(scene);
    } else {
        RenderUtils::generate(scene, sequence);
    }
}

int main(int argc, char *argv[]) {
    bool show = true;
    run(show);
}