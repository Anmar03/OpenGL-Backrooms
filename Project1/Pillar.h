#pragma once
#include "Primitive.h"

class Pillar // Pillar not working because whenn rotating a panel (primitive) it gets too complicated
{
private:
	glm::vec3 position;
	Primitive panel, panel1, panel2, panel3;

public:
	Pillar(glm::vec3 position = glm::vec3(0.0f));

	void initVao();
	void draw(Uniform_material uniform_material);
};


Pillar::Pillar(glm::vec3 position)
{
    const float panelWidth = 4.0f;
    const float panelHeight = 9.0f;

    // offsets for each panel relative to the pillars position
    glm::vec3 offset(0.0f, 0.0f, panelWidth / 2.0f);    // Front panel
    glm::vec3 offset1(panelWidth / 2.0f, 0.0f, 0.0f);   // Right panel
    glm::vec3 offset2(0.0f, 0.0f, -panelWidth / 2.0f);  // Back panel
    glm::vec3 offset3(-panelWidth / 2.0f, 0.0f, 0.0f);  // Left panel

    panel = Primitive(glm::vec3(position) + offset, panelWidth, panelHeight);
    panel1 = Primitive(glm::vec3(position) + offset1, panelWidth, panelHeight);
    panel2 = Primitive(glm::vec3(position) + offset2, panelWidth, panelHeight);
    panel3 = Primitive(glm::vec3(position) + offset3, panelWidth, panelHeight);

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    for (auto& vertex : panel1.vertices) {
        vertex = glm::vec3(rotationMatrix * glm::vec4(vertex, 1.0f));
    }

    for (auto& vertex : panel3.vertices) {
        vertex = glm::vec3(rotationMatrix * glm::vec4(vertex, 1.0f));
    }
}

void Pillar::initVao()
{
	panel.initVao();
	panel1.initVao();
	panel2.initVao();
	panel3.initVao();
}

void Pillar::draw(Uniform_material uniform_material)
{
	panel.draw(uniform_material);
	panel1.draw(uniform_material);
	panel2.draw(uniform_material);
	panel3.draw(uniform_material);
}