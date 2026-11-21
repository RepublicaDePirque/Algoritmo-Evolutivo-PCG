/**
 * @file nivel.h
 * @brief Constantes del problema y representación de un nivel.
 */

#ifndef NIVEL_H
#define NIVEL_H

// Constantes del enunciado
const int ANCHO = 150;      // columnas del nivel
const int ALTO = 16;        // filas (1 = la más baja, 16 = la más alta)
const int SALTO_ALTO = 4;   // altura máxima del salto (vértice de la parábola)
const int SALTO_LARGO = 6;  // alcance horizontal del salto a velocidad máxima,
                            // aterrizando a la misma altura del despegue

// Columnas iniciales sin enemigos ni obstáculos (0..ZONA_INICIO-1), para dejar
// un espacio libre al comenzar el nivel (indicación del foro del curso)
const int ZONA_INICIO = 5;

// Lo que puede haber sobre una plataforma.
// ENEMIGO: se puede derrotar cayéndole encima, por lo que la plataforma en que
//          está sigue siendo utilizable (observación del Estado de Avance).
// OBSTACULO: no se puede tocar; la plataforma que lo tiene no es pisable.
enum Entidad { NADA = 0, ENEMIGO = 1, OBSTACULO = 2 };

/**
 * @brief Un nivel es un arreglo de columnas.
 *
 * h[x]: altura de la plataforma en la columna x (1..ALTO). Un 0 significa
 *       que no hay plataforma (abismo). Se usan las 16 filas; como una
 *       entidad ocupa la celda de arriba, solo puede ir sobre plataformas de
 *       altura 1..ALTO-1 (lo mismo el inicio y la meta, marcados con S y G).
 * e[x]: entidad que está justo encima de la plataforma (solo importa si
 *       h[x] > 0).
 *
 * Se asume una plataforma por columna (confirmado por el ayudante), con el
 * inicio en la columna 0 y la meta en la columna ANCHO-1.
 */
struct Nivel {
    int h[ANCHO];
    int e[ANCHO];
};

#endif
