// Lo estoy haciendo mientras lloro por ella...
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
//    Script para la odometria    //                      //
//   y posicionamiento del robot  //        geo.cpp       //
//    en un plano bidimensional   //                      //
////////////////////////////////////////////////////////////

#include "main.h"
#include "geo.h"
#include "pros/adi.h"
#define ps(c) pros::screen::set_pen(c)
#define fr(x1, y1, x2, y2) pros::screen::fill_rect(x1, y1, x2, y2);
#define dl(x1, y1, x2, y2) pros::screen::draw_line(x1, y1, x2, y2);
#define fc(x, y, r) pros::screen::fill_circle(x, y, r);
using namespace pros;

double AbsolutePositionSystem::degreesToInches(double degrees, double wheelDiameter) {
    return (degrees / 100) * (M_PI * wheelDiameter) / 360.0;
}

double AbsolutePositionSystem::degToRad(double deg){
    return deg * M_PI / 180.0;
}

AbsolutePositionSystem::TravDelta AbsolutePositionSystem::getTravDelta(){
    double pX = degreesToInches(sX.get_position(), Diameter);
    double pY = degreesToInches(sY.get_position(), Diameter); 
    double pT = degToRad(sT.get_rotation());
    double rdPX = pX - prevX;
    double rdPY = pY - prevY;
    double dPT  = pT - prevT;
    double corrX = offX * (cos(dPT) - 1) - offY * sin(dPT);
    double corrY = offX * sin(dPT) + offY * (cos(dPT) - 1);
    double dPX = rdPX - corrX;
    double dPY = rdPY - corrY;
    prevX = pX;
    prevY = pY;
    prevT = pT;
    return TravDelta{ dPX, dPY };
}


AbsolutePositionSystem::AbsolutePositionSystem(pros::Rotation& SensorX, pros::Rotation& SensorY, pros::IMU& Inertial, double offsetX, double offsetY, double WheelDiameter)
    : sX(SensorX)
    , sY(SensorY)
    , sT(Inertial)
    , offX(offsetX)
    , offY(offsetY)
    , Diameter(WheelDiameter)
    , GlobalX(0.0)      
    , GlobalY(0.0)      
    , GlobalT(0.0)      
    , prevX(0.0)        
    , prevY(0.0)        
    , prevT(0.0)        {}

void AbsolutePositionSystem::reset_cords(double X, double Y, double T){
    GlobalX = X;
    GlobalY = Y;
    GlobalT = T;
    sT.set_rotation(T);
    prevX = degreesToInches(sX.get_position(), Diameter);
    prevY = degreesToInches(sY.get_position(), Diameter);
    prevT = degToRad(T);
}

void AbsolutePositionSystem::update_cords(){
    TravDelta delta = getTravDelta();
    double headingRad = degToRad(GlobalT);
    double cosH = cos(headingRad);
    double sinH = sin(headingRad);
    GlobalX += delta.deltaX * cosH - delta.deltaY * sinH;
    GlobalY += delta.deltaX * sinH + delta.deltaY * cosH;
    GlobalT = sT.get_rotation();
}

AbsolutePositionSystem::Coordinates AbsolutePositionSystem::get_coordinates(){
    return Coordinates{GlobalX, GlobalY, GlobalT};
}

void AbsolutePositionSystem::draw_graph(double PositionX, double PositionY, double Heading, double Zoom, ScreenDrawSpec config){
        int centerScreenX = config.ScreenWidth / 2;
        int centerScreenY = config.ScreenHeight / 2;
        double scl = config.BaseScale * Zoom;
        ps(Color::black);
        fr(0, 0, config.ScreenWidth, config.ScreenHeight);
        int fieldStartX = centerScreenX - (int)(PositionX * scl);
        int fieldStartY = centerScreenY - (int)(PositionY * scl);
        int fieldPixelSize = (int)(config.FieldSize * scl);
        ps(Color::white);
        dl(fieldStartX, fieldStartY, fieldStartX + fieldPixelSize, fieldStartY);
        dl(fieldStartX, fieldStartY + fieldPixelSize, fieldStartX + fieldPixelSize, fieldStartY + fieldPixelSize);
        dl(fieldStartX, fieldStartY, fieldStartX, fieldStartY + fieldPixelSize);
        dl(fieldStartX + fieldPixelSize, fieldStartY, fieldStartX + fieldPixelSize, fieldStartY + fieldPixelSize);
        double spacing = config.FieldSize / config.GridDivisions;
        ps(Color::gray);
        for (int i = 1; i < config.GridDivisions; i++) {
            int x = fieldStartX + (int)(i * spacing * scl);
            pros::screen::draw_line(x, fieldStartY, x, fieldStartY + fieldPixelSize);
        }
        for (int i = 1; i < config.GridDivisions; i++) {
            int y = fieldStartY + (int)(i * spacing * scl);
            pros::screen::draw_line(fieldStartX, y, fieldStartX + fieldPixelSize, y);
        }
        ps(Color::red);
        fc(centerScreenX, centerScreenY, 5);
        float angleRad = (90 - Heading) * M_PI / 180.0;
        int endX = centerScreenX + (int)(15 * cos(angleRad));
        int endY = centerScreenY - (int)(15 * sin(angleRad));
        ps(Color::yellow);
        dl(centerScreenX, centerScreenY, endX, endY);
        ps(Color::white);
        char buffer_text[100];
        snprintf(buffer_text, sizeof(buffer_text), "X: %.2f\"  Y: %.2f\"", PositionX, PositionY);
        pros::screen::print(pros::E_TEXT_SMALL, 10, 10, buffer_text);
        snprintf(buffer_text, sizeof(buffer_text), "Heading: %.1f°", Heading);
        pros::screen::print(pros::E_TEXT_SMALL, 10, 25, buffer_text);
        snprintf(buffer_text, sizeof(buffer_text), "Zoom: %.1fx", Zoom);
        pros::screen::print(pros::E_TEXT_SMALL, 10, 40, buffer_text);
}
