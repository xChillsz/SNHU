#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions


#define _USE_MATH_DEFINES
#include <cmath>

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnOpengl/camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "3D Scene by David Anguiano"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Pyramid mesh data
    GLMesh meshPyramid;
    GLMesh meshCube;
    GLMesh meshPlane;
    GLMesh meshPole;

    //Light Mesh
    GLMesh meshLight;
    GLMesh meshLightTwo;

    // Texture
    //1. Initialize Variables
    GLuint textureBrickRoof;
    GLuint textureBlueRoof;
    GLuint textureRedRoof;
    GLuint textureOrangeRoof;
    GLuint textureSmallOrangeHouse;
    GLuint textureTallBlueHouse;
    GLuint textureSmallGrayHouse;
    GLuint texturePlane;
    GLuint textureDoor;
    GLuint texturePole;

    //2. Configure how they react
    glm::vec2 gUVScale(5.0f, 5.0f);
    glm::vec2 enviroMap(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // Shader program
    GLuint shaderPyramid;
    GLuint shaderCube;
    GLuint shaderPlane;

    //Light Shaders
    GLuint shaderLight;
    GLuint shaderLightTwo;

    //Reflect Shader
    GLuint shaderReflect;
    
    //Light Color
    glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

    bool spinToggle = true;
    float currentRotation = 0.0f;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */


float translatePyramid[3] = { 0.0f, 0.0f, 0.0f };
float translateCube[3] = { 3.0f, 3.0f, 3.0f };

bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void createPyramidMesh(GLMesh& mesh);
void createCubeMesh(GLMesh& mesh);
void createPlaneMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId, GLint patternType, GLint filterType);
void UDestroyTexture(GLuint textureId);
void URender(GLuint shaderId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale, float degree, float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float firstLightX, float firstLightY, float firstLightZ, float secLightX, float secLightY, float secLightZ);
void renderLight(GLuint shaderId, GLuint shaderLightId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale, float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float xLightTrans, float yLightTrans, float zLightTrans, float lightScale);
void renderReflection(GLuint shaderId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale, float degree, float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float firstLightX, float firstLightY, float firstLightZ, float secLightX, float secLightY, float secLightZ);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    layout(location = 1) in vec3 normals;  // Texture data from Vertex Attrib Pointer 1
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormals; // variable to transfer texture data to the fragment shader
    out vec3 vertexFragmentPos;
    out vec2 vertexTextureCoordinate;

    //Global variables for the transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view & projection)

        vertexNormals = mat3(transpose(inverse(model))) * normals; // get normal vectors in world space only and exclude normal translation properties

        vertexTextureCoordinate = textureCoordinate; // references incoming color data
    }
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormals;
    in vec3 vertexFragmentPos;//incoming Fragment Position
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor; // For outgoing cube color to the GPU

    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPos;
    uniform vec3 objectColorTwo;
    uniform vec3 lightColorTwo;
    uniform vec3 lightPosTwo;
    uniform vec3 viewPosition;
    uniform sampler2D uTexture;
    uniform vec2 uvScale;

    void main()
    {
        /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

        //Calculate Ambient lighting*/
        float ambientStrength = 0.1f; // Set ambient or global lighting strength
        vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

        //Calculate 2nd Light
        float ambientStrength2 = 0.5f; // Set ambient or global lighting strength
        vec3 ambient2 = ambientStrength2 * lightColorTwo; // Generate ambient light color

        //Calculate Diffuse lighting*/
        vec3 norm = normalize(vertexNormals); // Normalize vectors to 1 unit
        vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
        float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
        vec3 diffuse = impact * lightColor; // Generate diffuse light color

        vec3 lightDirection2 = normalize(lightPosTwo - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
        float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
        vec3 diffuse2 = impact2 * lightColorTwo; // Generate diffuse light color

        //Calculate Specular lighting*/
        float specularIntensity = 3.0f; // Set specular light strength
        float highlightSize = 5.0f; // Set specular highlight size
        vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector


        float specularIntensity2 = 0.0f; // Set specular light strength
        float highlightSize2 = 5.0f; // Set specular highlight size

        vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
        vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector

        //Calculate specular component
        float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
        vec3 specular = specularIntensity * specularComponent * lightColor;

        float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
        vec3 specular2 = specularIntensity2 * specularComponent2 * lightColorTwo;

        //Texture holds the color to be used for all components
        vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

        // Calculate phong result
        vec3 phong = ((ambient + ambient2) + (diffuse + diffuse2) + (specular + specular2)) * textureColor.xyz;
        //vec3 phong2 = (ambient2 + diffuse2 + specular2) * textureColor.xyz;

        fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
    }
);

/*REFLECT VERTEX SHADER*/
const GLchar* reflectShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normals;
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormals;
    out vec2 vertexTextureCoordinate;
    out vec3 reflectedVector;
    out vec3 refractedVector;

    uniform mat4 model;
    uniform mat4 projection;
    uniform mat4 view;
    uniform vec3 cameraPosition;

void main(void) {
        vec4 worldPosition = model * vec4(position, 1.0);
        gl_Position = projection * view * worldPosition;

        vertexTextureCoordinate = textureCoordinate;
        vertexNormals = normals;
        vec3 unitNormal = normalize(normals);

        vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
        reflectedVector = reflect(viewVector, unitNormal);
        refractedVector = refract(viewVector, unitNormal, 1.0 / 1.33);

    }
);

/*REFLECT FRAGMENT SHADER*/
const GLchar* reflectFragmentShader = GLSL(440,
    out vec4 out_Color;

    in vec3 vertexNormals;
    in vec3 vertexFragmentPos;//incoming Fragment Position
    in vec2 vertexTextureCoordinate;
    in vec3 reflectedVector;
    in vec3 refractedVector;

    uniform sampler2D uTexture;
    uniform samplerCube enviroMap;

    const vec3 lightDirection = normalize(vec3(0.2, -1.0, 0.3));
    const float ambient = 0.3;

    void main(void) {

        float brightness = max(dot(-lightDirection, normalize(vertexNormals)), 0.0) + ambient;
        out_Color = texture(uTexture, vertexTextureCoordinate) * brightness;

        vec4 reflectedColor = texture(enviroMap, reflectedVector);
        vec4 refractedColor = texture(enviroMap, refractedVector);
        vec4 enviroColor = mix(reflectedColor, refractedColor, 0.5);

        out_Color = mix(out_Color, reflectedColor, 0.5);

        //out_color = mix(out_color, reflectedColor, 0.6);

    }
);


/* Lamp Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
    }
);


/* Fragment Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
    {
        fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
    }
);

/*FIX ME - NEW CODE*/



/*FIX ME - END NEW CODE*/

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    //Made Changes to the Header File to move Camera UP & Down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, textureBrickRoof);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_REPEAT;

        cout << "Current Texture Wrapping Mode: REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, textureBrickRoof);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_MIRRORED_REPEAT;

        cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
    {
        glBindTexture(GL_TEXTURE_2D, textureBrickRoof);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_EDGE;

        cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
    {
        float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        glBindTexture(GL_TEXTURE_2D, textureBrickRoof);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_BORDER;

        cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// Functioned called to render a frame
void URender(GLuint shaderId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale, float degree, float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float firstLightX, float firstLightY, float firstLightZ, float secLightX, float secLightY, float secLightZ)
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(xScale, yScale, zScale));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(degree, glm::vec3(xRot, yRot, zRot));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(xTrans, yTrans, zTrans));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Transforms the camera: move the camera back (z axis)
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a orthographic projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(shaderId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderId, "model");
    GLint viewLoc = glGetUniformLocation(shaderId, "view");
    GLint projLoc = glGetUniformLocation(shaderId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(shaderId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(shaderId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(shaderId, "lightPos");


    GLint objectColorLoc2 = glGetUniformLocation(shaderId, "objectColorTwo");
    GLint lightColorLoc2 = glGetUniformLocation(shaderId, "lightColorTwo");
    GLint lightPositionLoc2 = glGetUniformLocation(shaderId, "lightPosTwo");

    GLint viewPositionLoc = glGetUniformLocation(shaderId, "viewPosition");

    //FIX ME - NEED TO FIND A WAY TO GET LIGHT FROM TWO RESOURCES

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, 0.5f, 1.0f, 0.5f);
    glUniform3f(lightColorLoc, 0.5f, 1.0f, 0.5f);
    glUniform3f(lightPositionLoc, firstLightX, firstLightY, firstLightZ);


    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc2, 0.5f, 1.0f, 0.5f);
    glUniform3f(lightColorLoc2, 0.5f, 0.0f, 0.5f);
    glUniform3f(lightPositionLoc2, secLightX, secLightY, secLightZ);


    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    GLint UVScaleLoc = glGetUniformLocation(shaderId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(mesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(glTexture);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, mesh.nIndices); // Draws the triangle

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

}

void renderLight(GLuint shaderId, GLuint shaderLightId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale,float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float xLightTrans, float yLightTrans, float zLightTrans, float lightScale)
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(mesh.vao);

    // Set the shader to be used
    glUseProgram(shaderId);

    // Camera
    glm::mat4 view = gCamera.GetViewMatrix();
    // Creates a orthographic projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 rotation = glm::rotate(25.6f, glm::vec3(xRot, yRot, zRot));
    glm::mat4 model = glm::translate(glm::vec3(xLightTrans, yLightTrans, zLightTrans)) * glm::scale(glm::vec3(lightScale)) * rotation;

    // LAMP: draw lamp
    //----------------
    glUseProgram(shaderLightId);

    //Transform the smaller cube used as a visual que for the light source

    // Reference matrix uniforms from the Lamp Shader program
    GLint modelLoc = glGetUniformLocation(shaderLightId, "model");
    GLint viewLoc = glGetUniformLocation(shaderLightId, "view");
    GLint projLoc = glGetUniformLocation(shaderLightId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    // Draws the Object
    glDrawArrays(GL_TRIANGLES, 0, mesh.nIndices); // Draws the Object w/ Triangles

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
    glUseProgram(0);
}

void renderReflection(GLuint shaderId, GLMesh mesh, GLuint textureId, GLenum glTexture, float xScale, float yScale, float zScale, float degree, float xRot, float  yRot, float  zRot, float xTrans, float  yTrans, float  zTrans, float firstLightX, float firstLightY, float firstLightZ, float secLightX, float secLightY, float secLightZ)
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(xScale, yScale, zScale));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(degree, glm::vec3(xRot, yRot, zRot));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(xTrans, yTrans, zTrans));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Transforms the camera: move the camera back (z axis)
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a orthographic projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(shaderId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderId, "model");
    GLint viewLoc = glGetUniformLocation(shaderId, "view");
    GLint projLoc = glGetUniformLocation(shaderId, "projection");
    GLint cameraPositionLoc = glGetUniformLocation(shaderId, "cameraPosition");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    glUniformMatrix4fv(cameraPositionLoc, 1, GL_FALSE, glm::value_ptr(cameraPosition));

    GLint enviroMapLoc = glGetUniformLocation(shaderId, "enviroMap");
    glUniform2fv(enviroMapLoc, 1, glm::value_ptr(enviroMap));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(mesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(glTexture);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, mesh.nIndices); // Draws the triangle

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

}

// Implements the createPyramidMesh function
void createPyramidMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {

        //*Had to create new Verts for the Pyramid couldn't manage working w/ indicies*/
       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
       1.1f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
       -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

       0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
       0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.5f, 1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.1f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f,

       1.1f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      -0.1f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
       0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.5f, 1.0f,

       -0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nIndices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void createPlaneMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        -5.0f, -5.0f, -5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         5.0f, -5.0f, -5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         5.0f,  -5.0f,  5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        
         5.0f, -5.0f,  5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -5.0f, -5.0f,  5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -5.0f, -5.0f, -5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nIndices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void createCubeMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {

            //Positions          //Normals
         // ------------------------------------------------------
         //Back Face          //Negative Z Normal  Texture Coords.
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            //Front Face         //Positive Z Normal
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Right Face         //Positive X Normal
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Bottom Face        //Negative Y Normal
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        //Top Face           //Positive Y Normal
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nIndices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, mesh.vbos);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId, GLint patternType, GLint filterType)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, patternType); /*GL_REPEAT | GL_MIRRORED_REPEAT | GL_CLAMP_TO_EDGE | GL_CLAMP_TO BORDER*/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, patternType);

        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // acompile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    createPyramidMesh(meshPyramid); // Calls the function to create the Vertex Buffer Object
    createCubeMesh(meshCube);
    createPlaneMesh(meshPlane);

    //Create Light Mesh
    createCubeMesh(meshLight);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, shaderPyramid))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, shaderCube))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, shaderPlane))
        return EXIT_FAILURE;

    /*LIGHT TEXTURES*/
    if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, shaderLight))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, shaderLightTwo))
        return EXIT_FAILURE;

    //REFLECT SHADER
    if (!UCreateShaderProgram(reflectShaderSource, reflectFragmentShader, shaderReflect))
        return EXIT_FAILURE;


    // Load texture
    const char* textFileBricks = "res/textures/bricks3.png";
    const char* textBlueRoof = "res/textures/blue-roof.jpg";
    const char* textRedRoof = "res/textures/red-roof.jpg";
    const char* textOrangeRoof = "res/textures/orange-roof.jpg";
    const char* textFileOrangeSiding = "res/textures/orange-wall.jpg";
    const char* textFileBlueSiding = "res/textures/blue-wall.jpg";
    const char* textFileGraySiding = "res/textures/gray-siding.jpg";
    const char* textFileGrass = "res/textures/grass.jpg";
    const char* textFileDoor = "res/textures/door.jpg";
    const char* textFilePole = "res/textures/door.jpg";

    /*GL_REPEAT | GL_MIRRORED_REPEAT | GL_CLAMP_TO_EDGE | GL_CLAMP_TO BORDER*/

    if (!UCreateTexture(textFileBricks, textureBrickRoof, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileBricks << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textBlueRoof, textureBlueRoof, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textBlueRoof << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textRedRoof, textureRedRoof, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textRedRoof << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textOrangeRoof, textureOrangeRoof, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textOrangeRoof << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFileOrangeSiding, textureSmallOrangeHouse, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileOrangeSiding << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFileBlueSiding, textureTallBlueHouse, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileBlueSiding << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFileGraySiding, textureSmallGrayHouse, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileGraySiding << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFileGrass, texturePlane, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileGrass << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFileDoor, textureDoor, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFileDoor << endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture(textFilePole, texturePole, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << textFilePole << endl;
        return EXIT_FAILURE;
    }


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(shaderPyramid);
    glUseProgram(shaderCube);
    glUseProgram(shaderPlane);

    //Light Shaders
    glUseProgram(shaderLight);
    glUseProgram(shaderLightTwo);

    //Reflect Shader
    glUseProgram(shaderReflect);

    // We set the texture as texture unit last integer represents which is being called
    glUniform1i(glGetUniformLocation(shaderPyramid, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderCube, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderPlane , "uTexture"), 2);
    
    /*LIGHT TEXTURES*/

    glUniform1i(glGetUniformLocation(shaderLight, "uTexture"), 3);
    glUniform1i(glGetUniformLocation(shaderLightTwo, "uTexture"), 4);

    //REFLECT TEXTURE
    glUniform1i(glGetUniformLocation(shaderReflect, "uTexture"), 5);


    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render this frame
        /*
        1. I made this Dynamic but I'm not sure if it was worth it.
        2. There might not be a need to pass in the IDS to the Render. But create more items within the Create Mesh Method..
        */
        //3x Scale 1x Degree 3xRotation 3xTranslate
        
        /*LIGHT*/
        renderLight(shaderCube, shaderLight, meshLight, texturePlane, GL_TEXTURE0, /*SCALE */2.0f, 2.0f, 2.0f,/*ROTATION*/
            1.4f, 23.0f, 1.0f,/*TRANS*/ 0.0f, 0.0f, 0.0f,/*LIGHT TRANS*/ 10.0f, 2.0f, 10.0f,/*LIGHT SCALE*/ 0.8f);
        
        /*LIGHT POLE*/
        URender(shaderCube, meshCube, textureSmallGrayHouse, GL_TEXTURE0,/*Scale*/ 0.1f, 2.9f, 0.1f,/*Degree*/ 25.6f,/*Rot*/ 1.4f, 23.0f, 1.0f,/*TRANS*/  10.0f, 0.166f, 10.0f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);

        /*LEFT HOUSE - Short House Blue Roof */
        URender(shaderPyramid, meshPyramid, textureBlueRoof, GL_TEXTURE0, 2.0f, 2.2f, 2.4f, 25.6f, 2.5f, 28.0f, 1.0f, -5.485f, 1.0f, 1.178f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        URender(shaderCube, meshCube, textureSmallOrangeHouse, GL_TEXTURE0, 2.0f, 2.0f, 2.0f, 25.6f, 1.4f, 23.0f, 1.0f, -4.0f, 0.0f, 1.8f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);

        /*MIDDLE HOUSE - Short House Red Roof */ 
        URender(shaderPyramid, meshPyramid, textureRedRoof, GL_TEXTURE0, 2.0f, 2.2f, 2.2f, 25.6f, 2.5f, 28.0f, 1.0f, -1.4f, 1.2f, -0.5f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        URender(shaderCube, meshCube, textureSmallGrayHouse, GL_TEXTURE0, 2.0f, 2.1f, 2.0f, 25.6f, 1.4f, 23.0f, 1.0f, 0.0f, 0.15f, 0.0f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        
        /*RIGHT HOUSE - Long House Red Roof */
        URender(shaderPyramid, meshPyramid, textureOrangeRoof, GL_TEXTURE0, 2.0f, 2.2f, 2.4f, 25.6f, 2.8f, 23.0f, 1.0f, 4.5f, 3.278f, -3.7f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        URender(shaderCube, meshCube, textureTallBlueHouse, GL_TEXTURE0, 2.0f, 4.0f, 2.0f, 25.6f, 1.4f, 23.0f, 1.0f, 6.0f, 1.267f, -3.0f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);

        /*DOORS - LEFT | MIDDLE | RIGHT*/
        URender(shaderPlane, meshPlane, textureDoor, GL_TEXTURE0,/*Scale*/ 0.1f, 1.0f, 0.1655f,/*Degree*/ 10.9f,/*Rot*/ -11.888f, -3.233f, 2.88f,/*Trans*/ -1.05f, -0.3f, 7.07f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        URender(shaderPlane, meshPlane, textureDoor, GL_TEXTURE0,/*Scale*/ 0.1f, 1.0f, 0.1655f,/*Degree*/ 10.9f,/*Rot*/ -11.888f, -3.233f, 2.88f,/*Trans*/ 2.91f, -0.22f, 5.27f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
        URender(shaderPlane, meshPlane, textureDoor, GL_TEXTURE0,/*Scale*/ 0.1f, 1.0f, 0.1655f,/*Degree*/ 10.9f,/*Rot*/ -11.888f, -3.233f, 2.88f,/*Trans*/ 8.88f, -0.05f, 2.257f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);

        //SKY PLANE
        //renderReflection(shaderReflect, meshPlane, texturePlane, GL_TEXTURE0,/*Scale*/ 2.0f, 2.2f, 2.4f,/*Degree*/ 25.6f,/*Rot*/ 2.5f, 28.0f, 1.0f,/*Trans*/ 3.0f, 15.0f, 3.0f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);

        /*GROUND PLANE*/
        URender(shaderPlane, meshPlane, texturePlane, GL_TEXTURE0,/*Scale*/ 2.0f, 2.2f, 2.4f,/*Degree*/ 25.6f,/*Rot*/ 2.5f, 28.0f, 1.0f, /*Trans*/3.0f, 10.0f, 3.0f,/*lights*/ 10.0f, 2.0f, 10.0f, -10.0f, -2.0f, -10.0f);
       

        /*NEW CODE*/

        /*END OF NEW CODE*/
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(meshPyramid);
    UDestroyMesh(meshCube);
    UDestroyMesh(meshPlane);
    
    //Light Mesh
    UDestroyMesh(meshLight);

    // Release shader program
    UDestroyShaderProgram(shaderPyramid);
    UDestroyShaderProgram(shaderCube);
    UDestroyShaderProgram(shaderPlane);
    
    //Light Shaders
    UDestroyShaderProgram(shaderLight);
    UDestroyShaderProgram(shaderLightTwo);

    //Reflect Shader
    UDestroyShaderProgram(shaderReflect);


    exit(EXIT_SUCCESS); // Terminates the program successfully
}
