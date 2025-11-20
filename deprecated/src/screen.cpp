// Es solo un gimmick
////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
//        ESTA FUNCION ESTA COMPLETAMENTE OBSOLETA        //
////////////////////////////////////////////////////////////
//                    Datos del script:                   //
//                    Estado: obsoleto                    //
//       Ultima version: Jexssa v6.3B "pre-Regional"      //
//      Razon de obsolencia: Interfaz cambiada a LVGL     // 
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
//     Script para manejo de      //                      //
//  pantalla e imagenes formato   //      screen.cpp      //
//            .ipros              //                      //
////////////////////////////////////////////////////////////

// #include "main.h"
// #include "screen.h"
// #include "filereader.h"
// #include "pros/rtos.h"
// #include <cstdint>
// #include <sys/types.h>
// using namespace pros;
// using namespace std;

// bool drawing = true;
// int offset_x = 0;
// int offset_y = 0;
// Image viewport = {1, 1, 0, {Block{0,0,50,50,255,0,0}}, {}};


// void DrawImage(const Image &img, int offset_x, int offset_y) {
//     if (img.width == 0 || img.height == 0) return;
//     for (const Block &b : img.blocks) {
//         // pack color
//         uint32_t color = rgb_to_color(b.r, b.g, b.b);
//         pros::screen::set_pen(color);
//         pros::screen::fill_rect(
//             static_cast<std::int16_t>(int(b.x1) + offset_x),
//             static_cast<std::int16_t>(int(b.y1) + offset_y),
//             static_cast<std::int16_t>(int(b.x2) + offset_x),
//             static_cast<std::int16_t>(int(b.y2) + offset_y)
//         );
//     }
//     for (const Pixel &p : img.pixels) {
//         uint32_t color = rgb_to_color(p.r, p.g, p.b);
//         pros::screen::set_pen(color);
//         pros::screen::draw_pixel(
//             static_cast<std::int16_t>(int(p.x) + offset_x),
//             static_cast<std::int16_t>(int(p.y) + offset_y)
//         );
//     }
// }

// void draw_task(void* param) {
//     Image* img = static_cast<Image*>(param);
//     Controller master(E_CONTROLLER_MASTER);
//     master.print(0, 0, "Getted");
//     while (drawing){
//         if (img) {
//             DrawImage(*img, 0, 0);
//         }
//     }
//     master.print(0, 0, "Exit");
// }

// void stop_drawing(){
//     drawing = false;
//     screen::erase();
// }

// void start_drawing(){
//     drawing = true;
// }

// void show_logo(std::string logoPath){
//     Image logo = Load_iPROS(logoPath);
//     Controller master(E_CONTROLLER_MASTER);
//     master.print(0, 0, "Refreshing...");
//     viewport = logo;
//     Task logoDraw(
//         draw_task,
//         (void*)&viewport,
//         TASK_PRIORITY_MIN,
//         TASK_STACK_DEPTH_DEFAULT,
//         "Logo Show"
//     );
// }
