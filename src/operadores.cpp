/**
 * @file operadores.cpp
 * @brief Implementación del cruce, las mutaciones y la reparación.
 */

#include "operadores.h"
#include "salto.h"
#include <algorithm>
#include <vector>

using namespace std;

namespace {

int enteroAzar(int a, int b, mt19937& rng) {
    uniform_int_distribution<int> d(a, b);
    return d(rng);
}

double realAzar(mt19937& rng) {
    uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng);
}

int limitarAltura(int h) { return max(1, min(ALTO, h)); }

/** Sortea un tramo [desde, hasta) de largo en [L/2, 3L/2] dentro del nivel. */
void sortearTramo(int largoTramo, mt19937& rng, int& desde, int& hasta) {
    int minimo = max(1, largoTramo / 2);
    int maximo = max(minimo, (3 * largoTramo) / 2);
    int largo = min(ANCHO, enteroAzar(minimo, maximo, rng));
    desde = enteroAzar(0, ANCHO - largo, rng);
    hasta = desde + largo;
}

/** Altura de la plataforma más cercana a la izquierda de x (o 0 si no hay). */
int alturaAnterior(const Nivel& n, int x, int& columna) {
    for (int c = x - 1; c >= 0; c--) {
        if (n.h[c] > 0) { columna = c; return n.h[c]; }
    }
    columna = -1;
    return 0;
}

/** Columnas intermedias (1..ANCHO-2) que cumplen una condición. */
template <class Cond>
vector<int> columnasQue(const Nivel& n, Cond cond) {
    vector<int> v;
    for (int x = 1; x < ANCHO - 1; x++) if (cond(n, x)) v.push_back(x);
    return v;
}

int elegir(const vector<int>& v, mt19937& rng) {
    return v[enteroAzar(0, (int)v.size() - 1, rng)];
}

/** Agrega una plataforma en una columna vacía, cerca de la altura vecina. */
bool agregarPlataforma(Nivel& n, mt19937& rng) {
    vector<int> vacias = columnasQue(n, [](const Nivel& m, int x) { return m.h[x] == 0; });
    if (vacias.empty()) return false;
    int x = elegir(vacias, rng), c;
    int base = alturaAnterior(n, x, c);
    if (base == 0) base = ALTO / 2;
    n.h[x] = limitarAltura(base + enteroAzar(-1, 1, rng));
    n.e[x] = NADA;
    return true;
}

/** Quita una plataforma; prefiere una sin entidad para no alterar E y O. */
bool quitarPlataforma(Nivel& n, mt19937& rng) {
    vector<int> libres = columnasQue(n, [](const Nivel& m, int x) { return m.h[x] > 0 && m.e[x] == NADA; });
    if (libres.empty()) libres = columnasQue(n, [](const Nivel& m, int x) { return m.h[x] > 0; });
    if (libres.empty()) return false;
    int x = elegir(libres, rng);
    n.h[x] = 0;
    n.e[x] = NADA;
    return true;
}

void moverPlataforma(Nivel& n, const Instancia& inst, mt19937& rng) {
    int p = 0;
    for (int x = 0; x < ANCHO; x++) if (n.h[x] > 0) p++;
    if (p < inst.plataformas) agregarPlataforma(n, rng);
    else if (p > inst.plataformas) quitarPlataforma(n, rng);
    else if (quitarPlataforma(n, rng)) agregarPlataforma(n, rng);  // mover
}

/** Ajusta la cantidad de un tipo de entidad hacia la solicitada. */
void ajustarEntidad(Nivel& n, int tipo, int solicitado, mt19937& rng) {
    vector<int> conTipo = columnasQue(n, [tipo](const Nivel& m, int x) { return m.h[x] > 0 && m.e[x] == tipo; });
    // Una entidad solo puede ir fuera de la zona libre del inicio y sobre una
    // plataforma bajo la fila superior (ocupa la celda de arriba)
    vector<int> libres = columnasQue(n, [](const Nivel& m, int x) {
        return m.h[x] > 0 && m.h[x] < ALTO && m.e[x] == NADA && x >= ZONA_INICIO;
    });
    int actual = (int)conTipo.size();

    if (actual < solicitado && !libres.empty()) {
        n.e[elegir(libres, rng)] = tipo;                 // agregar
    } else if (actual > solicitado) {
        n.e[elegir(conTipo, rng)] = NADA;                // quitar
    } else if (!conTipo.empty() && !libres.empty()) {
        n.e[elegir(conTipo, rng)] = NADA;                // mover
        n.e[elegir(libres, rng)] = tipo;
    }
}

void moverEntidad(Nivel& n, const Instancia& inst, mt19937& rng) {
    int e = 0, o = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] == 0) continue;
        if (n.e[x] == ENEMIGO) e++;
        if (n.e[x] == OBSTACULO) o++;
    }
    // Se corrige primero el tipo que está desajustado; si ambos están bien,
    // se mueve uno al azar
    bool malE = (e != inst.enemigos), malO = (o != inst.obstaculos);
    int tipo;
    if (malE && !malO) tipo = ENEMIGO;
    else if (malO && !malE) tipo = OBSTACULO;
    else tipo = (realAzar(rng) < 0.5) ? ENEMIGO : OBSTACULO;
    ajustarEntidad(n, tipo, tipo == ENEMIGO ? inst.enemigos : inst.obstaculos, rng);
}

void cambiarAlturaColumna(Nivel& n, mt19937& rng) {
    vector<int> conPlataforma;
    for (int x = 0; x < ANCHO; x++) if (n.h[x] > 0) conPlataforma.push_back(x);
    if (conPlataforma.empty()) return;
    int x = elegir(conPlataforma, rng);
    int d = enteroAzar(1, 2, rng) * (realAzar(rng) < 0.5 ? -1 : 1);
    n.h[x] = limitarAltura(n.h[x] + d);
}

void desplazarTramo(Nivel& n, int largoTramo, mt19937& rng) {
    int desde, hasta;
    sortearTramo(largoTramo, rng, desde, hasta);
    int d = enteroAzar(1, 3, rng) * (realAzar(rng) < 0.5 ? -1 : 1);
    for (int x = desde; x < hasta; x++) {
        if (n.h[x] > 0) n.h[x] = limitarAltura(n.h[x] + d);
    }
}

/**
 * Rehace las alturas de las plataformas de un tramo con una caminata
 * aleatoria que parte de la plataforma anterior al tramo. Si se maximiza la
 * linealidad, la caminata casi no cambia de altura (tiende a aplanar); si se
 * minimiza, cambia seguido y con saltos de hasta 4 celdas.
 */
void regenerarTramo(Nivel& n, const Instancia& inst, int largoTramo, mt19937& rng) {
    int desde, hasta;
    sortearTramo(largoTramo, rng, desde, hasta);
    int cambioMaximo = inst.maximizarLinealidad ? 1 : SALTO_ALTO;
    double probCambio = inst.maximizarLinealidad ? 0.0 : 0.6;

    int ultimaX;
    int ultimaH = alturaAnterior(n, desde, ultimaX);
    for (int x = desde; x < hasta; x++) {
        if (n.h[x] == 0) continue;
        if (ultimaX < 0) { ultimaX = x; ultimaH = n.h[x]; continue; }
        int nuevaH = ultimaH;
        if (realAzar(rng) < probCambio) nuevaH += enteroAzar(-cambioMaximo, cambioMaximo, rng);
        nuevaH = min(nuevaH, ultimaH + subidaMaxima(x - ultimaX));  // subida alcanzable
        n.h[x] = limitarAltura(nuevaH);
        ultimaX = x;
        ultimaH = n.h[x];
    }
}

}  // namespace

void cruceTramo(const Nivel& p1, const Nivel& p2, Nivel& h1, Nivel& h2,
                int largoTramo, mt19937& rng) {
    int desde, hasta;
    sortearTramo(largoTramo, rng, desde, hasta);
    h1 = p1;
    h2 = p2;
    for (int x = desde; x < hasta; x++) {
        h1.h[x] = p2.h[x]; h1.e[x] = p2.e[x];
        h2.h[x] = p1.h[x]; h2.e[x] = p1.e[x];
    }
}

Movimiento mutar(Nivel& n, const Instancia& inst, int largoTramo, mt19937& rng) {
    // Probabilidades de cada movimiento (suman 1)
    static const double PROB[N_MOVIMIENTOS] = {0.25, 0.25, 0.15, 0.15, 0.20};
    double r = realAzar(rng), acumulada = 0;
    int m = 0;
    for (; m < N_MOVIMIENTOS - 1; m++) {
        acumulada += PROB[m];
        if (r < acumulada) break;
    }
    switch (m) {
        case MOV_PLATAFORMA:      moverPlataforma(n, inst, rng); break;
        case MOV_ENTIDAD:         moverEntidad(n, inst, rng); break;
        case MOV_ALTURA_COLUMNA:  cambiarAlturaColumna(n, rng); break;
        case MOV_DESPLAZAR_TRAMO: desplazarTramo(n, largoTramo, rng); break;
        default:                  regenerarTramo(n, inst, largoTramo, rng); break;
    }
    reparar(n);
    return (Movimiento)m;
}

void reparar(Nivel& n) {
    // Inicio y meta siempre con plataforma y sin entidades
    if (n.h[0] == 0) n.h[0] = ALTO / 2;
    if (n.h[ANCHO - 1] == 0) {
        int c;
        int h = alturaAnterior(n, ANCHO - 1, c);
        n.h[ANCHO - 1] = (h > 0) ? h : ALTO / 2;
    }
    n.e[0] = n.e[ANCHO - 1] = NADA;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] == 0) { n.e[x] = NADA; continue; }  // entidad sin plataforma
        n.h[x] = limitarAltura(n.h[x]);                 // altura dentro del nivel
        if (x < ZONA_INICIO) n.e[x] = NADA;             // zona libre del inicio
        // Si hay algo encima (entidad, S o G), la plataforma no puede estar en
        // la fila superior
        bool algoEncima = (n.e[x] != NADA || x == 0 || x == ANCHO - 1);
        if (algoEncima && n.h[x] > ALTO - 1) n.h[x] = ALTO - 1;
    }
}
