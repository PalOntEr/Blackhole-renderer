#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void render(unsigned int program, unsigned int VAO);
int initScreen();

int* compileShaders(const char* vertexPath, const char* fragmentPath);

int compileShader(GLenum shaderType, const char* path);

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Blackhole Renderer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    int VAO = initScreen();

    int* shaders = compileShaders("shaders/vertex.glsl", "shaders/fragment.glsl");

    unsigned int program = glCreateProgram();
    glAttachShader(program, shaders[0]);
    glAttachShader(program, shaders[1]);
    glLinkProgram(program);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render here
        render(program, VAO);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    delete[] shaders;

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void render(unsigned int program, unsigned int VAO) {
    // Clear the screen to blue 
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int initScreen() {

    float screenQuadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f 
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

int* compileShaders(const char* vertexPath, const char* fragmentPath) {
    
    int vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

    return new int[2]{ vertexShader, fragmentShader };

}

int compileShader(GLenum shaderType, const char* path) {

    try {

        ifstream file(path);
        if (!file.is_open()) {
            throw runtime_error("Could not open shader file");
        }

        stringstream buffer;
        buffer << file.rdbuf();
        string shaderSource = buffer.str();
        const char* source = shaderSource.c_str();

        unsigned int shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int success;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            throw runtime_error(infoLog);
        }

        return shader;
    }
    catch (const exception& e) {
        // Handle exceptions
        fprintf(stderr, "Error loading shader: %s\n", e.what());
        return -1;
    }
}