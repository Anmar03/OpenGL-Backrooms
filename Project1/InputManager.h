#pragma once
#include <stdexcept> 
#include <cstring>
#include <glm/glm.hpp>

const class InputManager 
{
private:
    bool keys[256]; 
    bool mouseButtons[3]; 
    bool firstMouse;
    bool isMouseWarped;
    float rotationSpeed;
    float yaw;   
    float pitch;
    float lastX;
    float lastY;
    glm::vec3 front;
    const int WIDTH, HEIGHT;
    bool isJumping = false;

public:
    InputManager(const int w, const int h);

    // Keyboard input
    void setKeyDown(unsigned char key);
    void setKeyUp(unsigned char key);

    bool isKeyDown(unsigned char key) const;
    bool isKeyUp(unsigned char key) const;

    void keyHandler(unsigned char key);

    // Mouse input
    float getYaw();
    float getPitch();
    void setYaw(float yaw);
    void setPitch(float pitch);
    glm::vec3 getFront();
    void setFront(glm::vec3 front);

    glm::vec3 mouseHandler(int xpos, int ypos);

    // bools
    bool droneMode = false;
    bool isCurrentlyJumping() const;

    void setJumpState(bool state);
};


InputManager::InputManager(const int w, const int h) : WIDTH(w), HEIGHT(h)
{
    // Initialize member variables
    memset(keys, false, sizeof(keys));
    memset(mouseButtons, false, sizeof(mouseButtons));

    firstMouse = true;
    isMouseWarped = false;
    lastX = WIDTH / 2.0f;
    lastY = HEIGHT / 2.0f;
    pitch = 0.0f;
    yaw = -90.0f; // Initialized to face towards negative z-axis
    rotationSpeed = 0.05f;

    front = glm::vec3(0.0f, 0.0f, -1.0f);
}

void InputManager::setKeyDown(unsigned char key) 
{
    keys[key] = true;
}

void InputManager::setKeyUp(unsigned char key) 
{
    keys[key] = false;
}

// Keboard
bool InputManager::isKeyDown(unsigned char key) const 
{
    return keys[key];
}

bool InputManager::isKeyUp(unsigned char key) const 
{
    return !keys[key];
}

void InputManager::keyHandler(unsigned char key)
{
    setKeyDown(key);

    if (key == 'v')
    {
        droneMode = !droneMode;
    }
}

glm::vec3 InputManager::mouseHandler(int xpos, int ypos)
{
    static int centerX = WIDTH / 2;
    static int centerY = HEIGHT / 2;

    if (xpos == centerX && ypos == centerY)
    {
        lastX = centerX;
        lastY = centerY;
        return front;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Invert y-axis movement

    lastX = xpos;
    lastY = ypos;

    // Smooth mouse movement using damping 
    const float damping = 0.1f;
    xOffset = xOffset * damping + (1 - damping) * (centerX - xpos);
    yOffset = yOffset * damping + (1 - damping) * (centerY - ypos);

    const float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw -= xOffset; // Reverse sign for yaw calculation
    pitch += yOffset;

    // Constrain pitch to avoid gimbal lock
    if (pitch > 89.0f) { pitch = 89.0f; }
    if (pitch < -89.0f) { pitch = -89.0f; }
       
    // Update camera front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return front;
}

float InputManager::getYaw()
{
    return yaw;
}

float InputManager::getPitch()
{
    return pitch;
}

void InputManager::setYaw(float yaw)
{
    this->yaw = yaw;
}

void InputManager::setPitch(float pitch)
{
    this->pitch = pitch;
}

glm::vec3 InputManager::getFront()
{
    return front;
}

void InputManager::setFront(glm::vec3 front)
{
    this->front = front;
}

bool InputManager::isCurrentlyJumping() const
{
    return isJumping;
}

void InputManager::setJumpState(bool state)
{
    isJumping = state;
}