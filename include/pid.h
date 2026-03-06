////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      pid.cpp                                //
// Funcion:        Manejo de controladores PID            //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////

#ifndef PID_H
#define PID_H

#include <cstdint>

class PIDController {
    private:
        struct PIDResult {
            double output = 0;
            double error = 0;
            double integralAccumulator = 0;
            double timeIn[4] = {0, 0, 0, 0};
            uint32_t lastTime = 0;
            bool on = true;
        };

        struct PIDConfigs {
            double setpoint = 0;
            double kP = 0;
            double kI = 0;
            double kD = 0;
            double errorBounds = 1;
            double exitTime = 350;
            double startIntegral = 3;
            double maxIntegral = -1;
            double start = 0;
            bool forwards = true;
            double maxSpeed = 127;
            double minSpeed = 0;
            bool dynamicSettle = false;
        };

        PIDResult previousResult;
        PIDConfigs config;

        /*
        0: Don't Stop
        1: Add time
        2: Stop
        */
        int pidExitCondition(double time, double error, double maxtime, double inerror);
        PIDResult pidCalculate(PIDResult prev, PIDConfigs values, double actual);

    public:
        // Constructor
        PIDController(double kP, double kI, double kD, double startIntegral = 3, double maxIntegral = -1);

        // Configurar parámetros PID
        void setPID(double kP, double kI, double kD);

        // Configurar setpoint
        void setTarget(double target);

        // Configurar condiciones de salida
        void setExitCondition(double errorBounds, double exitTime);

        // Activar/desactivar settle dinámico
        void setDynamicSettle(bool enabled);

        // Configurar límites de integral
        void setIntegralLimits(double startIntegral, double maxIntegral);

        // Configurar límites de velocidad
        void setSpeedLimits(double minSpeed, double maxSpeed);

        // Resetear PID
        void reset();

        // Calcular y obtener output
        double update(double currentValue);

        // Verificar si el PID ya llegó al target
        bool isSettled();

        // Obtener error actual
        double getError();

        // Obtener integral acumulada
        double getIntegral();
};

#endif // PID_H