#pragma once

#include <glm/glm.hpp>

struct Light {
    enum Mode {
        DEFAULT,
        PHONG
    };
    glm::vec3 position;
    Mode mode;
    float ambientIntensity;
    glm::vec3 colour;
    bool softShadows;
    Light();
    Light(glm::vec3 position, Mode mode, float ambientIntensity, glm::vec3 colour);
};