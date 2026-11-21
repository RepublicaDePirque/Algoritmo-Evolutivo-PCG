/**
 * @file ae.cpp
 * @brief Implementación del algoritmo evolutivo.
 *
 * Esquema por generación:
 *   1) Elitismo: los `elite` mejores pasan sin cambios.
 *   2) Hasta completar la población: se eligen dos padres por torneo, se
 *      cruzan por tramo con probabilidad probCruce (si no, se copian) y cada
 *      hijo se muta con probabilidad probMutacion.
 *   3) Se reparan y evalúan los hijos. La completitud se maneja con
 *      penalización en el fitness (evaluacion.cpp).
 * Término: `generaciones` generaciones o `sinMejora` generaciones seguidas
 * sin mejorar el mejor fitness.
 */

#include "ae.h"
#include "operadores.h"
#include "solucion_inicial.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>

using namespace std;

namespace {

struct Individuo {
    Nivel nivel;
    Evaluacion ev;
};

bool mejorQue(const Individuo& a, const Individuo& b) {
    return a.ev.fitness < b.ev.fitness;
}

/** Selección por torneo: devuelve el índice del mejor de k sorteados. */
int torneo(const vector<Individuo>& pob, int k, mt19937& rng) {
    uniform_int_distribution<int> d(0, (int)pob.size() - 1);
    int mejor = d(rng);
    for (int i = 1; i < k; i++) {
        int c = d(rng);
        if (pob[c].ev.fitness < pob[mejor].ev.fitness) mejor = c;
    }
    return mejor;
}

}  // namespace

Resultado ejecutarAE(const Instancia& inst, const Parametros& par) {
    auto t0 = chrono::steady_clock::now();
    mt19937 rng(par.semilla);
    uniform_real_distribution<double> azar(0.0, 1.0);
    uniform_int_distribution<int> nMut(1, max(1, par.maxMutaciones));

    Resultado res;
    res.evaluaciones = 0;

    // Población inicial aleatoria (solucion_inicial.cpp)
    vector<Individuo> pob(par.poblacion);
    for (auto& ind : pob) {
        ind.nivel = generarSolucionInicial(inst, rng);
        ind.ev = evaluar(ind.nivel, inst);
        res.evaluaciones++;
    }
    sort(pob.begin(), pob.end(), mejorQue);
    Individuo mejor = pob[0];
    res.evMejorInicial = pob[0].ev;
    res.generacionMejor = 0;

    FILE* conv = nullptr;
    if (!par.archivoConvergencia.empty()) {
        conv = fopen(par.archivoConvergencia.c_str(), "w");
        if (conv) fprintf(conv, "generacion,mejor_fitness,fitness_promedio,mejor_f,mejor_linealidad\n");
    }

    int gen = 0, sinMejorar = 0;
    while (gen < par.generaciones && sinMejorar < par.sinMejora) {
        if (conv) {
            double suma = 0;
            for (auto& ind : pob) suma += ind.ev.fitness;
            fprintf(conv, "%d,%.4f,%.4f,%d,%.4f\n", gen, mejor.ev.fitness,
                    suma / pob.size(), mejor.ev.f, mejor.ev.linealidad);
        }
        gen++;

        vector<Individuo> nueva;
        nueva.reserve(par.poblacion);
        // 1) Elitismo (la población está ordenada)
        for (int i = 0; i < par.elite && i < (int)pob.size(); i++) nueva.push_back(pob[i]);

        // 2) Selección, cruce y mutación
        while ((int)nueva.size() < par.poblacion) {
            const Individuo& p1 = pob[torneo(pob, par.torneo, rng)];
            const Individuo& p2 = pob[torneo(pob, par.torneo, rng)];
            Individuo h1, h2;
            if (azar(rng) < par.probCruce) {
                cruceTramo(p1.nivel, p2.nivel, h1.nivel, h2.nivel, par.tramo, rng);
            } else {
                h1.nivel = p1.nivel;
                h2.nivel = p2.nivel;
            }
            for (Individuo* h : {&h1, &h2}) {
                if (azar(rng) < par.probMutacion) {
                    int veces = nMut(rng);
                    for (int i = 0; i < veces; i++) mutar(h->nivel, inst, par.tramo, rng);
                }
                reparar(h->nivel);
                h->ev = evaluar(h->nivel, inst);
                res.evaluaciones++;
                if ((int)nueva.size() < par.poblacion) nueva.push_back(*h);
            }
        }

        // 3) Reemplazo generacional
        pob.swap(nueva);
        sort(pob.begin(), pob.end(), mejorQue);
        if (pob[0].ev.fitness < mejor.ev.fitness - 1e-9) {
            mejor = pob[0];
            res.generacionMejor = gen;
            sinMejorar = 0;
        } else {
            sinMejorar++;
        }
    }
    if (conv) fclose(conv);

    res.mejor = mejor.nivel;
    res.evMejor = mejor.ev;
    res.generacionesEjecutadas = gen;
    res.segundos = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
    return res;
}
