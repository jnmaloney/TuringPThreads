#pragma once

// WebCore (Avian)
#include "graphics.h"


// int bufferWidth = 256;
// int bufferHeight = 256;
int bufferWidth = 512;
int bufferHeight = 512;
GLubyte* bufferData = NULL;
GLuint textureId = 0;

int nthreads = 4;
int nthreads_change_to = nthreads;

int* thread_frames = NULL;

void random_buffers();
void reset_thread_pool();

float b = 1.4f;
float a = 0.025;
float D1 = 0.25;
float D2 = 0.0625;
float s = 0.015;
float ani = 1.0f;

bool play_pause = false;