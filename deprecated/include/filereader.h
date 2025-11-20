////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
//        ESTA FUNCION ESTA COMPLETAMENTE OBSOLETA        //
////////////////////////////////////////////////////////////
//                    Datos del script:                   //
//                    Estado: obsoleto                    //
//       Ultima version: Jexssa v6.3B "pre-Regional"      //
//      Razon de obsolencia: Interfaz cambiada a LVGL     // 
////////////////////////////////////////////////////////////
//       | |                          //         __       //
//       | | _____  _____ ___  __ _   //        / /_      //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //       / __ \     //
//  | |__| |  __/>  <\__ \__ \ (_| |  //    _ / / / /     //
//   \____/ \___/_/\_\___/___/\__,_|  //   (_)_/ /_/      //
////////////////////////////////////////////////////////////
// Header de:      Filereader.cpp                         //
// Funcion:        Manejo de formato iPROS                //
// Declaraciones:  Formatos, estructuras y variables      //
////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>
#include <vector>
#include <string>

enum ResponseStatus : uint16_t {
    SUCCESS = 0,
    FILE_TOO_SHORT = 1,
    INVALID_MAGIC = 2,
    DIMENTIONS_READ_ERROR = 3,
};

struct Pixel {
    uint16_t x;
    uint16_t y;
    uint8_t  r;
    uint8_t  g;
    uint8_t  b;
};

struct Block {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
    uint8_t  r;
    uint8_t  g;
    uint8_t  b;
};

struct Image {
    uint16_t width;
    uint16_t height;
    uint16_t responseStatus;
    std::vector<Block> blocks;
    std::vector<Pixel> pixels;
};

// Prototipo de función
Image Load_iPROS(const std::string &filename);
