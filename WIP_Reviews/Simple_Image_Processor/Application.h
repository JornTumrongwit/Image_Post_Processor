#pragma once

#include <GLFW/glfw3.h>
#include <string>

// Shader Compilation: Helps with compiling vertex shaders into a buffer for OpenGL to use
// --------------------------------------------------------------------------------------
unsigned int CompileVertexShader(std::string file);

// Shader Compilation: Helps with compiling fragment shaders into a buffer for OpenGL to use
// ----------------------------------------------------------------------------------------
unsigned int CompileFragmentShader(std::string file);

// Shader Compilation: Link the shaders that we will use
// -----------------------------------------------------
unsigned int LinkShaders(std::string vs, std::string fs);

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window);