#pragma once

#include <GLFW/glfw3.h>

// Main Loop: The main running loop of the application. Accepts the input from the user and acts accordingly
// ---------------------------------------------------------------------------------------------------------
void MainLoop(GLFWwindow* window);

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window);