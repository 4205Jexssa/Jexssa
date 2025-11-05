// struct PID {
//     double kP, kI, kD;
//     double integral = 0;
//     double prevError = 0;
//     double output = 0;
//     double integralLimit = 1000;
//     double outputLimit = 127;
//     bool signFlipReset = true;

//     PID(double _kP, double _kI, double _kD) : kP(_kP), kI(_kI), kD(_kD) {}

//     double update(double target, double current, double dt) {
//         double error = target - current;

//         // Integral
//         integral += error * dt;
//         if (fabs(error) > integralLimit) integral = 0;
//         if (signFlipReset && (error * prevError < 0)) integral = 0;

//         // Derivative sobre medición (para evitar derivative kick)
//         double derivative = -(current - (current - error)) / dt;

//         // PID output
//         output = kP * error + kI * integral + kD * derivative;

//         // Limitar salida
//         if (output > outputLimit) output = outputLimit;
//         if (output < -outputLimit) output = -outputLimit;

//         prevError = error;
//         return output;
//     }

//     void reset() {
//         integral = 0;
//         prevError = 0;
//         output = 0;
//     }
// };

// //main.cpp
// // pros::Motor left(1);
// // pros::Motor right(2, true);

// // PID drivePID(0.4, 0.002, 2.0);

// // void moveTo(double target) {
// //     left.tare_position();
// //     right.tare_position();
// //     uint32_t lastTime = pros::millis();

// //     while (true) {
// //         double current = (left.get_position() + right.get_position()) / 2.0;
// //         double now = pros::millis();
// //         double dt = (now - lastTime) / 1000.0;
// //         lastTime = now;

// //         double power = drivePID.update(target, current, dt);
// //         left.move(power);
// //         right.move(power);

// //         if (fabs(target - current) < 5) break; // simple settling condition
// //         pros::delay(10);
// //     }

// //     left.move(0);
// //     right.move(0);
// // }

