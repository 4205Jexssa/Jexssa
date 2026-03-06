// Falta NADA para el nacional
////////////////////////////////////////////////////////////
//       | |                          //           _____  //
//       | | _____  _____ ___  __ _   //    _   __/ ___/  //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //   | | / / __ \   //
//  | |__| |  __/>  <\__ \__ \ (_| |  //   | |/ / /_/ /   //
//   \____/ \___/_/\_\___/___/\__,_|  //   |___/\____/    //
////////////////////////////////////////////////////////////
//                     Jexssa 4205J                       //
//                      Polirobots                        //
//                    Arquitectura #6                     //
//                      gmaxd, kath                       //
////////////////////////////////////////////////////////////
//                                //                      //
//    Script manager de autons    //   autons_admin.cpp   //
//                                //                      //
////////////////////////////////////////////////////////////

#include "liblvgl/llemu.hpp"
#include "main.h"
#include "iostream"
#include <string>
#include <vector>
#include <functional>
#include "autons_admin.h"
using namespace pros::lcd;

int index = 0;


std::vector<Autonomous> autons = {};
static bool autons_callbacks_registered = false;

void selectorSafeCleaner(){
    pros::lcd::set_text(0, "");
    pros::lcd::set_text(1, "");
    pros::lcd::set_text(2, "");
    pros::lcd::set_text(3, "");
    pros::lcd::set_text(4, "");
    pros::lcd::set_text(5, "");
    pros::lcd::set_text(6, "");
    pros::lcd::set_text(7, "");
}

void selector_lcd_template(){
    selectorSafeCleaner();
    pros::lcd::set_text(0, "[ Jexssa ] Autonomous Selector");
    pros::lcd::set_text(6, "Previous | Test | Next");
}

void register_autonomous(const std::string &name, std::function<void()> auton){
    if (!auton) return; // ignore empty functions
    autons.push_back({name, auton});
}

void selector_shower(){
    selector_lcd_template();
    if (autons.size() > 0){
        if (index < 0 || (size_t)index >= autons.size()) index = 0;
        set_text(2, "Selected: " + std::to_string(index) + ": " + autons[index].name);
    } else {
        set_text(2, "No autonomous routines registered.");
    }
}

void selector_next(){
    if (autons.size() > 0){
        index = (index + 1) % autons.size();
        selector_shower();
    }
}

void selector_previous(){
    if (autons.size() > 0){
        index = (index - 1 + autons.size()) % autons.size();
        selector_shower();
    }
}

void run_selected_autonomous(){
    if (autons.size() > 0){
        autons[index].func();
    }
}

void selector_init(){
    selector_lcd_template();
    if (!autons_callbacks_registered) {
        pros::lcd::register_btn0_cb(selector_previous);
        pros::lcd::register_btn1_cb(run_selected_autonomous);
        pros::lcd::register_btn2_cb(selector_next);
        autons_callbacks_registered = true;
    }
}