#include "../include/Window.h"

#include <string>

#include "GL/glew.h"
#define GLEW_STATIC

#include "../include/ErrorHandler.h"

Window::Window(const char* title, int width, int height, int openGLMajorVersion,
  int openGLMinorVersion, bool vsync, bool fullscreen) :
  _title(title),
  _width(width),
  _height(height),
  _openGLMajorVersion(openGLMajorVersion),
  _openGLMinorVersion(openGLMinorVersion),
  _vsync(vsync), _fullscreen(fullscreen),
  _windowPtr(NULL)
  { }

Window::~Window() {
  Close();
}
bool Window::Init() {

  if(!InitSDLVideoSubsystem()) {
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _openGLMajorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _openGLMinorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  _windowPtr = SDL_CreateWindow(_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_OPENGL);

  if(_windowPtr == NULL) {
    LOG_ERROR("Unable to create window. " + std::string(SDL_GetError()));
    return false;
  }
  else {
    _openGLContext = SDL_GL_CreateContext(_windowPtr);
    if(_openGLContext == NULL) {
      LOG_ERROR("Unable to create OpenGL context. " + std::string(SDL_GetError()));
      return false;
    }
    else {
      glewExperimental = true;
      GLenum glewInitStatus = glewInit();
      if(glewInitStatus != GLEW_OK) {
        LOG_ERROR("Unable to initialize Glew. " +
          std::string((char*)glewGetErrorString(glewInitStatus)));
        return false;
      }
      else {

        // Prints current graphics device and OpenGL version
        const unsigned char* graphicsDeviceInfo = glGetString(GL_RENDERER);
        const unsigned char* openGLVersion = glGetString(GL_VERSION);
        const unsigned char* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        std::cout << graphicsDeviceInfo << std::endl;
        std::cout << "OpenGL Version: "<< openGLVersion << std::endl;
        std::cout << "GLSL Version: " << glslVersion << std::endl;
        if(_vsync) {
          SetVsync(true);
        }
        if(_fullscreen) {
          SetFullscreen(true);
        }
      }
    }
  }

  return true;
}

void Window::Close() {
  if(_windowPtr != NULL){
    SDL_DestroyWindow(_windowPtr);
    _windowPtr = NULL;
  }

  if(SDL_WasInit(SDL_INIT_VIDEO)){
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}

void Window::Swap() {
  SDL_GL_SwapWindow(_windowPtr);
}

void Window::SetFullscreen(bool fullscreen) {
  if(fullscreen) {
    if(SDL_SetWindowFullscreen(_windowPtr, SDL_WINDOW_FULLSCREEN_DESKTOP)) {
      LOG_ERROR("Unable to set fullscreen mode. " + std::string(SDL_GetError()));
    }
  }
  else {
      if(SDL_SetWindowFullscreen(_windowPtr, 0) < 0) {
        LOG_ERROR("Unable to set windowed mode. " + std::string(SDL_GetError()));
      }
  }
}

void Window::SetVsync(bool vsync) {
  if(SDL_GL_SetSwapInterval((int)vsync) < 0) {
    LOG_ERROR("Unable to use Vsync. " + std::string(SDL_GetError()));
  }
}

bool Window::InitSDLVideoSubsystem() {
  if(SDL_WasInit(SDL_INIT_VIDEO) == 0) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
      LOG_ERROR("Unable to initialize SDL_Video. " + std::string(SDL_GetError()));
      return false;
    }
  }

  return true;
}
