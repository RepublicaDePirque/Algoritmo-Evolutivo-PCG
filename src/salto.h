/**
 * @file salto.h
 * @brief Modelo del salto del personaje: parábola discretizada por columnas.
 *
 * Corresponde a la observación 2 del Estado de Avance: la tabla de alturas del
 * salto se extiende para considerar las distintas distancias y diferencias de
 * altura (saltos cortos más verticales y caídas más largas que 6 columnas).
 *
 * Parábola del enunciado (altura 4, alcance 6), con u = columnas avanzadas e
 * Y = altura de los pies relativa al despegue:
 *         Y(u) = 4 u (6 - u) / 9
 *   u:      1     2     3     4     5     6     7     8     9
 *   Y(u): 2.2   3.6   4.0   3.6   2.2   0.0  -3.1  -7.1 -12.0
 * Más allá de u = 6 la parábola sigue bajando: al caer a una plataforma más
 * baja se llega más lejos.
 *
 * Con ella se construyen tres arreglos (índice = distancia en columnas):
 *   ALCANCE[d]: subida máxima (h destino - h origen) para aterrizar a d
 *               columnas. Hasta d = 3 el personaje puede avanzar menos en el
 *               aire y alcanzar la cima de 4 antes de llegar; desde d = 4 es
 *               la altura de la parábola al entrar a la columna (u = d - 1/2).
 *               Un valor negativo significa que el destino debe estar más abajo.
 *   SOBRE[k]:   altura de la parábola en la columna intermedia k (redondeada
 *               hacia abajo). El salto pasa por encima si el tope de la
 *               columna (plataforma o entidad) no la supera.
 *   BAJO[k]:    altura mínima de la base de una plataforma para que el
 *               personaje (1 celda de alto) pase por debajo de ella.
 *
 * Coordenadas: la plataforma de altura h ocupa [h, h+1] y el personaje parado
 * sobre ella tiene los pies en h+1. El borde superior del nivel no es sólido
 * (como en Super Mario Bros., se puede salir por arriba al saltar).
 */

#ifndef SALTO_H
#define SALTO_H

#include "nivel.h"

/** Distancia horizontal máxima de un salto o caída (columnas). */
const int MAX_DISTANCIA = 9;

/**
 * @brief Calcula las columnas a las que se puede llegar con un salto desde la
 *        plataforma de la columna a (hacia la izquierda o la derecha).
 * @param n Nivel.
 * @param a Columna de despegue (debe tener plataforma).
 * @param destinos Arreglo donde se escriben las columnas alcanzadas.
 * @return Cantidad de destinos escritos.
 */
int destinosDesde(const Nivel& n, int a, int destinos[]);

/**
 * @brief Subida máxima con que se puede aterrizar a distancia k (ALCANCE[k]).
 *        Se usa al generar niveles para no proponer subidas imposibles.
 */
int subidaMaxima(int k);

#endif
