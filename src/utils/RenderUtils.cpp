#include <iomanip>
#include "RenderUtils.h"
#include "Scene.h"
#include "FilesUtils.h"

namespace {
    void save(Scene &scene, int &count) {
        scene.draw();
        scene.window.renderFrame();
        std::string countStr = std::to_string(count);
        int numZeros = 5 - countStr.size();
        std::string name = std::string(numZeros, '0') + countStr;
        FilesUtils::saveAsImage(scene.window, name);
        count++;
    }

    void wireFrameSequence(Scene &scene, int &count) {
        scene.renderMode = Scene::WIRE_FRAME;
        for (int _=0; _<10; _++) {
            scene.camera.rotate(Camera::Axis::y, 1.f);
            save(scene, count);
        }
    }

    void rasterisedNavigationSequence(Scene &scene, int &count) {
        scene.renderMode = Scene::RASTERISED;
        for (int _=0; _<40; _++) {
            scene.camera.translate(Camera::Axis::z, 1.f);
            save(scene, count);
        }
        for (int _=0; _<70; _++) {
            scene.camera.translate(Camera::Axis::y, 1.f);
            save(scene, count);
        }
        for (int _=0; _<70; _++) {
            scene.camera.translate(Camera::Axis::x, -1.f);
            save(scene, count);
        }
        scene.camera.lookAt({0.f, 0.f, 0.f});
        save(scene, count);
        for (int _=0; _<10; _++) {
            scene.camera.rotate(Camera::Axis::x, -1.f);
            save(scene, count);
        }
        for (int _=0; _<30; _++) {
            scene.camera.translate(Camera::Axis::z, -1.f);
            save(scene, count);
        }
        for (int _=0; _<100; _++) { // orbit
            scene.camera.rotate(Camera::Axis::y, 1.f);
            save(scene, count);
        }
    }

    void doSequence(Scene &scene, RenderUtils::Sequence sequence) {
        int count = 331;
        switch (sequence) {
            case RenderUtils::WIRE_FRAME:
                wireFrameSequence(scene, count);
                break;
            case RenderUtils::RASTERISED_NAVIGATION:
                rasterisedNavigationSequence(scene, count);
                break;
        }
    }
}

namespace RenderUtils {
    void generate(Scene &scene, Sequence sequence) {
        doSequence(scene, sequence);
    }
}