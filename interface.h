#pragma once

// ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h" // for PushItemFlag


#include "common.h"

    
void draw_left_panel()
{
  //
  // Info
  // 

  ImGui::TextUnformatted("Turing Patterns Demo");
  ImGui::Spacing();
  ImGui::TextWrapped("Demonstration of solving a 2D differential equation using pthreads in a web (wasm) environment.");
  ImGui::Spacing();

  ImGuiIO& io = ImGui::GetIO(); 
  ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  //
  // Settings ...
  //

  ImGui::TextUnformatted("Controls");
  
  bool disable_state = play_pause;
  if (disable_state == true) 
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xff999999);    
  }
  if (ImGui::Button("Start")) { reset_thread_pool(); play_pause = true; }
  if (disable_state == true) 
  {
    ImGui::PopItemFlag();  
    //ImGui::PopStyleVar();  
    ImGui::PopStyleColor();  
  }
  ImGui::SameLine();
  if (disable_state == false) 
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xff999999);    
  }
  if (ImGui::Button("Stop")) { play_pause = false; }
  if (disable_state == false) 
  {
    ImGui::PopItemFlag();  
    //ImGui::PopStyleVar();  
    ImGui::PopStyleColor();  
  }
  ImGui::Spacing();

  if (ImGui::Button("Reset Buffers")) { random_buffers(); }
  ImGui::Spacing();


  ImGui::TextUnformatted("Settings");

  ImGui::SliderFloat("##slider D1", &D1, 0.0f, 1.0f, "D1 = %.3f");
  ImGui::SliderFloat("##slider D2", &D2, 0.0f, 1.0f, "D2 = %.3f");
  ImGui::SliderFloat("##slider s", &s, 0.0f, 0.1f, "speed = %.3f");
  ImGui::SliderFloat("##slider ani", &ani, 0.0f, 2.0f, "anisotropy = %.3f");

  if (ImGui::Button("Preset #1")) 
  {
    b = 1.4f;
    a = 0.025;
    D1 = 0.253; // !!
    D2 = 0.0625;
    s = 0.015;
  }
  ImGui::SameLine();
  if (ImGui::Button("Preset #2")) 
  {
    b = 1.4f;
    a = 0.025;
    D1 = 0.25;
    D2 = 0.0625;
    s = 0.015 * 4;
  }
  ImGui::SameLine();
  if (ImGui::Button("Preset #3")) 
  {
    b = 1.4f;
    a = 0.025;
    D1 = 0.25;
    D2 = 0.03125;
    s = 0.015 * 4;//8;
  }
  //ImGui::SameLine();
  if (ImGui::Button("Preset #4")) 
  {
    b = 1.4f;
    a = 0.025;
    D1 = 0.25;
    D2 = 0.05;
    s = 0.015 * 4;
  }
  ImGui::SameLine();
  if (ImGui::Button("Preset #5")) 
  {
    b = 1.4f;
    a = 0.025;
    D1 = 0.25;
    D2 = 0.0625;
    s = 0.015 * 4;
  }
  ImGui::SameLine();
  if (ImGui::Button("Preset #6")) 
  {
    b = 4.7f;
    a = 4.1f;
    D1 = 0.253;
    D2 = 0.046;
    s = 0.010;
  }

  //
  // Threads
  //

  ImGui::TextUnformatted("Thread Info");

  ImGui::Text("Threads in pool: %i", nthreads_change_to);

  if (play_pause == true) 
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xff999999);    
  }
  ImGui::SameLine();
  if (ImGui::Button("-")) { --nthreads_change_to; if (nthreads_change_to < 1) nthreads_change_to = 1; }
  ImGui::SameLine();
  if (ImGui::Button("+")) { ++nthreads_change_to; }
  if (play_pause == true) 
  {
    ImGui::PopItemFlag();  
    ImGui::PopStyleColor();  
  }

  for (int i = 0; i < nthreads; ++i)  
    ImGui::Text("#%i: state %i", i, thread_frames[i]);
  //ImGui::Text("> %i", render_frame);
}


void draw_right_panel()
{
  ImGui::Image((ImTextureID)textureId, ImVec2(bufferWidth, bufferHeight));
}


void interface_loop()
{

  ImVec2 pos(0.f, 0.f);
  ImVec2 pivot(0, 0);
  ImGui::SetNextWindowPos(pos, 0, pivot);
  ImGui::SetNextWindowSize(ImVec2(WindowManager::getInstance()->width, WindowManager::getInstance()->height));
  ImGui::Begin("Dialog", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
  
  ImGui::Columns(2, 0, true);
  ImGui::BeginChild("left pane", ImVec2(260, 0));
  draw_left_panel();
  ImGui::EndChild();

  //ImGui::Separator();
  ImGui::NextColumn();
  draw_right_panel();

  ImGui::NextColumn();
  ImGui::Columns(1);

  ImGui::End();
}