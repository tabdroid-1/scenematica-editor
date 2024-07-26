#pragma once

#include <Tabby.h>
#include <scenematica.h>

namespace Editor {

struct ElementComponent {
    glm::vec4 Color;
    Tabby::Shared<Tabby::Mesh> Mesh;
    Scenematica::Element* Element;
};

struct EditorCameraData {
    // TODO:
    float Zoom = 1.0f;
};

}
