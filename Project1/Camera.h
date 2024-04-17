#pragma once
#include "Model.h"

class Camera : public Model
{
public:
	Camera();
	Camera(glm::vec3 position);

	void draw(glm::mat4 view, Uniform_material uniform_material);
};

Camera::Camera() {}

Camera::Camera(glm::vec3 position)
{
	this->position = position;

	material.ambient_color = glm::vec3(0.2, 0.2, 0.2);
	material.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material.specular = glm::vec3(1.0f);
	material.power = 1024;

	texture_id = loadBMP("Textures/Yellobrk.bmp"); 
	loadOBJ("Objects/camera.obj", vertices, uvs, normals);

	model = new glm::mat4(glm::translate(glm::mat4(), position));

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	*model = (*model) * rotation;

	Model::initVAO();
}

void Camera::draw(glm::mat4 view, Uniform_material uniform_material)
{
	Model::draw(view, uniform_material);
}