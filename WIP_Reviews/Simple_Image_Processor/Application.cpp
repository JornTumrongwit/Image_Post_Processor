#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <Application.h>
#include <Render.h>
#include <fstream>
#include <streambuf>

// Shader Compilation: Helps with compiling shaders into a buffer for OpenGL to use
// --------------------------------------------------------------------------------
unsigned int CompileVertexShader(std::string file) 
{
    std::ifstream ifs(file);
    std::string stringFile = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    const char *FileContent = stringFile.c_str();
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &FileContent , NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return vertexShader;
}

// Shader Compilation: Helps with compiling fragment shaders into a buffer for OpenGL to use
// ----------------------------------------------------------------------------------------
unsigned int CompileFragmentShader(std::string file)
{
    std::ifstream ifs(file);
    std::string stringFile = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    const char* FileContent = stringFile.c_str();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FileContent, NULL);
    glCompileShader(fragmentShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return fragmentShader;
}

// Shader Compilation: Link the shaders that we will use
// -----------------------------------------------------
unsigned int LinkShaders(std::string vs, std::string fs)
{
    unsigned int vertexShader = CompileVertexShader(vs);
    unsigned int fragmentShader = CompileFragmentShader(fs);
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}