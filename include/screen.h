#ifndef SCREENHANDLER_H
#define SCREENHANDLER_H

#include "main.h"
#include "filereader.h"
#include <cstdint>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////
// Estructuras globales y variables de control
////////////////////////////////////////////////////////////

extern bool drawing;          // Control del bucle de dibujo
extern int offset_x;          // Desplazamiento horizontal
extern int offset_y;          // Desplazamiento vertical
extern Image viewport;        // Imagen actualmente visible (buffer o logo)

////////////////////////////////////////////////////////////
// Funciones
////////////////////////////////////////////////////////////

/**
 * Convierte un color RGB a formato uint32_t compatible con PROS.
 */
inline uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           (static_cast<uint32_t>(b));
}

/**
 * Dibuja una imagen en la pantalla del Brain.
 * @param img Imagen cargada desde .iPROS
 * @param offset_x desplazamiento en X
 * @param offset_y desplazamiento en Y
 */
void DrawImage(const Image &img, int offset_x = 0, int offset_y = 0);

/**
 * Tarea de dibujo que mantiene la imagen en pantalla.
 * @param param puntero a una estructura Image
 */
void draw_task(void* param);

/**
 * Detiene el bucle de dibujo.
 */
void stop_drawing();

/**
 * Inicia el bucle de dibujo.
 */
void start_drawing();

/**
 * Carga un logo desde la SD y lo muestra en pantalla.
 * @param logoPath ruta al archivo .iPROS
 */
void show_logo(std::string logoPath);

#endif // SCREENHANDLER_H
