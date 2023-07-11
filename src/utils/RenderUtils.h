#pragma once

class Scene;

namespace RenderUtils {
    enum Sequence {
        WIRE_FRAME, // show wire frame
        RASTERISED_NAVIGATION, // pos, orientation, orbit, lookAt
    };
    void generate(Scene &scene, Sequence sequence);
}