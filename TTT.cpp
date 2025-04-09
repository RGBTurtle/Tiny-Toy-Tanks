#include <stdio.h>
#include "src/include/glad/gl.h"
#include "src/include/GLFW/glfw3.h"
#include "vertexdata.h"
#include <string>
#include <fstream>
#include <sstream>

//#Defines--------------------------------------------------------------
const double targetFrameTime = 1.0 / 150.0;

//Variables-------------------------------------------------------------
double frametime;
double FPStime;
int FPS;
unsigned int buffer;
//Functions-------------------------------------------------------------

struct shader_program_source {
    std::string VertexSource;
    std::string FragmentSource;
};


static shader_program_source parseshader(const std::string file_path){
    std::ifstream stream(file_path);

    if (stream.is_open()) {
        printf("040, file found >:)\n");
    }else {
        printf("File not found!\n");
    }

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
    window = glfwCreateWindow(750, 750, "Tiny Tanks", NULL, NULL);  //GLFW
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
    
    //create shaders
    shader_program_source source = parseshader("res/shaders/Basic.glsl");

    unsigned int shader = createshader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)){
        if (FPStime <= glfwGetTime()-1){
            printf("%i", FPS);
            FPStime = glfwGetTime();
            FPS = 0;
        }else{FPS++;}


        // Render!!
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}