#version 120

uniform sampler2D u_reality;

uniform float u_world;
uniform vec4 u_color;
uniform vec4 u_blendscale;

varying vec2 v_texcoord;

const vec4 weight = vec4(0.2989, 0.5870, 0.1140, 0.0);

void main() {
    /// Calculate main composite image
    vec4 reality = texture2D(u_reality, v_texcoord);

    // Calculate color blending effect
    vec2 delta = (v_texcoord - u_blendscale.xy) * u_blendscale.zw;
    float d = dot(delta, delta);
    vec4 color = u_color * d * d;

    vec4 toned = reality * (1.0 - color.a) + color * reality.a;
    float value = dot(reality, weight);
    vec4 monochrome = vec4(vec3(value), reality.a);

    gl_FragColor = mix(toned, monochrome, u_world);
}
