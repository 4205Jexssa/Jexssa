// Programa acá tus autonomos
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
//   Script contenedor de autons  //      autons.cpp      //
//                                //                      //
////////////////////////////////////////////////////////////

#include "autons_admin.h"
#include "autons.h"
#include "pros/motors.h"
#include "chassis.h"
using namespace pros;


void autonomo1();
void autonomo2();
void autonomo3();


void autonregister(){
    register_autonomous("Autonomo 1", autonomo1);
    register_autonomous("Autonomo 2", autonomo2);
    register_autonomous("Autonomo 3", autonomo3);
}

void autonomo1(){
    
}

void autonomo2(){
    pros::lcd::print(3, "Autonomo 2");
}

void autonomo3(){
    pros::lcd::print(3, "Autonomo 3");
}