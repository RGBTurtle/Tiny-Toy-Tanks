#include "src/include/glm/glm.hpp"
#include "src/include/glm/gtc/matrix_transform.hpp"
#include <string>
#include "src/include/assimp/Importer.hpp"
#include "src/include/assimp/scene.h"
#include "src/include/assimp/postprocess.h"
#include <vector>

struct vertex{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec2 texCoords = glm::vec2(0.0f, 0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
};

struct texture{
    unsigned int id;
    std::string type;
    
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