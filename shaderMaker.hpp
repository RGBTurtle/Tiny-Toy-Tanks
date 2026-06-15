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

struct Shader{
    unsigned int ID;

    Shader(const std::string file_path){
        shader_program_source source = parseshader(file_path);
        ID = createshader(source.VertexSource, source.FragmentSource);
    }

    void use(){
        glUseProgram(ID);
    }

    void setFloat(const std::string &name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setInt(const std::string &name, int value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }   

    void setBool(const std::string &name, bool value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), int(value)); 
    }

    void setMatrix(const std::string &name, glm::mat4 value) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1 ,GL_FALSE, &value[0][0]); 
    }

};