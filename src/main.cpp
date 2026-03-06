// Ayayay el regional
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
//        Script principal        //       main.cpp       //
//                                //                      //
////////////////////////////////////////////////////////////
#include "main.h"
#include <stdio.h>
#include <string>
#include "liblvgl/llemu.hpp"
#include "liblvgl/lvgl.h"
#include <memory>
#include "pid.h"
#include "chassis.h"
#include "geo.h"
#include "pros/device.hpp"
#include "pros/optical.h"
#include "pros/rtos.hpp"
using namespace pros;

Controller master(E_CONTROLLER_MASTER);

// Motores y chasis
Motor left1(-2, MotorGears::blue, MotorUnits::degrees);
Motor left2(-4, MotorGears::blue, MotorUnits::degrees);
Motor left3(-5, MotorGears::blue, MotorUnits::degrees);
Motor right1(6, MotorGears::blue, MotorUnits::degrees);
Motor right2(7, MotorGears::blue, MotorUnits::degrees);
Motor right3(8, MotorGears::blue, MotorUnits::degrees);

// Odometria
Rotation RotX(11);
Rotation RotY(12);
IMU Inertial(13);
AbsolutePositionSystem APS(RotX, RotY, Inertial, -0.10, 2.36, 2);

pros::adi::DigitalOut Piston1('A');
pros::adi::DigitalOut Piston2('B');

// Funciones miscelaneas
double gradosReales(double gradosAGirar) {
    return gradosAGirar / 0.75;
}

double cmToTicks(double cm) {
    return cm * (600.0 / 26.0); // ≈ cm * 23.0769
}

int encoderToPercent(int value, MotorGears gearset) {
	int maxRPM;
	
	switch ((int)gearset) {
		case 0: maxRPM = 100;  break;
		case 1: maxRPM = 200;  break;
		case 2: maxRPM = 600;  break;
		default: return 0;
	}
    
    // Clamp para no salirse del rango
    if (value >  maxRPM) value =  maxRPM;
    if (value < -maxRPM) value = -maxRPM;
    
    return (value * 100) / maxRPM;
}

double cmToDegrees(double cm) {
    return cm * (360.0 / 26.0); // ≈ cm * 13.846
}

double degreesToInches(double centidegrees){
	double inches_per_degree = (M_PI * 2) / 360.0;
	double distance_in = centidegrees * inches_per_degree;
	return distance_in;
}

// LVGL Code


void initialize(){
	//show_logo("/usd/balatro.ipros");
	//delay(5000);
	//stop_drawing();
	//screen::erase();
	//screen::erase();
	//delay(500);
	Piston1.set_value(false);
	Piston2.set_value(false);
	lcd::initialize();
}

void resetMotores(){
	left1.tare_position();
	left2.tare_position();
	left3.tare_position();
	right1.tare_position();
	right2.tare_position();
	right3.tare_position();
}

void motorMove(int leftSpeed, int rightSpeed){
	left1.move(leftSpeed);
	left2.move(leftSpeed);
	left3.move(leftSpeed);
	right1.move(rightSpeed);
	right2.move(rightSpeed);
	right3.move(rightSpeed);
}

void motorUnbrake(){
	left1.set_brake_mode(MotorBrake::coast);
	left2.set_brake_mode(MotorBrake::coast);
	left3.set_brake_mode(MotorBrake::coast);
	right1.set_brake_mode(MotorBrake::coast);
	right2.set_brake_mode(MotorBrake::coast);
	right3.set_brake_mode(MotorBrake::coast);
}

void motorBrakeHold(){
	left1.set_brake_mode(MotorBrake::hold);
	left2.set_brake_mode(MotorBrake::hold);
	left3.set_brake_mode(MotorBrake::hold);
	right1.set_brake_mode(MotorBrake::hold);
	right2.set_brake_mode(MotorBrake::hold);
	right3.set_brake_mode(MotorBrake::hold);
	left1.brake();
	left2.brake();
	left3.brake();
	right1.brake();
	right2.brake();
	right3.brake();
}


void motorBrake(){
	left1.set_brake_mode(MotorBrake::brake);
	left2.set_brake_mode(MotorBrake::brake);
	left3.set_brake_mode(MotorBrake::brake);
	right1.set_brake_mode(MotorBrake::brake);
	right2.set_brake_mode(MotorBrake::brake);
	right3.set_brake_mode(MotorBrake::brake);
	left1.brake();
	left2.brake();
	left3.brake();
	right1.brake();
	right2.brake();
	right3.brake();
}


// Optical a(2);
// 		a.set_led_pwm(100);
// 		bool isred = a.get_hue() >= 340 && a.get_hue() < 360 || a.get_hue() >= 0 && a.get_hue() <= 20;
// 		bool isblue = a.get_hue() >= 190 && a.get_hue() <= 270;
// 		delay(200);
// 		if (a.get_proximity() > 230){
// 			if(isred){lcd::set_text(1, "Bloque color rojo");}
// 			else if(isblue){lcd::set_text(1, "Bloque color azul");}
// 			else {lcd::set_text(1, "Bloque desconocido");}
// 		} else {lcd::set_text(1, "No se han detectado bloques");}
		
bool hasvib = -1;
bool lastMotorState = false;

void advbrake(){ 
	left1.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	left2.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	left3.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	right1.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	right2.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	right3.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	left2.brake();
	right2.brake();
	if (encoderToPercent(left2.get_actual_velocity(), left2.get_gearing()) < 70 && encoderToPercent(right2.get_actual_velocity(), right2.get_gearing()) < 70){
		left1.brake();
		right1.brake();
		if (encoderToPercent(left1.get_actual_velocity(), left1.get_gearing()) < 70 && encoderToPercent(right1.get_actual_velocity(), right1.get_gearing()) < 70){
			right3.brake();
			left3.brake();
		}
	}
}



void opcontrol(){
	Motor intake1(19);
	Motor intake2(-14);
	adi::DigitalOut p1('A');
	adi::DigitalOut p2('B');
	bool p1e = false;
	bool p2e = false;
	while (true){
		// if (!checkMotors()){
			lcd::set_text(3, std::to_string(left1.get_actual_velocity()));
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)){
				p1e = !p1e;
				p1.set_value(p1e);
			}
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
				p2e = !p2e;
				p2.set_value(p2e);
			}
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
				intake1.move(127); intake2.move(0);
			}
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
				intake1.move(127); intake2.move(127);
			}
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
				intake1.move(-127); intake2.move(-127);
			}
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
				intake1.move(0); intake2.move(0);
			}
			if (master.get_analog(ANALOG_LEFT_Y) < 5 &&
					master.get_analog(ANALOG_LEFT_Y) > -5 &&
					master.get_analog(ANALOG_RIGHT_Y) < 5 &&
					master.get_analog(ANALOG_RIGHT_Y) > -5){
					
				} else {
					motorUnbrake();
			}
			motorMove(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		// }
		delay(20);
	}
	// APS.reset_cords(24, 12, 90);
	// while (true){
	// 	if (checkMotors()) return;
	// 	APS.update_cords();
	// 	AbsolutePositionSystem::Coordinates pos = APS.get_coordinates();
	// 	APS.draw_graph(pos.X, pos.Y, pos.Theta, 2.0);
	// 	if (master.get_analog(ANALOG_LEFT_Y) < 5 &&
    //             master.get_analog(ANALOG_LEFT_Y) > -5 &&
    //             master.get_analog(ANALOG_RIGHT_Y) < 5 &&
    //             master.get_analog(ANALOG_RIGHT_Y) > -5){
    //             motorBrakeHold();
    //         } else {
    //             motorUnbrake();
    //         }
	// 	motorMove(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
	// 	delay(20);
	// }
    // while (true){ 
    //     bool motorError = checkMotors();

    //     if (motorError && !lastMotorState) {
    //         vib();
    //     }

    //     lastMotorState = motorError;

    //     if (!motorError){
    //         motorMove(master.get_analog(ANALOG_LEFT_Y),
    //                   master.get_analog(ANALOG_RIGHT_Y));

    //         if (master.get_analog(ANALOG_LEFT_Y) < 5 &&
    //             master.get_analog(ANALOG_LEFT_Y) > -5 &&
    //             master.get_analog(ANALOG_RIGHT_Y) < 5 &&
    //             master.get_analog(ANALOG_RIGHT_Y) > -5){
    //             motorBrakeHold();
    //         } else {
    //             motorUnbrake();
    //         }
    //     }

    //     delay(20);
    // }
}





void pidMove(double target, double kP, double kI, double kD, double startI, double settleError, double settleTime, double windup) {
	resetMotores();
	if (checkMotors()) return;
	float accumulated_error = 0;
	float last_error = 0;
	float time_in = 0;
	while (true){
		float time_ = millis();
		float actual = left1.get_position();
		float error = target - actual;
		if (fabs(error) < startI && startI != -1){
			accumulated_error += error;
		}
		if (accumulated_error > windup) accumulated_error = windup;
		if ((error>0 && last_error < 0)||(error<0 && last_error > 0)){
			accumulated_error = 0;
		}
		float derivative = error - last_error;
		float output = (kP * error) + (kI * accumulated_error) + (kD * derivative);
		last_error = error;
		if (output > 127) output = 127;
		if (output < -127) output = -127;
		motorMove(output, output);
		time_ = millis() - time_;
		if (fabs(error) < settleError){
			time_in += time_;
		} else {
			time_in = 0;
		}
		if (time_in >= settleTime){
			break;
		}
		pros::delay(10);
	}
}

void autonomous(){
	
}