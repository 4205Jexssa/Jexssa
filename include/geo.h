////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      geo.cpp                                //
// Funcion:        Manejo de control APS                  //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////

#ifndef GEO_H
#define GEO_H

#include "pros/rotation.hpp"
#include "pros/imu.hpp"

class AbsolutePositionSystem {
private:
    struct ScreenDrawSpec{
        int ScreenWidth;
        int ScreenHeight;
        double FieldSize;
        double BaseScale;
        int GridDivisions;
    };

    struct TravDelta{
        double deltaX;
        double deltaY;
    };


    pros::Rotation& sX;
    pros::Rotation& sY;
    pros::IMU& sT;
    double GlobalX = 0;
    double GlobalY = 0;
    double Diameter = 0;
    double GlobalT = 0;
    double prevX = 0;
    double prevY = 0;
    double prevT = 0;
    double offX = 0;
    double offY = 0;
    
    double degreesToInches(double degrees, double wheelDiameter);
    double degToRad(double deg);
    TravDelta getTravDelta();

public:
    struct Coordinates {
        double X;
        double Y;
        double Theta;
    };

    AbsolutePositionSystem(pros::Rotation& SensorX, pros::Rotation& SensorY, pros::IMU& Inertial, double offsetX, double offsetY, double WheelDiameter);
    
    void reset_cords(double X, double Y, double T);
    void update_cords();
    Coordinates get_coordinates();
    void draw_graph(double PositionX, double PositionY, double Heading, double Zoom, ScreenDrawSpec config = ScreenDrawSpec{480, 240, 144.0, 3.0, 6});
};

#endif // GEO_H
