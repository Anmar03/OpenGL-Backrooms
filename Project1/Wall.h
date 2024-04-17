#pragma once
#include "Model.h"

class Wall : public Model
{
public:
	Wall();
	Wall(glm::vec3 position);

	void setObject(const char* objPath);
	void draw(glm::mat4 view, Uniform_material uniform_material);
};

Wall::Wall() {}

Wall::Wall(glm::vec3 position) : Model(position, 0, Material(), "")
{
	this->position = position;

	material.ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	//material.specular = glm::vec3(0.1);
	material.power = 1024;

	texture_id = loadBMP("Textures/wallpaper.bmp");
}

void Wall::setObject(const char* objPath)
{
	loadOBJ(objPath, vertices, uvs, normals);

	model = new glm::mat4(glm::translate(glm::mat4(), position));

	Model::initVAO();
}

void Wall::draw(glm::mat4 view, Uniform_material uniform_material)
{
	Model::draw(view, uniform_material);
}