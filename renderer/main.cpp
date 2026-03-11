#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void render(GLFWwindow* window, unsigned int program, unsigned int VAO);
pair<unsigned int, unsigned int> initScreen();

unsigned int createShaderProgram();

pair<int, int> compileShaders(const char* vertexPath, const char* fragmentPath);

int compileShader(GLenum shaderType, const char* path);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

    // Register resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    // Set initial viewport
    glViewport(0, 0, 800, 600);

    // Set up vertex data and buffers
    pair<unsigned int, unsigned int> VAOAndVBO = initScreen();
    unsigned int VAO = VAOAndVBO.first;
    unsigned int VBO = VAOAndVBO.second;

    // Compile shaders and link program
    unsigned int program = createShaderProgram();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render here
        render(window, program, VAO);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    
    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void render(GLFWwindow* window, unsigned int program, unsigned int VAO) {
    // Clear the screen to blue 
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Get current size and pass to shader
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glUniform2f(glGetUniformLocation(program, "resolution"), (float)width, (float)height);

    // Change colors over time
    float timeValue = (float)glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    float redValue = (sin(timeValue*3) / 2.0f) + 0.5f;
    float blueValue = (sin(timeValue*5) / 2.0f) + 0.5f;
    glUniform4f(glGetUniformLocation(program, "color"), redValue, greenValue, blueValue, 1.0f);

    // Draw the screen quad using the shader program
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

pair<unsigned int, unsigned int> initScreen() {

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

    return make_pair(VAO, VBO);
}

unsigned int createShaderProgram() {

    pair<int, int> shaderPair = compileShaders("shaders/vertex.glsl", "shaders/fragment.glsl");
    int vertexShader = shaderPair.first;
    int fragmentShader = shaderPair.second;

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Error linking shader program: %s\n", infoLog);
        return -1;
    }

    return program;
}

pair<int, int> compileShaders(const char* vertexPath, const char* fragmentPath) {
    
    int vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

    return make_pair(vertexShader, fragmentShader);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}