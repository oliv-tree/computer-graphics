#pragma once

#include <SDL_events.h>

class Scene; // pre-declare to avoid circular dependency

namespace EventUtils {
    void handleEvent(SDL_Event event, Scene &scene);
}