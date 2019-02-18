#include <iostream>
#include "SDL.h"
#include "gl_core_4_3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const int width = 640;
const int height = 480;
const int FRAMES_PER_SECOND = 60;

SDL_Window *window;

SDL_GLContext maincontext;

void drawCube(float xrf, float yrf, float zrf);
bool SetOpenGLAttributes();
void PrintSDL_GL_Attributes();
void CheckSDLError(int line);
void printVersion();
void Run();
void Cleanup();

bool Init()
{
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        std::cout << "Unable to init SDL, error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if(window == NULL)
    {
        std::cout << "Unable to create window\n";
        CheckSDLError(__LINE__);
        return false;
    }

    SDL_GLContext maincontext = SDL_GL_CreateContext(window);

    int loaded = ogl_LoadFunctions();
    if(loaded == ogl_LOAD_FAILED) {
        std::cout << "opengl func loading failed" << std::endl;
        return false;
    }

    int num_failed = loaded - ogl_LOAD_SUCCEEDED;
    std::cout << "Number of functions that failed to load: " << num_failed << std::endl;

    printVersion();

    SetOpenGLAttributes();

    SDL_GL_SetSwapInterval(1);

    return true;
}

bool SetOpenGLAttributes()
{
    // Set our OpenGL version.
    // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /*
    // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    */

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    return true;
}

int eventListener(SDL_Event e)
{
    switch(e.type)
    {
        case SDL_QUIT: return 0;
        case SDL_KEYDOWN:

            break;
        case SDL_KEYUP:

            break;
    }
    return 1;
}

int main(int argc, char ** argv) {
    if (!Init())
        return -1;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    //glShadeModel(GL_SMOOTH);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(45.0f, (float)width / (float) height, 0.1f, 100.0f);
    //glMatrixMode(GL_MODELVIEW);

    Run();
    return 0;
}
/*
void drawCube(float xrf, float yrf, float zrf){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);	// Сдвинуть вглубь экрана

    glRotatef(xrf, 1.0f, 0.0f, 0.0f);	// Вращение куба по X, Y, Z
    glRotatef(yrf, 0.0f, 1.0f, 0.0f);	// Вращение куба по X, Y, Z
    glRotatef(zrf, 0.0f, 0.0f, 1.0f);	// Вращение куба по X, Y, Z

    glBegin(GL_QUADS);		// Рисуем куб

    glColor3f(0.0f, 1.0f, 0.0f);		// Синяя сторона (Верхняя)
    glVertex3f( 1.0f, 1.0f, -1.0f);		// Верхний правый угол квадрата
    glVertex3f(-1.0f, 1.0f, -1.0f);		// Верхний левый
    glVertex3f(-1.0f, 1.0f,  1.0f);		// Нижний левый
    glVertex3f( 1.0f, 1.0f,  1.0f);		// Нижний правый

    glColor3f(1.0f, 0.5f, 0.0f);		// Оранжевая сторона (Нижняя)
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Верхний левый
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Нижний левый
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Нижний правый

    glColor3f(1.0f, 0.0f, 0.0f);		// Красная сторона (Передняя)
    glVertex3f( 1.0f,  1.0f, 1.0f);		// Верхний правый угол квадрата
    glVertex3f(-1.0f,  1.0f, 1.0f);		// Верхний левый
    glVertex3f(-1.0f, -1.0f, 1.0f);		// Нижний левый
    glVertex3f( 1.0f, -1.0f, 1.0f);		// Нижний правый

    glColor3f(1.0f,1.0f,0.0f);			// Желтая сторона (Задняя)
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Верхний левый
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Нижний левый
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Нижний правый

    glColor3f(0.0f,0.0f,1.0f);			// Синяя сторона (Левая)
    glVertex3f(-1.0f,  1.0f,  1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Верхний левый
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Нижний левый
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Нижний правый

    glColor3f(1.0f,0.0f,1.0f);			// Фиолетовая сторона (Правая)
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Верхний правый угол квадрата
    glVertex3f( 1.0f,  1.0f,  1.0f);	// Верхний левый
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Нижний левый
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Нижний правый

    glEnd();	// Закончили квадраты

}*/

void Run()
{
    Uint32 start;
    char running = 1;
    float xrf = 0, yrf = 0, zrf = 0;
    glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 5.0),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0));

    while(running != 0)
    {
        start = SDL_GetTicks();
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(eventListener(e) == 0) {
                running = 0;
            }
        }

        xrf -= 0.5;
        yrf -= 0.5;
        zrf -= 0.5;

        //drawCube(xrf, yrf, zrf);

        glFlush();
        SDL_GL_SwapWindow(window);

        if(1000/FRAMES_PER_SECOND > SDL_GetTicks() - start)
            SDL_Delay(1000/FRAMES_PER_SECOND - (SDL_GetTicks() - start));
    }
}

void CheckSDLError(int line = -1)
{
    std::string error = SDL_GetError();

    if (error != "")
    {
        std::cout << "SLD Error : " << error << std::endl;

        if (line != -1)
            std::cout << "\nLine : " << line << std::endl;

        SDL_ClearError();
    }
}

void PrintSDL_GL_Attributes()
{
    int value = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
    std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << value << std::endl;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
    std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
}

void printVersion()
{
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout <<

    printf("GL Vendor           : %s\n", vendor);
    printf("GL Renderer         : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
    printf("GL Version (integer): %d.%d\n", major, minor);
    printf("GLSL Version        : %s\n", glslVersion);
}