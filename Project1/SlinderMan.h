#include "Model.h"
#include <glm/glm.hpp>

class SlinderMan : public Model
{
private:
    const float speed;
    float health;
    glm::vec3 direction;
    glm::vec3 playerPosition;
    glm::mat4 rotation;
    float currentAngle;
    float distanceToPlayer;
    float attackRange;
    float chaseRange;
    float attackDamage;
    enum State { Idle, Chasing, Attacking }; 
    State currentState;
    float attackCooldown;
    const float cooldownDuration;
    std::vector<glm::vec3> waypoints; 
    glm::vec3 destination;
    const float heightFromGround;
    const float radius;

public:
    SlinderMan();

    void update(float deltaTime, glm::vec3 playerPosition);

    // Methods for enemy behavior
    void chasePlayer();
    void attackPlayer();
    void patrol();
    void checkPlayerInRange();
    void setWaypoints(std::vector<glm::vec3> waypoints);
    void initVao();
    void draw(glm::mat4 view, Uniform_material uniform_material);
};
