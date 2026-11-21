/**
 * @file operadores.h
 * @brief Operadores del algoritmo evolutivo: cruce por tramo, mutaciones y
 *        reparación.
 *
 * Un "tramo" es un bloque de columnas consecutivas. Su largo es un parámetro
 * del algoritmo (Parametros::tramo). Según la observación 1 del Estado de
 * Avance, el tramo debe ser lo bastante largo para que los movimientos que lo
 * usan (cruce, desplazar y regenerar tramo) cambien de forma significativa el
 * nivel y aporten exploración; su efecto se estudia en los experimentos.
 */

#ifndef OPERADORES_H
#define OPERADORES_H

#include "nivel.h"
#include "instancia.h"
#include <random>

using namespace std;

/** Tipos de mutación (se usan también para registrar estadísticas). */
enum Movimiento {
    MOV_PLATAFORMA = 0,   // agregar / quitar / mover una plataforma
    MOV_ENTIDAD,          // agregar / quitar / mover un enemigo u obstáculo
    MOV_ALTURA_COLUMNA,   // cambiar la altura de una columna en +-1 o +-2
    MOV_DESPLAZAR_TRAMO,  // subir o bajar un tramo completo
    MOV_REGENERAR_TRAMO,  // rehacer las alturas de un tramo
    N_MOVIMIENTOS
};

/**
 * @brief Cruce por tramo: el hijo 1 es p1 con un tramo de p2 y el hijo 2 es
 *        p2 con el mismo tramo de p1.
 * @param largoTramo Largo base del tramo; el largo real se sortea en
 *        [largoTramo/2, 3*largoTramo/2].
 */
void cruceTramo(const Nivel& p1, const Nivel& p2, Nivel& h1, Nivel& h2,
                int largoTramo, mt19937& rng);

/**
 * @brief Aplica un movimiento de mutación elegido al azar.
 * @return El movimiento aplicado.
 */
Movimiento mutar(Nivel& n, const Instancia& inst, int largoTramo, mt19937& rng);

/**
 * @brief Repara restricciones que se garantizan por construcción: inicio y
 *        meta con plataforma y sin entidades, zona libre al inicio, alturas
 *        dentro de 1..ALTO (1..ALTO-1 si hay algo encima) y
 *        entidades solo sobre plataformas.
 */
void reparar(Nivel& n);

#endif
