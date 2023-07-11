#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    float width;
    float height;
    glm::mat4 camera;
    glm::mat4 projection;
    void updateVP();
public:
    enum Axis { x, y, z };
    glm::mat4 vp;
    const glm::mat4 model = glm::translate(glm::mat4(0.35f), {0.f, 0.f, 0.f}); // place object at origin, scaled to 0.35x
    glm::vec3 position;
    const float near = 0.1f;
    const float far = 100.f;
    bool orbit;
    std::vector<std::vector<float>> depthBuffer;
    Camera(float width, float height, glm::vec3 position, bool orbit);
    void resetDepthBuffer();
    void translate(Axis axis, float sign);
    void rotate(Axis axis, float sign);
    void lookAt(glm::vec3 vertex);
};
