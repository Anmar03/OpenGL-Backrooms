#include "Model.h"

Model::Model()
{

}

Model::Model(glm::vec3 position, const Material& mat, const char* objPath)
{
    hasTexture = false;
    material = mat;

    if (objPath != nullptr && objPath[0] != '\0')
    {
        loadOBJ(objPath, vertices, uvs, normals);
    }

    this->position = position;

    model = new glm::mat4(glm::translate(glm::mat4(), position));
}

Model::Model(glm::vec3 position, GLuint texture, const Material& mat, const char* objPath)
{
    hasTexture = true;
    material = mat;
    texture_id = texture;

    if (objPath != nullptr && objPath[0] != '\0') 
    {
        loadOBJ(objPath, vertices, uvs, normals);
    }

    this->position = position;

    model = new glm::mat4(glm::translate(glm::mat4(), position));
}

glm::vec3 Model::getPosition()
{
    return position;
}

void Model::setPosition(glm::vec3 position)
{
    this->position = position;
}

void Model::initVAO()
{
    // Generate VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate VBOs for vertices, normals, and UVs
    GLuint vbo_vertices, vbo_normals, vbo_uvs;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_uvs);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    // Enable vertex attributes
    glEnableVertexAttribArray(0); // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1); // Vertex normal
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2); // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0); // Unbind VAO
}

glm::vec3 Model::calculateDimension()
{
    glm::vec3 minCoords = glm::vec3(std::numeric_limits<float>::infinity());
    glm::vec3 maxCoords = glm::vec3(-std::numeric_limits<float>::infinity());

    for (const auto& vertex : vertices) {
        // min and max coordinates for each axis
        minCoords.x = std::min(minCoords.x, vertex.x);
        minCoords.y = std::min(minCoords.y, vertex.y);
        minCoords.z = std::min(minCoords.z, vertex.z);

        maxCoords.x = std::max(maxCoords.x, vertex.x);
        maxCoords.y = std::max(maxCoords.y, vertex.y);
        maxCoords.z = std::max(maxCoords.z, vertex.z);
    }

    glm::vec3 dimensions = maxCoords - minCoords;

    return dimensions;
}

void Model::draw(glm::mat4 view, Uniform_material uniform_material)
{
    mv = view * (*model);

    // Send mv
    glUniformMatrix4fv(uniform_material.uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));

    // Set material properties
    glUniform3fv(uniform_material.ambient, 1, glm::value_ptr(material.ambient_color));
    glUniform3fv(uniform_material.diffuse, 1, glm::value_ptr(material.diffuse_color));
    glUniform3fv(uniform_material.specular, 1, glm::value_ptr(material.specular));
    glUniform1f(uniform_material.power, material.power);

    // Bind texture
    if (hasTexture)
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    // Draw the model
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}