// Me creo barcbots
////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
//   ESTA FUNCION ESTA EN ESTADO ALPHA, NO ES FUNCIONAL   //
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
//       de los motores del       //      chassis.cpp     //
//             robot              //                      //
////////////////////////////////////////////////////////////

#include "chassis.h"
#include "pid.h"
#include "geo.h"
#include "main.h"
#include <cmath>

// ─── Constructor ────────────────────────────────────────────────────────────
NeoChassis::NeoChassis(pros::Motor &left1, pros::Motor &left2, pros::Motor &left3,
                       pros::Motor &right1, pros::Motor &right2, pros::Motor &right3,
                       pros::Rotation &RotX, pros::Rotation &RotY, pros::IMU &Inertial,
                       AbsolutePositionSystem &aps)
    : left1(left1), left2(left2), left3(left3)
    , right1(right1), right2(right2), right3(right3)
    , RotX(RotX), RotY(RotY), Inertial(Inertial)
    , aps(aps)
    , maxVelocity(127), minVelocity(-127), dZone(5)
    , brakeMode(Coast), isPidRunning(false)
{}

// ─── Privados ────────────────────────────────────────────────────────────────
void NeoChassis::p_move(int leftSpeed, int rightSpeed) {
    left1.move(leftSpeed);
    left2.move(leftSpeed);
    left3.move(leftSpeed);
    right1.move(rightSpeed);
    right2.move(rightSpeed);
    right3.move(rightSpeed);
}

int NeoChassis::encoderToPercent(int value, pros::MotorGears gearset) {
    int maxRPM;
    switch ((int)gearset) {
        case 0: maxRPM = 100; break;
        case 1: maxRPM = 200; break;
        case 2: maxRPM = 600; break;
        default: return 0;
    }
    if (value >  maxRPM) value =  maxRPM;
    if (value < -maxRPM) value = -maxRPM;
    return (value * 100) / maxRPM;
}

void NeoChassis::brake_coast() {
    left1.set_brake_mode(pros::MotorBrake::coast);
    left2.set_brake_mode(pros::MotorBrake::coast);
    left3.set_brake_mode(pros::MotorBrake::coast);
    right1.set_brake_mode(pros::MotorBrake::coast);
    right2.set_brake_mode(pros::MotorBrake::coast);
    right3.set_brake_mode(pros::MotorBrake::coast);
    p_move(0, 0);
    left1.brake(); left2.brake(); left3.brake();
    right1.brake(); right2.brake(); right3.brake();
}

void NeoChassis::brake_instant() {
    left1.set_brake_mode(pros::MotorBrake::brake);
    left2.set_brake_mode(pros::MotorBrake::brake);
    left3.set_brake_mode(pros::MotorBrake::brake);
    right1.set_brake_mode(pros::MotorBrake::brake);
    right2.set_brake_mode(pros::MotorBrake::brake);
    right3.set_brake_mode(pros::MotorBrake::brake);
    p_move(0, 0);
    left1.brake(); left2.brake(); left3.brake();
    right1.brake(); right2.brake(); right3.brake();
}

void NeoChassis::advbrake() {
    left1.set_brake_mode(pros::MotorBrake::brake);
    left2.set_brake_mode(pros::MotorBrake::brake);
    left3.set_brake_mode(pros::MotorBrake::brake);
    right1.set_brake_mode(pros::MotorBrake::brake);
    right2.set_brake_mode(pros::MotorBrake::brake);
    right3.set_brake_mode(pros::MotorBrake::brake);
    p_move(0, 0);
    left2.brake(); right2.brake();
    if (encoderToPercent(left2.get_actual_velocity(),  left2.get_gearing())  < 70 &&
        encoderToPercent(right2.get_actual_velocity(), right2.get_gearing()) < 70) {
        left1.brake(); right1.brake();
        if (encoderToPercent(left1.get_actual_velocity(),  left1.get_gearing())  < 70 &&
            encoderToPercent(right1.get_actual_velocity(), right1.get_gearing()) < 70) {
            left3.brake(); right3.brake();
        }
    }
}

void NeoChassis::pid_turn(double targetAngle, double kP, double kI, double kD,
                          double startI, double settleError, double settleTime, double windup) {
    PIDController pidTurn(kP, kI, kD, startI, windup);
    pidTurn.setExitCondition(settleError, settleTime);
    pidTurn.setSpeedLimits(0, maxVelocity);
    pidTurn.setTarget(0);
    pidTurn.setDynamicSettle(true);

    char buf[100];
    pros::lcd::set_text(1, "pid_turn");

    while (!pidTurn.isSettled()) {
        isPidRunning = true;
        aps.update_cords();
        auto pos = aps.get_coordinates();

        double error = targetAngle - pos.Theta;
        while (error >  180) error -= 360;
        while (error < -180) error += 360;

        snprintf(buf, sizeof(buf), "actual:   %.2f deg", pos.Theta);
        pros::lcd::set_text(2, buf);
        snprintf(buf, sizeof(buf), "setpoint: %.2f deg", targetAngle);
        pros::lcd::set_text(3, buf);
        snprintf(buf, sizeof(buf), "error:    %.2f deg", error);
        pros::lcd::set_text(4, buf);

        double output = pidTurn.update(error);
        p_move(-output, output);
        pros::delay(10);
    }
    brake_instant();
    isPidRunning = false;
}

void NeoChassis::pid_look(double targetX, double targetY, double kP, double kI, double kD,
                          double startI, double settleError, double settleTime, double windup) {
    PIDController pidGiro(kP, kI, kD, startI, windup);
    pidGiro.setExitCondition(settleError, settleTime);
    pidGiro.setSpeedLimits(0, maxVelocity);
    pidGiro.setTarget(0);
    pidGiro.setDynamicSettle(true);


    char buf[100];
    pros::lcd::set_text(1, "pid_look");

    while (!pidGiro.isSettled()) {
        isPidRunning = true;
        aps.update_cords();
        auto pos = aps.get_coordinates();

        double dx = targetX - pos.X;
        double dy = targetY - pos.Y;
        double anguloTarget = atan2(dx, -dy) * 180.0 / M_PI;

        double errorAngulo = anguloTarget - pos.Theta;
        while (errorAngulo >  180) errorAngulo -= 360;
        while (errorAngulo < -180) errorAngulo += 360;

        snprintf(buf, sizeof(buf), "actual:   %.2f deg", pos.Theta);
        pros::lcd::set_text(2, buf);
        snprintf(buf, sizeof(buf), "setpoint: %.2f deg", anguloTarget);
        pros::lcd::set_text(3, buf);
        snprintf(buf, sizeof(buf), "error:    %.2f deg", errorAngulo);
        pros::lcd::set_text(4, buf);

        double output = pidGiro.update(errorAngulo);
        p_move(-output, output);
        pros::delay(10);
    }
    brake_instant();
    isPidRunning = false;
}

void NeoChassis::pid_straight(double inches, double kP, double kI, double kD,
                               double startI, double settleError, double settleTime, double windup) {
    PIDController pidDist(kP, kI, kD, startI, windup);
    pidDist.setExitCondition(settleError, settleTime);
    pidDist.setSpeedLimits(0, maxVelocity);
    pidDist.setTarget(0);
    pidDist.setDynamicSettle(true);


    aps.update_cords();
    auto startPos = aps.get_coordinates();

    // Eje de avance fijo al inicio
    double headingRad = startPos.Theta * M_PI / 180.0;
    double forwardX =  sin(headingRad);
    double forwardY = -cos(headingRad);

    char buf[100];
    pros::lcd::set_text(1, "pid_straight");

    while (!pidDist.isSettled()) {
        isPidRunning = true;
        aps.update_cords();
        auto pos = aps.get_coordinates();

        double dx = pos.X - startPos.X;
        double dy = pos.Y - startPos.Y;

        // Con signo: positivo = avanzó, negativo = retrocedió
        double traveled  = dx * forwardX + dy * forwardY;
        double remaining = inches - traveled;

        snprintf(buf, sizeof(buf), "traveled: %.2f\"", traveled);
        pros::lcd::set_text(2, buf);
        snprintf(buf, sizeof(buf), "target:   %.2f\"", inches);
        pros::lcd::set_text(3, buf);
        snprintf(buf, sizeof(buf), "error:    %.2f\"", remaining);
        pros::lcd::set_text(4, buf);

        double output = pidDist.update(remaining);
        p_move(-output, -output); // mismo signo que pid_move
        pros::delay(10);
    }
    brake_instant();
    isPidRunning = false;
}

void NeoChassis::pid_move(double targetX, double targetY,
                          double turnkP, double turnkI, double turnkD, double turnStartI,
                          double turnSettleError, double turnSettleTime, double turnWindup,
                          double distkP, double distkI, double distkD, double distStartI,
                          double distSettleError, double distSettleTime, double distWindup,
                          bool moonwalk) {
    AbsolutePositionSystem::Coordinates inipos = aps.get_coordinates();
    char buf[100];

    // Calcular error angular inicial
    double dx0 = targetX - inipos.X;
    double dy0 = targetY - inipos.Y;
    double anguloTarget0 = atan2(dx0, -dy0) * 180.0 / M_PI;
    double errorInicial = anguloTarget0 - inipos.Theta;
    while (errorInicial >  180) errorInicial -= 360;
    while (errorInicial < -180) errorInicial += 360;

    // Si moonwalk está activo y el target está detrás (|error| > 90°),
    // no giramos: simplemente invertimos el drive en la fase 2
    bool doMoonwalk = moonwalk && (fabs(errorInicial) > 90.0);

    // ─── FASE 1: Mirar hacia el punto (solo si no hay moonwalk y es necesario)
    if (!doMoonwalk && fabs(errorInicial) > turnSettleError) {
        PIDController pidGiro(turnkP, turnkI, turnkD, turnStartI, turnWindup);
        pidGiro.setExitCondition(turnSettleError, turnSettleTime);
        pidGiro.setSpeedLimits(0, maxVelocity);
        pidGiro.setTarget(0);
        pidGiro.setDynamicSettle(true);

        pros::lcd::set_text(1, "pid_move: girando");

        while (!pidGiro.isSettled()) {
            isPidRunning = true;
            aps.update_cords();
            auto pos = aps.get_coordinates();

            double dx = targetX - pos.X;
            double dy = targetY - pos.Y;
            double anguloTarget = atan2(dx, -dy) * 180.0 / M_PI;

            double errorAngulo = anguloTarget - pos.Theta;
            while (errorAngulo >  180) errorAngulo -= 360;
            while (errorAngulo < -180) errorAngulo += 360;

            snprintf(buf, sizeof(buf), "actual:   %.2f deg", pos.Theta);
            pros::lcd::set_text(2, buf);
            snprintf(buf, sizeof(buf), "setpoint: %.2f deg", anguloTarget);
            pros::lcd::set_text(3, buf);
            snprintf(buf, sizeof(buf), "error:    %.2f deg", errorAngulo);
            pros::lcd::set_text(4, buf);

            double output = pidGiro.update(errorAngulo);
            p_move(-output, output);
            pros::delay(10);
        }
        brake_instant();
    }

    aps.reset_cords(inipos.X, inipos.Y, aps.get_coordinates().Theta);

    // ─── FASE 2: Avanzar (o retroceder si moonwalk) ─────────────────────────
    PIDController pidDist(distkP, distkI, distkD, distStartI, distWindup);
    pidDist.setExitCondition(distSettleError, distSettleTime);
    pidDist.setSpeedLimits(0, maxVelocity);
    pidDist.setTarget(0);

    pros::lcd::set_text(1, doMoonwalk ? "pid_move: moonwalk" : "pid_move: avanzando");

    while (!pidDist.isSettled()) {
        isPidRunning = true;
        aps.update_cords();
        auto pos = aps.get_coordinates();

        double dx = targetX - pos.X;
        double dy = targetY - pos.Y;
        double headingRad = pos.Theta * M_PI / 180.0;
        double forwardX =  sin(headingRad);
        double forwardY = -cos(headingRad);
        double distancia = dx * forwardX + dy * forwardY;

        snprintf(buf, sizeof(buf), "actual:   %.2f\"", distancia);
        pros::lcd::set_text(2, buf);
        snprintf(buf, sizeof(buf), "setpoint: 0.00\"");
        pros::lcd::set_text(3, buf);
        snprintf(buf, sizeof(buf), "error:    %.2f\"", distancia);
        pros::lcd::set_text(4, buf);

        double output = pidDist.update(distancia);

        // moonwalk: invertir output para ir en reversa
        if (doMoonwalk) output = -output;

        p_move(-output, -output);
        pros::delay(10);
    }
    brake_instant();
    isPidRunning = false;
}



// ─── Públicos ────────────────────────────────────────────────────────────────
void NeoChassis::move(int leftSpeed, int rightSpeed)    { p_move(leftSpeed, rightSpeed); }
void NeoChassis::setBrakeMode(BrakeMode mode)           { brakeMode = mode; }
void NeoChassis::setDeathZone(int deathZone)            { dZone = std::max(0, std::min(20, deathZone)); }

void NeoChassis::setMaxVelocity(int maxVel) {
    maxVelocity = std::max(0, std::min(127, maxVel));
}
void NeoChassis::setMinVelocity(int minVel) {
    minVelocity = std::max(-127, std::min(0, minVel));
}

void NeoChassis::brake() {
    switch (brakeMode) {
        case Coast:   brake_coast();   break;
        case Instant: brake_instant(); break;
        case Smooth:  advbrake();      break;
    }
}

void NeoChassis::pid_turn_to(double targetAngle, double kP, double kI, double kD,
                             double startI, double settleError, double settleTime, double windup) {
    pid_turn(targetAngle, kP, kI, kD, startI, settleError, settleTime, windup);
}

void NeoChassis::pid_look_at(double targetX, double targetY, double kP, double kI, double kD,
                             double startI, double settleError, double settleTime, double windup) {
    pid_look(targetX, targetY, kP, kI, kD, startI, settleError, settleTime, windup);
}

void NeoChassis::pid_move_to(double targetX, double targetY,
                             double turnkP, double turnkI, double turnkD, double turnStartI,
                             double turnSettleError, double turnSettleTime, double turnWindup,
                             double distkP, double distkI, double distkD, double distStartI,
                             double distSettleError, double distSettleTime, double distWindup,
                             bool moonwalk) {
    pid_move(targetX, targetY,
             turnkP, turnkI, turnkD, turnStartI, turnSettleError, turnSettleTime, turnWindup,
             distkP, distkI, distkD, distStartI, distSettleError, distSettleTime, distWindup,
             moonwalk);
}

void NeoChassis::pid_advance_straight(double inches, double kP, double kI, double kD,
                             double startI, double settleError, double settleTime, double windup) {
    pid_straight(inches, kP, kI, kD, startI, settleError, settleTime, windup);
}