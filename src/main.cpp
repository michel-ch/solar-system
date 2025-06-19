#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera_ubo.hpp"
#include "transform_ubo.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "imgui_impl.hpp"
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int currentEffect = 0; 
float planetRotationSpeed = 1.0f;

GLuint FBO;
GLuint FBOColorTexture;
GLuint FBODepthTexture;
GLuint quadVAO;
GLuint quadVBO;

bool initFBO(GLFWwindow* window) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glGenTextures(1, &FBOColorTexture);
    glBindTexture(GL_TEXTURE_2D, FBOColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &FBODepthTexture);
    glBindTexture(GL_TEXTURE_2D, FBODepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOColorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBODepthTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Erreur: FBO incomplet!" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glBindVertexArray(0);

    return true;
}

void cleanupFBO() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &FBOColorTexture);
    glDeleteTextures(1, &FBODepthTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
};

unsigned int loadCubemap(std::vector<std::string> faces, const std::string& directory) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        std::string path = directory + faces[i];
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load: " << path << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
void processInput(GLFWwindow *window) {
    // Controles camera existants
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // Camera orientation
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(TURN_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(TURN_RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(TURN_UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(TURN_DOWN, deltaTime);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System Viewer", NULL, NULL);
    if (!window) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to initialize GLAD" << std::endl; return -1; }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    if (!g_imguiManager.Init(window)) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Initialisation des shaders..." << std::endl;
    
    Shader skyboxShader("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");
    std::cout << "Skybox shader créé" << std::endl;

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> skyboxFaces = {
        "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
    };
    std::vector<std::string> jupiterFaces = {
        "jupiter_right.jpg", "jupiter_left.jpg", "jupiter_top.jpg", "jupiter_bottom.jpg", "jupiter_front.jpg", "jupiter_back.jpg"
    };

    GLuint cubemapTexture = loadCubemap(skyboxFaces, "../assets/skybox_real/");
    GLuint jupiterCubemapTexture = loadCubemap(jupiterFaces, "../assets/skybox_jupiter/");

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Shader shaderColor("../assets/shaders/color.vert", "../assets/shaders/color.frag");
    std::cout << "Color shader créé" << std::endl;
    
    Shader shaderTexture("../assets/shaders/texture.vert", "../assets/shaders/texture.frag");
    std::cout << "Texture shader créé" << std::endl;
    
    Shader shaderEnv("../assets/shaders/envmap.vert", "../assets/shaders/envmap.frag");
    std::cout << "Env shader créé" << std::endl;

    std::cout << "Tentative de création du shader effect..." << std::endl;
    
    std::ifstream vertFile("../assets/shaders/effect.vert");
    std::ifstream fragFile("../assets/shaders/effect.frag");
    
    if (!vertFile.good()) {
        std::cout << "ERREUR: effect.vert non trouvé!" << std::endl;
        std::cout << "Mode sans post-processing activé" << std::endl;
        
        // MODE SANS POST-PROCESSING
        CameraUBO cameraUBO;
        TransformUBO transformUBO;
        if (!cameraUBO.Init()) {
            std::cerr << "Failed to initialize Camera UBO" << std::endl;
        }
        if (!transformUBO.Init()) {
            std::cerr << "Failed to initialize Transform UBO" << std::endl;
        }

        // Lier les UBOs aux shaders
        shaderColor.use();
        shaderColor.bindUBO("CameraMatrices", 0);
        shaderColor.bindUBO("TransformMatrices", 1);
        
        shaderTexture.use();
        shaderTexture.bindUBO("CameraMatrices", 0);
        shaderTexture.bindUBO("TransformMatrices", 1);
        
        shaderEnv.use();
        shaderEnv.bindUBO("CameraMatrices", 0);
        shaderEnv.bindUBO("TransformMatrices", 1);
        
        Model pluto("../assets/models/pluto.obj");
        Model moon("../assets/models/moon.obj");
        Model jupiter("../assets/models/jupiter.obj");

        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            processInput(window);

            glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);

            CameraUBO cameraUBO;
            TransformUBO transformUBO;
            if (!cameraUBO.Init()) {
                std::cerr << "Failed to initialize Camera UBO" << std::endl;
            }
            if (!transformUBO.Init()) {
                std::cerr << "Failed to initialize Transform UBO" << std::endl;
            }

            // Lier les UBOs aux shaders
            shaderColor.use();
            shaderColor.bindUBO("CameraMatrices", 0);
            shaderColor.bindUBO("TransformMatrices", 1);
            
            shaderTexture.use();
            shaderTexture.bindUBO("CameraMatrices", 0);
            shaderTexture.bindUBO("TransformMatrices", 1);
            
            shaderEnv.use();
            shaderEnv.bindUBO("CameraMatrices", 0);
            shaderEnv.bindUBO("TransformMatrices", 1);

            cameraUBO.UpdateMatrices(camera, (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

            // Rendu Pluto avec couleur simple
            shaderColor.use();
            shaderColor.setVec3("light_position", glm::vec3(0.0f, 5.0f, 10.0f));
            shaderColor.setVec3("light_ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
            shaderColor.setVec3("light_diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
            shaderColor.setVec3("light_specular", glm::vec3(1.0f, 1.0f, 1.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 0.0f));
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 0, 1));
            model = glm::scale(model, glm::vec3(2.0f));
            transformUBO.UpdateTransform(model);
            shaderColor.setVec3("u_Color", glm::vec3(0.7f, 0.65f, 0.6f));
            pluto.Draw(shaderColor);

            // Rendu Moon avec texture
            shaderTexture.use();
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, -20.0f));
            model = glm::scale(model, glm::vec3(5.0f));
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 1, 0));
            transformUBO.UpdateTransform(model);
            moon.Draw(shaderTexture);

            // Rendu Jupiter avec sa propre environment map
            shaderEnv.use();
            shaderEnv.setVec3("cameraPos", camera.Position);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, -5.0f, 10.0f));
            model = glm::scale(model, glm::vec3(7.0f));
            model= glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 1, 0));
            transformUBO.UpdateTransform(model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, jupiterCubemapTexture);
            jupiter.Draw(shaderEnv);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwTerminate();
        return 0;
    }
    
    if (!fragFile.good()) {
        std::cout << "ERREUR: effect.frag non trouvé!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    vertFile.close();
    fragFile.close();

    try {
        Shader effectShader("../assets/shaders/effect.vert", "../assets/shaders/effect.frag");
        std::cout << "Effect shader créé avec succès" << std::endl;

        // Initialisation les UBOs
        CameraUBO cameraUBO;
        TransformUBO transformUBO;
        if (!cameraUBO.Init()) {
            std::cerr << "Failed to initialize Camera UBO" << std::endl;
            glfwTerminate();
            return -1;
        }
        if (!transformUBO.Init()) {
            std::cerr << "Failed to initialize Transform UBO" << std::endl;
            glfwTerminate();
            return -1;
        }

        shaderColor.use();
        shaderColor.bindUBO("CameraMatrices", 0);
        shaderColor.bindUBO("TransformMatrices", 1);
        
        shaderTexture.use();
        shaderTexture.bindUBO("CameraMatrices", 0);
        shaderTexture.bindUBO("TransformMatrices", 1);
        
        shaderEnv.use();
        shaderEnv.bindUBO("CameraMatrices", 0);
        shaderEnv.bindUBO("TransformMatrices", 1);

        std::cout << "Initialisation du FBO..." << std::endl;
        if (!initFBO(window)) {
            std::cout << "Erreur initialisation FBO" << std::endl;
            glfwTerminate();
            return -1;
        }
        std::cout << "FBO initialisé avec succès" << std::endl;
        
        Model pluto("../assets/models/pluto.obj");
        Model moon("../assets/models/moon.obj");
        Model jupiter("../assets/models/jupiter.obj");
        
        std::cout << "Démarrage de la boucle principale..." << std::endl;

        // BOUCLE PRINCIPALE AVEC POST-PROCESSING
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            processInput(window);

            g_imguiManager.NewFrame();
            // Afficher l'interface
            g_imguiManager.ShowInterface(currentEffect, planetRotationSpeed);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            // Rendu de la scene dans le FBO
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glViewport(0, 0, width, height);
            
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Rendu skybox
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / height, 0.1f, 100.0f);
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
            
            cameraUBO.UpdateMatrices(camera, (float)width / height, 0.1f, 100.0f);

            // Rendu Pluto
            shaderColor.use();
            shaderColor.setVec3("light_position", glm::vec3(0.0f, 5.0f, 10.0f));
            shaderColor.setVec3("light_ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
            shaderColor.setVec3("light_diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
            shaderColor.setVec3("light_specular", glm::vec3(1.0f, 1.0f, 1.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 0.0f));
            model = glm::rotate(model, (float)glfwGetTime() * planetRotationSpeed, glm::vec3(1, 0, 1));
            model = glm::scale(model, glm::vec3(2.0f));
            transformUBO.UpdateTransform(model);
            shaderColor.setVec3("u_Color", glm::vec3(0.7f, 0.65f, 0.6f));
            pluto.Draw(shaderColor);

            // Rendu Moon
            shaderTexture.use();
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, -20.0f));
            model = glm::scale(model, glm::vec3(5.0f));
            model = glm::rotate(model, (float)glfwGetTime() * planetRotationSpeed, glm::vec3(1, 1, 0));
            transformUBO.UpdateTransform(model);
            moon.Draw(shaderTexture);

            // Rendu Jupiter
            shaderEnv.use();
            shaderEnv.setVec3("cameraPos", camera.Position);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, -5.0f, 10.0f));
            model = glm::scale(model, glm::vec3(7.0f));
            model = glm::rotate(model, (float)glfwGetTime() * planetRotationSpeed, glm::vec3(1, 1, 0));
            transformUBO.UpdateTransform(model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, jupiterCubemapTexture);
            jupiter.Draw(shaderEnv);

            // Post-processing
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
            
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            effectShader.use();
            effectShader.setVec2("u_textureSize", glm::vec2((float)width, (float)height));
            effectShader.setInt("u_texture", 0);
            effectShader.setInt("effectType", currentEffect);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, FBOColorTexture);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            g_imguiManager.Render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // Nettoyage
        g_imguiManager.Shutdown();
        cleanupFBO();
        
    } catch (const std::exception& e) {
        std::cout << "Erreur lors de la création du shader effect: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}
