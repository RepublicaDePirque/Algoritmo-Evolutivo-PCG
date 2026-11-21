/**
 * @file evaluacion.h
 * @brief Función de evaluación: cantidades, completitud (BFS) y linealidad.
 */

#ifndef EVALUACION_H
#define EVALUACION_H

#include "nivel.h"
#include "instancia.h"

// Pesos del fitness. La completitud pesa más que las cantidades, y estas más
// que la linealidad, que queda como criterio secundario.
const double PESO_CANTIDADES = 20.0;
const double PESO_COMPLETITUD = 100.0;
const double PESO_LINEALIDAD = 1.0;

// Variedad (observación del foro: evitar niveles con patrones evidentes).
// - Si se minimiza la linealidad, se premia además la fracción de plataformas
//   consecutivas que cambian de altura; así no basta con juntar las
//   plataformas en las filas extremas (la métrica de regresión lo premia, pero
//   el nivel resulta casi plano en dos franjas); y la fracción de las filas
//   distintas usadas, para que las alturas no se concentren en pocas franjas.
//   Los tres términos de forma suman a lo más 7,5 + 6 + 3 < PESO_CANTIDADES.
// - Si se maximiza, una desviación media hasta TOLERANCIA_LINEAL se considera
//   igual de lineal: el nivel puede tener pequeñas variaciones de altura en
//   vez de ser una recta perfecta.
// Penalización por cada plataforma inútil (inaccesible, o trampa desde la que ya
// no se llega a la meta; observación del foro sobre plataformas inaccesibles).
const double PESO_SIN_SALIDA = 5.0;
const double PESO_CAMBIOS = 6.0;
const double PESO_FILAS = 3.0;   // premio por la fracción de las 16 filas utilizadas (solo al minimizar)
const double TOLERANCIA_LINEAL = 0.8;

/** Resultado de evaluar un nivel. */
struct Evaluacion {
    int nPlataformas, nEnemigos, nObstaculos;  // cantidades reales
    int deltaP, deltaE, deltaO;                // diferencia con lo solicitado
    int f;                                     // deltaP + deltaE + deltaO
    bool completable;                          // ¿se llega del inicio a la meta?
    int columnaMasLejana;                      // columna más lejana alcanzada
    double linealidad;                         // desviación media a la recta (menor = más lineal)
    int sinSalida;                             // plataformas inútiles (inaccesibles o sin salida)
    double filas;                              // fracción de las ALTO filas usadas por alguna plataforma
    double cambios;                            // fracción de pares de plataformas consecutivas con distinta altura
    double fitness;                            // valor que se minimiza
};

/**
 * @brief Indica si se puede aterrizar en la columna (tiene plataforma y no
 *        tiene obstáculo; un enemigo se derrota al caerle encima).
 */
bool pisable(const Nivel& n, int x);

/**
 * @brief BFS desde la columna 0.
 * @param n Nivel a revisar.
 * @param llegaMeta Queda en true si se alcanza la columna ANCHO-1.
 * @return La columna más lejana que se puede alcanzar.
 */
int alcanceMaximo(const Nivel& n, bool& llegaMeta);

/**
 * @brief Atajo para saber si el nivel se puede completar.
 */
bool esCompletable(const Nivel& n);

/**
 * @brief Desviación media de las alturas respecto de la recta de regresión.
 */
double calcularLinealidad(const Nivel& n);

/**
 * @brief Cantidad de plataformas inútiles: pisables pero inaccesibles desde
 *        el inicio, o alcanzables pero sin camino hacia la meta (trampas).
 *        Las plataformas con obstáculo no se cuentan (no son pisables).
 */
int contarSinSalida(const Nivel& n);

/**
 * @brief Fracción de pares de plataformas consecutivas (en orden de columna)
 *        cuya altura es distinta.
 */
double fraccionCambios(const Nivel& n);

/** @brief Fracción de las ALTO filas en que hay al menos una plataforma. */
double fraccionFilas(const Nivel& n);

/**
 * @brief Evalúa el nivel completo según la instancia.
 */
Evaluacion evaluar(const Nivel& n, const Instancia& inst);

#endif
