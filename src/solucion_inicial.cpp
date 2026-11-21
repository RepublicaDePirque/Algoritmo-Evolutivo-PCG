/**
 * @file solucion_inicial.cpp
 * @brief Solución inicial aleatoria: caminata aleatoria de alturas y
 *        entidades puestas al azar.
 */

#include "solucion_inicial.h"
#include "evaluacion.h"
#include "salto.h"
#include <algorithm>
#include <vector>

using namespace std;

/** Entero aleatorio en [a, b]. */
static int enteroAzar(int a, int b, mt19937& rng) {
    uniform_int_distribution<int> d(a, b);
    return d(rng);
}

/** Real aleatorio en [0, 1). */
static double realAzar(mt19937& rng) {
    uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng);
}

/** Paso 1: plataformas con una caminata aleatoria de izquierda a derecha. */
static void generarPlataformas(Nivel& n, const Instancia& inst, mt19937& rng) {
    // Probabilidad de poner plataforma en una columna para acercarse a P*
    // (se restan inicio y meta, que siempre tienen plataforma)
    double probPlataforma = (double)(inst.plataformas - 2) / (ANCHO - 2);

    // Con linealidad alta la altura cambia poco y pocas veces; con linealidad
    // baja cambia más y más seguido
    int cambioMaximo = inst.maximizarLinealidad ? 1 : SALTO_ALTO;
    double probCambio = inst.maximizarLinealidad ? 0.15 : 0.5;

    // Plataforma de inicio en la columna 0
    n.h[0] = enteroAzar(3, ALTO - 6, rng);
    int ultimaX = 0;
    int ultimaH = n.h[0];

    for (int x = 1; x < ANCHO; x++) {
        bool esMeta = (x == ANCHO - 1);
        // Si el abismo ya es tan largo como el salto, se obliga una plataforma
        bool obligada = (x - ultimaX == SALTO_LARGO);

        if (esMeta || obligada || realAzar(rng) < probPlataforma) {
            int nuevaH = ultimaH;
            if (realAzar(rng) < probCambio) {
                nuevaH += enteroAzar(-cambioMaximo, cambioMaximo, rng);
            }
            // Se mantiene la altura dentro del nivel, dejando libre la fila de
            // arriba para la entidad que pueda ir sobre la plataforma
            // No se sube más de lo que permite la parábola a esa distancia
            nuevaH = min(nuevaH, ultimaH + subidaMaxima(x - ultimaX));
            nuevaH = max(1, min(ALTO, nuevaH));
            if (esMeta) nuevaH = min(nuevaH, ALTO - 1);  // la meta lleva el marcador G encima

            n.h[x] = nuevaH;
            ultimaX = x;
            ultimaH = nuevaH;
        }
    }
}

/**
 * Paso 2: ubica enemigos y obstáculos al azar sobre las plataformas.
 * Cada entidad se prueba en una columna; si el nivel deja de ser completable
 * se deshace y se prueba en otra.
 */
static void colocarEntidades(Nivel& n, const Instancia& inst, mt19937& rng) {
    // Lista mezclada de las entidades que hay que ubicar
    vector<int> porColocar;
    for (int i = 0; i < inst.enemigos; i++) porColocar.push_back(ENEMIGO);
    for (int i = 0; i < inst.obstaculos; i++) porColocar.push_back(OBSTACULO);
    shuffle(porColocar.begin(), porColocar.end(), rng);

    // Columnas candidatas: con plataforma bajo la fila superior, fuera de la
    // zona libre del inicio y sin contar la meta
    vector<int> candidatas;
    for (int x = ZONA_INICIO; x < ANCHO - 1; x++) {
        if (n.h[x] > 0 && n.h[x] < ALTO) candidatas.push_back(x);
    }
    shuffle(candidatas.begin(), candidatas.end(), rng);

    size_t siguiente = 0;  // índice de la próxima entidad a ubicar
    for (int x : candidatas) {
        if (siguiente == porColocar.size()) break;  // ya se ubicaron todas
        n.e[x] = porColocar[siguiente];
        if (esCompletable(n)) {
            siguiente++;       // se acepta
        } else {
            n.e[x] = NADA;     // se deshace: rompía la completitud
        }
    }
}

Nivel generarSolucionInicial(const Instancia& inst, mt19937& rng) {
    Nivel n;
    for (int x = 0; x < ANCHO; x++) {
        n.h[x] = 0;
        n.e[x] = NADA;
    }
    generarPlataformas(n, inst, rng);
    colocarEntidades(n, inst, rng);
    return n;
}
