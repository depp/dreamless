#version 120

uniform sampler2D u_picture;
uniform sampler2D u_pattern;
uniform vec2 u_pixscale;
varying vec2 v_texcoord;

void main() {
    vec4 picture = texture2D(u_picture, v_texcoord);
    vec4 pattern = texture2D(u_pattern, v_texcoord * u_pixscale * 0.125);
    pattern *= texture2D(u_pattern, v_texcoord * u_pixscale * 0.0625);
    gl_FragColor = picture * pattern;
}
