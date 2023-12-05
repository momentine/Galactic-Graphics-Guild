#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void helper(SceneNode *node, RenderData &data, glm::mat4 ctm) {
    // Apply Transformations to CTM
    for (int i = 0; i < node->transformations.size(); i++) {
        switch(node->transformations[i]->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = ctm * glm::translate(node->transformations[i]->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = ctm * glm::scale(node->transformations[i]->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            // Ensure a Rotation Axis is specified
            if (!glm::all(glm::lessThan(glm::abs(node->transformations[i]->rotate), glm::vec3(glm::epsilon<float>())))) {
                ctm = ctm * glm::rotate(node->transformations[i]->angle, node->transformations[i]->rotate);
            }
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm * node->transformations[i]->matrix;
            break;
        }
    }

    // Save primitives
    for (const auto& p : node->primitives) {
        RenderShapeData newPrimitive = RenderShapeData{*p, ctm};
        data.shapes.push_back(newPrimitive);
    }

    // Save lights
    for (const auto& l : node->lights) {
        SceneLightData newLight = SceneLightData{l->id, l->type, l->color, l->function, ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), ctm * l->dir, l->penumbra, l->angle, l->width, l->height};
        data.lights.push_back(newLight);
    }

    // call recursively on children
    for (const auto& c : node->children) {
        helper(c, data, ctm);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // populate renderData's list of primitives and their transforms.
    auto root = fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();
    helper(root, renderData, glm::mat4(1.f)); // Traverses scene graph from root down

    return true;
}
