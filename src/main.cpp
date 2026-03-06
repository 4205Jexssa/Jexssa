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
#include "pros/misc.h"
#include "pros/rtos.h"
#include "redundance.h"
#include "liblvgl/llemu.hpp"
#include "pid.h"
#include "chassis.h"
#include "geo.h"
#include "pros/device.hpp"
#include "pros/rtos.hpp"
#include "autons.h"
#include "autons_admin.h"
#include <string>
using namespace pros;

Controller master(E_CONTROLLER_MASTER);
Device radio(21);
Optical optical(20);

// Motores y chasis
Motor left1(-1, MotorGears::blue, MotorUnits::degrees);
Motor left2(2, MotorGears::blue, MotorUnits::degrees);
Motor left3(-3, MotorGears::blue, MotorUnits::degrees);
Motor right1(4, MotorGears::blue, MotorUnits::degrees);
Motor right2(-5, MotorGears::blue, MotorUnits::degrees);
Motor right3(6, MotorGears::blue, MotorUnits::degrees);
Motor intake1(8);
Motor intake2(-11);

// Odometria
Rotation RotX(17);
Rotation RotY(15);
IMU Inertial(19);
AbsolutePositionSystem APS(RotX, RotY, Inertial, 2.5240, 0.2005, 2);

pros::adi::DigitalOut Piston1('A');
pros::adi::DigitalOut Piston2('B');

// Funciones miscelaneas
double gradosReales(double gradosAGirar) {
    return gradosAGirar / 0.75;
}

double cmToInches(double cm) {
	return cm * 0.393701;
}

double cmToTicks(double cm) {
    return cm * (600.0 / 26.0); // ≈ cm * 23.0769
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
	Piston2.set_value(true);
	lcd::initialize();
	autonregister();
	APS.reset_cords(0, 0, 0);
}

void resetMotores(){
	left1.tare_position();
	left2.tare_position();
	left3.tare_position();
	right1.tare_position();
	right2.tare_position();
	right3.tare_position();
}

void motorUnbrake();

void motorMove(int leftSpeed, int rightSpeed){
	motorUnbrake();
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

void disabled(){
	selector_init();
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

NeoChassis chassis(left1, left2, left3, right1, right2, right3, RotX, RotY, Inertial, APS);

////////////////////////////////////////////////////////////
// Función: tunoffset                                     //
// Itera hasta que el drift sea menor a 0.1" o 10 rondas//
////////////////////////////////////////////////////////////

void tunoffset(AbsolutePositionSystem& aps, NeoChassis& chassis,
               double currentOffX, double currentOffY,
               int turnSpeed = 127) {

    double offX = currentOffX;
    double offY = currentOffY;
    const double DRIFT_THRESHOLD = 0.1; // pulgadas
    const int MAX_ROUNDS = 15;

    for (int round = 1; round <= MAX_ROUNDS; round++) {

        // Mostrar ronda actual
        char buf[100];
        snprintf(buf, sizeof(buf), "Ronda %d/10 - girando...", round);
        pros::lcd::set_text(1, buf);

        // Resetear posición
        aps.reset_cords(0, 0, 0);
        pros::delay(500);

        double startAngle = aps.get_coordinates().Theta;

        // Girar 10 segundos
        uint32_t startTime = pros::millis();
        while (pros::millis() - startTime < 10000) {
            aps.update_cords();
            chassis.move(-turnSpeed, turnSpeed);
            pros::delay(10);
        }
        chassis.move(0, 0);
        pros::delay(300);

        // Leer drift
        aps.update_cords();
        auto finalPos = aps.get_coordinates();
        double driftX = finalPos.X;
        double driftY = finalPos.Y;

        double totalDegrees = finalPos.Theta - startAngle;
        double turns = totalDegrees / 360.0;

        // Calcular nuevos offsets
        double newOffX = offX - (driftY / (2.0 * M_PI * turns));
        double newOffY = offY - (driftX / (2.0 * M_PI * turns));

        // Mostrar resultados de esta ronda
        snprintf(buf, sizeof(buf), "Ronda %d - Drift X:%.3f Y:%.3f", round, driftX, driftY);
        pros::lcd::set_text(1, buf);

        snprintf(buf, sizeof(buf), "Vueltas: %.2f", turns);
        pros::lcd::set_text(2, buf);

        snprintf(buf, sizeof(buf), "offX: %.4f -> %.4f", offX, newOffX);
        pros::lcd::set_text(3, buf);

        snprintf(buf, sizeof(buf), "offY: %.4f -> %.4f", offY, newOffY);
        pros::lcd::set_text(4, buf);

        offX = newOffX;
        offY = newOffY;

        // Verificar si el drift ya es despreciable
        double totalDrift = sqrt(driftX * driftX + driftY * driftY);
        if (totalDrift < DRIFT_THRESHOLD) {
            snprintf(buf, sizeof(buf), "LISTO en ronda %d (drift<0.1\")", round);
            pros::lcd::set_text(5, buf);
            break;
        }

        if (round < MAX_ROUNDS) {
            pros::lcd::set_text(5, "Siguiente ronda en 3s...");
            pros::delay(3000);
        }
    }

    // Mostrar valores finales
    char buf[100];
    pros::lcd::set_text(6, "=== VALORES FINALES ===");
    snprintf(buf, sizeof(buf), "offX final: %.4f", offX);
    pros::lcd::set_text(7, buf);
    snprintf(buf, sizeof(buf), "offY final: %.4f", offY);
    pros::lcd::set_text(8, buf);

    // Esperar para que puedas leer
    pros::delay(30000);
}

bool left1enabled = true;
bool left2enabled = true;
bool left3enabled = true;
bool right1enabled = true;
bool right2enabled = true;
bool right3enabled = true;
bool redudEnabled = false;

bool checkRedud(){
	left1enabled = left1.is_installed();
	left2enabled = left2.is_installed();
	left3enabled = left3.is_installed();
	right1enabled = right1.is_installed();
	right2enabled = right2.is_installed();
	right3enabled = right3.is_installed();
	if (left1enabled && left2enabled && left3enabled && right1enabled && right2enabled && right3enabled){
		return true;
	} else {
		return false;
	}
}

void redudmove(int leftSpeed, int rightSpeed){
	if (left1enabled) {right1enabled = true;} else {right1enabled = false;}
	if (left2enabled) {right2enabled = true;} else {right2enabled = false;}
	if (left3enabled) {right3enabled = true;} else {right3enabled = false;}
	if (right1enabled) {left1enabled = true;} else {left1enabled = false;}
	if (right2enabled) {left2enabled = true;} else {left2enabled = false;}
	if (right3enabled) {left3enabled = true;} else {left3enabled = false;}
	if (left1enabled) left1.move(leftSpeed); else left1.move(0);
	if (left2enabled) left2.move(leftSpeed); else left2.move(0);
	if (left3enabled) left3.move(leftSpeed); else left3.move(0);
	if (right1enabled) right1.move(rightSpeed); else right1.move(0);
	if (right2enabled) right2.move(rightSpeed); else right2.move(0);
	if (right3enabled) right3.move(rightSpeed); else right3.move(0);
}

bool lastChargeStatus = false;
int activateTimestamp = 0;
bool alreadyTimeout = false;
int intakeTimestamp = 0;
bool intakeTimeout = false;

void autonomous();

// APS.update_cords();
// 		AbsolutePositionSystem::Coordinates pos = APS.get_coordinates();
// 		APS.draw_graph(pos.X, pos.Y, pos.Theta, 2.0);
// 		char buf[64];
// 		snprintf(buf, sizeof(buf), "(%.2f, %.2f - %.2f)",
// 				 pos.X, pos.Y, pos.Theta);
// 		delay(20);
// 		if (master.get_digital_new_press(DIGITAL_A)){
// 			APS.reset_cords(0, 0, 0);
// 		}
// 		if (recorder){
// 			master.set_text(0, 0, buf);
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)){
// 				p1e = !p1e;
// 				p1.set_value(p1e);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
// 				p2e = !p2e;
// 				p2.set_value(p2e);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
// 				m1e = !m1e;
// 				if (m1e){
// 					intake1.move(127);
// 					intake2.move(0);
// 				} else {
// 					intake1.move(127);
// 					intake2.move(127);
// 				}
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
// 				m1e = false;
// 				intake1.move(0);
// 				intake2.move(0);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
// 				m1e = false;
// 				intake1.move(-127);
// 				intake2.move(-127);
// 			}
// 			if (master.get_analog(ANALOG_LEFT_Y) < 5 &&
// 					master.get_analog(ANALOG_LEFT_Y) > -5 &&
// 					master.get_analog(ANALOG_RIGHT_Y) < 5 &&
// 					master.get_analog(ANALOG_RIGHT_Y) > -5){
// 					motorBrake();
// 				} else {
// 					motorUnbrake();
// 					motorMove(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_RIGHT_Y));
// 			}
// 			if (master.get_digital(pros::E_CONTROLLER_DIGITAL_UP)){
// 				motorMove(127/5, 127/5);
// 			} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)){
// 				motorMove(-127/5, -127/5);
// 			} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)){
// 				motorMove(-127/5, 127/5);
// 			} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)){
// 				motorMove(127/5, -127/5);
// 			}
// 		} else {
// 			lcd::set_text(0, std::to_string(intake1.get_torque()));
// 			lcd::set_text(1, std::to_string(intake2.get_torque()));
// 			if ((millis() - activateTimestamp == 6000) && !alreadyTimeout){
// 				master.rumble("..");
// 				p2e = true;
// 				p2.set_value(p2e);
// 				alreadyTimeout = true;
// 			}
// 			if ((millis() - intakeTimestamp == 6000) && !intakeTimeout){
// 				master.rumble("--");
// 				intake2.move(0);
// 				intakeTimeout = true;
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
// 				p1e = !p1e;
// 				if (p1e){
// 					lastChargeStatus = p2e;
// 					p2e = true;
// 				} else {
// 					p2e = lastChargeStatus;
// 				}
// 				p2.set_value(p2e);
// 				p1.set_value(p1e);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y) && !p1e){
// 				alreadyTimeout = false;
// 				activateTimestamp = millis();
// 				p2e = !p2e;
// 				p2.set_value(p2e);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
// 				m1e = !m1e;
// 				if (m1e){
// 					intake1.move(127);
// 					intake2.move(0);
// 				} else {
// 					intake1.move(127);
// 					intake2.move(127);
// 					intakeTimestamp = millis();
// 					intakeTimeout = false;
// 				}
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
// 				m1e = false;
// 				intake1.move(0);
// 				intake2.move(0);
// 			}
// 			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
// 				m1e = false;
// 				intake1.move(-127);
// 				intake2.move(-127);
// 			}
// 			bool check = checkRedud();
// 			if (!check && master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
// 				redudEnabled = !redudEnabled;
// 			}
// 			if (master.get_analog(ANALOG_LEFT_Y) < 5 &&
// 					master.get_analog(ANALOG_LEFT_Y) > -5 &&
// 					master.get_analog(ANALOG_RIGHT_Y) < 5 &&
// 					master.get_analog(ANALOG_RIGHT_Y) > -5){
// 					motorBrake();
// 				} else {
// 					motorUnbrake();

void opcontrol(){
	const bool recorder = false;
	adi::DigitalOut p1('A');
	adi::DigitalOut p2('B');
	bool p1e = false;
	bool p2e = false;
	bool m1e = false;
	APS.reset_cords(0, 0, 0);
	while (true){
		motorMove(master.get_analog(ANALOG_LEFT_Y) - master.get_analog(ANALOG_RIGHT_Y), master.get_analog(ANALOG_LEFT_Y) + master.get_analog(ANALOG_RIGHT_Y));
		if (master.get_digital_new_press(DIGITAL_A)){
			p1e = !p1e;
			p1.set_value(p1e);
		}
		if (master.get_digital_new_press(DIGITAL_B)){
			p2e = !p2e;
			p2.set_value(p2e);
		}
		if (master.get_digital_new_press(DIGITAL_R1)){
			m1e = !m1e;
			if (m1e){
				intake1.move(127);
				intake2.move(0);
			} else {
				intake1.move(127);
				intake2.move(127);
			}
		}
		if (master.get_digital_new_press(DIGITAL_R2)){
			m1e = false;
			intake1.move(0);
			intake2.move(0);
		}
		if (master.get_digital_new_press(DIGITAL_L1)){
			m1e = false;
			intake1.move(-127);
			intake2.move(-127);
		}
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

const int maxvel = 127;

#define activar_recarga Piston2.set_value(false);
#define desactivar_recarga Piston2.set_value(true);
#define avanzar(X) APS.reset_cords(0, 0, 0); chassis.pid_move_to(0, X, 3.0, 0.05, 20, 15, 1.0, 90, 100, 20, 0, 100, 0, 1.0, 50, 100, true);
#define girar(X) APS.reset_cords(0, 0, 0); chassis.pid_turn_to(-90,  3.0, 0.05, 20, 15, 1.5, 90, 100);
#define absorber intake1.move(127); intake2.move(0);
#define anotar intake1.move(127); intake2.move(127);
#define expulsar intake1.move(-127); intake2.move(-127);
#define pausar intake1.move(0); intake2.move(0);


void autonomous(){
	activar_recarga
	avanzar(-36)
	girar(-88.7)
	absorber
	avanzar(-15)
	delay(3000);
	motorMove(0,0);
}