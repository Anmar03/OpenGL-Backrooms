#pragma once
#include "Model.h"

class Sun : public Model
{
public:
	Sun();
	Sun(glm::vec3 position);

	void draw(glm::mat4 view, Uniform_material uniform_material);
};

Sun::Sun() {}

Sun::Sun(glm::vec3 position)
{
	this->position = position;
	material.ambient_color = glm::vec3(1.0, 0.5, 0.2);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material.power = 1024;
	
	texture_id = loadBMP("Textures/8k_sun.bmp");
	loadOBJ("Objects/sphere.obj", vertices, uvs, normals);

	model = new glm::mat4(glm::translate(glm::mat4(), position));

	Model::initVAO();
}

void Sun::draw(glm::mat4 view, Uniform_material uniform_material)
{
	*model = glm::rotate(*model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
	glBindTexture(GL_TEXTURE_2D, texture_id); // this works yes
	Model::draw(view, uniform_material);
}