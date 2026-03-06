////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      chassis.cpp                            //
// Funcion:        Manejo de chasis                       //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////
#pragma once
#include "api.h"
#include "pid.h"
#include "geo.h"

enum BrakeMode { Coast, Instant, Smooth };

class NeoChassis {
public:
    NeoChassis(pros::Motor &left1, pros::Motor &left2, pros::Motor &left3,
               pros::Motor &right1, pros::Motor &right2, pros::Motor &right3,
               pros::Rotation &RotX, pros::Rotation &RotY, pros::IMU &Inertial,
               AbsolutePositionSystem &aps);

    void move(int leftSpeed, int rightSpeed);
    void setBrakeMode(BrakeMode mode);
    void setMaxVelocity(int maxVel);
    void setMinVelocity(int minVel);
    void setDeathZone(int deathZone);
    void brake();

    void pid_turn_to(double targetAngle,
                     double kP, double kI, double kD, double startI,
                     double settleError, double settleTime, double windup);

    void pid_look_at(double targetX, double targetY,
                     double kP, double kI, double kD, double startI,
                     double settleError, double settleTime, double windup);

    void pid_move_to(double targetX, double targetY,
                     double turnkP, double turnkI, double turnkD, double turnStartI,
                     double turnSettleError, double turnSettleTime, double turnWindup,
                     double distkP, double distkI, double distkD, double distStartI,
                     double distSettleError, double distSettleTime, double distWindup, bool moonwalk = false);
    void pid_advance_straight(double inches, double kP, double kI, double kD,
                             double startI, double settleError, double settleTime, double windup);

private:
    pros::Motor &left1, &left2, &left3;
    pros::Motor &right1, &right2, &right3;
    pros::Rotation &RotX, &RotY;
    pros::IMU &Inertial;
    AbsolutePositionSystem &aps;

    int maxVelocity;
    int minVelocity;
    int dZone;
    BrakeMode brakeMode;
    bool isPidRunning;

    void p_move(int leftSpeed, int rightSpeed);
    int  encoderToPercent(int value, pros::MotorGears gearset);

    void brake_coast();
    void brake_instant();
    void advbrake();

    void pid_turn(double targetAngle,
                  double kP, double kI, double kD, double startI,
                  double settleError, double settleTime, double windup);

    void pid_look(double targetX, double targetY,
                  double kP, double kI, double kD, double startI,
                  double settleError, double settleTime, double windup);
    
    void pid_straight(double inches, double kP, double kI, double kD,
                  double startI, double settleError, double settleTime, double windup);

    void pid_move(double targetX, double targetY,
                  double turnkP, double turnkI, double turnkD, double turnStartI,
                  double turnSettleError, double turnSettleTime, double turnWindup,
                  double distkP, double distkI, double distkD, double distStartI,
                  double distSettleError, double distSettleTime, double distWindup, bool moonwalk);
};