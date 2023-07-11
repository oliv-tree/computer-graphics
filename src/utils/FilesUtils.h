#pragma once

#include <vector>
#include <string>
#include <ModelTriangle.h>
#include <DrawingWindow.h>

namespace FilesUtils {
    std::vector<ModelTriangle> loadOBJ(std::string objFileName, std::string mtlFileName);
    void saveAsImage(DrawingWindow &window, std::string &name);
}