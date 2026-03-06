// Es el sexto intento de este sistema
////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
//   ESTA FUNCION ESTA EN ESTADO BETA, CONTIENE ERRORES   //
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
//    del calculo y condiciones   //        pid.cpp       //
//        de salida del PID       //                      //
////////////////////////////////////////////////////////////

#include "main.h"
#include "pid.h"
#include <cmath>

// Constructor
PIDController::PIDController(double kP, double kI, double kD, double startIntegral, double maxIntegral) {
    config.kP = kP;
    config.kI = kI;
    config.kD = kD;
    config.startIntegral = startIntegral;
    config.maxIntegral = maxIntegral;
    reset();
}

// Configurar parámetros PID
void PIDController::setPID(double kP, double kI, double kD) {
    config.kP = kP;
    config.kI = kI;
    config.kD = kD;
}

// Configurar setpoint
void PIDController::setTarget(double target) {
    config.setpoint = target;
}

// Configurar condiciones de salida
void PIDController::setExitCondition(double errorBounds, double exitTime) {
    config.errorBounds = errorBounds;
    config.exitTime = exitTime;
}

// Activar/desactivar settle dinámico
void PIDController::setDynamicSettle(bool enabled) {
    config.dynamicSettle = enabled;
}

// Configurar límites de integral
void PIDController::setIntegralLimits(double startIntegral, double maxIntegral) {
    config.startIntegral = startIntegral;
    config.maxIntegral = maxIntegral;
}

// Configurar límites de velocidad
void PIDController::setSpeedLimits(double minSpeed, double maxSpeed) {
    config.minSpeed = minSpeed;
    config.maxSpeed = maxSpeed;
}

// Resetear PID
void PIDController::reset() {
    previousResult.output = 0;
    previousResult.error = 0;
    previousResult.integralAccumulator = 0;
    for (int i = 0; i < 4; i++) previousResult.timeIn[i] = 0;
    previousResult.lastTime = pros::millis();
    previousResult.on = true;
}

// Calcular y obtener output
double PIDController::update(double currentValue) {
    previousResult = pidCalculate(previousResult, config, currentValue);
    return previousResult.output;
}

// Verificar si el PID ya llegó al target
bool PIDController::isSettled() {
    return !previousResult.on;
}

// Obtener error actual
double PIDController::getError() {
    return previousResult.error;
}

// Obtener integral acumulada
double PIDController::getIntegral() {
    return previousResult.integralAccumulator;
}

// Función privada: Verificar condición de salida (modo fijo)
int PIDController::pidExitCondition(double time, double error, double maxtime, double inerror) {
    if (fabs(error) < inerror) {
        if (time >= maxtime) {
            return 2;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

// Función privada: Calcular PID
PIDController::PIDResult PIDController::pidCalculate(PIDResult prev, PIDConfigs values, double actual) {
    PIDResult result;

    // Calcular delta de tiempo real
    uint32_t now = pros::millis();
    double dt = now - prev.lastTime;
    result.lastTime = now;

    // Inicializar valores
    result.on = prev.on;
    for (int i = 0; i < 4; i++) result.timeIn[i] = prev.timeIn[i];

    double error = values.setpoint - actual;

    // ── Condiciones de salida ──────────────────────────────
    if (values.dynamicSettle) {
        // Settle dinámico: 4 niveles con error y tiempo escalados
        // Nivel i+1: errorBounds*(i+1), exitTime*(i+1)
        bool anySettled = false;

        for (int i = 0; i < 4; i++) {
            double levelError = values.errorBounds * (i + 1);
            double levelTime  = values.exitTime    * (i + 1);

            if (fabs(error) < levelError) {
                result.timeIn[i] += dt;
                if (result.timeIn[i] >= levelTime) {
                    anySettled = true;
                }
            } else {
                result.timeIn[i] = 0; // salió de este rango, resetear solo este nivel
            }
        }

        if (anySettled) result.on = false;

    } else {
        // Settle fijo: comportamiento original
        int rsec = pidExitCondition(prev.timeIn[0], error, values.exitTime, values.errorBounds);
        if (rsec == 0) {
            result.timeIn[0] = 0;
        } else if (rsec == 1) {
            result.timeIn[0] += dt;
        } else {
            result.on = false;
        }
    }

    // ── Integral ───────────────────────────────────────────
    double integral = prev.integralAccumulator + error;
    if (fabs(error) <= values.startIntegral) {
        result.integralAccumulator = integral;
    } else {
        result.integralAccumulator = 0;
    }

    // Anti-windup
    if (values.maxIntegral > 0 && fabs(result.integralAccumulator) > values.maxIntegral) {
        result.integralAccumulator = copysign(values.maxIntegral, result.integralAccumulator);
    }

    // ── Derivada ───────────────────────────────────────────
    double derivative = error - prev.error;

    // ── Output PID ─────────────────────────────────────────
    result.output = (error * values.kP) + (result.integralAccumulator * values.kI) + (derivative * values.kD);

    // Limitar velocidad
    if (fabs(result.output) > values.maxSpeed) {
        result.output = copysign(values.maxSpeed, result.output);
    }
    if (fabs(result.output) < values.minSpeed && result.output != 0) {
        result.output = copysign(values.minSpeed, result.output);
    }

    result.error = error;

    return result;
}