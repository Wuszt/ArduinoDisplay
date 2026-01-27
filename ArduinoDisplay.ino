#include <Arduino.h>
#include "DisplayManager.h"
#include "DisplayDrawerInterface.h"
#include "BitmapDrawer.h"
#include <memory>
#include <LittleFS.h>
#include "GifDrawer.h"
#include <random>

std::vector<std::unique_ptr<AD::IDisplayDrawer>> g_drawers;
int g_currentDrawerIndex = -1;

void ActivateNextDrawer()
{
  if (g_currentDrawerIndex >= 0)
  {
    g_drawers[g_currentDrawerIndex]->Deactivate();
  }

  g_currentDrawerIndex = (g_currentDrawerIndex + 1) % g_drawers.size();
  if (g_currentDrawerIndex == 0)
  {
    std::shuffle(g_drawers.begin(), g_drawers.end(), std::default_random_engine(random()));
  }

  g_drawers[g_currentDrawerIndex]->Activate();
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {}

  if (!LittleFS.begin(true)) 
  {
    Serial.println("LittleFS mount failed");
  }
  
  g_drawers.push_back(std::make_unique<AD::BitmapDrawer>("rarog", 5u * 1000u));
  g_drawers.push_back(std::make_unique<AD::BitmapDrawer>("polaris", 5u * 1000u));
  g_drawers.push_back(std::make_unique<AD::BitmapDrawer>("doge", 5u * 1000u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("homer", 3u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("monke", 2u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("witcher", 1u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("samurai", 3u));

  ActivateNextDrawer();
}

void loop() 
{
  if (g_drawers[g_currentDrawerIndex]->IsFinished())
  {
    ActivateNextDrawer();
  }

  g_drawers[g_currentDrawerIndex]->Update();
}