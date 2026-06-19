#define _USE_MATH_DEFINES
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/include/glad/gl.h"
#include "src/include/GLFW/glfw3.h"

#include "vertexdata.hpp"
#include "trinkets.hpp"

#include "src/include/glm/glm.hpp"
#include "src/include/glm/gtc/matrix_transform.hpp"

#include "src/include/assimp/Importer.hpp"
#include "src/include/assimp/scene.h"
#include "src/include/assimp/postprocess.h"

#include "shaderMaker.hpp"
//stucts/classes

//#Defines--------------------------------------------------------------
//Shader shader("res/shaders/Basic.glsl");
glm::mat4 proj;
glm::mat4 view;
glm::mat4 MVP;
void pollKeys();
GLFWwindow* window;

const double targetFrameTime = 1.0 / 180.0;

//Variables-------------------------------------------------------------
double squareRot;
double squareRotGoal;

//int windowWidth = 1920;
//int windowHeight = 1080;
glm::vec2 thrust = glm::vec2(0.0f, 0.0f);
glm::vec2 tankpos = glm::vec2(0.0f, 0.0f);
glm::vec2 slide = glm::vec2(0.0f, 0.0f);
double squareRotSpeed = 0.008 * M_PI;
float movSpeed;

double frametime;
unsigned int buffer;

unsigned int fps;
double fpsOldTime;
//Functions-------------------------------------------------------------
void fpstest(){
    fps++;
    if (fpsOldTime <= glfwGetTime() - 1){
        printf("%i\n", fps);
        fps = 0;
        fpsOldTime = glfwGetTime();
    }
}

void boundWalls(){
    if (tankpos.x > 750.0f){tankpos.x -= tankpos.x - 750.0f;}
    if (tankpos.x < -750.0f){tankpos.x -= tankpos.x + 750.0f;}
    if (tankpos.y > 750.0f){tankpos.y -= tankpos.y - 750.0f;}
    if (tankpos.y < -750.0f){tankpos.y -= tankpos.y + 750.0f;}
}


void tankTurnFasterFinder() {
    if (abs(squareRot - squareRotGoal) > M_PI) { 
        if (abs(squareRot - squareRotGoal + M_PI * 2) > abs(squareRot - squareRotGoal - M_PI * 2)) { squareRot = squareRot - M_PI * 2; } else { squareRot = squareRot + M_PI * 2; } 
    }
    if (abs(squareRot - squareRotGoal) > M_PI * 0.4) { 
        if (abs(squareRot - squareRotGoal + M_PI) > abs(squareRot - squareRotGoal - M_PI)) { squareRotGoal += M_PI; } else { squareRotGoal -= M_PI; } 
    }

    if (squareRot > squareRotGoal + squareRotSpeed) {
        squareRot -= squareRotSpeed;
        movSpeed *= 0.2;
    } else if (squareRot < squareRotGoal - squareRotSpeed) {
        squareRot += squareRotSpeed;
        movSpeed *= 0.2;
    } else {squareRot = squareRotGoal;}
};

void movHandle(){
    movSpeed = 15;
    //proform all additive movement buffs
    if (goldenRodDust.have){movSpeed += goldenRodDustCoefficiant;}


    //proform all multiplicative movement buffs
    if (supercharger.have){movSpeed *= superchargerCoefficiant;}
    

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){movSpeed = 7.5;}
    //-----
    if (glm::length(thrust) > 0.0f){thrust = glm::normalize(thrust);}
    squareRotGoal = atan2(thrust.x, thrust.y);
    if (glm::length(thrust) <= 0.0f){squareRotGoal = squareRot;}
    tankTurnFasterFinder();

    tankpos.x += thrust.x * movSpeed * 0.25;
    tankpos.y += thrust.y * movSpeed * 0.25;
    boundWalls();
    thrust = glm::vec2(0.0f, 0.0f);
}

void gameloop(){
    pollKeys();
    movHandle();
}

void error_callback(int error, const char* description) //GLFW
{
    fprintf(stderr, "Error: %s\n", description);
}

//Keys------------------------------------------------------------------
void pollKeys(){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){glfwSetWindowShouldClose(window, GLFW_TRUE);}
    thrust = glm::vec2(0,0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){thrust.y += 1;}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){thrust.x -= 1;}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){thrust.y -= 1;}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){thrust.x += 1;}
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){supercharger.have, goldenRodDust.have = 1;}
}

void drawObject(unsigned int va, unsigned int ib, Shader shader){
    glBindVertexArray(va);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

int main() { //---------------------------------------------------------
    //intialize GLFW
    if (!glfwInit()){   //GLFW

        printf("glfw failed to initialize!");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    glfwWindowHint(GLFW_SAMPLES, 2);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    window = glfwCreateWindow(mode->width, mode->height, "Tiny Toy Tanks", NULL, NULL);  //GLFW
    if (!window){

        printf("window failed to create!");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowSize(window, mode->width, mode->height);

    // Initialize GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("failed to initialize glad!");
        return 0;
    }

    glfwSwapInterval(0);

    //OpenGL buffer binding
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &buffer);  
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int IBO;
    glGenBuffers(1, &IBO);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), float(mode->width) / float(mode->height), 0.1f, 12800.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1100.0f));
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 MVP = proj * view * model;


    //create shaders
    Shader shader("res/shaders/Basic.glsl");
    shader.use();

    GLint location = glGetUniformLocation(shader.ID, "u_MVP");

    glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  

    glClearColor(0.05,0.05,0.1,1.0);

    frametime = glfwGetTime();
    fpsOldTime = glfwGetTime();



    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        while (glfwGetTime() - targetFrameTime >= frametime){

            gameloop();
            frametime += targetFrameTime;
        }
        fpstest();

        model = glm::translate(glm::mat4(1.0f), glm::vec3(float(tankpos[0]), sin(glfwGetTime()*4)*6, float(-tankpos[1])));
        model = glm::rotate(model, float(-squareRot), glm::vec3(0.0f, 1.0f, 0.0f));

        MVP = proj * view * model;
        
        // Render!!
        glfwPollEvents();
        shader.setMatrix("u_MVP", MVP);
        drawObject(buffer, IBO, shader);
        glfwSwapBuffers(window);
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    glDeleteProgram(shader.ID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}