#pragma once
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.h"
#include "texture.h"
#include "Model.h"

class Primitive
{
private:
	const float ceilingHeight = 10.0f;
	glm::vec3 vertex1; // Bottom left
	glm::vec3 vertex2; // Bottom right
	glm::vec3 vertex3; // Top right
	glm::vec3 vertex4; // Top left
	glm::vec3 normal;

	glm::vec2 uv1; // Bottom left
	glm::vec2 uv2; // Bottom right
	glm::vec2 uv3; // Top right
	glm::vec2 uv4; // Top left

	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	Material material;

	bool hasTexture;
	GLuint texture;
	GLuint vao;

public:
	Primitive(glm::vec3 position);
	Primitive(glm::vec3 position, float width, float height);

	void initVao();
	void draw(Uniform_material uniform_material);

	std::vector<glm::vec3> vertices;

	Primitive& operator=(const Primitive& other);
};

Primitive::Primitive(glm::vec3 position = glm::vec3(0.0f))
{
	hasTexture = false;
	const float width = 3.0f;
	const float height = 5.0f;

	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;

	vertex1 = position + glm::vec3(-halfWidth, 0.0f, -halfHeight); // Bottom left
	vertex2 = position + glm::vec3(halfWidth, 0.0f, -halfHeight);  // Bottom right
	vertex3 = position + glm::vec3(halfWidth, 0.0f, halfHeight);    // Top right
	vertex4 = position + glm::vec3(-halfWidth, 0.0f, halfHeight);   // Top left
	vertices = { vertex1, vertex2, vertex3, vertex4 };

	normal = glm::vec3(0.0f, -1.0f, 0.0f);
	normals = { normal, normal, normal, normal };

	uv1 = glm::vec2(0.0f, 0.0f); // Bottom left
	uv2 = glm::vec2(1.0f, 0.0f); // Bottom right
	uv3 = glm::vec2(1.0f, 1.0f); // Top right
	uv4 = glm::vec2(0.0f, 1.0f); // Top left
	uvs = { uv1, uv2, uv3, uv4 };

	material.ambient_color = glm::vec3(1.0, 1.0, 1.0);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material.specular = glm::vec3(1.0);
	material.power = 4;
}

// For upright panels, example pillars
Primitive::Primitive(glm::vec3 position, float width, float height)
{
	hasTexture = true;
	texture = loadBMP("Textures/wallpaper.bmp");
	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;

	vertex1 = position + glm::vec3(0.0f, -height, -halfWidth); // Bottom left
	vertex2 = position + glm::vec3(0.0f, -height, halfWidth);  // Bottom right
	vertex3 = position + glm::vec3(0.0f, height, halfWidth);   // Top right
	vertex4 = position + glm::vec3(0.0f, height, -halfWidth);  // Top left
	vertices = { vertex1, vertex2, vertex3, vertex4 };

	normal = glm::vec3(0.0f, -1.0f, 0.0f);
	normals = { normal, normal, normal, normal };

	uv1 = glm::vec2(0.0f, 0.0f); // Bottom left
	uv2 = glm::vec2(1.0f, 0.0f); // Bottom right
	uv3 = glm::vec2(1.0f, 1.0f); // Top right
	uv4 = glm::vec2(0.0f, 1.0f); // Top left
	uvs = { uv1, uv2, uv3, uv4 };

	material.ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	//material.specular = glm::vec3(1.0);
	material.power = 4;
}

void Primitive::initVao()
{
	GLuint vertex_vbo, normal_vbo, uv_vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate VBOs
	glGenBuffers(1, &vertex_vbo);
	glGenBuffers(1, &normal_vbo);
	glGenBuffers(1, &uv_vbo);

	// Bind VBO for vertices and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind VBO for normals and upload data
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	// Bind VBO for UV coordinates and upload data
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

	// Bind VBO for vertices and set up vertex attribute pointer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Bind VBO for normals and set up vertex attribute pointer
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Bind VBO for UV coordinates and set up vertex attribute pointer
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	// Unbind VAO
	glBindVertexArray(0);
}

void Primitive::draw(Uniform_material uniform_material)
{
	if (hasTexture)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Set material properties
	glUniform3fv(uniform_material.ambient, 1, glm::value_ptr(material.ambient_color));
	glUniform3fv(uniform_material.diffuse, 1, glm::value_ptr(material.diffuse_color));
	glUniform3fv(uniform_material.specular, 1, glm::value_ptr(material.specular));
	glUniform1f(uniform_material.power, material.power);

	// Bind the VAO for the ceiling mesh
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
	glBindVertexArray(0);
}

Primitive& Primitive::operator=(const Primitive& other)
{
	if (this != &other) // Check for self-assignment
	{
		// Copy member variables from 'other' to 'this'
		this->hasTexture = other.hasTexture;
		this->material = other.material;

		// Deep copy vertices
		this->vertices = other.vertices;

		// Deep copy normals
		this->normals = other.normals;

		// Deep copy UVs
		this->uvs = other.uvs;
	}
	return *this;
}
