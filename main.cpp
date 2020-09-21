// Emscripten API
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// WebCore (Avian)
#include "graphics.h"
#include "RenderSystem.h"
#include "RenderQueue.h"
#include "WindowManager.h"
#include "MenuManager.h"
#include "Random.h"

#include "interface.h"
#include "simulate.h"


RenderSystem* g_rs = 0;
WindowManager g_windowManager;
MenuManager g_menuManager;


void init()
{
  //
  // Create buffer
  //
  int pixelSize = 4 * sizeof(GLubyte);
  bufferData = (GLubyte*)malloc(bufferWidth * bufferHeight * pixelSize);
  memset((void*)bufferData, 0xffffffff, bufferWidth * bufferHeight * 4);

  //
  // Create texture
  //
  glActiveTexture(GL_TEXTURE1);
  glGenTextures(1, &textureId);

  //
  // Create data buffers
  // 
  u = (float*)malloc(bufferWidth * bufferHeight * sizeof(float));
  v = (float*)malloc(bufferWidth * bufferHeight * sizeof(float));
  du = (float*)malloc(bufferWidth * bufferHeight * sizeof(float));
  dv = (float*)malloc(bufferWidth * bufferHeight * sizeof(float));

  random_buffers();
}


void loop()
{
  //
  // Check thread work
  //
  if (play_pause)
  {
    thread_loop();
  }

  //
  // Draw UI
  //
  g_menuManager.predraw();
  interface_loop();
  g_menuManager.postdraw();
};


EM_BOOL key_callback(int action, const EmscriptenKeyboardEvent *e, void *userData)
{
  return true;
}


extern "C" int main(int argc, char** argv)
{
  g_windowManager.width = 871;
  g_windowManager.height = 490;
  g_windowManager.init("Project");
  g_menuManager.fontNameTTF =  "data/font/OpenSans-Bold.ttf";
  g_menuManager.init(g_windowManager);

  g_rs = new RenderSystem();
  g_rs->init();

  Random::init();
  init();

  // Key callbacks
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);

  // Initial launch threads
  launch_process();

  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
  #endif

  glfwTerminate();

  return 0;
}
