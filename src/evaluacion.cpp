/**
 * @file evaluacion.cpp
 * @brief Implementación de la función de evaluación.
 */

#include "evaluacion.h"
#include "salto.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <vector>

using namespace std;

bool pisable(const Nivel& n, int x) {
    // Un enemigo no impide usar la plataforma: se derrota cayéndole encima
    return n.h[x] > 0 && n.e[x] != OBSTACULO;
}

int alcanceMaximo(const Nivel& n, bool& llegaMeta) {
    llegaMeta = false;
    if (!pisable(n, 0)) return 0;  // sin plataforma de inicio no se avanza

    bool visitado[ANCHO] = {false};
    int cola[ANCHO];   // cada columna entra una sola vez
    int destinos[ANCHO];
    int inicio = 0, fin = 0;
    visitado[0] = true;
    cola[fin++] = 0;
    int masLejana = 0;

    while (inicio < fin) {
        int a = cola[inicio++];
        if (a > masLejana) masLejana = a;

        // Columnas a las que se llega con algún salto o caída desde a (salto.cpp)
        int k = destinosDesde(n, a, destinos);
        for (int i = 0; i < k; i++) {
            int b = destinos[i];
            if (!visitado[b]) {
                visitado[b] = true;
                cola[fin++] = b;
            }
        }
    }

    llegaMeta = visitado[ANCHO - 1];
    return masLejana;
}

bool esCompletable(const Nivel& n) {
    bool llega;
    alcanceMaximo(n, llega);
    return llega;
}

/**
 * Linealidad adaptada de Launchpad (Smith et al., 2011).
 * Se ajusta una recta por mínimos cuadrados a los puntos (x, h[x]) de las
 * columnas con plataforma, y se calcula la distancia vertical media de esos
 * puntos a la recta. 0 es un nivel perfectamente recto; mientras más alto el
 * valor, más cambios de altura hay.
 */
double calcularLinealidad(const Nivel& n) {
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    int k = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] > 0) {
            sx += x;
            sy += n.h[x];
            sxx += (double)x * x;
            sxy += (double)x * n.h[x];
            k++;
        }
    }
    if (k < 2) return 0.0;

    double pendiente = (k * sxy - sx * sy) / (k * sxx - sx * sx);
    double intercepto = (sy - pendiente * sx) / k;

    double suma = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] > 0) suma += fabs(n.h[x] - (intercepto + pendiente * x));
    }
    return suma / k;
}

int contarSinSalida(const Nivel& n) {
    vector<int> adj[ANCHO], radj[ANCHO];
    int destinos[ANCHO];
    for (int a = 0; a < ANCHO; a++) {
        if (!pisable(n, a)) continue;
        int k = destinosDesde(n, a, destinos);
        for (int i = 0; i < k; i++) { adj[a].push_back(destinos[i]); radj[destinos[i]].push_back(a); }
    }
    // Alcanzables desde el inicio (hacia adelante) y desde la meta (hacia atrás)
    bool ida[ANCHO] = {false}, vuelta[ANCHO] = {false};
    vector<int> cola;
    ida[0] = true; cola.push_back(0);
    for (size_t i = 0; i < cola.size(); i++)
        for (int b : adj[cola[i]]) if (!ida[b]) { ida[b] = true; cola.push_back(b); }
    cola.clear();
    vuelta[ANCHO - 1] = true; cola.push_back(ANCHO - 1);
    for (size_t i = 0; i < cola.size(); i++)
        for (int b : radj[cola[i]]) if (!vuelta[b]) { vuelta[b] = true; cola.push_back(b); }
    int cuenta = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (ida[x] && !vuelta[x]) cuenta++;          // trampa sin salida
        else if (!ida[x] && pisable(n, x)) cuenta++; // plataforma inaccesible
    }
    return cuenta;
}

double fraccionCambios(const Nivel& n) {
    int pares = 0, distintos = 0, previa = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] == 0) continue;
        if (previa > 0) {
            pares++;
            if (n.h[x] != previa) distintos++;
        }
        previa = n.h[x];
    }
    return pares > 0 ? (double)distintos / pares : 0.0;
}

double fraccionFilas(const Nivel& n) {
    bool usada[ALTO + 1] = {false};
    int k = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] > 0 && !usada[n.h[x]]) { usada[n.h[x]] = true; k++; }
    }
    return (double)k / ALTO;
}

Evaluacion evaluar(const Nivel& n, const Instancia& inst) {
    Evaluacion ev;

    // 1) Contar elementos (una plataforma por columna con h > 0)
    ev.nPlataformas = ev.nEnemigos = ev.nObstaculos = 0;
    for (int x = 0; x < ANCHO; x++) {
        if (n.h[x] > 0) {
            ev.nPlataformas++;
            if (n.e[x] == ENEMIGO) ev.nEnemigos++;
            if (n.e[x] == OBSTACULO) ev.nObstaculos++;
        }
    }
    ev.deltaP = abs(ev.nPlataformas - inst.plataformas);
    ev.deltaE = abs(ev.nEnemigos - inst.enemigos);
    ev.deltaO = abs(ev.nObstaculos - inst.obstaculos);
    ev.f = ev.deltaP + ev.deltaE + ev.deltaO;

    // 2) Completitud con BFS
    ev.columnaMasLejana = alcanceMaximo(n, ev.completable);
    int columnasFaltantes = (ANCHO - 1) - ev.columnaMasLejana;

    // 3) Linealidad: si se maximiza conviene una desviación baja, y si se
    //    minimiza una alta (por eso se cambia el signo)
    //    Se agrega la variedad (ver evaluacion.h): al minimizar se premian los
    //    cambios de altura; al maximizar se tolera una desviación pequeña.
    ev.linealidad = calcularLinealidad(n);
    ev.cambios = fraccionCambios(n);
    ev.filas = fraccionFilas(n);
    double terminoLinealidad;
    if (inst.maximizarLinealidad) terminoLinealidad = max(ev.linealidad, TOLERANCIA_LINEAL);
    else terminoLinealidad = -ev.linealidad - (PESO_CAMBIOS * ev.cambios + PESO_FILAS * ev.filas) / PESO_LINEALIDAD;

    ev.sinSalida = ev.completable ? contarSinSalida(n) : 0;

    // 4) Fitness (se minimiza)
    ev.fitness = PESO_CANTIDADES * ev.f
               + PESO_COMPLETITUD * columnasFaltantes
               + PESO_SIN_SALIDA * ev.sinSalida
               + PESO_LINEALIDAD * terminoLinealidad;
    return ev;
}
