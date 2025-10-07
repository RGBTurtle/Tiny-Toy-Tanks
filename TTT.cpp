#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdint.h>

#include "src/include/glad/gl.h"
#include "src/include/GLFW/glfw3.h"

#include "vertexdata.h"
#include "trinkets.hpp"

#include "src/include/glm/glm.hpp"
#include "src/include/glm/gtc/matrix_transform.hpp"

#include "src/include/assimp/Importer.hpp"
#include "src/include/assimp/scene.h"
#include "src/include/assimp/postprocess.h"
//stucts/classes

//#Defines--------------------------------------------------------------
unsigned int shader;
glm::mat4 proj;
glm::mat4 view;
glm::mat4 MVP;
void pollKeys();
GLFWwindow* window;

const double targetFrameTime = 1.0 / 180.0;

//Variables-------------------------------------------------------------
double squareRot;
double squareRotGoal;

int windowWidth = 1920;
int windowHeight = 1080;
glm::vec2 thrust = glm::vec2(0.0f, 0.0f);
glm::vec2 tankpos = glm::vec2(0.0f, 0.0f);
glm::vec2 slide = glm::vec2(0.0f, 0.0f);
double squareRotSpeed = 0.006 * M_PI;
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
    //NOT CHASE ROTATE FIX
    if (abs(squareRot - squareRotGoal) > M_PI) { 
        if (abs(squareRot - squareRotGoal + M_PI * 2) > abs(squareRot - squareRotGoal - M_PI * 2)) { squareRot = squareRot - M_PI * 2; } else { squareRot = squareRot + M_PI * 2; } 
    }
    if (abs(squareRot - squareRotGoal) > M_PI * 0.4) { 
        if (abs(squareRot - squareRotGoal + M_PI) > abs(squareRot - squareRotGoal - M_PI)) { squareRotGoal += M_PI; } else { squareRotGoal -= M_PI; } 
    }

    if (squareRot > squareRotGoal + squareRotSpeed) {
        squareRot -= squareRotSpeed;
        movSpeed -= 12;
    } else if (squareRot < squareRotGoal - squareRotSpeed) {
        squareRot += squareRotSpeed;
        movSpeed -= 12;
    } else {squareRot = squareRotGoal;}
};

void movHandle(){
    movSpeed = 15;
    //proform all additive movement buffs
    if (goldenRodDust.have){movSpeed += 3;}


    //proform all multiplicative movement buffs
    if (supercharger.have){movSpeed *= 1.15;}
    

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


struct shader_program_source {
    std::string VertexSource;
    std::string FragmentSource;
};


static shader_program_source parseshader(const std::string file_path){
    std::ifstream stream(file_path);

    enum class shadertype
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    shadertype type = shadertype::NONE;
    while (std::getline(stream, line)){

        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos){
                type = shadertype::VERTEX;
            }else if (line.find("fragment") != std::string::npos){
                type = shadertype::FRAGMENT;
            }
        }else{
            ss[int(type)] << line << '\n';
        }
    }
    glm::mat4x4 projection;

    return { ss[0].str(), ss[1].str() };
}

static unsigned int compileshader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (!result){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetShaderInfoLog(id, length, &length, message);
        printf("failed to compile shader :)");
        printf("%s", message);

        glDeleteShader(id);
        return 0;
    };

    return id;
};

static unsigned int createshader(const std::string& vertexshader, const std::string& fragmentshader){
    unsigned int program = glCreateProgram();
    unsigned int vs = compileshader(GL_VERTEX_SHADER, vertexshader);
    unsigned int fs = compileshader(GL_FRAGMENT_SHADER, fragmentshader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
};

void error_callback(int error, const char* description) //GLFW
{
    fprintf(stderr, "Error: %s\n", description);
}

//Keys------------------------------------------------------------------
void pollKeys(){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){glfwSetWindowShouldClose(window, GLFW_TRUE);}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){thrust.y =  1;}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){thrust.x = -1;}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){thrust.y = -1;}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){thrust.x =  1;}
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){supercharger.have, goldenRodDust.have = 1;}
}







void drawObject(glm::mat4 objmodel, unsigned int va, unsigned int ib){
    MVP = proj * view * objmodel;
    glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &MVP[0][0]);
    glBindVertexArray(va);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
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
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(windowWidth, windowHeight, "Tiny Toy Tanks", NULL, NULL);  //GLFW
    if (!window){

        printf("window failed to create!");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), float(windowWidth) / float(windowHeight), 0.1f, 12800.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1100.0f));
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 MVP = proj * view * model;
    //uniforms


    
    //create shaders
    shader_program_source source = parseshader("res/shaders/Basic.glsl");

    unsigned int shader = createshader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    GLint location = glGetUniformLocation(shader, "u_MVP");

    glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  

    frametime = glfwGetTime();
    fpsOldTime = glfwGetTime();
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)){
        while (glfwGetTime() - targetFrameTime >= frametime){

            gameloop();
            frametime += targetFrameTime;
        }
        fpstest();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(float(tankpos[0]), sin(glfwGetTime()*4)*6, float(-tankpos[1])));
        model = glm::rotate(model, float(-squareRot), glm::vec3(0.0f, 1.0f, 0.0f));

        MVP = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &MVP[0][0]);

        // Render!!
        glfwPollEvents();
        glClearColor(0.1,0.1,0.1,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawObject(glm::translate(glm::mat4(1.0f), glm::vec3(float(tankpos[0]), sin(glfwGetTime()*4)*6, float(-tankpos[1]))), buffer, IBO);
        glfwSwapBuffers(window);
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}