#version 330 core


// Create a UV coordinate in variable
in vec2 originaluv;

// Add a sampler2D uniform
uniform sampler2D tex;

// Add a bool on whether or not to filter the texture
uniform bool inv;
uniform bool gray;
uniform bool hueRotation;
uniform bool blur;
uniform bool sharpen;
uniform bool sobel;
//-------------weixiang zhao-------------------
uniform bool FXAA;
uniform bool grav;
uniform vec2 gravPoint;
uniform float gravStrength;
//-------------end weixiang zhao-------------------
uniform float height;
uniform float width;

out vec4 fragColor;

vec2 uv = originaluv;
//-----------------------------------FXAA---------------------------------------------
vec2 resolution = vec2(width, height);

const float FXAA_REDUCE_MIN = 1.0 / 128.0;
const float FXAA_REDUCE_MUL = 1.0 / 8.0;
const float FXAA_SPAN_MAX = 8.0;

vec3 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution) {
    vec2 invRes = 1.0 / resolution.xy;

        vec3 rgbNW = texture(tex, uv + (vec2(-1.0, -1.0) * invRes)).rgb;
        vec3 rgbNE = texture(tex, uv + (vec2(1.0, -1.0) * invRes)).rgb;
        vec3 rgbSW = texture(tex, uv + (vec2(-1.0, 1.0) * invRes)).rgb;
        vec3 rgbSE = texture(tex, uv + (vec2(1.0, 1.0) * invRes)).rgb;
        vec3 rgbM  = texture(tex, uv).rgb;

        // Luminance calculation
        float lumaNW = dot(rgbNW, vec3(0.299, 0.587, 0.114));
        float lumaNE = dot(rgbNE, vec3(0.299, 0.587, 0.114));
        float lumaSW = dot(rgbSW, vec3(0.299, 0.587, 0.114));
        float lumaSE = dot(rgbSE, vec3(0.299, 0.587, 0.114));
        float lumaM  = dot(rgbM, vec3(0.299, 0.587, 0.114));

        // Edge detection
        float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
        float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

        vec2 dir;
        dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
        dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

        float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
        float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
        dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * invRes;

        vec3 rgbA = 0.5 * (
            texture(tex, uv + dir * (1.0 / 3.0 - 0.5)).rgb +
            texture(tex, uv + dir * (2.0 / 3.0 - 0.5)).rgb);
        vec3 rgbB = rgbA * 0.5 + 0.25 * (
            texture(tex, uv + dir * -0.5).rgb +
            texture(tex, uv + dir * 0.5).rgb);

        float lumaB = dot(rgbB, vec3(0.299, 0.587, 0.114));
        if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
            return rgbA;
        } else {
            return rgbB;
        }
}
//-----------------------------------End FXAA-----------------------------------------


//-----------------------------------Gravitational Lens-------------------------------
vec2 gravitationalLensEffect(vec2 uv, vec2 resolution, vec2 gravPoint, float gravStrength) {
    vec2 screenCoord = uv * resolution;
    vec2 toGravPoint = gravPoint - screenCoord;
    float distance = length(toGravPoint);
    float distortion = gravStrength / distance;
    return uv + (normalize(toGravPoint) * distortion / resolution);
}
//-----------------------------------End Gravitational Lens---------------------------

void main()
{
    if (grav) {
        uv = gravitationalLensEffect(uv, vec2(width, height), gravPoint, gravStrength);
    }
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

    if (FXAA) {
        fragColor = vec4(fxaa(tex, uv * resolution, resolution), fragColor.a);
    }

}
