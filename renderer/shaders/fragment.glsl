#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

void main() {
    // For now, just visualize UV coordinates as color
    FragColor = vec4(TexCoord, 0.0, 1.0);
}