/**
 * @file solucion_inicial.h
 * @brief Generación aleatoria de niveles para la población inicial.
 */

#ifndef SOLUCION_INICIAL_H
#define SOLUCION_INICIAL_H

#include "nivel.h"
#include "instancia.h"
#include <random>

using namespace std;

/**
 * @brief Genera un nivel aleatorio que respeta los saltos válidos.
 *
 * Primero crea las plataformas con una caminata aleatoria y después ubica
 * enemigos y obstáculos sin romper la completitud.
 *
 * @param inst Instancia con las cantidades solicitadas.
 * @param rng Generador de números aleatorios.
 * @return El nivel generado.
 */
Nivel generarSolucionInicial(const Instancia& inst, mt19937& rng);

#endif
