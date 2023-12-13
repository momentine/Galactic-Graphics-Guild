#version 330 core

in vec4 worldspace_position;
in vec3 worldspace_normal;
in vec2 uv;

out vec4 fragColor;

uniform vec4 cam_pos;

uniform float ka;
uniform float kd;
uniform float ks;
uniform int num_lights;

const int max_lights = 8;
uniform int lightType[max_lights];
uniform vec4 lightColor[max_lights];
uniform vec3 lightFunction[max_lights];
uniform vec4 lightPos[max_lights];
uniform vec4 lightDir[max_lights];
uniform float lightPenumbra[max_lights];
uniform float lightAngle[max_lights];

uniform vec4 ambient_c;
uniform vec4 diffuse_c;
uniform vec4 specular_c;
uniform float n;

uniform bool textureMapping;
uniform bool objTextureMap;
uniform sampler2D tex;
uniform float blend;

uniform bool doppler;
uniform float speedRatio;

void main() {
    fragColor = vec4(0.0f);

    fragColor += ka * ambient_c;
    for (int i = 0; i < num_lights; i++) {
        vec4 directionToLight;
        float f_att;
        vec4 intensity;
        if (lightType[i] == 0) {
            directionToLight = normalize(lightPos[i] - worldspace_position);
            float distance = length(vec3(lightPos[i]) - vec3(worldspace_position));
            f_att = min(1.0f, 1.0f/(lightFunction[i][0] + distance * lightFunction[i][1] + distance * distance * lightFunction[i][2]));
            intensity = lightColor[i];
        } else if (lightType[i] == 1) {
            directionToLight = normalize(-lightDir[i]);
            f_att = 1.0f;
            intensity = lightColor[i];
        } else if (lightType[i] == 2) {
            directionToLight = normalize(lightPos[i] - worldspace_position);
            float distance = length(vec3(lightPos[i]) - vec3(worldspace_position));
            f_att = min(1.0f, 1.0f/(lightFunction[i][0] + distance * lightFunction[i][1] + distance * distance * lightFunction[i][2]));

            // Find Angle Between Direction of Light and Light To Position
            float x = acos(dot(normalize(vec3(-directionToLight)), normalize(vec3(lightDir[i]))));
            float outer = lightAngle[i];
            float inner = outer - lightPenumbra[i];

            if (x <= inner) {
                intensity = lightColor[i];
            } else if ((x > inner) && (x <= outer)) {
                float frac = (x - inner)/(lightPenumbra[i]);
                float falloff = -2.0f*frac*frac*frac + 3.0f*frac*frac;
                intensity = lightColor[i] * (1.0f-falloff);
            } else {
                intensity = vec4(0.0f);
            }
        }

        vec4 diffuseColor = kd * diffuse_c;
        vec4 specularColor = ks * specular_c;

        if (textureMapping && objTextureMap) {
            vec4 color = texture(tex, uv);
            diffuseColor = blend * vec4(color[0], color[1], color[2], color[3]) + (1.0f-blend) * diffuseColor;
            specularColor = diffuseColor;
        }

        fragColor += clamp(f_att * diffuseColor * intensity * clamp(dot(vec4(normalize(worldspace_normal), 0.0f), directionToLight), 0.0f, 1.0f), 0, 1);

        vec3 R = normalize(vec3(reflect(-directionToLight, vec4(normalize(worldspace_normal), 0.0f))));
        vec3 E = normalize(vec3(cam_pos) - vec3(worldspace_position));
        if (n > 0) {
            fragColor += clamp(f_att * specularColor * intensity * pow(clamp(dot(R, E), 0.0f, 1.0f), n), 0.0f, 1.0f);
        }
    }

    if(doppler){
        float red = fragColor.r;
        float blue = fragColor.b;
        float green = fragColor.g;

        if(fragColor.r != 0.0f || fragColor.g != 0.0f || fragColor.b != 0.0f)
        if (speedRatio > 0) { // red shift
            red = min(fragColor.r - speedRatio, 1.0);
            blue = max(fragColor.b + speedRatio, 0.0);
            green = max(fragColor.g + speedRatio, 0.0);
        }
        else if (speedRatio < 0){ // blue shift
            red = max(fragColor.r - speedRatio, 0.0);
            blue = max(fragColor.b + speedRatio, 0.0);
            green = min(fragColor.g + speedRatio, 1.0);
        }
        fragColor.r = red;
        fragColor.b = blue;
        fragColor.g = green;
    }

//    fragColor = vec4(uv[0], 0.0, 0.0, 1.0);

//    fragColor = vec4(abs(worldspace_normal), 1.0);
}
