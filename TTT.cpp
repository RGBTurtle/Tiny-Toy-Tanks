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
int windowWidth = 800;
int windowHeight = 800;

const double targetFrameTime = 1.0 / 180.0;

//Variables-------------------------------------------------------------
int squarex;
int squarey;
int slidex;
int slidey;

double frametime;
double FPStime;
int FPS;
unsigned int buffer;
//Functions-------------------------------------------------------------
void slidehandle(){
    if (slidex > 0){
        slidex--;        
    }else if(slidex < 0){
        slidex++;
    }

    if (slidey > 0){
        slidey--;        
    }else if(slidey < 0){
        slidey++;
    }
    squarex += slidex;
    squarey += slidey;
}

void gameloop(){
    glfwPollEvents();
    slidehandle();
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
/*escape*/static void escape(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
/*W*/static void keyw(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        slidey++;
}
/*A*/static void keya(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        slidex--;
}
/*S*/static void keys(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        slidey--;
}
/*D*/static void keyd(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        slidex++;
};




int main() { //---------------------------------------------------------

    //intialize GLFW

    if (!glfwInit()){   //GLFW

        printf("glfw failed to initialize!");
        return 0;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
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

    glfwSetKeyCallback(window, escape);
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

    float rotate = glm::radians(45.0f);
    glm::mat4 proj = glm::ortho(float(-windowWidth * 0.5), float(windowWidth * 0.5), float(-windowHeight * 0.5), float(windowHeight * 0.5));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.0f, 0.0f, 1.0f));


    glm::mat4 MVP = proj * view * model;
    //uniforms


    
    //create shaders
    shader_program_source source = parseshader("res/shaders/Basic.glsl");

    unsigned int shader = createshader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    GLint location = glGetUniformLocation(shader, "u_MVP");

    glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

    frametime = glfwGetTime();
    FPStime = glfwGetTime();
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)){
        while (glfwGetTime() - targetFrameTime >= frametime){
            gameloop();
            frametime += targetFrameTime;
        }

        glfwGetWindowFrameSize(window, NULL, &windowHeight, &windowWidth, NULL);
        model = glm::rotate(glm::mat4(1.0f), float(glm::radians(glfwGetTime() * 50)), glm::vec3(0.0f, 0.0f, 1.0f));
        MVP = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &MVP[0][0]);

        // Render!!
        glClearColor(0.1,0.1,0.1,1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}