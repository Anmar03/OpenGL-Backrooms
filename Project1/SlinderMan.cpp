#include "SlinderMan.h"
#include <cstdlib> 
#include <ctime>  

SlinderMan::SlinderMan() : Model(glm::vec3(50.0f, 0.0f, -35.0f), 0, Material(), ""), speed(0.1f), health(100.0f), currentState(Idle), attackCooldown(0.0f), cooldownDuration(1.0f), chaseRange(60.0f), attackRange(2.0f), heightFromGround(0.0f), radius(10.0f)
{
    position = glm::vec3(50.0f, 0.0f, -35.0f);
    destination = position;

    material.ambient_color = glm::vec3(0.0, 0.0, 0.0);
    material.diffuse_color = glm::vec3(0.2, 0.2, 0.2);
    material.specular = glm::vec3(1.0f);
    material.power = 1024;

    loadOBJ("Objects/bacteria.obj", vertices, uvs, normals);

    model = new glm::mat4(glm::translate(glm::mat4(), position));
}


void SlinderMan::update(float deltaTime, glm::vec3 playerPosition)
{
    this->playerPosition = playerPosition;

    if (position.y != heightFromGround) { position.y = heightFromGround; }

    switch (currentState)
    {
    case Idle:
        patrol();
        checkPlayerInRange();
        break;
    case Chasing:
        chasePlayer();
        checkPlayerInRange();
        break;
    case Attacking:
        attackPlayer();
        break;
    default:
        break;
    }

    // rotation angle to face the direction
    float targetAngle = glm::degrees(atan2(direction.x, direction.z));

    // Interpolate between current rotation angle and target angle
    float smoothedAngle = glm::mix(currentAngle, targetAngle, 0.01f * deltaTime);
    currentAngle = smoothedAngle;

    // rotation matrix for slinderMan
    rotation = glm::rotate(glm::mat4(), glm::radians(currentAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    *model = glm::translate(glm::mat4(), position) * rotation;
    
    // Update attack cooldown timer
    if (attackCooldown > 0.0f)
    {
        attackCooldown -= deltaTime;
    }
}


void SlinderMan::chasePlayer()
{
    direction = glm::normalize(playerPosition - getPosition());

    position += direction * speed;
}


void SlinderMan::attackPlayer()
{
    if (attackCooldown <= 0.0f)
    {
        // TO-DO::Attack Player

        // Reset attack cooldown
        attackCooldown = cooldownDuration;
    }
}

// Function to patrol between waypoints
void SlinderMan::patrol()
{
    if (waypoints.empty()) 
    {
        return; 
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    position += direction * speed;

    float distance = glm::distance(destination, getPosition());

    if (radius >= distance)
    {
        // random index to select a waypoint
        int randomIndex = rand() % waypoints.size();

        // random waypoint
        destination = waypoints[randomIndex];

        direction = glm::normalize(destination - getPosition());
    }
}

// Function to check if the player is within detection range
void SlinderMan::checkPlayerInRange()
{
    distanceToPlayer = glm::distance(playerPosition, getPosition());

    // Check if player is within chase range
    if (distanceToPlayer <= chaseRange)
    {
        currentState = Chasing;
    }
    else if (currentState == Chasing && distanceToPlayer > chaseRange)
    {
        currentState = Idle;
    }

    // If player is within attack range
    if (distanceToPlayer <= attackRange)
    {
        currentState = Attacking;
    }
}

void SlinderMan::setWaypoints(std::vector<glm::vec3> waypoints)
{
    this->waypoints = waypoints;
}

void SlinderMan::initVao()
{
    Model::initVAO();
}

void SlinderMan::draw(glm::mat4 view, Uniform_material uniform_material)
{
    Model::draw(view, uniform_material);
}

