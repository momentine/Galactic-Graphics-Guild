#version 330 core

// add a second layout variable representing a UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 text;

// create an "out" variable representing a UV coordinate
out vec2 originaluv;

void main() {
    // assign the UV layout variable to the UV "out" variable
    originaluv = text;
    gl_Position = vec4(position, 1.0);
}
