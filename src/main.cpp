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
#include "liblvgl/lvgl.h"
#include <memory>
using namespace pros;

Controller master(E_CONTROLLER_MASTER);

// Motores y chasis
Motor left1(-1, MotorGears::blue, MotorUnits::degrees);
Motor left2(-2, MotorGears::blue, MotorUnits::degrees);
Motor left3(-3, MotorGears::blue, MotorUnits::degrees);
Motor right1(7, MotorGears::blue, MotorUnits::degrees);
Motor right2(10, MotorGears::blue, MotorUnits::degrees);
Motor right3(11, MotorGears::blue, MotorUnits::degrees);


// Funciones miscelaneas
double gradosReales(double gradosAGirar) {
    return gradosAGirar / 0.75;
}

double cmToTicks(double cm) {
    return cm * (600.0 / 26.0); // ≈ cm * 23.0769
}

double cmToDegrees(double cm) {
    return cm * (360.0 / 26.0); // ≈ cm * 13.846
}



// LVGL Code


void initialize(){
	//show_logo("/usd/balatro.ipros");
	//delay(5000);
	//stop_drawing();
	//screen::erase();
	//screen::erase();
	//delay(500);
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


void opcontrol(){
	while(true){
		motorMove(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_RIGHT_Y));
		if (master.get_analog(ANALOG_LEFT_Y) < 5 && master.get_analog(ANALOG_LEFT_Y) > -5 &&
		    master.get_analog(ANALOG_RIGHT_Y) < 5 && master.get_analog(ANALOG_RIGHT_Y) > -5){
			motorBrakeHold();
		} else {
			motorUnbrake();
		}
		delay(20);
	}
}


void pidMove(double target, double kP, double kI, double kD, double startI, double settleError, double settleTime) {
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
	resetMotores();
	motorUnbrake();
	pidMove(gradosReales(cmToDegrees(57)), 1, 0, 1, 0, cmToDegrees(1), 20);
	motorMove(0, 0);
	motorBrake();
}