#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.h"
#include "texture.h"

struct Material
{
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular;
    float power;
};

struct Uniform_material
{
    GLuint uniform_mv;
    GLuint ambient;
    GLuint diffuse;
    GLuint specular;
    GLuint power;
};

class Model
{
private:
    bool hasTexture;

protected:
    glm::mat4 mv;
    Material material;
    GLuint texture_id;
    GLuint vao;
    vector<glm::vec3> normals;
    vector<glm::vec2> uvs;
    glm::vec3 position;

    // Ids
    GLuint uniform_mv;

public:
    glm::mat4* model;
    vector<glm::vec3> vertices;
    Model();
    Model(glm::vec3 position, const Material& mat, const char* objPath);
    Model(glm::vec3 position, GLuint texture, const Material& mat, const char* objPath);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    void initVAO();
    glm::vec3 calculateDimension();
    virtual void draw(glm::mat4 view, Uniform_material uniform_material);
};