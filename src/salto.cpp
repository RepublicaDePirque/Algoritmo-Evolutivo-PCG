/**
 * @file salto.cpp
 * @brief Implementación del modelo de salto con la parábola discretizada.
 *
 * Un salto de la columna a a la columna b (distancia d = |b - a|) es válido si:
 *   1) b tiene plataforma y no tiene obstáculo (un enemigo se derrota al caerle
 *      encima, observación 3 del Estado de Avance);
 *   2) la subida hasta el tope de b no supera ALCANCE[d];
 *   3) cada columna intermedia, a distancia k de a, queda completamente por
 *      encima del arco (SOBRE[k]) o con espacio para pasar por debajo (BAJO[k]).
 */

#include "salto.h"

namespace {

// Índice = distancia en columnas (la posición 0 no se usa).
//                                   d:  0  1  2  3  4  5  6   7   8    9
const int ALCANCE[MAX_DISTANCIA + 1] = { 0, 4, 4, 4, 3, 3, 1, -2, -5, -10 };
const int SOBRE[MAX_DISTANCIA + 1]   = { 0, 2, 3, 4, 3, 2, 0, -4, -8, -12 };
const int BAJO[MAX_DISTANCIA + 1]    = { 0, 4, 5, 5, 5, 4, 1, -2, -6, -11 };

/** Tope de la columna c relativo a los pies al despegar desde altura ha. */
int topeRelativo(const Nivel& n, int c, int ha) {
    return n.h[c] - ha + (n.e[c] != NADA ? 1 : 0);
}

/** Base de la plataforma de la columna c relativa a los pies al despegar. */
int baseRelativa(const Nivel& n, int c, int ha) {
    return n.h[c] - ha - 1;
}

}  // namespace

int subidaMaxima(int k) {
    if (k < 1 || k > MAX_DISTANCIA) return -1000;
    return ALCANCE[k];
}

int destinosDesde(const Nivel& n, int a, int destinos[]) {
    int cantidad = 0;
    int ha = n.h[a];
    for (int dir = -1; dir <= 1; dir += 2) {
        for (int d = 1; d <= MAX_DISTANCIA; d++) {
            int b = a + dir * d;
            if (b < 0 || b >= ANCHO) break;
            if (n.h[b] == 0) continue;  // abismo: el salto sigue su curso

            // ¿Se puede aterrizar en b? (2) subida alcanzable y (1) sin obstáculo
            if (n.e[b] != OBSTACULO && topeRelativo(n, b, ha) <= ALCANCE[d]) {
                destinos[cantidad++] = b;
            }

            // (3) Si la columna b bloquea el arco, los destinos más lejanos
            // en esta dirección tampoco se alcanzan
            bool pasaEncima = topeRelativo(n, b, ha) <= SOBRE[d];
            bool pasaDebajo = baseRelativa(n, b, ha) >= BAJO[d];
            if (!pasaEncima && !pasaDebajo) break;
        }
    }
    return cantidad;
}
