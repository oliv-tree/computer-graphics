#include "Light.h"

Light::Light() = default;
Light::Light(glm::vec3 _position, Mode _mode, float _ambientIntensity, glm::vec3 _colour): position(_position), mode(_mode), ambientIntensity(_ambientIntensity), colour(_colour) {};