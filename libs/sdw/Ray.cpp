#include "Ray.h"

Ray::Ray() = default;
Ray::Ray(glm::vec3 o, glm::vec3 d): origin(o), direction(d) {};