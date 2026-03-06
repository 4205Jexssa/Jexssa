////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      redundance.cpp                         //
// Funcion:        Manejo de alertas y redundancias       //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////

#pragma once

#include "pros/imu.hpp"
#include "pros/misc.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/rotation.hpp"

namespace Redundance {

    // ── Estado interno de alertas (banderas de "ya anunciado") ──────────────
    extern bool an_usd;
    extern bool an_temp_l1, an_temp_l2, an_temp_l3;
    extern bool an_temp_r1, an_temp_r2, an_temp_r3;
    extern bool an_fr_l1,   an_fr_l2,   an_fr_l3;
    extern bool an_fr_r1,   an_fr_r2,   an_fr_r3;
    extern bool an_temp_um, an_temp_lm;
    extern bool an_fr_um,   an_fr_lm;
    extern bool an_ros;
    extern bool an_bat;
    extern bool an_ctr;
    extern bool an_cal;
    extern bool an_opt;
    extern int lastime;

    // ── Declaraciones de funciones ──────────────────────────────────────────

    /**
     * Revisa el estado de todos los dispositivos del robot y devuelve un
     * código de error (0 = sin errores).
     */
    int check_errors(
        pros::Motor& Left1,  pros::Motor& Left2,  pros::Motor& Left3,
        pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3,
        pros::Motor& LMotor, pros::Motor& UMotor,
        pros::Controller& Master,
        pros::Device& Radio,
        pros::IMU& Inertial,
        pros::Rotation& OdomX, pros::Rotation& OdomY,
        pros::Optical& Optical
    );

    /**
     * Muestra en el controlador el mensaje asociado al código de error.
     */
    void show_message(
        int id,
        pros::Motor& Left1,  pros::Motor& Left2,  pros::Motor& Left3,
        pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3,
        pros::Motor& LMotor, pros::Motor& UMotor,
        pros::Controller& Master,
        pros::Device& Radio,
        pros::IMU& Inertial,
        pros::Rotation& OdomX, pros::Rotation& OdomY,
        pros::Optical& Optical
    );

    /**
     * Vibra el controlador con el patrón asociado al código de error.
     */
    void vibrate(int id, pros::Controller& Master);

    /**
     * Combina show_message + vibrate para manejar un error.
     */
    void handle_error(
        int id,
        pros::Motor& Left1,  pros::Motor& Left2,  pros::Motor& Left3,
        pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3,
        pros::Motor& LMotor, pros::Motor& UMotor,
        pros::Controller& Master,
        pros::Device& Radio,
        pros::IMU& Inertial,
        pros::Rotation& OdomX, pros::Rotation& OdomY,
        pros::Optical& Optical
    );

    /**
     * Punto de entrada principal: verifica errores y, si corresponde,
     * los reporta cada 5 segundos.
     */
    bool launch(
        pros::Motor& Left1,  pros::Motor& Left2,  pros::Motor& Left3,
        pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3,
        pros::Motor& LMotor, pros::Motor& UMotor,
        pros::Controller& Master,
        pros::Device& Radio,
        pros::IMU& Inertial,
        pros::Rotation& OdomX, pros::Rotation& OdomY,
        pros::Optical& Optical
    );

} // namespace Redundance