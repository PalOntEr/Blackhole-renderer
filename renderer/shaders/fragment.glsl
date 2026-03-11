#version 330 core
uniform vec2 resolution;
uniform vec4 color;

out vec4 FragColor;
in vec2 TexCoord;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;  // 0 to 1
    vec2 ndc = uv * 2.0 - 1.0;               // -1 to 1
    // For now, just visualize UV coordinates as color
    FragColor = color;
}