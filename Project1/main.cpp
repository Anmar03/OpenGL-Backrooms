#include <iostream>
#include <vector>

#include "InputManager.h"
#include "Model.h"
#include "Floor.h"
#include "Wall.h"
#include "Primitive.h"
#include "Pillar.h"
#include "Sun.h"
#include "Camera.h"
#include "SlinderMan.h"
#include "Shelf.h"

using namespace std;

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1200, HEIGHT = 800;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 10;

// light variables
struct LightSource
{
    glm::vec3 position;
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
GLuint vao;

// Uniform ID's
Uniform_material uniform_material;

// Matrices
glm::mat4 view, projection;

InputManager inputManager(WIDTH, HEIGHT);
Floor flooring;
Wall wall;
Wall wall1;
Wall wall2;
Wall wall3;
//Pillar pillar;
Camera camera;
Sun sun;
Model desk;
Shelf shelf;
Material deskMaterial;
LightSource Light;

SlinderMan slinderMan;

float movementSpeed = 0.2f;
float jumpVelocity = 0.0f;
float gravity = 0.01f; 

//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

const float cameraHeight = 3.75f;
glm::vec3 cameraPosition = glm::vec3(0.0f, cameraHeight, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

const float initialYaw = -90.0f;
const float initialPitch = 0.0f;

glm::vec3 savedPosition;
glm::vec3 savedCameraFront;
glm::vec3 savedCameraUp;
float savedPitch;
float savedYaw;

// Ceiling mesh
float ceilingheight = 9.5f;
glm::vec3 vertex1(-200.0f, ceilingheight, -200.0f); // bottom left
glm::vec3 vertex2(200.0f, ceilingheight, -200.0f);  // bottom right
glm::vec3 vertex3(200.0f, ceilingheight, 200.0f);   // top right
glm::vec3 vertex4(-200.0f, ceilingheight, 200.0f);  // top left

glm::vec3 normal(0.0f, -1.0f, 0.0f);

glm::vec2 uv1(0.0f, 0.0f); // bottom left
glm::vec2 uv2(20.0f, 0.0f); // bottom right
glm::vec2 uv3(20.0f, 20.0f); // top right
glm::vec2 uv4(0.0f, 20.0f); // top left

std::vector<glm::vec3> vertices = { vertex1, vertex2, vertex3, vertex4 };
std::vector<glm::vec3> normals = { normal, normal, normal, normal };
std::vector<glm::vec2> uvs = { uv1, uv2, uv3, uv4 };

GLuint texture_id;

std::vector<Primitive> lightMeshes;

//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void droneModem()
{
    if (inputManager.droneMode)
    {
        // save camera positon
        savedPosition = cameraPosition;
        savedCameraFront = inputManager.getFront();
        savedCameraUp = cameraUp;
        savedPitch = inputManager.getPitch();
        savedYaw = inputManager.getYaw();

        // reset camera position
        cameraPosition = glm::vec3(0, cameraHeight, 0);
        inputManager.setFront(glm::vec3(0.0f, 0.0f, -1.0f));
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        inputManager.setYaw(initialYaw);
        inputManager.setPitch(initialPitch);
    }
    else
    {
        cameraPosition = savedPosition;
        inputManager.setFront(savedCameraFront);
        cameraUp = savedCameraUp;
        inputManager.setPitch(savedPitch);
        inputManager.setYaw(savedYaw);
    }
}

void keyboardHandler(unsigned char key, int a, int b)
{
    if (key == 27)
        glutExit();

    inputManager.keyHandler(key);

    if (inputManager.isKeyDown('v')) { droneModem(); }

    if (inputManager.isKeyDown(' ') && !inputManager.isCurrentlyJumping())
    { 
        jumpVelocity = 0.25f; 
        inputManager.setJumpState(true);
    }
}

void keyboardUpHandler(unsigned char key, int a, int b)
{
    inputManager.setKeyUp(key);
}

//--------------------------------------------------------------------------------
// Camera Movement
//--------------------------------------------------------------------------------

void updateJump()
{
    if (inputManager.isCurrentlyJumping()) {
        // Update vertical position based on jump velocity
        cameraPosition.y += jumpVelocity;
        jumpVelocity -= gravity;

        // Check if the jump is finished
        if (cameraPosition.y <= cameraHeight) {
            cameraPosition.y = cameraHeight; // Ensure camera doesn't go below ground level
            inputManager.setJumpState(false);
        }
    }
}

void updateCameraPosition()
{
    glm::vec3 movementDirection(0.0f);

    // Forward vector projected onto the ground plane
    glm::vec3 forwardDirection = glm::normalize(glm::vec3(inputManager.getFront().x, 0.0f, inputManager.getFront().z));

    if (inputManager.droneMode)
    {
        if (inputManager.isKeyDown('w')) movementDirection += inputManager.getFront();
        if (inputManager.isKeyDown('s')) movementDirection -= inputManager.getFront();

        if (inputManager.isKeyDown('q')) cameraPosition.y += movementSpeed;
        if (inputManager.isKeyDown('e')) cameraPosition.y -= movementSpeed;
    }
    else
    {
        if (inputManager.isKeyDown('w')) movementDirection += forwardDirection;
        if (inputManager.isKeyDown('s')) movementDirection -= forwardDirection;
    }
    if (inputManager.isKeyDown('a')) movementDirection -= glm::normalize(glm::cross(inputManager.getFront(), cameraUp));
    if (inputManager.isKeyDown('d')) movementDirection += glm::normalize(glm::cross(inputManager.getFront(), cameraUp));

    // Normalize movement direction
    if (glm::length(movementDirection) > 0.0f)
        movementDirection = glm::normalize(movementDirection);

    // Update camera position based on movement speed and direction
    cameraPosition += movementSpeed * movementDirection;
}

void updateViewMatrix()
{
    view = glm::lookAt(cameraPosition, cameraPosition + inputManager.getFront(), cameraUp);
}

//--------------------------------------------------------------------------------
// MouseHandling
//--------------------------------------------------------------------------------

void mouseMovementHandler(int xpos, int ypos)
{
    inputManager.mouseHandler(xpos, ypos);
}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraPosition();
    updateViewMatrix();
    updateJump();
    glutWarpPointer(WIDTH/2, HEIGHT/2);

    for (auto& lightMesh : lightMeshes) 
    {
        lightMesh.draw(uniform_material);
    }
    
    slinderMan.update(DELTA_TIME, cameraPosition);
    slinderMan.draw(view, uniform_material);
    sun.draw(view, uniform_material);
    //pillar.draw(uniform_material);
    camera.draw(view, uniform_material);
    desk.draw(view, uniform_material);
    shelf.draw(view, uniform_material);
    flooring.draw(view, uniform_material);
    wall.draw(view, uniform_material);
    wall1.draw(view, uniform_material);
    wall2.draw(view, uniform_material);
    wall3.draw(view, uniform_material);

    // Attach to program_id
    glUseProgram(program_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Bind the VAO for the ceiling mesh
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Swap buffers
    glutSwapBuffers();
}

//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}

//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 0); 
    glutCreateWindow("Final Assignment");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutKeyboardUpFunc(keyboardUpHandler); 
    glutTimerFunc(DELTA_TIME, Render, 0);
    glutPassiveMotionFunc(mouseMovementHandler);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glutSetCursor(GLUT_CURSOR_NONE);
    glewInit();
}

//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char* vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char* fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}

//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    view = glm::lookAt(cameraPosition, cameraPosition + inputManager.getFront(), cameraUp);

    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        250.0f);
}

// InitLightAndObjects
void InitObjectsLight()
{
    glm::mat4 scaleMatrix = glm::mat4(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));

    Light.position = glm::vec3(0.0, 1.0, 0.0);

    flooring = Floor(glm::vec3(0.0, -1.0, 0.0));

    shelf = Shelf(glm::vec3(10.0f, -1.0f, -20.0f));

    /*pillar = Pillar(glm::vec3(20, 1, 15));*/
    //pillar.initVao();

    camera = Camera(glm::vec3(63.0, 4.0, 18.0));

    // Desk Attributes
    GLuint deskTexture = loadBMP("Textures/wood_0019_color_2k.bmp"); 
    deskMaterial.ambient_color = glm::vec3(0.0, 0.0, 0.0);
    deskMaterial.diffuse_color = glm::vec3(0.0, 0.0, 0.0);
    deskMaterial.power = 1024;

    desk = Model(glm::vec3(5.6, -0.25, 9.9), deskTexture, deskMaterial, "Objects/desk.obj");
    (*desk.model) = scaleMatrix * (*desk.model);

    const float wallxpos = 75.0;
    wall = Wall(glm::vec3(wallxpos-1, -1.0, 0.0));
    wall.setObject("Objects/backrooms_wall_long.obj");

    glm::vec3 wallDimension = wall.calculateDimension(); // x = depth, y = height, z = width 

    const float wallxpos2 = wallxpos - wallDimension.z - wallDimension.x;

    wall1 = Wall(glm::vec3(wallxpos2+1, -1.0, 0.0));
    wall1.setObject("Objects/backrooms_wall_long.obj");

    const float center = (wallxpos + wallxpos2) / 2.0f;

    wall2 = Wall(glm::vec3(center, -1.0, -wallDimension.z / 2));
    wall2.setObject("Objects/backrooms_wall_long.obj");

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    *wall2.model = (*wall2.model) * rotationMatrix;

    wall3 = Wall(glm::vec3(center, -1.0, wallDimension.z / 2));
    wall3.setObject("Objects/backrooms_wall.obj");
    *wall3.model = (*wall3.model) * rotationMatrix;

    sun = Sun(glm::vec3(45.0f, 3.4f, 23.0f));

    texture_id = loadBMP("Textures/tiles_color.bmp");
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
    GLuint position_id;
    GLuint normal_id;
    GLuint uv_id;
    GLuint vertex_vbo, normal_vbo, uv_vbo;

    // grid vertices
    const int gridSizeX = 10; 
    const int gridSizeZ = 10; 
    const float cellSize = 35.0f; // Size of grid cell
    const float bias = 10.0f;
    std::vector<glm::vec3> gridVertices;

    for (int i = 0; i < gridSizeX; ++i)
    {
        for (int j = 0; j < gridSizeZ; ++j) 
        {
            // Vertex position for each grid cell
            float x = bias + i * cellSize - (gridSizeX * cellSize) / 2.0f;
            float z = bias + j * cellSize - (gridSizeZ * cellSize) / 2.0f;
            float y = 1; 
            gridVertices.push_back(glm::vec3(x, y, z));
        }
    }

    slinderMan.initVao();
    slinderMan.setWaypoints(gridVertices);
    desk.initVAO();

    //ceiling.InitVao();
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

    // lights at each grid point
    for (const auto& vertex : gridVertices) 
    {
        LightSource light;
        light.position = vertex;
        
        glm::vec3 adjustHeight = glm::vec3(0, 8.25, 0);
        Primitive lightMesh = Primitive(vertex + adjustHeight);
        lightMesh.initVao();
        lightMeshes.push_back(lightMesh);
    }
    
    position_id = glGetAttribLocation(program_id, "position");
    normal_id = glGetAttribLocation(program_id, "normal");
    uv_id = glGetAttribLocation(program_id, "uv");

    // Make uniform vars
    uniform_material.uniform_mv = glGetUniformLocation(program_id, "mv");

    GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
    GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    uniform_material.ambient = glGetUniformLocation(program_id, "mat_ambient");
    uniform_material.diffuse = glGetUniformLocation(program_id, "mat_diffuse");
    uniform_material.specular = glGetUniformLocation(program_id, "mat_specular");
    uniform_material.power = glGetUniformLocation(program_id, "mat_power");

    // send Uniforms
    glUseProgram(program_id);
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(Light.position));
}


int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    InitMatrices();
    InitObjectsLight();
    InitBuffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}