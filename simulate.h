#pragma once

// standard c++
#include <stdio.h>
#include <iostream>
#include <pthread.h>

// WebCore (Avian)
#include "Random.h"

#include "common.h"


float* u = NULL;
float* v = NULL;
float* du = NULL;
float* dv = NULL;

float u_max = 8.f;
float v_max = 8.f;

bool all_threads_cancel = false;

struct ThreadDomain
{
  int index;
  int begin;
  int end;
};
int render_frame = 0;
pthread_t* thread_id = NULL;
ThreadDomain* thread_domains = NULL;


pthread_cond_t* cond_array;
pthread_mutex_t* mutex_array;


void apply();
void launch_process();


void wait_for_cancel_routine()
{
  // wait for threads to cancel...?
  bool all_cancelled = true;
  for (int i = 0; i < nthreads; ++i)
  {
    if (thread_frames[i] != 2)
    {
      all_cancelled = false;
      break;
    }
  }

  if (all_cancelled == false)
    return;

  // Finished cancelling...
  printf("All threads cancelled\n");
  all_threads_cancel = false;

  // Clean up the thread data
  printf("delete[] thread_frames\n");
  delete[] thread_frames;
  // printf("delete[] thread_id\n");
  // delete[] thread_id;
  printf("free(thread_id)\n");
  for (int i = 0; i < nthreads; ++i)
    thread_id[i] = NULL;
  free(thread_id);
  printf("free(thread_domains)\n");
  free(thread_domains);
  printf("free(cond_array)\n");
  free(cond_array);
  printf("free(mutex_array)\n");
  free(mutex_array);

  // restart process
  nthreads = nthreads_change_to;
  printf("launching next thread process.\n");
  launch_process();
  printf("launched.\n");
}


void random_buffers()
{
  for (int i = 0; i < bufferWidth; i++) 
  {
    for (int j = 0; j < bufferHeight; j++) 
    {
      u[j * bufferWidth + i] = Random::dice(0, u_max);
      v[j * bufferWidth + i] = Random::dice(0, v_max);      
    }
  }

  apply();
}


float react_f(float p_a, float p_b)
{
  //return s * (a - p_a * p_b);
  return s * (16.0 - p_a * p_b);
}


float react_g(float p_a, float p_b, float d)
{
  d;
  //return s * (p_a * p_b - p_b - b);
  return s * (p_a * p_b - p_b - d);
}


void apply()
{
  // apply differences to array
  for (int i = 0; i < bufferWidth; i++) 
  {
    for (int j = 0; j < bufferHeight; j++) 
    {
      int i_c = j * bufferWidth + i;

      u[i_c] += du[i_c];
      v[i_c] += dv[i_c];

      if (u[i_c] > u_max) { u[i_c] = u_max; }
      if (u[i_c] < 0)     { u[i_c] = 0; }
      if (v[i_c] > v_max) { v[i_c] = v_max; }
      if (v[i_c] < 0)     { v[i_c] = 0; }
    }
  }

  // apply array values to render buffer
  int pixelSize = 4;
  for (int i = 0; i < bufferWidth; i++) 
  {
    for (int j = 0; j < bufferHeight; j++) 
    {
      int i_c = j * bufferWidth + i;

      GLubyte r_fill = (GLubyte)(v[i_c]/v_max * 0xff);
      GLubyte g_fill = (GLubyte)(v[i_c]/v_max * 0xa8);
      GLubyte b_fill = 0;

      bufferData[(j * bufferWidth + i) * pixelSize + 0] = r_fill;
      bufferData[(j * bufferWidth + i) * pixelSize + 1] = g_fill;
      bufferData[(j * bufferWidth + i) * pixelSize + 2] = b_fill;
      bufferData[(j * bufferWidth + i) * pixelSize + 3] = 0xff;
    }
  }

  //
  // Update image buffer
  //
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    bufferWidth,
    bufferHeight,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    bufferData);
  glGenerateMipmap(GL_TEXTURE_2D); // Need to do this!?
}


void thread_loop()
{
  if (all_threads_cancel)
  {
    wait_for_cancel_routine();
    return;
  }

  // lock?
  // check thread_frames all 1
  bool all_done = true;
  for (int i = 0; i < nthreads; ++i)
  {
    if (thread_frames[i] == 0)
    {
      all_done = false;
      break;
    }
  }
  // perform copy update
  if (all_done)
  {
    apply();
    // set thread_frames to 0
    for (int i = 0; i < nthreads; ++i) 
    {
      thread_frames[i] = 0;
      pthread_cond_signal(&cond_array[i]);
    }
  }
  // unlock
}


void step_simulation(int domain_begin, int domain_end)
{
  //
  // Simulate
  // 

  float C1 = D1;
  float C2 = D2;
  
  // calculate differences
  for (int i = domain_begin; i < domain_end; i++) 
  {
    for (int j = 0; j < bufferHeight; j++) 
    {
      int i_c = j * bufferWidth + i;
      int i_e = (j * bufferWidth + i + 1) % (bufferWidth * bufferHeight);
      int i_w = (j * bufferWidth + i - 1 + (bufferWidth * bufferHeight)) % (bufferWidth * bufferHeight);
      int i_n = ((j + 1) * bufferWidth + i) % (bufferWidth * bufferHeight);
      int i_s = ((j - 1) * bufferWidth + i + (bufferWidth * bufferHeight)) % (bufferWidth * bufferHeight);

      du[i_c] = ani * C1 * (-2*u[i_c] + u[i_n] + u[i_s]) + 
                      C1 * (-2*u[i_c] + u[i_e] + u[i_w]) + 
                      react_f(u[i_c], v[i_c]);
      dv[i_c] = ani * C2 * (-2*v[i_c] + v[i_n] + v[i_s]) + 
                      C2 * (-2*v[i_c] + v[i_e] + v[i_w]) + 
                      react_g(u[i_c], v[i_c], 12.f);
    }
  }
}


bool wait_for_task(int index)
{
  // If next frame:
  return thread_frames[index] == 0;
}


void deliver_result(int index)
{
  thread_frames[index] = 1;
}


void report_cancel(int index)
{
  thread_frames[index] = 2;
}


void* do_task(void* arg)
{
  ThreadDomain* domain = (ThreadDomain*)arg;

  while(all_threads_cancel == false)
  {
    while (wait_for_task(domain->index))
    {
      step_simulation(domain->begin, domain->end);
      deliver_result(domain->index);

      pthread_cond_wait(&cond_array[domain->index], &mutex_array[domain->index]);
      
      if (all_threads_cancel) break;
    }
    //printf("task done.\n");
  }

  report_cancel(domain->index);
  //printf("Thread returning %i\n", domain->index);
  return NULL;
}


void check_result(int result)
{
  if (result != 0)
  {
    if (result == EAGAIN) std::cout << "EAGAIN" << std::endl;
    if (result == EINVAL) std::cout << "EINVAL" << std::endl;
    if (result == EPERM) std::cout << "EPERM" << std::endl;            
  }
}


void launch_process()
{
  thread_frames = new int(nthreads);
  //thread_id = new pthread_t(nthreads);
  thread_id = (pthread_t*)malloc(sizeof(pthread_t) * nthreads);
  thread_domains = (ThreadDomain*)malloc(sizeof(ThreadDomain) * nthreads);

  cond_array = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * nthreads);
  mutex_array = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * nthreads);

  for (int i = 0; i < nthreads; ++i)
  {
    thread_frames[i] = 0;
  }
  render_frame = 0;

  for (int i = 0; i < nthreads; ++i)
  {
    thread_domains[i].index = i;
    thread_domains[i].begin = i * bufferWidth / nthreads;
    thread_domains[i].end = (i + 1) * bufferWidth / nthreads;
  }

  pthread_condattr_t attr;
  for (int i = 0; i < nthreads; ++i)
  {
    pthread_cond_init(&cond_array[i], &attr);
  }

  pthread_t thr;
  for (int i = 0; i < nthreads; ++i)
  {
    // Set up thread parameters
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    
    // Attempt to launch thread process
    int result = pthread_create(&thr, &attr, &do_task, &(thread_domains[i]));
    check_result(result);

    // global id pool
    thread_id[i] = thr;
  }
}


void clean_process()
{
  if (thread_frames == NULL) return; // not initialised
  
  // for (int i = 0; i < nthreads; ++i)
  // {
  //   pthread_cancel(thread_id[i]);
  // }

  // Apply cancel value
  all_threads_cancel = true;

  // Signal threads to wake up
  for (int i = 0; i < nthreads; ++i) 
  {
    thread_frames[i] = 0; // ?
    pthread_cond_signal(&cond_array[i]);
  }
  
  return;

  // wait for threads to cancel...?
  while (1) 
  {
    bool all_cancelled = true;
    for (int i = 0; i < nthreads; ++i)
    {
      if (thread_frames[i] != 2)
      {
        all_cancelled = false;
        break;
      }
    }
    if (all_cancelled)
      break;
  }

  all_threads_cancel = false;

  // Clean up the thread data
  delete[] thread_frames;
  delete[] thread_id;
  free(thread_domains);
  free(cond_array);
  free(mutex_array);
}


void reset_thread_pool()
{
  if (nthreads == nthreads_change_to) 
    return;

  // delete
  clean_process();

  // update
  //nthreads = nthreads_change_to;

  // create
  //launch_process();
}