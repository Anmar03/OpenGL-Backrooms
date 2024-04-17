#pragma once
#include "Model.h"

class Floor : public Model
{
public:
	Floor();
	Floor(glm::vec3 position);

	void draw(glm::mat4 view, Uniform_material uniform_material);
};

Floor::Floor()
{

}

Floor::Floor(glm::vec3 position) : Model(position, 0, Material(), "")
{
	this->position = position;

	material.ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material.power = 1024;

	texture_id = loadBMP("Textures/backrooms_floor.bmp");
	loadOBJ("Objects/backrooms_floor.obj", vertices, uvs, normals);

	model = new glm::mat4(glm::translate(glm::mat4(), position));

	Model::initVAO();
}

void Floor::draw(glm::mat4 view, Uniform_material uniform_material) {

	Model::draw(view, uniform_material);
}