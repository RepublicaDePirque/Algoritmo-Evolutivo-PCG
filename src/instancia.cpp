/**
 * @file instancia.cpp
 * @brief Lectura y validación de la instancia.
 */

#include "instancia.h"
#include "nivel.h"
#include <fstream>
#include <iostream>

using namespace std;

/** Obtiene "instancia1" a partir de "instancias/instancia1.txt". */
static string nombreBase(const string& ruta) {
    size_t inicio = ruta.find_last_of("/\\");
    inicio = (inicio == string::npos) ? 0 : inicio + 1;
    size_t fin = ruta.find_last_of('.');
    if (fin == string::npos || fin < inicio) fin = ruta.size();
    return ruta.substr(inicio, fin - inicio);
}

bool leerInstancia(const string& ruta, Instancia& inst) {
    ifstream archivo(ruta);
    if (!archivo) {
        cerr << "Error: no se pudo abrir el archivo " << ruta << "\n";
        return false;
    }

    string linealidad;
    if (!(archivo >> inst.plataformas >> inst.enemigos >> inst.obstaculos >> linealidad)) {
        cerr << "Error: formato esperado <plataformas> <enemigos> <obstaculos> <max|min>\n";
        return false;
    }

    // Validaciones
    if (inst.plataformas < 2 || inst.plataformas > ANCHO) {
        cerr << "Error: las plataformas deben estar entre 2 y " << ANCHO << "\n";
        return false;
    }
    if (inst.enemigos < 0 || inst.obstaculos < 0) {
        cerr << "Error: enemigos y obstaculos no pueden ser negativos\n";
        return false;
    }
    if (linealidad == "max") {
        inst.maximizarLinealidad = true;
    } else if (linealidad == "min") {
        inst.maximizarLinealidad = false;
    } else {
        cerr << "Error: la linealidad debe ser 'max' o 'min'\n";
        return false;
    }

    inst.nombre = nombreBase(ruta);
    return true;
}
