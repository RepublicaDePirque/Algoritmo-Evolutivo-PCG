/**
 * @file salida.cpp
 * @brief Escritura de la solución.
 */

#include "salida.h"
#include <fstream>
#include <iostream>

using namespace std;

/** Carácter que corresponde a la celda (columna x, altura fila). */
static char simbolo(const Nivel& n, int x, int fila) {
    if (n.h[x] == 0) return '.';
    if (fila == n.h[x]) return '#';
    if (fila == n.h[x] + 1) {
        if (x == 0) return 'S';
        if (x == ANCHO - 1) return 'G';
        if (n.e[x] == ENEMIGO) return 'E';
        if (n.e[x] == OBSTACULO) return 'O';
    }
    return '.';
}

bool escribirSolucion(const string& ruta, const Nivel& n, const Instancia& inst,
                      const Evaluacion& ev, unsigned semilla) {
    ofstream out(ruta);
    if (!out) {
        cerr << "Error: no se pudo crear " << ruta << "\n";
        return false;
    }

    out << "# instancia: " << inst.nombre << "\n";
    out << "# solicitado: plataformas=" << inst.plataformas
        << " enemigos=" << inst.enemigos
        << " obstaculos=" << inst.obstaculos
        << " linealidad=" << (inst.maximizarLinealidad ? "max" : "min") << "\n";
    out << "# obtenido: plataformas=" << ev.nPlataformas
        << " enemigos=" << ev.nEnemigos
        << " obstaculos=" << ev.nObstaculos << "\n";
    out << "# f=" << ev.f << " (dP=" << ev.deltaP << " dE=" << ev.deltaE
        << " dO=" << ev.deltaO << ")\n";
    out << "# completable=" << (ev.completable ? "si" : "no")
        << " linealidad=" << ev.linealidad
        << " fitness=" << ev.fitness << "\n";
    out << "# semilla=" << semilla << "\n";

    // Grilla: se imprime desde la fila más alta (ALTO) hasta la más baja (1)
    for (int fila = ALTO; fila >= 1; fila--) {
        for (int x = 0; x < ANCHO; x++) out << simbolo(n, x, fila);
        out << "\n";
    }
    return true;
}
