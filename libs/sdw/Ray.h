#pragma once

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    Ray();
    Ray(glm::vec3 o, glm::vec3 d);
};