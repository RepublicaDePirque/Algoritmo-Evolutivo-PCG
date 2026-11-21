/**
 * @file ae.h
 * @brief Algoritmo evolutivo generacional con elitismo.
 */

#ifndef AE_H
#define AE_H

#include "nivel.h"
#include "instancia.h"
#include "evaluacion.h"
#include <random>
#include <string>

using namespace std;

/** Parámetros del algoritmo (valores por defecto = configuración final). */
struct Parametros {
    unsigned semilla = 1;
    int poblacion = 50;          // tamaño de la población
    int generaciones = 1000;     // máximo de generaciones
    int sinMejora = 300;         // término si el mejor no mejora en estas generaciones
    int torneo = 3;              // individuos por torneo
    double probCruce = 0.8;      // probabilidad de cruzar una pareja
    double probMutacion = 0.5;   // probabilidad de mutar a cada hijo
    int maxMutaciones = 3;       // un hijo mutado recibe entre 1 y este número de movimientos
    int elite = 2;               // mejores que pasan directo a la siguiente generación
    int tramo = 50;              // largo base del tramo (columnas)
    string archivoConvergencia;  // si no está vacío, se guarda la curva de convergencia
};

/** Resultado de una ejecución. */
struct Resultado {
    Nivel mejor;
    Evaluacion evMejor;
    Evaluacion evMejorInicial;   // mejor de la población inicial (para comparar)
    int generacionesEjecutadas;
    int generacionMejor;         // generación en que se encontró el mejor
    long evaluaciones;
    double segundos;
};

/**
 * @brief Ejecuta el algoritmo evolutivo sobre una instancia.
 */
Resultado ejecutarAE(const Instancia& inst, const Parametros& par);

#endif
