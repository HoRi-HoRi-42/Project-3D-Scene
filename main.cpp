#define GLEW_STATIC

#include "Windows.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"


#include <iostream>

//#include <irrklang/irrKlang.h>
//using namespace irrklang;

//ISoundEngine *SoundEngine = createIrrKlangDevice();

// window
gps::Window myWindow;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//shade
const unsigned int SHADOW_WIDTH = 20000;
const unsigned int SHADOW_HEIGHT = 20000;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint modelLoc2;
GLint viewLoc;
GLint viewLoc2;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
glm::mat4 lightRotation;

float sensitivity = 0.1f;
float pitch, yaw;
bool firstMouse = true;

float lastX, lastY;

// camera
gps::Camera myCamera(
        glm::vec3(23.0072, -1.21548f, -4.06333f),
        glm::vec3(22.1129f, -1.15343f, -10.0f), // cand modific in -3.67348 mi se schimba axele
        glm::vec3(0.0f, 2.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
//gps::Model3D teapot;
//gps::Model3D nanosut;
gps::Model3D scena;
gps::Model3D clj;
gps::Model3D clj_teava;
gps::Model3D clj_tank;
gps::Model3D clj_masa;
gps::Model3D clj_prelata;
gps::Model3D mani;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D elice;
gps::Model3D eliceSp;
gps::Model3D glont;

GLfloat angle;
GLfloat lightAngle;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
const GLfloat near_plane = 0.1f, far_plane = 50.0f;
std::vector<const GLchar *> faces;

float delta = 0;
float alfa = 0;
float acceleration=0.0f;//acceleratia pustii
float speed=0.0f;//viteza pustii
float position=0.0f;//pozitia pustii
float fireSelect=0;
int framesGlont=0;//un counter ca sa asteptam un nr de frame-uri intre glonturi la automata
GLint muzzleLoc;
int muzzle=0;//daca sa fie pornita lumina punctiforma
float intensity=0.0f;//intensitatea luminii punctiforme
GLint intensityLoc;
int gloante=30;//nr de gloante
glm::vec3 glontDir=glm::vec3(1.0f,0.0f,0.0f);//directia pe care miscam shellul
float glontDistanta=10.0f;//distanta shellului fata de pusca


bool menu = false;

bool prew = false;
float angl;


//cond fog
GLint fogCond;
GLint fogCondLoc;



void previewFunction() {
    if (prew) {
        angl += 0.5f;
        myCamera.scenePreview(angl);
    }
}

int count = 0;
int count1 = 0;

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
    WindowDimensions resize{};
    resize.width = width;
    resize.height = height;
    myWindow.setWindowDimensions(resize);
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (pressedKeys[GLFW_KEY_M] && action == GLFW_PRESS) {
        if(!menu){
            glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            menu=!menu;
        }
        else{
            glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            menu=!menu;
        }
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        fogCond = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(fogCondLoc, fogCond);

    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        fogCond = 1;
        myBasicShader.useShaderProgram();
        glUniform1i(fogCondLoc, fogCond);
    }
}

//void procesareMani(gps::Shader shader, glm::vec3 z){
//    shader.useShaderProgram();
//
//    view = myCamera.getViewMatrix();
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//
//    mani.Draw(shader);
//}

static void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xDiff = (float) xpos - lastX;
    float yDiff = (float) ypos - lastY;
    lastX = (float) xpos;
    lastY = (float) ypos;

    xDiff *= sensitivity;
    yDiff *= sensitivity;

    yaw -= xDiff;
    pitch -= yDiff;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);

    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));




//    std::cout << " mouse x =" << myCamera.getCameraPosition().x << " y= " << myCamera.getCameraPosition().y << " z= "
//              << myCamera.getCameraPosition().z << "\n";

//    std::cout << " mouse x =" << myCamera.getCameraFrontDirection().x << " y= " << myCamera.getCameraFrontDirection().y
//              << " z= "
//              << myCamera.getCameraFrontDirection().z << "\n";

    //std::cout << " mouse x =" << myCamera.getCameraTarget().x << " y= " << myCamera.getCameraTarget().y << " z= "
              //<< myCamera.getCameraTarget().z << "\n";
}

void fire(int mode){
    if(gloante>0){
        glontDistanta=0;
        if(mode == 1) {
            PlaySound("poc.wav", nullptr, SND_ASYNC | SND_FILENAME);
        }

        //initiem valorile la acceleratie, viteza si pozitie ca sa arate bine animatia de tras
        acceleration=0.0001f;
        speed=-0.01f;
        position=+0.25f;
        myCamera.fire();//aici ridicam camera un picut
        gloante--;

        float angle=(float)rand()/(float)RAND_MAX*2.0f-1.0f;
        glontDir.y=0.5f*angle;//calculam directia pe care sa mearga glontul tras
    }
    else
        PlaySound("poc123.wav", nullptr, SND_ASYNC | SND_FILENAME);
}

int zoom=0;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if(fireSelect==0)
            fire(1);//daca suntem pe modul semi doar tragem cand se apasa butonul;
        if(fireSelect==1){
            PlaySound("full.wav", nullptr, SND_ASYNC|SND_FILENAME|SND_LOOP );
            framesGlont=0;
            fireSelect=2;//daca suntem pe modul auto, punem fireSelect pe 2, ceea ce inseamna ca tragem
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        if(fireSelect==2){
            fireSelect=1;//punem fireSelect inapoi pe
            PlaySound("poc.wav", nullptr, SND_ASYNC | SND_FILENAME);
        }

    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        myBasicShader.useShaderProgram();
        if(zoom==0){
            projection = glm::perspective(glm::radians(45.0f),
                                          (float) myWindow.getWindowDimensions().width /
                                          (float) myWindow.getWindowDimensions().height,
                                          0.1f, 70.0f);
            glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE,
                               glm::value_ptr(projection));
        }
        else{
            projection = glm::perspective(glm::radians(23.0f),
                                          (float) myWindow.getWindowDimensions().width /
                                          (float) myWindow.getWindowDimensions().height,
                                          0.1f, 60.0f);
            glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE,
                               glm::value_ptr(projection));
        }
        zoom=(zoom+1)%2;
    }
}

void frwd() {
    std::cout << " Camera Position " << "x=" << myCamera.getCameraPosition().x << " " << "y="
              << myCamera.getCameraPosition().y << " "
              << "z=" << myCamera.getCameraPosition().z << " " << '\n';


    myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    //update view matrix

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

}


bool verificare(gps::Camera zzz) {
    if (zzz.getCameraPosition().x <= 25.6 && zzz.getCameraPosition().x > 13.5 &&
        zzz.getCameraPosition().z < 7.5 &&
        zzz.getCameraPosition().z > -11)
        return true;
    else
        return false;
}

bool verificare2(gps::Camera zzz) {
    if (zzz.getCameraPosition().y < 1.8)
        return true;
    else
        return false;
}

//int mouseState = glfwGetMouseButton(myWindow.getWindow(), GLFW_MOUSE_BUTTON_LEFT);

bool inters(gps::Camera zzz, gps::Model3D a, gps::Model3D b, gps::Model3D c, gps::Model3D d) {
    if (zzz.getCameraPosition().x <= a.getX())
        return true;
    else
        return false;
}

//float SquaredDistPointAABB(gps::Camera p, gps::Model3D j )
//{
//    float sqDist = 0.0f;
//
//    glm::vec3 mic;
//    glm::vec3 mare;
//
//    mic = j.CalculateAxismin();
//    mare = j.CalculateAxisMax();
//
//    for (int i = 0; i < 3; i++)
//    {
//        // For each axis, count any excess distance outside box extents
//        float v = p.getCameraPosition().x;
//        if (v < mic[i]) sqDist += (mic[i] - v) * (mic[i] - v);
//        else
//        if (v > mare[i]) sqDist += (v - mare[i]) * (v - mare[i]);
//    }
//    return sqDist;
//}

double distance(gps::Camera p, const gps::Model3D &j) {

    // Calculating distance
    return sqrt(pow(j.getX() - p.getCameraPosition().x, 2) + pow(j.getZ() - p.getCameraPosition().z, 2) * 1.0f);
}


//vreau sa plec cu was doar ci nu cu camera in sus
void processMovement() {


//    std::cout << "x=" << myCamera.getCameraTarget().x  << " " << "y=" << myCamera.getCameraTarget().y << " " << "z="
//              << myCamera.getCameraTarget().z  << " "  << myCamera.getCameraFrontDirection().x  << " " <<'\n';
//    coordonatele la care nu am voie
    //(myCamera.getCameraPositionX() >= 11.0 && myCamera.getCameraPositionX() <= 26.0 &&
    //            myCamera.getCameraPositionZ() >= -14 && myCamera.getCameraPositionZ() <= 10

    // mouse x =0.0267726 y= 0.0448842 z= -0.998633
    //mouse x =0.0917272 y= 0.0130344 z= 0.995699
//2 directii
//    if (pressedKeys[GLFW_KEY_W]) {
//        if (myCamera.getCameraPosition().x >= 13.0 && myCamera.getCameraPosition().x <= 26.0  ) {
//            frwd();
//        } else if(myCamera.getCameraTarget().x >10 && myCamera.getCameraTarget().x<26){
//            frwd();
//        }
//    }

//3 directii bune
//    if (pressedKeys[GLFW_KEY_W]) {
//        if (myCamera.getCameraTarget().x < 23 && myCamera.getCameraPosition().z < 7) {
//            if (myCamera.getCameraPosition().x >= 13.0 && myCamera.getCameraPosition().x <= 26.0) {
//                frwd();
//            } else if (myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
//                frwd();
//            }
//        } else if (myCamera.getCameraTarget().z < 7 && myCamera.getCameraPosition().x <= 26.0 &&
//                   myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
//            frwd();
//        }
//    }

//&& myCamera.getCameraPosition().x  >= clj.getX()+0.2

//4 directii
//    if (pressedKeys[GLFW_KEY_W]) {
// //       std::cout << "x=" << myCamera.getCameraTarget().x  << " " << "y=" << myCamera.getCameraTarget().y << " " << "z="
//  //                << myCamera.getCameraTarget().z  << " "  << myCamera.getCameraFrontDirection().x  << " " <<'\n';
//            if (myCamera.getCameraTarget().x < 23 && myCamera.getCameraPosition().z < 7 &&
//                myCamera.getCameraPosition().z > -10) {
//                if (myCamera.getCameraPosition().x >= 13.0 && myCamera.getCameraPosition().x <= 26.0) {
//                    frwd();
//                } else if (myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
//                    frwd();
//                }
//            } else {
//                if (myCamera.getCameraTarget().z < 7 && myCamera.getCameraTarget().z > -11 &&
//                    myCamera.getCameraPosition().x <= 26.0 &&
//                    myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
//                    frwd();
//                }
//            }
//        }

    if (pressedKeys[GLFW_KEY_W] ) {
        //incercare de coliziune, mai incercam
//        if ((trunc(myCamera.getCameraPosition().x) - 0.5 != trunc(clj.getX()) - 0.5) &&
//            (trunc(myCamera.getCameraPosition().z) - 0.7 == trunc(clj.getZ()) - 0.7)
//            ) {

        //std::cout << trunc(myCamera.getCameraPosition().x) - 0.5 << " " << trunc(clj.getX()) - 0.5 << " "
        //  << trunc(myCamera.getCameraPosition().z) - 0.7 << " " << trunc(clj.getZ()) - 0.7 << "       ";
        // std::cout << clj.getX() << " " << clj.getY() << " " << clj.getZ() << "   ";
//        std::cout << "x=" << myCamera.getCameraTarget().x << " " << "y=" << myCamera.getCameraTarget().y << " " << "z="
//                  << myCamera.getCameraTarget().z << " " << myCamera.getCameraFrontDirection().x << " " << '\n';

        //std::cout << "TEST NEBUN    " << SquaredDistPointAABB(myCamera, clj) << "  ";

//        std::cout << "TEST NEBUN V2    " << "tank " << distance(myCamera, clj_teava) << "  " << "masa "
//                  << distance(myCamera, clj_tank) << "  " << "teava " << distance(myCamera, clj_masa) << "  "
//                  << "bolov " << distance(myCamera, clj) << "\n";


        //if (SquaredDistPointAABB(myCamera, clj) < 905279 ) {
        if (distance(myCamera, clj) > 2.0 && distance(myCamera, clj_masa) > 1.1 && distance(myCamera, clj_tank) > 1.3 &&
            distance(myCamera, clj_teava) > 1.8) {
            if (myCamera.getCameraTarget().x < 23 && myCamera.getCameraPosition().z < 7 &&
                myCamera.getCameraPosition().z > -10) {
                if (myCamera.getCameraPosition().x >= 13.0 && myCamera.getCameraPosition().x <= 26.0) {
                    frwd();
                } else if (myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
                    frwd();
                }
            } else {
                if (myCamera.getCameraTarget().z < 7 && myCamera.getCameraTarget().z > -12 &&
                    myCamera.getCameraPosition().x <= 26.0 &&
                    myCamera.getCameraTarget().x > 10 && myCamera.getCameraTarget().x < 25) {
                    frwd();
                }
            }
        }
    }

    if (pressedKeys[GLFW_KEY_S] && verificare(myCamera)) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A] && verificare(myCamera)) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D] && verificare(myCamera)) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0, 1, 0));
        myBasicShader.useShaderProgram();
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0, 1, 0));
        myBasicShader.useShaderProgram();
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
// start preview
    if (pressedKeys[GLFW_KEY_V]) {
        prew = true;
    }

    // stop preview
    if (pressedKeys[GLFW_KEY_B]) {
        prew = false;
    }

    if (pressedKeys[GLFW_KEY_N]) {
        myCamera.setCameraPosition(glm::vec3(23.0072, -1.21548f, -4.06333f));

    }
//    if (pressedKeys[GLFW_KEY_Q]) {
//        angle -= 1.0f;
//        // update model matrix for teapot
//        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
//        // update normal matrix for teapot
//        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
//    }
//
//    if (pressedKeys[GLFW_KEY_E]) {
//        angle += 1.0f;
//        // update model matrix for teapot
//        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
//        // update normal matrix for teapot
//        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
//    }
    if (pressedKeys[GLFW_KEY_O]) { //SOLID
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_P]) { //WIREFRAME
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_I]) { //POLIGONAL
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        //PlaySound("file_name.wav", nullptr, SND_FILENAME | SND_ASYNC);
        //SoundEngine->play2D("beep.mp3", true);
    }


    if (pressedKeys[GLFW_KEY_1]) {
        fireSelect=0;
    }

    if (pressedKeys[GLFW_KEY_2]) {
        fireSelect=1;
    }

    if (pressedKeys[GLFW_KEY_R]) {
        PlaySound("reload.wav", nullptr, SND_ASYNC|SND_FILENAME);
        gloante=30;
    }

}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetMouseButtonCallback(myWindow.getWindow(), mouse_button_callback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    if(menu) {
        glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise


}

void initModels() {
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    //nanosut.LoadModel("models/nanosuit/nanosuit.obj");
    scena.LoadModel("models/scena/scena.obj");
    clj.LoadModel("models/coliziuni/coliziuni.obj");
    clj_teava.LoadModel("models/coliziuni/coliziuni_masa.obj");
    clj_masa.LoadModel("models/coliziuni/coliziuni_teava_tank.obj");
    clj_tank.LoadModel("models/coliziuni/coliziuni_tank.obj");
    clj_prelata.LoadModel("models/coliziuni/coliziuni_prelata.obj");
    elice.LoadModel("models/heli/MI_24.obj");
    mani.LoadModel("models/mani/maini.obj");
    eliceSp.LoadModel("models/eliceSp/elice_spate.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    glont.LoadModel("models/mani/glont.obj");
}

void initShaders() {
    myBasicShader.loadShader(
            "shaders/basic.vert",
            "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}


void initSkyBox() {
    std::vector<const GLchar *> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");


    mySkyBox.Load(faces);

    view = myCamera.getViewMatrix();
    skyboxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    //glCheckError();

    projection = glm::perspective(glm::radians(45.0f), (float) 1920 / (float) 1080, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));


}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float) myWindow.getWindowDimensions().width /
                                  (float) myWindow.getWindowDimensions().height,
                                  0.1f, 40.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));


    //set fogCond
    fogCond = 1;
    fogCondLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogCond");
    glUniform1i(fogCondLoc, fogCond);

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    muzzleLoc=glGetUniformLocation(myBasicShader.shaderProgram, "muzzleFlash");
    glUniform1i(muzzleLoc, muzzle);

    intensityLoc=glGetUniformLocation(myBasicShader.shaderProgram, "intensity");
    glUniform1f(intensityLoc, intensity);

    //glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {//nu ii aici
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    //unbind our frame buffer, otherwise it would be the only frame buffer used by OpenGl
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt((glm::mat3(lightRotation) * 25.0f * lightDir), glm::vec3(0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    const GLfloat near_plane = 0.1f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;

}

void computeMani(){//calculeaza pozitia mainilor
    if(position<0){
        position=0.0f;
        acceleration=0.0f;
        speed=0.0f;
    }
    speed+=acceleration;//viteza instantanee e calculata in funcite de acceleratie
    position+=speed;//pozitia calculata in functie de viteza instantanee
    intensity=position/0.25f;//intensitatea e direct proportionala cu distanta de la pozitia initiala.
}

void renderTeapot(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    modelLoc2 = glGetUniformLocation(shader.shaderProgram, "model");
    viewLoc2 = glGetUniformLocation(shader.shaderProgram, "view");

    //send scena model matrix data to shader
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send scena normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    //model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glCheckError();

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    //glCheckError();

    // draw teapot
    //teapot.Draw(shader);
    //nanosut.Draw(shader);
    scena.Draw(shader);
    //clj.Draw(shader);
    clj_masa.Draw(shader);
    clj_tank.Draw(shader);
    clj_teava.Draw(shader);
    clj_prelata.Draw(shader);
    //shader.useShaderProgram();
    //glCheckError();

    //model = myCamera.getViewMatrix();
    //alfa+=0.1;
    // model = glm::translate(model, glm::vec3(alfa, 0, 0));
    model = glm::mat4(1.0f);
    view = glm::lookAt(myCamera.getCameraPosition(), myCamera.getCameraTarget(), myCamera.getCameraUpDirection());
    glm::mat4 modelView = view * model;
    //glCheckError();

   // myBasicShader.useShaderProgram();

    //model = glm::translate(model, myCamera.getCameraPosition());
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelView));
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
    //glDisable(GL_DEPTH_TEST);
    //glCheckError();

    model = glm::mat4(1.0f);
//x = 8.331, y = -1.50276 z= -2.16294  m -blender
// x = 8.331, y =-2.16294  z= 1.50276m -blender
//y = -0.525431 m   0.977329
    delta += 0.01;
    //model = glm::translate(model,glm::vec3(0.977329,2.16294, -1.50276));
    //model = glm::rotate(model, delta, glm::vec3(8.331, 0.950246 , 0.973211));
    //model = glm::translate(model,glm::vec3(-0.977329,-2.16294,1.50276));
    //send matrix data to vertex shader
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //glCheckError();

    glm::vec3 originalPosition = glm::vec3(8.331, 0.950246, 0.973211);

    // Translate the model to the origin
    model = glm::translate(model, originalPosition);

    // Rotate the model around the x-axis
    model = glm::rotate(model, delta, glm::vec3(0.0f, 1.0f, 0.0f));

    // Translate the model back to its original position
    model = glm::translate(model, -originalPosition);
    //glCheckError();

    shader.useShaderProgram();
    // send matrix data to vertex shader
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
    //glCheckError();

    elice.Draw(shader);
    //shader.useShaderProgram();

    //delta += 0.1;
    model = glm::mat4(1.0f);

    glm::vec3 originalPosition1 = glm::vec3(8.94319, 0.943219, -7.55477);

    // Translate the model to the origin
    model = glm::translate(model, originalPosition1);

    // Rotate the model around the x-axis
    model = glm::rotate(model, delta, glm::vec3(1.0f, 0.0f, 0.0f));

    // Translate the model back to its original position
    model = glm::translate(model, -originalPosition1);
    //glCheckError();
    //myBasicShader.useShaderProgram();

    // send matrix data to vertex shader
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
    //glCheckError();

    eliceSp.Draw(shader);

    //sex
    model=glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));

    if(fireSelect==2&&framesGlont==0) {//daca suntem in modul de tras full auto si au trecut 15 frame-uri tragem din nou
        fire(0);
    }

        framesGlont=(framesGlont+1)%11;//la fiecare 15 frame-uri se reseteaza framesGlont la valoarea 0

    computeMani();//calculeaza pozitia mainilor
    model=glm::translate(model, glm::vec3(0.0f,0.0f,position));//mutam mainile
    view=glm::mat4(1.0f);//cu acest view o sa urmareasca mainile camera

    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(v
    iew));
    //glCheckError();

    if(acceleration!=0){//daca pusca e in miscare ii spunem la shader ca s-a tras si ca poate porni lumina puinctiforma
        muzzle=1;
        glUniform1i(muzzleLoc, muzzle);
    }
    else{
        muzzle=0;
        glUniform1i(muzzleLoc, muzzle);
    }
    glUniform1f(intensityLoc, intensity);
    //glCheckError();
    mani.Draw(shader);

    model=glm::mat4(1.0f);
    model=glm::translate(model,glontDistanta*glontDir);//translatam glontul pe directia calculata
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
    glontDistanta+=0.02f;
    glont.Draw(shader);


    if(!depthPass){//cand desenam obiectele pt utilizator trimitem din nou view-ul ca sa deseneze bine skyboxul
        view=myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        mySkyBox.Draw(skyboxShader, view, projection);
    }
}

//float xcam = myCamera.getCameraPosition().x ;
//float ycam = myCamera.getCameraPosition().y;
//float zcam = myCamera.getCameraPosition().z;
//
//void cameraAutoMove(gps::Camera my) {
//    alfa += 0.10f;
//    myCamera.setCameraPosition(glm::vec3(xcam, ycam, zcam + (alfa/10)));
//    //myCamera.setCameraTarget(glm::vec3(xcam, ycam, zcam + (alfa/10)));
//    std::cout << "xcam = " << xcam << "angleCamera = " << alfa << "\n";
//}

void renderScene() {
    // render the scene to the depth buffer

    depthMapShader.useShaderProgram();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClear(GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));


    renderTeapot(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // final scene rendering pass (with shadows)
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));

    renderTeapot(myBasicShader, false);

    //draw a white cube around the light
    lightShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    model = lightRotation;
    model = glm::translate(model, 1.0f * lightDir);
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    lightCube.Draw(lightShader);



    previewFunction();
}


void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}


int main(int argc, const char *argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    initFBO();

    initOpenGLState();
    initModels();

    initShaders();

    initUniforms();
    initSkyBox();



    //dezacrivare
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {

        //glCheckError();
        processMovement();
        /////glCheckError();
        renderScene();
        ////glCheckError();

        glfwPollEvents();
        setWindowCallbacks();

        ////glCheckError();
        glfwSwapBuffers(myWindow.getWindow());

    }

    cleanup();

    return EXIT_SUCCESS;
}
