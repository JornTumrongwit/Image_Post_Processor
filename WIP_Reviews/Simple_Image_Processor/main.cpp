#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Display.h>
#include <Shader.h>
#include <Geometry.h>
#include <texture.h>
#include <iostream>

void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = WindowStart(SCR_WIDTH, SCR_HEIGHT, "SIMPLE IMAGE PROCESSOR");
    if (window == NULL) {
        return -1;
    }
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    Shader ourShader = Shader();
    ourShader.LinkShaders("SimpleVertexShader.vs", "SimpleFragmentShader.fs");

    Geometry geoms = Geometry();
    unsigned int VAO, VBO, EBO;
    geoms.BindGeoms(&VAO, &VBO, &EBO, 1.0f, -1.0f);

    unsigned int texture1;
    Texture texture = Texture();
    texture.BindTexture(&texture1, "./img/clubexpo.jpg");

    // be sure to activate the shader
    ourShader.use();
    ourShader.setInt("texture1", 0);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        // clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader
        ourShader.use();

        // update the uniform color
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        ourShader.setVec4("ourColor", 0.0f, greenValue, 0.0f, 0.0f);

        // render the triangle
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}