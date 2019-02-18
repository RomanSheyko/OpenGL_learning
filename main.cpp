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
GLuint loadShader(const char *path, GLenum shader_type);
GLchar* loadShaderAsString(const char *path);


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

    SetOpenGLAttributes();

    SDL_GL_SetSwapInterval(1);

    GLuint vertShader = loadShader("../basic.vert", GL_VERTEX_SHADER);
    GLuint fragShader = loadShader("../base.frag", GL_FRAGMENT_SHADER);

    GLuint programHandle = glCreateProgram();
    if(programHandle == 0)
    {
        fprintf(stderr, "Error creating program object.\n");
        exit(1);
    }

    glAttachShader(programHandle, vertShader);
    glAttachShader(programHandle, fragShader);

    //Linking
    glLinkProgram(programHandle);

    //Checking
    GLint status;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
    if(status ==GL_FALSE)
    {
        fprintf(stderr, "Failed to link shader program!\n");

        GLint logLen;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
        if(logLen > 0)
        {
            char *log = new char[logLen];
            GLsizei written;
            glGetProgramInfoLog(programHandle, logLen, &written, log);
            fprintf(stderr, "Program log: \n%s", log);
            delete [] log;
        }
    }
    else {
        glUseProgram(programHandle);
    }

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

    printVersion();
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

GLchar* loadShaderAsString(const char *path)
{
    const int READ_CONST = 255;
    bool memory_flag = false;
    FILE *f = fopen(path, "r");
    char *shaderCode = new char[READ_CONST];
    int i = 0, symbol;

    while((symbol = fgetc(f)) != EOF)
    {
        if(i < READ_CONST) shaderCode[i] = (char)symbol;
        else memory_flag = true;
        i++;
    }

    if(memory_flag)
    {
        delete [] shaderCode;
        shaderCode = new char[i+1];
        i = 0;
        while((symbol = fgetc(f)) != EOF)
        {
            shaderCode[i] = (char)symbol;
            i++;
        }
    }

    shaderCode[i] = '\0';

    return (GLchar*)shaderCode;
}

GLuint loadShader(const char *path, GLenum type)
{
    GLuint shader = glCreateShader(type);
    if(shader == 0)
    {
        fprintf(stderr, "Error creating shader\n");
        exit(EXIT_FAILURE);
    }

    //Compile source
    const GLchar *shaderCode = loadShaderAsString(path);
    const GLchar *codeArray[] = {shaderCode};
    glShaderSource(shader, 1, codeArray, NULL);

    //Compile shader
    glCompileShader(shader);

    //check
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        fprintf(stderr, "shader compilation failed!");

        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

        if(logLen > 0)
        {
            char *log = new char[logLen];

            GLsizei written;
            glGetShaderInfoLog(shader, logLen, &written, log);

            fprintf(stderr, "Shader log:\n%s", log);
            delete [] log;
        }
    }

    delete [] shaderCode;

    return shader;
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

    printf("GL Vendor           : %s\n", vendor);
    printf("GL Renderer         : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
    printf("GL Version (integer): %d.%d\n", major, minor);
    printf("GLSL Version        : %s\n", glslVersion);
}

/*
void drawCube(float xrf, float yrf, float zrf){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);

    glRotatef(xrf, 1.0f, 0.0f, 0.0f);
    glRotatef(yrf, 0.0f, 1.0f, 0.0f);
    glRotatef(zrf, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f( 1.0f, 1.0f,  1.0f);

    glColor3f(1.0f, 0.5f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, 1.0f);

    glColor3f(1.0f,1.0f,0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);

    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);

    glColor3f(1.0f,0.0f,1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);

    glEnd();

}*/