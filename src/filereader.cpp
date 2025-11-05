// No era mas facil hacerlo desde codigo?
////////////////////////////////////////////////////////////
//                    ⚠ DISCLAIMER ⚠                     //
// ESTA FUNCION ESTA EN ESTADO BETA, TODAVIA NO FUNCIONA  //
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

// File handler para leer archivos .ipros

#include "filereader.h"
#include "main.h"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include "pros/screen.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdint>


pros::Controller master(pros::E_CONTROLLER_MASTER);

void prn(const std::string &msg) {
    master.print(0, 0, msg.c_str());
}

void dump(std::string error) {
    FILE *f = std::fopen("/usd/dump.txt", "w");
    if (!f) return;
    std::fputs(error.c_str(), f);
    std::fclose(f);
}

// ------------------------------------------------------------
// Saltar marcadores opcionales 0xAA, 0x55, 0xBE
// Devuelve true si encuentra EOF
// ------------------------------------------------------------
bool read_next_flag(FILE *f, uint8_t &flag) {
    while (true) {
        int b = std::fgetc(f);
        if (b == EOF) return true;
        if (b == 0xAA || b == 0x55 || b == 0xBE) continue;
        flag = static_cast<uint8_t>(b);
        return false;
    }
}

// ------------------------------------------------------------
// Carga un archivo .ipros en estructura Image
// ------------------------------------------------------------
Image Load_iPROS(const std::string &filename) {
    prn("Loading...");
    FILE *f = std::fopen(filename.c_str(), "rb");
    if (!f) {
        prn("ERROR 1");
        dump("Error abriendo archivo: " + std::to_string(errno));
        std::exit(EXIT_FAILURE);
    }

    // --- Encabezado "imgPROS" ---
    char magic[8] = {0};
    std::fread(magic, 1, 7, f);
    if (std::string(magic) != "imgPROS") {
        prn("BAD HDR");
        dump("Cabecera inválida");
        std::exit(EXIT_FAILURE);
    }

    // --- Dimensiones ---
    uint16_t width = 0, height = 0;
    std::fread(&width, sizeof(width), 1, f);
    std::fread(&height, sizeof(height), 1, f);

    if (width == 0 || height == 0 || width > 4096 || height > 4096) {
        prn("DIM ERR");
        dump("Dimensiones inválidas");
        std::exit(EXIT_FAILURE);
    }

    Image img;
    img.width  = width;
    img.height = height;
    img.responseStatus = SUCCESS;

    const uint16_t blocks_x = (width  + 15) / 16;
    const uint16_t blocks_y = (height + 15) / 16;

    // --- Leer bloques raster ---
    long proc = 0;
    for (uint16_t by = 0; by < blocks_y; ++by) {
        for (uint16_t bx = 0; bx < blocks_x; ++bx) {
            proc++;
            prn("LD " + std::to_string(proc));
            uint8_t flag = 0;
            if (read_next_flag(f, flag)) {
                prn("EOF FLAG");
                dump("EOF inesperado leyendo flag");
                std::exit(EXIT_FAILURE);
            }

            // Posible terminador mal ubicado
            if (flag == 'E') {
                prn("TRUNCADO");
                dump("Se encontró 'E' en medio del archivo");
                std::exit(EXIT_FAILURE);
            }

            // --- Bloque uniforme ---
            if (flag == 0) {
                Block blk{};
                if (std::fread(&blk.x1, sizeof(blk.x1), 1, f) != 1 ||
                    std::fread(&blk.y1, sizeof(blk.y1), 1, f) != 1 ||
                    std::fread(&blk.x2, sizeof(blk.x2), 1, f) != 1 ||
                    std::fread(&blk.y2, sizeof(blk.y2), 1, f) != 1 ||
                    std::fread(&blk.r,  sizeof(blk.r),  1, f) != 1 ||
                    std::fread(&blk.g,  sizeof(blk.g),  1, f) != 1 ||
                    std::fread(&blk.b,  sizeof(blk.b),  1, f) != 1) {
                    prn("ERR4");
                    dump("Bloque uniforme corrupto");
                    std::exit(EXIT_FAILURE);
                }
                img.blocks.push_back(blk);

                // Pintar píxeles
                for (uint16_t yy = blk.y1; yy <= blk.y2; ++yy) {
                    for (uint16_t xx = blk.x1; xx <= blk.x2; ++xx) {
                        img.pixels.push_back({xx, yy, blk.r, blk.g, blk.b});
                    }
                }
            }

            // --- Bloque de píxeles individuales ---
            else if (flag == 1) {
                const uint16_t bx16 = bx * 16;
                const uint16_t by16 = by * 16;
                const uint16_t bw = std::min<uint16_t>(16, width  - bx16);
                const uint16_t bh = std::min<uint16_t>(16, height - by16);
                const uint32_t expected = bw * bh;

                for (uint32_t i = 0; i < expected; ++i) {
                    Pixel p{};
                    if (std::fread(&p.x, sizeof(p.x), 1, f) != 1 ||
                        std::fread(&p.y, sizeof(p.y), 1, f) != 1 ||
                        std::fread(&p.r, sizeof(p.r), 1, f) != 1 ||
                        std::fread(&p.g, sizeof(p.g), 1, f) != 1 ||
                        std::fread(&p.b, sizeof(p.b), 1, f) != 1) {
                        prn("EOF PX");
                        dump("EOF inesperado en bloque de píxeles");
                        std::exit(EXIT_FAILURE);
                    }
                    img.pixels.push_back(p);
                }
            }

            // --- Flag desconocido ---
            else {
                long pos = std::ftell(f);
                prn("BAD FLAG");
                dump("Flag desconocido en byte " + std::to_string(pos));
                std::exit(EXIT_FAILURE);
            }
        }
    }

    // --- Leer terminador "End imgPROS" (11 bytes) ---
    char tail[11] = {};
    std::fread(tail, 1, 11, f);
    if (std::string(tail) != "End imgPROS") {
        prn("TAIL ERR");
        prn(tail);
        dump(std::string("Cabecera final corrupta: ") + tail);
    }

    // --- Leer versión + checksum (si existen) ---
    std::fclose(f);
    prn("SUCESS");
    return img;
}