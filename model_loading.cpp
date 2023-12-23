#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(vector<std::string> faces);

// Consts
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 3.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// misc vars
// Boolean: If true, control the imgui. If false, control the camera
bool imgui_visible = false;
// Boolean: If true, don't update imgui control pref. If false, do.
bool shift_active = false;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Load", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Imgui Lighting variables
    //Ambient and Directional
    float ambientStrength = 0.160f;
    glm::vec3 ambientColour = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightDirection(0.1f, -1.0f, 0.7f);
    glm::vec3 dirLightColour = glm::vec3(1.0f, 0.1f, 0.1f);

    // Point Light 1
    glm::vec3 pointLight1Colour = glm::vec3(1.0f, 0.305f, 0.305f);
    glm::vec3 pointLight1Position = glm::vec3(-15.0f, 4.0f, 0.0f);
    float pointLight1Constant = 0.170f;
    float pointLight1Linear = 0.103f;
    float pointLight1Quadratic = 0.064f;

    // Point Light 2
    glm::vec3 pointLight2Colour = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 pointLight2Position = glm::vec3(6.0f, 9.0f, 0.0f);
    float pointLight2Constant = 0.170f;
    float pointLight2Linear = 0.103f;
    float pointLight2Quadratic = 0.064f;

    // Fog
    glm::vec3 fogColour = glm::vec3(1.0f, 0.25f, 0.25f);
    float fogDensity = 0.160f;
    float fogStart = 60.0f;
    float fogEnd = 40.0f;

    //Skybox
    float skyboxVertices[] = {       
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        "cubemap/posx.png",
        "cubemap/negx.png",
        "cubemap/posy.png",
        "cubemap/negy.png",
        "cubemap/posz.png",
        "cubemap/negz.png"
    };
    
    glEnable(GL_DEPTH_TEST);

    // Build/Compile Shaders
    // -------------------------
    Shader ourShader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    
    // Loading Robot1 (all parts)
    // Robot1Body is the main model, the rest are objects attached
    Model robot1Body("models/robot/robot_body.obj");
    
    // Adding child parts
    Model robot1LeftArm("models/robot/robot_armL.obj");
    Model robot1RightArm("models/robot/robot_armR.obj");
    Model robot1Head("models/robot/robot_head.obj");

    // Loading Robot2 (all parts)
    // Robot1Body is the main model, the rest are objects attached
    Model robot2Body("models/robot/robot_body.obj");
    
    // Adding child parts
    Model robot2LeftArm("models/robot/robot_armL.obj");
    Model robot2RightArm("models/robot/robot_armR.obj");
    Model robot2Head("models/robot/robot_head.obj");

    // Loading Robot2 (all parts)
    // Robot1Body is the main model, the rest are objects attached
    Model robot3Body("models/robot/robot_body.obj");
    
    // Adding child parts
    Model robot3LeftArm("models/robot/robot_armL.obj");
    Model robot3RightArm("models/robot/robot_armR.obj");
    Model robot3Head("models/robot/robot_head.obj");

    // Loading Robot2 (all parts)
    // Robot1Body is the main model, the rest are objects attached
    Model robot4Body("models/robot/robot_body.obj");
    
    // Adding child parts
    Model robot4LeftArm("models/robot/robot_armL.obj");
    Model robot4RightArm("models/robot/robot_armR.obj");
    Model robot4Head("models/robot/robot_head.obj");

    // Loading Spire Base
    Model spireBase("models/spirebase/spirebase.obj");

    // Loading Spire Top
    Model spireTop("models/spiretop/spiretop.obj");

    // Loading Buildings
    Model building1("models/buildings/Building01.obj");
    Model building2("models/buildings/Building01.obj");

    // Loading Floor Plane
    Model floor("models/floor/floor.obj");

    ourShader.use();
    ourShader.setFloat("ambientStrength", ambientStrength);
    ourShader.setVec3("ambientColour", ambientColour);
    ourShader.setInt("main", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    //Load cube map
    unsigned int cubemapTexture = loadCubemap(faces);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Initialize seperate imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Lighting Controls");

        // Ambient Light Controller
        ImGui::ColorEdit3("Ambient Colour", (float*)&ambientColour);
        ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
        ImGui::ColorEdit3("Directional Light Colour", (float*)&dirLightColour);
        ImGui::SliderFloat("Directional Light X", &lightDirection.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Directional Light Y", &lightDirection.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Directional Light Z", &lightDirection.z, -1.0f, 1.0f);
        ImGui::End();

        // Point Light 1 Controller
        ImGui::Begin("Point Light 0");
        ImGui::ColorEdit3("Point Light 0 Colour", (float*)&pointLight1Colour);
        ImGui::SliderFloat("Light X", &pointLight1Position.x, -200.0f, 200.0f);
        ImGui::SliderFloat("Light Y", &pointLight1Position.y, -200.0f, 200.0f);
        ImGui::SliderFloat("Light Z", &pointLight1Position.z, -200.0f, 200.0f);
        ImGui::SliderFloat("Point Light 0 Constant", &pointLight1Constant, 0.0f, 1.0f);
        ImGui::SliderFloat("Point Light 0 Linear", &pointLight1Linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Point Light 0 Quadratic", &pointLight1Quadratic, 0.0f, 1.0f);
        ImGui::End();

        // Point Light 2 Controller
        ImGui::Begin("Point Light 1");
        ImGui::ColorEdit3("Point Light 1 Colour", (float*)&pointLight2Colour);
        ImGui::SliderFloat("Light X", &pointLight2Position.x, -200.0f, 200.0f);
        ImGui::SliderFloat("Light Y", &pointLight2Position.y, -200.0f, 200.0f);
        ImGui::SliderFloat("Light Z", &pointLight2Position.z, -200.0f, 200.0f);
        ImGui::SliderFloat("Point Light 1 Constant", &pointLight2Constant, 0.0f, 1.0f);
        ImGui::SliderFloat("Point Light 1 Linear", &pointLight2Linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Point Light 1 Quadratic", &pointLight2Quadratic, 0.0f, 1.0f);
        ImGui::End();

        // Fog Controller
        ImGui::Begin("Fog Controls");
        ImGui::ColorEdit3("Fog Colour", (float*)&fogColour);
        ImGui::SliderFloat("Fog Density", &fogDensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Fog Start", &fogStart, 0.0f, 100.0f);
        ImGui::SliderFloat("Fog End", &fogEnd, 0.0f, 100.0f);
        ImGui::End();

        // Enable shaders
        ourShader.use();
       
        //Set Shader uniforms 
        ourShader.setFloat("ambientStrength", ambientStrength);
        ourShader.setVec3("ambientColour", ambientColour);
        ourShader.setVec3("viewPos", camera.Position); 

        ourShader.setVec3("dirColour", dirLightColour);
        ourShader.setVec3("lightDirection", lightDirection);

        ourShader.setVec3("pointLights[0].colour", pointLight1Colour);
        ourShader.setVec3("pointLights[0].position", pointLight1Position);
        ourShader.setFloat("pointLights[0].constant", pointLight1Constant);
        ourShader.setFloat("pointLights[0].linear", pointLight1Linear);
        ourShader.setFloat("pointLights[0].quadratic", pointLight1Quadratic);

        ourShader.setVec3("pointLights[1].colour", pointLight2Colour);
        ourShader.setVec3("pointLights[1].position", pointLight2Position);
        ourShader.setFloat("pointLights[1].constant", pointLight2Constant);
        ourShader.setFloat("pointLights[1].linear", pointLight2Linear);
        ourShader.setFloat("pointLights[1].quadratic", pointLight2Quadratic);

        ourShader.setVec3("fogColour", fogColour);
        ourShader.setFloat("fogDensity", fogDensity);
        ourShader.setFloat("fogStart", fogStart);
        ourShader.setFloat("fogEnd", fogEnd);

        // View/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Matrix for each loaded model
        glm::mat4 model_robot1 = glm::mat4(1.0f);
        glm::mat4 model_leftArm1 = glm::mat4(1.0f);
        glm::mat4 model_rightArm1 = glm::mat4(1.0f);
        glm::mat4 model_head1 = glm::mat4(1.0f);

        glm::mat4 model_robot2 = glm::mat4(1.0f);
        glm::mat4 model_leftArm2 = glm::mat4(1.0f);
        glm::mat4 model_rightArm2 = glm::mat4(1.0f);
        glm::mat4 model_head2 = glm::mat4(1.0f);

        glm::mat4 model_robot3 = glm::mat4(1.0f);
        glm::mat4 model_leftArm3 = glm::mat4(1.0f);
        glm::mat4 model_rightArm3 = glm::mat4(1.0f);
        glm::mat4 model_head3 = glm::mat4(1.0f);

        glm::mat4 model_robot4 = glm::mat4(1.0f);
        glm::mat4 model_leftArm4 = glm::mat4(1.0f);
        glm::mat4 model_rightArm4 = glm::mat4(1.0f);
        glm::mat4 model_head4 = glm::mat4(1.0f);

        glm::mat4 model_spirebase = glm::mat4(1.0f);
        glm::mat4 model_spiretop = glm::mat4(1.0f);

        glm::mat4 model_floor = glm::mat4(1.0f);
        glm::mat4 model_building1 = glm::mat4(1.0f);
        glm::mat4 model_building2 = glm::mat4(1.0f);

        // moving spirebase, floor to center of screen
        model_spirebase = glm::translate(model_spirebase, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model_floor = glm::translate(model_floor, glm::vec3(0.0f, 0.0f, 0.0f)); 

        // move and rotate buildings
        model_building1 = glm::translate(model_building1, glm::vec3(-25.0f, -1.0f, 0.0f));
        model_building1 = glm::rotate(model_building1, 1.5708f, glm::vec3(0.0f, 1.0f, 0.0f));
        model_building1 = glm::scale(model_building1, glm::vec3(4.0f, 2.0f, 2.0f));
        
        model_building2 = glm::translate(model_building2, glm::vec3(0.0f, -1.0f, -25.0f));
        model_building2 = glm::scale(model_building2, glm::vec3(4.0f, 2.0f, 2.0f));

        //move spiretop aside
        model_spiretop = glm::translate(model_spiretop, glm::vec3(70.0f, 0.0f, 0.0));
        model_spiretop = glm::scale(model_spiretop, glm::vec3(2.0f, 1.0f, 1.0f));

        //Crowd of four robots walking
        //Move to Beginning positions
        model_robot1 = glm::translate(model_robot1, glm::vec3(-8.472f, 0.0f, -4.784f)); 
        model_robot2 = glm::translate(model_robot2, glm::vec3(-12.472f, 0.0f, -4.784f)); 
        model_robot3 = glm::translate(model_robot3, glm::vec3(-8.472f, 0.0f, -8.784f)); 
        model_robot4 = glm::translate(model_robot4, glm::vec3(-12.472f, 0.0f, -8.784f));

        //Make models 'walk'
        float walkVelocity = 0.6f; //Velocity for model arm rotation
        model_robot1 = glm::translate(model_robot1, glm::vec3(0.0f, 0.0f, static_cast<float>(glfwGetTime()*walkVelocity))); 
        model_robot2 = glm::translate(model_robot2, glm::vec3(0.0f, 0.0f, static_cast<float>(glfwGetTime()*walkVelocity)));
        model_robot3 = glm::translate(model_robot3, glm::vec3(0.0f, 0.0f, static_cast<float>(glfwGetTime()*walkVelocity)));
        model_robot4 = glm::translate(model_robot4, glm::vec3(0.0f, 0.0f, static_cast<float>(glfwGetTime()*walkVelocity)));

        //Move child parts to Beginning positions
        model_head1 = glm::translate(model_robot1, glm::vec3(0.0f, 0.0f, 0.0f));
        model_head2 = glm::translate(model_robot2, glm::vec3(0.0f, 0.0f, 0.0f));
        model_head3 = glm::translate(model_robot3, glm::vec3(0.0f, 0.0f, 0.0f));
        model_head4 = glm::translate(model_robot4, glm::vec3(0.0f, 0.0f, 0.0f));

        model_leftArm1 = glm::translate(model_robot1, glm::vec3(0.0f, 0.0f, 0.0f));
        model_leftArm2 = glm::translate(model_robot2, glm::vec3(0.0f, 0.0f, 0.0f));
        model_leftArm3 = glm::translate(model_robot3, glm::vec3(0.0f, 0.0f, 0.0f));
        model_leftArm4 = glm::translate(model_robot4, glm::vec3(0.0f, 0.0f, 0.0f));

        model_rightArm1 = glm::translate(model_robot1, glm::vec3(0.0f, 0.0f, 0.0f));
        model_rightArm2 = glm::translate(model_robot2, glm::vec3(0.0f, 0.0f, 0.0f));
        model_rightArm3 = glm::translate(model_robot3, glm::vec3(0.0f, 0.0f, 0.0f));
        model_rightArm4 = glm::translate(model_robot4, glm::vec3(0.0f, 0.0f, 0.0f));

        //Swing robot arms
        float armVelocity = 3.0f; //Velocity for model arm rotation
        model_leftArm1 = glm::rotate(model_leftArm1, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_leftArm2 = glm::rotate(model_leftArm2, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_leftArm3 = glm::rotate(model_leftArm3, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_leftArm4 = glm::rotate(model_leftArm4, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));

        model_rightArm1 = glm::rotate(model_rightArm1, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_rightArm2 = glm::rotate(model_rightArm2, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_rightArm3 = glm::rotate(model_rightArm3, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        model_rightArm4 = glm::rotate(model_rightArm4, static_cast<float>(0.2f *(sin(glfwGetTime()))) * (armVelocity), glm::vec3(0.0f, 1.0f, 0.0f));
        

        // Set the shaders for models
        ourShader.setMat4("model", model_robot1);
        robot1Body.Draw(ourShader);
        ourShader.setMat4("model", model_leftArm1);
        robot1LeftArm.Draw(ourShader);
        ourShader.setMat4("model", model_rightArm1);
        robot1RightArm.Draw(ourShader);
        ourShader.setMat4("model", model_head1);
        robot1Head.Draw(ourShader);

        ourShader.setMat4("model", model_robot2);
        robot2Body.Draw(ourShader);
        ourShader.setMat4("model", model_leftArm2);
        robot2LeftArm.Draw(ourShader);
        ourShader.setMat4("model", model_rightArm2);
        robot2RightArm.Draw(ourShader);
        ourShader.setMat4("model", model_head2);
        robot2Head.Draw(ourShader);

        ourShader.setMat4("model", model_robot3);
        robot3Body.Draw(ourShader);
        ourShader.setMat4("model", model_leftArm3);
        robot3LeftArm.Draw(ourShader);
        ourShader.setMat4("model", model_rightArm3);
        robot3RightArm.Draw(ourShader);
        ourShader.setMat4("model", model_head3);
        robot3Head.Draw(ourShader);

        ourShader.setMat4("model", model_robot4);
        robot4Body.Draw(ourShader);
        ourShader.setMat4("model", model_leftArm4);
        robot4LeftArm.Draw(ourShader);
        ourShader.setMat4("model", model_rightArm4);
        robot4RightArm.Draw(ourShader);
        ourShader.setMat4("model", model_head4);
        robot4Head.Draw(ourShader);

        ourShader.setMat4("model", model_spiretop);
        spireTop.Draw(ourShader);

        ourShader.setMat4("model", model_spirebase);
        spireBase.Draw(ourShader);

        ourShader.setMat4("model", model_building1);
        building1.Draw(ourShader);
        ourShader.setMat4("model", model_building2);
        building2.Draw(ourShader);

        ourShader.setMat4("model", model_floor);
        floor.Draw(ourShader);

        // Draw skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // If user holds shift button imgui window is visible, else invisible
        if (imgui_visible == false) {
            ImGui::GetStyle().Alpha = 0.0f;
        }
        else {
            ImGui::GetStyle().Alpha = 1.0f;
        }

        // Rendering Imgui
        // (Your code clears your framebuffer, renders your other stuff etc.)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents(); // polling IO events
    }

    // Shutdown Imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // If user holds down shift, imgui window and user cursor is visible.
    // Shift_active exists so it doesn't update every frame.
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && imgui_visible == false && shift_active == false){
        imgui_visible = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        shift_active = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && imgui_visible == true && shift_active == false){
        imgui_visible = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        shift_active = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        shift_active = false;
    }
    // Move camera if imgui not visible
    
    if (!imgui_visible)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            camera.ProcessKeyboard(UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
            camera.ProcessKeyboard(DOWN, deltaTime);
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Only move camera when imgui window is closed
    if (imgui_visible == false)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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