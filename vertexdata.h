#include "src/include/glm/glm.hpp"
#include "src/include/glm/gtc/matrix_transform.hpp"
#include <string>
#include "src/include/assimp/Importer.hpp"
#include "src/include/assimp/scene.h"
#include "src/include/assimp/postprocess.h"
#include <vector>
#include "src/include/glad/gl.h"

struct vertex{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec2 texCoords = glm::vec2(0.0f, 0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
};

struct texture{
    unsigned int id;
    std::string type;
};

class mesh{
    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }
    public:
        std::vector<vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<texture>      textures;

        void Draw(unsigned int shader){
            
        };

    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
  
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);  

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
             &indices[0], GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);	
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);	
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);	
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoords));

            glBindVertexArray(0);
        }
};

float vertices[] = {
    //positions                  //colours
    -50.0f, -50.0f, -50.0f,   1.0f, 1.0f, 1.0f,
    +50.0f, -50.0f, -50.0f,   1.0f, 1.0f, 1.0f,
    -50.0f, +50.0f, -50.0f,   1.0f, 1.0f, 1.0f,
    +50.0f, +50.0f, -50.0f,   1.0f, 1.0f, 1.0f,
    -50.0f, -50.0f, +50.0f,   0.0f, 0.0f, 0.0f,
    +50.0f, -50.0f, +50.0f,   0.0f, 0.0f, 0.0f,
    -50.0f, +50.0f, +50.0f,   0.0f, 0.0f, 0.0f,
    +50.0f, +50.0f, +50.0f,   0.0f, 0.0f, 0.0f,
};
 
unsigned int indicies [] = {
    0, 1, 2,
    1, 2, 3,
    0, 2, 4,
    2, 4, 6,
    0, 1, 4,
    1, 4, 5,
    1, 3, 5,
    3, 5, 7,
    2, 3, 6,
    3, 6, 7,
    4, 5, 6,
    5, 6, 7  
};