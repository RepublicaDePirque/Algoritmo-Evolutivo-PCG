/**
 * @file instancia.h
 * @brief Lectura de una instancia desde un archivo de texto.
 */

#ifndef INSTANCIA_H
#define INSTANCIA_H

#include <string>

using namespace std;

/** Datos que definen una instancia del problema. */
struct Instancia {
    string nombre;              // nombre del archivo, sin carpeta ni extensión
    int plataformas;            // plataformas solicitadas (P*)
    int enemigos;               // enemigos solicitados (E*)
    int obstaculos;             // obstáculos solicitados (O*)
    bool maximizarLinealidad;   // true = "max", false = "min"
};

/**
 * @brief Lee el archivo de instancia.
 *
 * El formato es una línea con: <plataformas> <enemigos> <obstaculos> <max|min>
 *
 * @param ruta Ruta del archivo.
 * @param inst Instancia que se llena con los datos leídos.
 * @return false si el archivo no existe o el formato es inválido.
 */
bool leerInstancia(const string& ruta, Instancia& inst);

#endif
