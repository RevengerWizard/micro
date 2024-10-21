#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "spxe.h"

#ifndef __APPLE__
    #include <GL/glew.h>
    #define GLFW_MOD_CAPS_LOCK 0x0010
#else
    #define GL_SILENCE_DEPRECATION
    #define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>
#include "gleq.h"

/* spxe shader strings */

#define _SPXE_TOK2STR(s) #s
#define SPXE_TOK2STR(s) _SPXE_TOK2STR(s)

#ifndef SPXE_SHADER_LAYOUT_LOCATION
    #define SPXE_SHADER_LAYOUT_LOCATION 0
#endif

#ifndef SPXE_SHADER_HEADER
    #ifdef __APPLE__ 
        #define SPXE_SHADER_HEADER "#version 330 core\n"
    #else 
        #define SPXE_SHADER_HEADER "#version 300 es\nprecision mediump float;\n"
    #endif
#endif

#define SPXE_SHADER_LAYOUT_STR SPXE_TOK2STR(SPXE_SHADER_LAYOUT_LOCATION)

#define SPXE_SHADER_VERTEX "layout (location = " SPXE_SHADER_LAYOUT_STR ")" \
"in vec4 vertCoord;\n"                          \
"out vec2 TexCoords;\n"                         \
"void main(void) {\n"                           \
"    TexCoords = vertCoord.zw;\n"               \
"    gl_Position = vec4(vertCoord.x,vertCoord.y,0.,1.); \n"\
"}\n"

#define SPXE_SHADER_FRAGMENT                    \
"in vec2 TexCoords;\n"                          \
"out vec4 FragColor;\n"                         \
"uniform sampler2D tex;\n"                      \
"void main(void) {\n"                           \
"    FragColor = texture(tex, TexCoords);\n"    \
"}\n"

static const char* vertexShader = SPXE_SHADER_HEADER SPXE_SHADER_VERTEX;
static const char* fragmentShader = SPXE_SHADER_HEADER SPXE_SHADER_FRAGMENT;

/* spxe core handler */

static struct spxeInfo {
    GLFWwindow* window;
    struct spxeRes {
        int width;
        int height;
    } scrres, winres;
    struct spxeRatio {
        float width;
        float height;
    } ratio;
} spxe = {NULL, {400, 300}, {800, 600}, {1.0, 1.0}};

/* implementation only static functions */

static void spxeFrame(void)
{
    int i;

    float vertices[16] = {
        1.0f,   1.0f,   1.0f,   0.0f,
        1.0f,  -1.0f,   1.0f,   1.0f,
        -1.0f, -1.0f,   0.0f,   1.0f,
        -1.0f,  1.0f,   0.0f,   0.0f
    };

    const float w = (float)spxe.winres.width / (float)spxe.scrres.width;
    const float h = (float)spxe.winres.height / (float)spxe.scrres.height;
    
    spxe.ratio.width = (h < w) ? (h / w) : 1.0f;
    spxe.ratio.height = (w < h) ? (w / h) : 1.0f;

    for (i = 0; i < 16; i += 4) {
        vertices[i] *= spxe.ratio.width;
        vertices[i + 1] *= spxe.ratio.height;
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void spxeWindow(GLFWwindow* window, int width, int height)
{
    (void)window;
#ifndef __APPLE__
    glViewport(0, 0, width, height);
#endif
    spxe.winres.width = width;
    spxe.winres.height = height;
    spxeFrame();
}

/* spxe core */

Px* spxeStart(          
    const char* title,  const int winwidth, const int winheight, 
    const int scrwidth, const int scrheight)
{
    Px* pixbuf;
    GLFWwindow* window;
    unsigned int id, vao, ebo, texture;
    unsigned int shader, vshader, fshader;

    const size_t scrsize = scrwidth * scrheight;
    const unsigned int indices[] = {
        0,  1,  3,
        1,  2,  3 
    };

    /* init glfw */
    if(!glfwInit())
    {
        fprintf(stderr, "spxe failed to initiate glfw.\n");
        return NULL;
    }

    /* open and setup window */
#ifdef __APPLE__
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#endif

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    gleqInit();

    window = glfwCreateWindow(winwidth, winheight, title, NULL, NULL);
    if(!window)
    {
        fprintf(stderr, "spxe failed to open glfw window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowSizeLimits(window, scrwidth, scrheight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSizeCallback(window, spxeWindow);

    gleqTrackWindow(window);

    /* OpenGL context and settings */
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        fprintf(stderr, "spxe failed to initiate glew.\n");
        return NULL;
    }
#endif

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    
    /* allocate pixel framebuffer */
    pixbuf = (Px*)calloc(scrsize, sizeof(Px));
    if(!pixbuf)
    {
        fprintf(stderr, "spxe failed to allocate pixel framebuffer.\n");
        return NULL;
    }

    /* set global information */
    spxe.window = window;
    spxe.winres.width = winwidth;
    spxe.winres.height = winheight;
    spxe.scrres.width = scrwidth;
    spxe.scrres.height = scrheight;
    
    /* compile and link shaders */
    shader = glCreateProgram();
    
    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertexShader, NULL);
    glCompileShader(vshader);

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragmentShader, NULL);
    glCompileShader(fshader);

    glAttachShader(shader, vshader);
    glAttachShader(shader, fshader);
    glLinkProgram(shader);
    
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    
    glUseProgram(shader);

    /* create vertex buffers */
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    glGenBuffers(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    spxeFrame();
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(SPXE_SHADER_LAYOUT_LOCATION);
    glVertexAttribPointer(SPXE_SHADER_LAYOUT_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    /* create render texture (framebuffer) */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuf
    );

    return pixbuf;
}

void spxeBackgroundColor(const Px c)
{
    const float n = 1.0F / 255.0F;
    glClearColor((float)c.r * n, (float)c.g * n, (float)c.b * n, (float)c.a * n);
}

void spxeRender(const Px* pixbuf)
{
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuf
    );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int spxeStep(void)
{ 
    glfwSwapBuffers(spxe.window);
    glClear(GL_COLOR_BUFFER_BIT);
    return !glfwWindowShouldClose(spxe.window);
}

int spxeEnd(Px* pixbuf)
{
    glfwTerminate();
    if (pixbuf) {
        free(pixbuf);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

/* window and screen size getters */

void spxeWindowSize(int* width, int* height)
{
    *width = spxe.winres.width;
    *height = spxe.winres.height;
}

void spxeScreenSize(int* width, int* height)
{
    *width = spxe.scrres.width;
    *height = spxe.scrres.height;
}

/* mouse input */

void spxeMousePos(int* x, int* y)
{
    double dx, dy;
    float width, height, hwidth, hheight;
    
    glfwGetCursorPos(spxe.window, &dx, &dy);
    width = (float)spxe.scrres.width;
    height = (float)spxe.scrres.height;
    hwidth = width * 0.5;
    hheight = height * 0.5;

    dx = dx * (width / (float)spxe.winres.width);
    dy = dy * (height / (float)spxe.winres.height);
    *x = (int)((dx - hwidth) / spxe.ratio.width + hwidth);
    *y = (int)((dy - hheight) / spxe.ratio.height + hheight);
}

GLFWwindow* spxeGetWindow()
{
    return spxe.window;
}

static GLFWmonitor* previous_monitor = NULL;
static int previous_x = 0, previous_y = 0;
static int previous_width = 0, previous_height = 0;

void spxeSetFullscreen(int enable)
{
    if(enable && !previous_monitor)
    {
        // Store current window position and size
        glfwGetWindowPos(spxe.window, &previous_x, &previous_y);
        glfwGetWindowSize(spxe.window, &previous_width, &previous_height);
        
        // Get the primary monitor
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        if(!primary) return;
        
        // Get the monitor's video mode
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        if(!mode) return;
        
        // Store the monitor for later use when exiting fullscreen
        previous_monitor = primary;
        
        // Set fullscreen mode
        glfwSetWindowMonitor(spxe.window, primary, 0, 0, 
                           mode->width, mode->height, 
                           mode->refreshRate);   
    }
    else if(!enable && previous_monitor)
    {
        // Restore the window to its previous position and size
        glfwSetWindowMonitor(spxe.window, NULL,
                            previous_x, previous_y,
                            previous_width, previous_height,
                            0);
                            
        // Clear the stored monitor
        previous_monitor = NULL;
    }
}