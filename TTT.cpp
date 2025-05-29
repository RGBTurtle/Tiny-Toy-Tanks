#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/include/glad/gl.h"
#include "src/include/GLFW/glfw3.h"
#include "vertexdata.h"

#include "src/include/glm/glm.hpp"
#include "src/include/glm/gtc/matrix_transform.hpp"
//stucts/classes

//#Defines--------------------------------------------------------------
void pollKeys();
GLFWwindow* window;

const double targetFrameTime = 1.0 / 180.0;

//Variables-------------------------------------------------------------
double squareRot;
double squareRotGoal;

double camDeltax;
double camDeltay;

int windowWidth = 1920;
int windowHeight = 1080;
glm::vec2 thrust = glm::vec2(0.0f, 0.0f);
glm::vec2 tankpos = glm::vec2(0.0f, 0.0f);
glm::vec2 slide = glm::vec2(0.0f, 0.0f);
double squareRotSpeed = 0.0075 * M_PI;
float movSpeed;

double frametime;
double FPStime;
unsigned int buffer;
//Functions-------------------------------------------------------------

void tankTurnFasterFinder(){
    if (squareRotGoal < 0){squareRotGoal += 2.0f * M_PI;}

    if (abs(squareRot - squareRotGoal) > (0.6f * M_PI)){
        if (squareRotGoal < M_PI){squareRotGoal += M_PI;} else {squareRotGoal -= M_PI;}

        if (abs(squareRot - squareRotGoal) > (0.4f * M_PI)){
            if (squareRot < M_PI){squareRot += M_PI;} else {squareRot -= M_PI;}
        }
    }
    
    if (squareRot > squareRotGoal + squareRotSpeed) {
        squareRot -= squareRotSpeed;
        movSpeed = movSpeed * 0.6;
    } else if (squareRot < squareRotGoal - squareRotSpeed) {
        squareRot += squareRotSpeed;
        movSpeed = movSpeed * 0.6;
    } else {squareRot = squareRotGoal;}
};

void movHandle(){
    movSpeed = 12;
    if (glm::length(thrust) > 0.0f){thrust = glm::normalize(thrust);}
    squareRotGoal = atan2(thrust.x, thrust.y);
    if (glm::length(thrust) <= 0.0f){squareRotGoal = squareRot;}
    tankTurnFasterFinder();

    tankpos.x += thrust.x * movSpeed;
    tankpos.y += thrust.y * movSpeed;
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
}







void drawObject(unsigned int va, unsigned int ib){
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

    window = glfwCreateWindow(windowWidth, windowHeight, "Tiny Tanks", NULL, NULL);  //GLFW
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

    glfwSwapInterval(1);

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

    //glm::mat4 proj = glm::ortho(float(-windowWidth), float(windowWidth), float(-windowHeight), float(windowHeight), -800.0f, 800.0f);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), float(windowWidth) / float(windowHeight), 0.1f, 12800.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2400.0f));
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

    // glEnable(GL_CULL_FACE);  
    // glFrontFace(GL_CCW);  

    frametime = glfwGetTime();
    FPStime = glfwGetTime();
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)){
        while (glfwGetTime() - targetFrameTime >= frametime){


            gameloop();
            frametime += targetFrameTime;
        }
        
        
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(float(tankpos[0]), sin(glfwGetTime()*6)*15, float(-tankpos[1])));
        model = glm::rotate(model, float(-squareRot), glm::vec3(0.0f, 1.0f, 0.0f));

        MVP = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &MVP[0][0]);

        // Render!!
        glfwPollEvents();
        glClearColor(0.15,0.15,0.15,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawObject(buffer, IBO);
        glfwSwapBuffers(window);
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}