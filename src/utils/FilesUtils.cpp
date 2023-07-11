#include "FilesUtils.h"
#include <Utils.h>
#include <algorithm>
#include <map>
#include <Colour.h>
#include <glm/glm.hpp>

namespace {
    /* Colour (.mtl) */

    int parseRawColour(std::string raw) {
        int val = (int) std::lround(std::stof(raw) * 255.0);
        return val;
    }

    Colour parseColourValue(std::string line) {
        std::vector<std::string> colourRaw = split(line, ' ');
        Colour colour(parseRawColour(colourRaw[1]), parseRawColour(colourRaw[2]), parseRawColour(colourRaw[3]));
        return colour;
    }

    std::string parseColourName(std::string line) {
        return split(line, ' ')[1];
    }

    std::map<std::string, Colour> loadColours(std::string mtlFileName) {
        std::map<std::string, Colour> colourMap;
        std::vector<std::string> colourNames;
        std::vector<Colour> colourValues;
        std::ifstream filein("resources/models/" + mtlFileName); // "cornell-box.mtl"
        for (std::string line; std::getline(filein, line); ) {
            if (line[0] == 'n') {
                colourNames.push_back(parseColourName(line));
            } else if (line[0] == 'K') {
                colourValues.push_back(parseColourValue(line));
            }
        }
        for (int i=0; i<colourNames.size(); i++) {
            colourMap.insert({colourNames[i], colourValues[i]});
        }
        return colourMap;
    }

    /* Object (.obj) */

    glm::vec3 parseVector(std::string line) {
        std::vector<std::string> trianglePointRaw = split(line, ' ');
        glm::vec3 trianglePoint = {std::stof(trianglePointRaw[1]), std::stof(trianglePointRaw[2]), std::stof(trianglePointRaw[3])};
        return trianglePoint;
    }

    int cleanFacet(std::string triangleRawItem) {
        return std::stof(triangleRawItem.substr(0, triangleRawItem.size()-1));
    }

    std::vector<int> parseFacet(std::string line) {
        std::vector<std::string> triangleRaw = split(line, ' ');
        std::vector<int> triangle = {cleanFacet(triangleRaw[1]), cleanFacet(triangleRaw[2]), cleanFacet(triangleRaw[3])};
        return triangle;
    }

    std::vector<ModelTriangle> createTriangles(std::vector<glm::vec3> trianglePoints, std::vector<std::vector<int>> triangles, std::vector<std::string> colours, std::map<std::string, Colour> colourMap) {
        std::vector<ModelTriangle> modelTriangles;
        for (int i=0; i<triangles.size(); i++) {
            Colour colour = colourMap.at(colours[i]);
            ModelTriangle modelTriangle = {trianglePoints[triangles[i][0] - 1], trianglePoints[triangles[i][1] - 1], trianglePoints[triangles[i][2] - 1], colour};
            //glm::vec3 normal = calculateSurfaceNormal(modelTriangle.vertices);
            //modelTriangle.surfaceNormal = normal;
            modelTriangles.push_back(modelTriangle);
        }
//        for (auto &triangle : modelTriangles) {
//            triangle.vertexNormals = calculateVertexNormals(triangle, modelTriangles);
//        }
        return modelTriangles;
    }
}

namespace FilesUtils {
    void saveAsImage(DrawingWindow &window, std::string &name) {
        window.savePPM("output/" + name + ".ppm");
    }

    std::vector<ModelTriangle> loadOBJ(std::string objFileName, std::string mtlFileName) {
        std::vector<glm::vec3> trianglePoints;
        std::vector<std::vector<int>> triangles;
        std::vector<std::string> colours;
        std::map<std::string, Colour> colourMap = loadColours(mtlFileName);
        std::ifstream filein("resources/models/" + objFileName); // "cornell-box.obj"
        std::string lastColourName;
        for (std::string line; std::getline(filein, line); ) {
            if (line[0] == 'v') {
                glm::vec3 trianglePoint = parseVector(line);
                trianglePoints.push_back(trianglePoint);
            } else if (line[0] == 'f') {
                triangles.push_back(parseFacet(line));
                colours.push_back(lastColourName);
            } else if (line[0] == 'u') {
                lastColourName = parseColourName(line);
            }
        }
        std::vector<ModelTriangle> modelTriangles = createTriangles(trianglePoints, triangles, colours, colourMap);
        return modelTriangles;
    }
}