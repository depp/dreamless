#version 120

uniform sampler2D u_reality;
uniform sampler2D u_noise;

uniform float u_world;
uniform vec4 u_color;
uniform vec4 u_blendscale;
uniform vec2 u_noisescale;
uniform vec4 u_noiseoffset;

varying vec2 v_texcoord;

const vec4 weight = vec4(0.2989, 0.5870, 0.1140, 0.0);

void main() {
    vec2 noisecoord = v_texcoord * u_noisescale;
    vec2 noise = vec2(
        texture2D(u_noise, noisecoord + u_noiseoffset.xy).r - 0.5,
        texture2D(u_noise, noisecoord + u_noiseoffset.zw).g - 0.5);
    noise *= u_world * 0.02;

    /// Calculate main composite image
    vec4 reality = texture2D(u_reality, v_texcoord + noise);

    // Calculate color blending effect
    vec2 delta = (v_texcoord - u_blendscale.xy) * u_blendscale.zw;
    float d = dot(delta, delta);
    vec4 color = u_color * d * d;

    vec4 toned = reality * (1.0 - color.a) + color * reality.a;
    float value = dot(reality, weight);
    vec4 monochrome = vec4(vec3(value), reality.a);

    vec4 background = vec4(0.2, 0.3, 0.2, 1.0);
    vec4 foreground = mix(toned, monochrome, u_world);

    gl_FragColor = background * vec4(1.0 - foreground.a) + foreground;
}
