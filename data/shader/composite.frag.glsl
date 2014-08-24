#version 120

uniform sampler2D u_tile1;
uniform sampler2D u_tile2;
uniform sampler2D u_sprite1;
uniform sampler2D u_sprite2;

uniform float u_world;
uniform vec4 u_color;
uniform vec4 u_blendscale;

varying vec2 v_texcoord;

void main() {
    /// Calculate main composite image
    vec4 tile1 = texture2D(u_tile1, v_texcoord);
    vec4 tile2 = texture2D(u_tile2, v_texcoord);
    vec4 sprite1 = texture2D(u_sprite1, v_texcoord);
    vec4 sprite2 = texture2D(u_sprite2, v_texcoord);

    vec4 tile = mix(tile1, tile2, u_world);
    vec4 sprite = mix(sprite1, sprite2, u_world);
    vec4 composite = sprite + vec4(1.0 - sprite.a) * tile;

    // Calculate color blending effect
    vec2 delta = (v_texcoord - u_blendscale.xy) * u_blendscale.zw;
    float d = dot(delta, delta);
    d = d * d;

    gl_FragColor = composite * (1.0 - u_color.a * d) + u_color * d;
}
