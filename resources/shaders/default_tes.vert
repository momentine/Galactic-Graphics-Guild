#version 330 core

layout(location = 0) in vec3 objspace_position;
layout(location = 1) in vec3 objspace_normal;

out vec4 worldspace_position;
out vec3 worldspace_normal;
out mat4 pvm;

uniform mat4 model_matrix;
uniform mat3 inv_t_model_matrix;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main() {
    worldspace_position = model_matrix * vec4(objspace_position, 1.0);
    worldspace_normal = inv_t_model_matrix * normalize(objspace_normal);

    gl_Position = vec4(objspace_position, 1.0);
    pvm = proj_matrix * view_matrix * model_matrix;
}
