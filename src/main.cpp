/**
 * @file main.cpp
 * @brief Programa principal: lee la instancia, ejecuta el algoritmo
 *        evolutivo y guarda el mejor nivel encontrado.
 *
 * Uso: ./generador <archivo_instancia> [semilla] [opciones]
 * (ver README para la lista de opciones)
 */

#include "nivel.h"
#include "instancia.h"
#include "evaluacion.h"
#include "salto.h"
#include "ae.h"
#include "salida.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <sys/stat.h>

using namespace std;

static void uso(const char* prog) {
    cerr << "Uso: " << prog << " <archivo_instancia> [semilla] [opciones]\n"
         << "  -s N   semilla (tambien puede ir como segundo argumento)\n"
         << "  -p N   tamano de la poblacion           (50)\n"
         << "  -g N   maximo de generaciones           (1000)\n"
         << "  -n N   generaciones sin mejora para parar (300)\n"
         << "  -k N   tamano del torneo                (3)\n"
         << "  -c X   probabilidad de cruce            (0.8)\n"
         << "  -m X   probabilidad de mutacion         (0.5)\n"
         << "  -e N   individuos de elite              (2)\n"
         << "  -t N   largo base del tramo, columnas   (50)\n"
         << "  -x N   maximo de mutaciones por hijo    (3)\n"
         << "  -o DIR carpeta de salida                (salidas)\n"
         << "  -l ARC guarda la curva de convergencia en ARC (CSV)\n"
         << "  -q     no escribe el archivo de solucion (para experimentos)\n";
}

/** Crea la carpeta de salida y sus carpetas intermedias (como mkdir -p). */
static void crearCarpeta(const string& ruta) {
    for (size_t i = 1; i <= ruta.size(); i++) {
        if (i == ruta.size() || ruta[i] == '/') mkdir(ruta.substr(0, i).c_str(), 0755);  // si existe no pasa nada
    }
}

/** Convierte la semilla a entero; devuelve false si no es un número válido. */
static bool leerSemilla(const char* texto, unsigned& semilla) {
    char* fin = nullptr;
    unsigned long v = strtoul(texto, &fin, 10);
    if (fin == texto || *fin != '\0') {
        cerr << "Error: la semilla debe ser un entero no negativo (se recibio '" << texto << "')\n";
        return false;
    }
    semilla = (unsigned)v;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { uso(argv[0]); return 1; }

    // 1) Lectura de la instancia
    Instancia inst;
    if (!leerInstancia(argv[1], inst)) return 1;

    // 2) Parámetros
    Parametros par;
    par.semilla = random_device{}();
    string carpeta = "salidas";
    bool guardar = true;
    int i = 2;
    if (i < argc && argv[i][0] != '-') {
        if (!leerSemilla(argv[i++], par.semilla)) return 1;
    }
    for (; i < argc; i++) {
        string op = argv[i];
        bool tieneValor = (i + 1 < argc);
        if (op == "-q") { guardar = false; continue; }
        if (!tieneValor) { uso(argv[0]); return 1; }
        const char* v = argv[++i];
        if (op == "-s") { if (!leerSemilla(v, par.semilla)) return 1; }
        else if (op == "-p") par.poblacion = max(2, atoi(v));
        else if (op == "-g") par.generaciones = max(0, atoi(v));
        else if (op == "-n") par.sinMejora = max(1, atoi(v));
        else if (op == "-k") par.torneo = max(1, atoi(v));
        else if (op == "-c") par.probCruce = atof(v);
        else if (op == "-m") par.probMutacion = atof(v);
        else if (op == "-e") par.elite = max(0, atoi(v));
        else if (op == "-t") par.tramo = max(1, atoi(v));
        else if (op == "-x") par.maxMutaciones = max(1, atoi(v));
        else if (op == "-o") carpeta = v;
        else if (op == "-l") par.archivoConvergencia = v;
        else { uso(argv[0]); return 1; }
    }

    // 3) Algoritmo evolutivo
    Resultado res = ejecutarAE(inst, par);
    const Evaluacion& ev = res.evMejor;

    // 4) Resumen por consola
    printf("Instancia %s (P*=%d E*=%d O*=%d, linealidad %s), semilla %u\n",
           inst.nombre.c_str(), inst.plataformas, inst.enemigos, inst.obstaculos,
           inst.maximizarLinealidad ? "max" : "min", par.semilla);
    printf("  Mejor inicial: f=%d linealidad=%.3f fitness=%.2f\n",
           res.evMejorInicial.f, res.evMejorInicial.linealidad, res.evMejorInicial.fitness);
    printf("  Mejor final:   P=%d E=%d O=%d | f=%d | completable=%s | linealidad=%.3f | fitness=%.2f\n",
           ev.nPlataformas, ev.nEnemigos, ev.nObstaculos, ev.f,
           ev.completable ? "si" : "no", ev.linealidad, ev.fitness);
    printf("  Generaciones: %d (mejor en la %d) | evaluaciones: %ld | tiempo: %.2f s\n",
           res.generacionesEjecutadas, res.generacionMejor, res.evaluaciones, res.segundos);
    // Línea resumen para los scripts de experimentos
    printf("RESULTADO,%s,%u,%d,%d,%d,%d,%d,%.4f,%.4f,%d,%.4f,%d,%d,%ld,%.3f\n",
           inst.nombre.c_str(), par.semilla, ev.nPlataformas, ev.nEnemigos, ev.nObstaculos,
           ev.f, ev.completable ? 1 : 0, ev.linealidad, ev.fitness,
           res.evMejorInicial.f, res.evMejorInicial.linealidad,
           res.generacionesEjecutadas, res.generacionMejor, res.evaluaciones, res.segundos);

    // 5) Se guarda el mejor nivel en <carpeta>/<nombre>_semilla<S>_solucion.txt
    if (guardar) {
        crearCarpeta(carpeta);
        string ruta = carpeta + "/" + inst.nombre + "_semilla" + to_string(par.semilla) + "_solucion.txt";
        if (!escribirSolucion(ruta, res.mejor, inst, ev, par.semilla)) return 1;
        printf("  Solucion guardada en %s\n", ruta.c_str());
    }
    return 0;
}
