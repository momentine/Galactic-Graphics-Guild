#version 330 core

// Create a UV coordinate in variable
in vec2 uv;

// Add a sampler2D uniform
uniform sampler2D tex;

// Add a bool on whether or not to filter the texture
uniform bool inv;
uniform bool gray;
uniform bool hueRotation;
uniform bool blur;
uniform bool sharpen;
uniform bool sobel;
uniform float height;
uniform float width;

out vec4 fragColor;

void main()
{
    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(tex, uv);

    if (inv) {
        fragColor = vec4(1.0f - fragColor[0], 1.0f - fragColor[1], 1.0f - fragColor[2], fragColor[3]);
    }

    if (gray) {
        float a = (fragColor[0] + fragColor[1] + fragColor[2])/3.0f;
        fragColor = vec4(a, a, a, fragColor[3]);;
    }


    if (hueRotation) {
        // Convert RGB to HSV
        float cMax = max(max(fragColor.r, fragColor.g), fragColor.b);
        float cMin = min(min(fragColor.r, fragColor.g), fragColor.b);
        float delta = cMax - cMin;

        float hue = 0.0;
        if (delta > 0.0) {
            if (cMax == fragColor.r) {
                hue = 60.0 * mod((fragColor.g - fragColor.b) / delta, 6.0);
            } else if (cMax == fragColor.g) {
                hue = 60.0 * ((fragColor.b - fragColor.r) / delta + 2.0);
            } else if (cMax == fragColor.b) {
                hue = 60.0 * ((fragColor.r - fragColor.g) / delta + 4.0);
            }
        }

        // Apply hue rotation
        hue = mod(hue + 90.0, 360.0);

        // Convert back to RGB
        float c = delta;
        float x = c * (1.0 - abs(mod(hue / 60.0, 2.0) - 1.0));
        float m = cMin;

        vec3 rotatedColor;
        if (hue >= 0.0 && hue < 60.0) {
            rotatedColor = vec3(c, x, 0.0);
        } else if (hue >= 60.0 && hue < 120.0) {
            rotatedColor = vec3(x, c, 0.0);
        } else if (hue >= 120.0 && hue < 180.0) {
            rotatedColor = vec3(0.0, c, x);
        } else if (hue >= 180.0 && hue < 240.0) {
            rotatedColor = vec3(0.0, x, c);
        } else if (hue >= 240.0 && hue < 300.0) {
            rotatedColor = vec3(x, 0.0, c);
        } else {
            rotatedColor = vec3(c, 0.0, x);
        }

        fragColor = vec4(rotatedColor + m, fragColor.a);
    }

    if (blur) {
        int kernel_size = 5;
        fragColor = vec4(0.0f);
        float x = 1.0f/width;
        float y = 1.0f/height;
        float m = -2*y;

        for (int i = 0 ; i < kernel_size; i++){
            float n = -2*x;
            for (int j = 0; j < kernel_size; j++){
                fragColor += (1.0f/(kernel_size*kernel_size)) * texture(tex, vec2(uv[0]+n, uv[1]+m));
                n+=x;
            }
            m+=y;
        }
    }


    if(sharpen) {
        int kernel_size = 3;

        mat3 kernel = mat3(-1.0/9.0, -1.0/9.0, -1.0/9.0,
                           -1.0/9.0, 17.0/9.0, -1.0/9.0,
                           -1.0/9.0, -1.0/9.0, -1.0/9.0);

        fragColor = vec4(0.0f);
        float x = 1.0f/width;
        float y = 1.0f/height;
        float m = -1*y;

        for (int i = 0 ; i < kernel_size; i++){
            float n = -1*x;
            for (int j = 0; j < kernel_size; j++){
                fragColor += (kernel[i][j]) * texture(tex, vec2(uv[0]+n, uv[1]+m));
                n+=x;
            }
            m+=y;
        }
    }

    if(sobel) {
        int kernel_size = 3;

        mat3 kernelX = mat3(-1, 0, 1,
                           -2, 0, 2,
                           -1, 0, 1);

        mat3 kernelY = mat3(-1, -2, -1,
                             0, 0, 0,
                             1, 2, 1);

        fragColor = vec4(0.0f);
        float x = 1.0f/width;
        float y = 1.0f/height;
        float m = -1*y;

        vec4 fragColor1;
        vec4 fragColor2;

        for (int i = 0 ; i < kernel_size; i++){
            float n = -1*x;
            for (int j = 0; j < kernel_size; j++){
                fragColor1 += (kernelX[j][i]) * texture(tex, vec2(uv[0]+n, uv[1]+m));
                n+=x;
            }
            m+=y;
        }

        for (int i = 0 ; i < kernel_size; i++){
            float n = -1*x;
            for (int j = 0; j < kernel_size; j++){
                fragColor2 += (kernelY[j][i]) * texture(tex, vec2(uv[0]+n, uv[1]+m));
                n+=x;
            }
            m+=y;
        }

        fragColor = sqrt(fragColor1 * fragColor1 + fragColor2 * fragColor2);
    }
}
