#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Display.h>
#include <Shader.h>
#include <Geometry.h>
#include <texture.h>
#include <iostream>
#include "stb_image.h"

void processInput(GLFWwindow* window, unsigned int framebuffer, Shader ourShader, unsigned int VAO);
void SaveImage(GLFWwindow* window, unsigned int framebuffer, Shader ourShader, unsigned int VAO);

// settings
int SCR_WIDTH = 5472;
int SCR_HEIGHT = 3648;
int display_width = 1500;
int display_height = 1000;

bool saved = false;

float aspectRatio = SCR_WIDTH / SCR_HEIGHT; 

const char* img = "./img/GBVSRR.jpg";
const char* target = "./imgresult/GBVSRR.jpg";

int* buffer = new int[SCR_WIDTH * SCR_HEIGHT * 3];

int main()
{
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    int nrChannels;
    unsigned char* data = stbi_load(img, &SCR_WIDTH, &SCR_HEIGHT, &nrChannels, 0);
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
    GLFWwindow* window = WindowStart(display_width, display_height, "SIMPLE IMAGE PROCESSOR");
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

    Geometry geoms = Geometry();
    unsigned int VAO, VBO, EBO;
    geoms.BindGeoms(&VAO, &VBO, &EBO, 1.0f, -1.0f);
    unsigned int quadVAO, quadVBO;
    geoms.BindQuad(&quadVAO, &quadVBO);

    Shader ourShader = Shader();
    ourShader.LinkShaders("SimpleVertexShader.vs", "SimpleFragmentShader.fs");
    Shader frameShader = Shader();
    frameShader.LinkShaders("FrameBuffer.vs", "FrameBuffer.fs");

    unsigned int texture1;
    Texture texture = Texture();
    texture.BindTexture(&texture1, img);

    // be sure to activate the shader
    ourShader.use();
    ourShader.setInt("texture1", 0);

    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_width, display_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_width, display_height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        processInput(window, framebuffer, ourShader, VAO);

        // render
        // clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render the triangle
        ourShader.use();
        glBindVertexArray(VAO); 
        glBindTexture(GL_TEXTURE_2D, texture1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        frameShader.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(1, &rbo);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, unsigned int framebuffer, Shader ourShader, unsigned int VAO)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !saved)
        SaveImage(window, framebuffer, ourShader, VAO);
}

// save the image
// --------------
void SaveImage(GLFWwindow* window, unsigned int framebuffer, Shader ourShader, unsigned int VAO)
{
    saved = true;
    std::cout << "SAVING";
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // input
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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
    return;
}