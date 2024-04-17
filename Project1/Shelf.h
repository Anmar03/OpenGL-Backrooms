#pragma once
#include "Model.h"

class Shelf : public Model
{
public:
	Shelf();
	Shelf(glm::vec3 position);

	void draw(glm::mat4 view, Uniform_material uniform_material);
};

Shelf::Shelf() {}

Shelf::Shelf(glm::vec3 position)
{
	this->position = position;
	material.ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material.specular = glm::vec3(0.5f);
	material.power = 400;

	texture_id = loadBMP("Textures/wood_0019_color_2k.bmp");
	loadOBJ("Objects/shelf.obj", vertices, uvs, normals);

	model = new glm::mat4(glm::translate(glm::mat4(), position));

	Model::initVAO();
}

void Shelf::draw(glm::mat4 view, Uniform_material uniform_material)
{
	glBindTexture(GL_TEXTURE_2D, texture_id); 
	Model::draw(view, uniform_material);
}