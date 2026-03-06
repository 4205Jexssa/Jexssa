// And you don't seem to understand...
////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
//    ESTA FUNCION ESTA EN ESTADO BETA, PUEDE CAMBIARSE   //
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
//                        gmaxd                           //
////////////////////////////////////////////////////////////
//      Script para el manejo     //                      //
//    de alertas y redundancias   //    redundance.cpp    //
//           del robot            //                      //
////////////////////////////////////////////////////////////

#include "pros/imu.h"
#include "pros/imu.hpp"
#include "pros/misc.hpp"
#include "pros/motors.h"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/rotation.hpp"
#include <cerrno>
#include <sys/_types.h>
namespace Redundance {
    bool an_usd = false;
    bool an_temp_l1 = false;
    bool an_temp_l2 = false;
    bool an_temp_l3 = false;
    bool an_temp_r1 = false;
    bool an_temp_r2 = false;
    bool an_temp_r3 = false;
    bool an_fr_l1 = false;
    bool an_fr_l2 = false;
    bool an_fr_l3 = false;
    bool an_fr_r1 = false;
    bool an_fr_r2 = false;
    bool an_fr_r3 = false;
    bool an_temp_um = false;
    bool an_temp_lm = false;
    bool an_fr_um = false;
    bool an_fr_lm = false;
    bool an_ros = false;
    bool an_bat = false;
    bool an_ctr = false;
    bool an_cal = false;
    bool an_opt = false;
    bool lastime = 0;

    int check_errors(pros::Motor& Left1, pros::Motor& Left2, pros::Motor& Left3, pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3, pros::Motor& LMotor, pros::Motor& UMotor, pros::Controller& Master, pros::Device& Radio, pros::IMU& Inertial, pros::Rotation& OdomX, pros::Rotation& OdomY, pros::Optical& Optical){
        unsigned int l1f = Left1.get_flags();
        unsigned int l2f = Left2.get_flags();
        unsigned int l3f = Left3.get_flags();
        unsigned int r1f = Right1.get_flags();
        unsigned int r2f = Right2.get_flags();
        unsigned int r3f = Right3.get_flags();
        unsigned int lmf = LMotor.get_flags();
        unsigned int umf = UMotor.get_flags();
        pros::imu_accel_s_t accel = Inertial.get_accel();
        if ((pros::millis() - lastime) > 5000) {
         lastime = pros::millis();   
        }
        if (!Left1.is_installed()){
            return 1;
        }

        if (!Left2.is_installed()){
            return 2;
        }
        if (!Left3.is_installed()){
            return 3;
        }
        if (!Right1.is_installed()){
            return 4;
        }
        if (!Right2.is_installed()){
            return 5;
        }
        if (!Right3.is_installed()){
            return 6;
        }
        if (Left1.is_over_temp()){
            if (!an_temp_l1){
                an_temp_l1 = true;
                return 7;
            }
        } else {
            an_temp_l1 = false;
        }
        if (Left2.is_over_temp()){
            if (!an_temp_l2){
                an_temp_l2 = true;
                return 8;
            }
        } else {
            an_temp_l2 = false;
        }
        if (Left3.is_over_temp()){
            if (!an_temp_l3){
                an_temp_l3 = true;
                return 9;
            }
        } else {
            an_temp_l3 = false;
        }
        if (Right1.is_over_temp()){
            if (!an_temp_r1){
                an_temp_r1 = true;
                return 10;
            }
        } else {
            an_temp_r1 = false;
        }
        if (Right2.is_over_temp()){
            if (!an_temp_r2){
                an_temp_r2 = true;
                return 11;
            }
        } else {
            an_temp_r2 = false;
        }
        if (Right3.is_over_temp()){
            if (!an_temp_r3){
                an_temp_r3 = true;
                return 12;
            }
        } else {
            an_temp_r3 = false;
        }
        if (Left1.get_efficiency() >= 80){
            return 13;
        }
        if (Left2.get_efficiency() >= 80){
            return 14;
        }
        if (Left3.get_efficiency() >= 80){
            return 15;
        }
        if (Right1.get_efficiency() >= 80){
            return 16;
        }
        if (Right2.get_efficiency() >= 80){
            return 17;
        }
        if (Right3.get_efficiency() >= 80){
            return 18;
        }
        if (Left1.get_efficiency() >= 60){
            return 19;
        }
        if (Left2.get_efficiency() >= 60){
            if (!an_fr_l2){
                an_fr_l2 = true;
                return 20;
            }
        } else {
            an_fr_l2 = false;
        }
        if (Left3.get_efficiency() >= 60){
            if (!an_fr_l3){
                an_fr_l3 = true;
                return 21;
            }
        } else {
            an_fr_l3 = false;
        }
        if (Right1.get_efficiency() >= 60){
            if (!an_fr_r1){
                an_fr_r1 = true;
                return 22;
            }
        } else {
            an_fr_r1 = false;
        }
        if (Right2.get_efficiency() >= 60){
            if (!an_fr_r2){
                an_fr_r2 = true;
                return 23;
            }
        } else {
            an_fr_r2 = false;
        }
        if (Right3.get_efficiency() >= 60){
            if (!an_fr_r3){
                an_fr_r3 = true;
                return 24;
            }
        } else {
            an_fr_r3 = false;
        }
        if (Left1.is_over_current()){
            return 25;
        }
        if (Left2.is_over_current()){
            return 26;
        }
        if (Left3.is_over_current()){
            return 27;
        }
        if (Right1.is_over_current()){
            return 28;
        }
        if (Right2.is_over_current()){
            return 29;
        }
        if (Right3.is_over_current()){
            return 30;
        }
        if (l1f == 0x08){
            return 31;
        }
        if (l2f == 0x08){
            return 32;
        }
        if (l3f == 0x08){
            return 33;
        }
        if (r1f == 0x08){
            return 34;
        }
        if (r2f == 0x08){
            return 35;
        }
        if (r3f == 0x08){
            return 36;
        }
        if (l1f == 0x04){
            return 37;
        }
        if (l2f == 0x04){
            return 38;
        }
        if (l3f == 0x04){
            return 39;
        }
        if (r1f == 0x04){
            return 40;
        }
        if (r2f == 0x04){
            return 41;
        }
        if (r3f == 0x04){
            return 42;
        }
        if (!UMotor.is_installed()){
            return 43;
        }
        if (!LMotor.is_installed()){
            return 44;
        }
        if (UMotor.is_over_temp()){
            if (!an_temp_um){
                an_temp_um = true;
                return 45;
            }
        } else {
            an_temp_um = false;
        }
        if (LMotor.is_over_temp()){
            if (!an_temp_lm){
                an_temp_lm = true;
                return 46;
            }
        } else {
            an_temp_lm = false;
        }
        if (UMotor.get_efficiency() >= 80){
            return 47;
        }
        if (LMotor.get_efficiency() >= 80){
            return 48;
        }
        if (UMotor.get_efficiency() >= 60){
            if (!an_fr_um){
                an_fr_um = true;
                return 49;
            }
        }
        if (LMotor.get_efficiency() >= 60){
            if (!an_fr_lm){
                an_fr_lm = true;
                return 50;
            }
        } else {
            an_fr_lm = false;
        }
        if (UMotor.is_over_current()){
            return 51;
        }
        if (LMotor.is_over_current()){
            return 52;
        }
        if (lmf == 0x08){
            return 53;
        }
        if (umf == 0x08){
            return 54;
        }
        if (lmf == 0x04){
            return 55;
        }
        if (umf == 0x04){
            return 56;
        }
        if (accel.x >= 1.5 || accel.x <= -1.5 || accel.y >= 1.5 || accel.y <= -1.5 || accel.z >= 1.5 || accel.z <= -1.5){
            if (!an_ros){
                an_ros = true;
                return 57;
            }
        } else {
            an_ros = false;
        }
        if (!Master.is_connected()){
            return 58;
        }
        if (Radio.is_installed()){
            return 59;
        }
        if (!pros::usd::is_installed()){
            if (!an_usd){
                an_usd = true;
                return 60;
            }
        } else {
            an_usd = false;
        }
        if (pros::battery::get_temperature() > 85){
            return 61; 
        }
        if ((pros::battery::get_current() / pros::battery::get_capacity()) * 100 < 30){
            if (!an_bat){
                an_bat = true;
                return 62;
            }
        } else {
            an_bat = false;
        }
        if ((pros::battery::get_current() / pros::battery::get_capacity()) * 100 < 15){
            return 63;
        }
        if ((pros::battery::get_current() / pros::battery::get_capacity()) * 100 < 5){
            return 64;
        }
        if ((Master.get_battery_level() < 30)){
            if (!an_ctr){
                an_ctr = true;
                return 65;
            }
        } else {
            an_ctr = false;
        }
        if ((Master.get_battery_level() < 15)){
            return 66;
        }
        if (!Inertial.is_installed()){
            if (!an_cal){
                an_cal = true;
                return 67;
            }
        } else {
            an_cal = false;
        }
        if (Inertial.is_calibrating()){
            return 77;
        }
        if (!OdomX.is_installed()){
            return 78;
        }
        if (!OdomY.is_installed()){
            return 79;
        }
        if (!Optical.is_installed()){
            if (!an_opt){
                an_opt = true;
                return 80;
            }
        } else {
            an_opt = false;
        }
        return 0;
    }
    void show_message(int id, pros::Motor& Left1, pros::Motor& Left2, pros::Motor& Left3, pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3, pros::Motor& LMotor, pros::Motor& UMotor, pros::Controller& Master, pros::Device& Radio, pros::IMU& Inertial, pros::Rotation& OdomX, pros::Rotation& OdomY, pros::Optical& Optical) {
    switch(id) {

        case 1:  Master.set_text(0, 0, "BD01: Left1 (" + std::to_string(Left1.get_port()) + ") Disabled"); break;
        case 2:  Master.set_text(0, 0, "BD02: Left2 (" + std::to_string(Left2.get_port()) + ") Disabled"); break;
        case 3:  Master.set_text(0, 0, "BD03: Left3 (" + std::to_string(Left3.get_port()) + ") Disabled"); break;
        case 4:  Master.set_text(0, 0, "BD04: Right1 (" + std::to_string(Right1.get_port()) + ") Disabled"); break;
        case 5:  Master.set_text(0, 0, "BD05: Right2 (" + std::to_string(Right2.get_port()) + ") Disabled"); break;
        case 6:  Master.set_text(0, 0, "BD06: Right3 (" + std::to_string(Right3.get_port()) + ") Disabled"); break;

        case 7:  Master.set_text(0, 0, "BT01: Left1 (" + std::to_string(Left1.get_port()) + ") Overtemp"); break;
        case 8:  Master.set_text(0, 0, "BT02: Left2 (" + std::to_string(Left2.get_port()) + ") Overtemp"); break;
        case 9:  Master.set_text(0, 0, "BT03: Left3 (" + std::to_string(Left3.get_port()) + ") Overtemp"); break;
        case 10: Master.set_text(0, 0, "BT04: Right1 (" + std::to_string(Right1.get_port()) + ") Overtemp"); break;
        case 11: Master.set_text(0, 0, "BT05: Right2 (" + std::to_string(Right2.get_port()) + ") Overtemp"); break;
        case 12: Master.set_text(0, 0, "BT06: Right3 (" + std::to_string(Right3.get_port()) + ") Overtemp"); break;

        case 13: Master.set_text(0, 0, "BF01: Left1 (" + std::to_string(Left1.get_port()) + ") Stall"); break;
        case 14: Master.set_text(0, 0, "BF02: Left2 (" + std::to_string(Left2.get_port()) + ") Stall"); break;
        case 15: Master.set_text(0, 0, "BF03: Left3 (" + std::to_string(Left3.get_port()) + ") Stall"); break;
        case 16: Master.set_text(0, 0, "BF04: Right1 (" + std::to_string(Right1.get_port()) + ") Stall"); break;
        case 17: Master.set_text(0, 0, "BF05: Right2 (" + std::to_string(Right2.get_port()) + ") Stall"); break;
        case 18: Master.set_text(0, 0, "BF06: Right3 (" + std::to_string(Right3.get_port()) + ") Stall"); break;

        case 19: Master.set_text(0, 0, "BF11: Left1 (" + std::to_string(Left1.get_port()) + ") Undereff"); break;
        case 20: Master.set_text(0, 0, "BF12: Left2 (" + std::to_string(Left2.get_port()) + ") Undereff"); break;
        case 21: Master.set_text(0, 0, "BF13: Left3 (" + std::to_string(Left3.get_port()) + ") Undereff"); break;
        case 22: Master.set_text(0, 0, "BF14: Right1 (" + std::to_string(Right1.get_port()) + ") Undereff"); break;
        case 23: Master.set_text(0, 0, "BF15: Right2 (" + std::to_string(Right2.get_port()) + ") Undereff"); break;
        case 24: Master.set_text(0, 0, "BF16: Right3 (" + std::to_string(Right3.get_port()) + ") Undereff"); break;

        case 25: Master.set_text(0, 0, "BC01: Left1 (" + std::to_string(Left1.get_port()) + ") Overcurrent"); break;
        case 26: Master.set_text(0, 0, "BC02: Left2 (" + std::to_string(Left2.get_port()) + ") Overcurrent"); break;
        case 27: Master.set_text(0, 0, "BC03: Left3 (" + std::to_string(Left3.get_port()) + ") Overcurrent"); break;
        case 28: Master.set_text(0, 0, "BC04: Right1 (" + std::to_string(Right1.get_port()) + ") Overcurrent"); break;
        case 29: Master.set_text(0, 0, "BC05: Right2 (" + std::to_string(Right2.get_port()) + ") Overcurrent"); break;
        case 30: Master.set_text(0, 0, "BC06: Right3 (" + std::to_string(Right3.get_port()) + ") Overcurrent"); break;

        case 31: Master.set_text(0, 0, "BH01: Left1 (" + std::to_string(Left1.get_port()) + ") H Over"); break;
        case 32: Master.set_text(0, 0, "BH02: Left2 (" + std::to_string(Left2.get_port()) + ") H Over"); break;
        case 33: Master.set_text(0, 0, "BH03: Left3 (" + std::to_string(Left3.get_port()) + ") H Over"); break;
        case 34: Master.set_text(0, 0, "BH04: Right1 (" + std::to_string(Right1.get_port()) + ") H Over"); break;
        case 35: Master.set_text(0, 0, "BH05: Right2 (" + std::to_string(Right2.get_port()) + ") H Over"); break;
        case 36: Master.set_text(0, 0, "BH06: Right3 (" + std::to_string(Right3.get_port()) + ") H Over"); break;

        case 37: Master.set_text(0, 0, "BH11: Left1 (" + std::to_string(Left1.get_port()) + ") H Fault"); break;
        case 38: Master.set_text(0, 0, "BH12: Left2 (" + std::to_string(Left2.get_port()) + ") H Fault"); break;
        case 39: Master.set_text(0, 0, "BH13: Left3 (" + std::to_string(Left3.get_port()) + ") H Fault"); break;
        case 40: Master.set_text(0, 0, "BH14: Right1 (" + std::to_string(Right1.get_port()) + ") H Fault"); break;
        case 41: Master.set_text(0, 0, "BH15: Right2 (" + std::to_string(Right2.get_port()) + ") H Fault"); break;
        case 42: Master.set_text(0, 0, "BH16: Right3 (" + std::to_string(Right3.get_port()) + ") H Fault"); break;

        case 43: Master.set_text(0, 0, "ID01: LMotor (" + std::to_string(LMotor.get_port()) + ") Disabled"); break;
        case 44: Master.set_text(0, 0, "ID02: UMotor (" + std::to_string(UMotor.get_port()) + ") Disabled"); break;
        case 45: Master.set_text(0, 0, "IT01: LMotor (" + std::to_string(LMotor.get_port()) + ") Overtemp"); break;
        case 46: Master.set_text(0, 0, "IT02: UMotor (" + std::to_string(UMotor.get_port()) + ") Overtemp"); break;
        case 47: Master.set_text(0, 0, "IF01: LMotor (" + std::to_string(LMotor.get_port()) + ") Stall"); break;
        case 48: Master.set_text(0, 0, "IF02: UMotor (" + std::to_string(UMotor.get_port()) + ") Stall"); break;
        case 49: Master.set_text(0, 0, "IF11: LMotor (" + std::to_string(LMotor.get_port()) + ") Undereff"); break;
        case 50: Master.set_text(0, 0, "IF12: UMotor (" + std::to_string(UMotor.get_port()) + ") Undereff"); break;
        case 51: Master.set_text(0, 0, "IC01: LMotor (" + std::to_string(LMotor.get_port()) + ") Overcurrent"); break;
        case 52: Master.set_text(0, 0, "IC02: UMotor (" + std::to_string(UMotor.get_port()) + ") Overcurrent"); break;
        case 53: Master.set_text(0, 0, "IH01: LMotor (" + std::to_string(LMotor.get_port()) + ") H Over"); break;
        case 54: Master.set_text(0, 0, "IH02: UMotor (" + std::to_string(UMotor.get_port()) + ") H Over"); break;
        case 55: Master.set_text(0, 0, "IH11: LMotor (" + std::to_string(LMotor.get_port()) + ") H Fault"); break;
        case 56: Master.set_text(0, 0, "IH12: UMotor (" + std::to_string(UMotor.get_port()) + ") H Fault"); break;

        case 57: Master.set_text(0, 0, "RW01: Robot Overspeed"); break;
        case 58: Master.set_text(0, 0, "RW02: Control Disconnected"); break;
        case 59: Master.set_text(0, 0, "RW03: Radio (" + std::to_string(Radio.get_port()) + ") Disconnected"); break;
        case 60: Master.set_text(0, 0, "RW04: USD Disabled"); break;
        case 61: Master.set_text(0, 0, "RW05: Battery Overtemp"); break;

        case 62: Master.set_text(0, 0, "CB01: Low Battery (30%)"); break;
        case 63: Master.set_text(0, 0, "CB02: Ultra Low Battery (15%)"); break;
        case 64: Master.set_text(0, 0, "CB03: Change Now (5%)"); break;

        case 65: Master.set_text(0, 0, "CC01: Control Low (30%)"); break;
        case 66: Master.set_text(0, 0, "CC02: Control Ultra (15%)"); break;

        case 67: Master.set_text(0, 0, "GO01: Colorsort Enabled"); break;
        case 68: Master.set_text(0, 0, "GO02: Idlemode Coast"); break;
        case 69: Master.set_text(0, 0, "GO03: Idlemode Brake"); break;
        case 70: Master.set_text(0, 0, "GO04: Idlemode Hold"); break;
        case 71: Master.set_text(0, 0, "GO05: Idlemode Smooth"); break;

        case 72: Master.set_text(0, 0, "GS01: Disabled mode"); break;
        case 73: Master.set_text(0, 0, "GS02: Autonomous mode"); break;

        case 74: Master.set_text(0, 0, "GR01: Balance Motors"); break;
        case 75: Master.set_text(0, 0, "GR02: Unbalance Motors"); break;

        case 76: Master.set_text(0, 0, "MI01: Inertial (" + std::to_string(Inertial.get_port()) + ") Disabled"); break;
        case 77: Master.set_text(0, 0, "MI02: Inertial (" + std::to_string(Inertial.get_port()) + ") Calibrating"); break;

        case 78: Master.set_text(0, 0, "MR01: OdomX (" + std::to_string(OdomX.get_port()) + ") Disabled"); break;
        case 79: Master.set_text(0, 0, "MR02: OdomY (" + std::to_string(OdomY.get_port()) + ") Disabled"); break;
        case 80: Master.set_text(0, 0, "MO01: Optical (" + std::to_string(Optical.get_port()) + ") Disabled"); break;

        default:
            Master.set_text(0, 0, "ERROR: ID not recognized");
            break;
        }
    }
    void vibrate(int id, pros::Controller& Master) {
    switch(id) {
        case 1:  Master.rumble("- .     "); break;
        case 2:  Master.rumble("- .     "); break;
        case 3:  Master.rumble("- .     "); break;
        case 4:  Master.rumble("- .     "); break;
        case 5:  Master.rumble("- .     "); break;
        case 6:  Master.rumble("- .     "); break;

        case 7:  Master.rumble("- . .   "); break;
        case 8:  Master.rumble("- . .   "); break;
        case 9:  Master.rumble("- . .   "); break;
        case 10: Master.rumble("- . .   "); break;
        case 11: Master.rumble("- . .   "); break;
        case 12: Master.rumble("- . .   "); break;

        case 13: Master.rumble("--.     "); break;
        case 14: Master.rumble("--.     "); break;
        case 15: Master.rumble("--.     "); break;
        case 16: Master.rumble("--.     "); break;
        case 17: Master.rumble("--.     "); break;
        case 18: Master.rumble("--.     "); break;

        case 19: Master.rumble(". - .   "); break;
        case 20: Master.rumble(". - .   "); break;
        case 21: Master.rumble(". - .   "); break;
        case 22: Master.rumble(". - .   "); break;
        case 23: Master.rumble(". - .   "); break;
        case 24: Master.rumble(". - .   "); break;

        case 25: Master.rumble("-.-     "); break;
        case 26: Master.rumble("-.-     "); break;
        case 27: Master.rumble("-.-     "); break;
        case 28: Master.rumble("-.-     "); break;
        case 29: Master.rumble("-.-     "); break;
        case 30: Master.rumble("-.-     "); break;

        case 31: Master.rumble("-- -    "); break;
        case 32: Master.rumble("-- -    "); break;
        case 33: Master.rumble("-- -    "); break;
        case 34: Master.rumble("-- -    "); break;
        case 35: Master.rumble("-- -    "); break;
        case 36: Master.rumble("-- -    "); break;

        case 37: Master.rumble("---     "); break;
        case 38: Master.rumble("---     "); break;
        case 39: Master.rumble("---     "); break;
        case 40: Master.rumble("---     "); break;
        case 41: Master.rumble("---     "); break;
        case 42: Master.rumble("---     "); break;

        case 43: Master.rumble("-. -    "); break;
        case 44: Master.rumble("-. -    "); break;

        case 45: Master.rumble("-.. .   "); break;
        case 46: Master.rumble("-.. .   "); break;

        case 47: Master.rumble("--..    "); break;
        case 48: Master.rumble("--..    "); break;

        case 49: Master.rumble(". - ..  "); break;
        case 50: Master.rumble(". - ..  "); break;

        case 51: Master.rumble("-.-     "); break;
        case 52: Master.rumble("-.-     "); break;

        case 53: Master.rumble("---.    "); break;
        case 54: Master.rumble("---.    "); break;

        case 55: Master.rumble("----.   "); break;
        case 56: Master.rumble("----.   "); break;

        case 57: Master.rumble(". .-.   "); break;
        case 58: Master.rumble("---.    "); break;
        case 59: Master.rumble("- --    "); break;
        case 60: Master.rumble(".. ..   "); break;
        case 61: Master.rumble("-.-.    "); break;

        case 62: Master.rumble(". . .   "); break;
        case 63: Master.rumble("-. .    "); break;
        case 64: Master.rumble("---- -- "); break;

        case 65: Master.rumble(".. .    "); break;
        case 66: Master.rumble("--.     "); break;

        case 67: Master.rumble(". .     "); break;
        case 68: Master.rumble(". .     "); break;
        case 69: Master.rumble(". .     "); break;
        case 70: Master.rumble(". .     "); break;
        case 71: Master.rumble(". .     "); break;
        case 72: Master.rumble(". .     "); break;
        case 73: Master.rumble(". .     "); break;
        case 74: Master.rumble(". .     "); break;
        case 75: Master.rumble(". .     "); break;

        case 76: Master.rumble("-..-    "); break;
        case 77: Master.rumble("..-.    "); break;
        case 78: Master.rumble(".-.-    "); break;
        case 79: Master.rumble("..--    "); break;
        case 80: Master.rumble("-.-.    "); break;

        default:
            break;
        }
    }

    void handle_error(int id, pros::Motor& Left1, pros::Motor& Left2, pros::Motor& Left3, pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3, pros::Motor& LMotor, pros::Motor& UMotor, pros::Controller& Master, pros::Device& Radio, pros::IMU& Inertial, pros::Rotation& OdomX, pros::Rotation& OdomY, pros::Optical& Optical) {
        show_message(id, Left1, Left2, Left3, Right1, Right2, Right3, LMotor, UMotor, Master, Radio, Inertial, OdomX, OdomY, Optical);
        vibrate(id, Master);
    }

    void launch(pros::Motor& Left1, pros::Motor& Left2, pros::Motor& Left3, pros::Motor& Right1, pros::Motor& Right2, pros::Motor& Right3, pros::Motor& LMotor, pros::Motor& UMotor, pros::Controller& Master, pros::Device& Radio, pros::IMU& Inertial, pros::Rotation& OdomX, pros::Rotation& OdomY, pros::Optical& Optical) {
        int error_id = check_errors(Left1, Left2, Left3, Right1, Right2, Right3, LMotor, UMotor, Master, Radio, Inertial, OdomX, OdomY, Optical);
        if (error_id == 0) {
            Master.clear_line(0);
            lastime = 0;
            return;
        }
        if ((pros::millis() - lastime) > 5000) {
            if (error_id != 0) {
                handle_error(error_id, Left1, Left2, Left3, Right1, Right2, Right3, LMotor, UMotor, Master, Radio, Inertial, OdomX, OdomY, Optical);
            }
        }
    }

}