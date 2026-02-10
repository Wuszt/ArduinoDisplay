#include <Arduino.h>
#include "DisplayManager.h"
#include "DisplayDrawerInterface.h"
#include "BitmapDrawer.h"
#include <memory>
#include <LittleFS.h>
#include "GifDrawer.h"
#include "GameOfLife.h"
#include <random>
#include "SandSimulation.h"
//#include "GasSimulation.h"
//#include "HeksaGasSimulation.h"

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
    std::shuffle(g_drawers.begin(), g_drawers.end(), std::default_random_engine(esp_random()));
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
  g_drawers.push_back(std::make_unique<AD::BitmapDrawer>("pyrka", 5u * 1000u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("homer", 2u));
  //g_drawers.push_back(std::make_unique<AD::GifDrawer>("winton", 5u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("monke", 2u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("witcher", 1u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("samurai", 3u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("catDevil", 15u));
  g_drawers.push_back(std::make_unique<AD::GifDrawer>("pc", 40u));
  //g_drawers.push_back(std::make_unique<AD::TestShader>());
  g_drawers.push_back(std::make_unique<AD::GameOfLife>(60u * 1000u));
  g_drawers.push_back(std::make_unique<AD::SandSimulation>(60u * 1000u));
  //g_drawers.push_back(std::make_unique<AD::GasSimulation>(60u * 1000u));
  //g_drawers.push_back(std::make_unique<AD::HeksaGasSimulation>(60u * 1000u));
  ActivateNextDrawer();
}

void loop() 
{
  AD::DisplayManager::Get().Tick();

  if (g_drawers[g_currentDrawerIndex]->IsFinished())
  {
    ActivateNextDrawer();
  }

  g_drawers[g_currentDrawerIndex]->Update();
}