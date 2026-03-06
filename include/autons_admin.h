////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      autons_admin.cpp                       //
// Funcion:        Administración de autonomos            //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////

#pragma once

#include "liblvgl/llemu.hpp"
#include "main.h"
#include <string>
#include <vector>
#include <functional>

// Declaraciones para el selector de autonomos

struct Autonomous {
    std::string name;
    std::function<void()> func;
};

extern int index;
extern std::vector<Autonomous> autons;

void selector_lcd_template();
void register_autonomous(const std::string &name, std::function<void()> auton);
void selector_shower();
void selector_next();
void selector_previous();
void run_selected_autonomous();
void selector_init();
