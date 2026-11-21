/**
 * @file salida.h
 * @brief Escritura de la solución en un archivo de texto.
 */

#ifndef SALIDA_H
#define SALIDA_H

#include "nivel.h"
#include "instancia.h"
#include "evaluacion.h"
#include <string>

using namespace std;

/**
 * @brief Escribe la solución en un archivo.
 *
 * Primero va un encabezado con los resultados (líneas que parten con '#') y
 * luego el nivel como una grilla de ALTO filas por ANCHO columnas.
 * Símbolos: '.' vacío, '#' plataforma, 'E' enemigo, 'O' obstáculo,
 *           'S' inicio (jugador), 'G' meta.
 *
 * @param ruta Archivo de salida.
 * @param n Nivel a guardar.
 * @param inst Instancia resuelta.
 * @param ev Evaluación del nivel.
 * @param semilla Semilla usada, para poder repetir el resultado.
 * @return false si no se pudo crear el archivo.
 */
bool escribirSolucion(const string& ruta, const Nivel& n, const Instancia& inst,
                      const Evaluacion& ev, unsigned semilla);

#endif
