"""Grafica un nivel generado a partir de su archivo de solución.

Uso: python3 graficar.py salidas/instancia1_solucion.txt [imagen.png]
Requiere matplotlib.
"""
import glob
import os
import sys

import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

# Símbolo -> índice de color
SIMBOLOS = {".": 0, "#": 1, "E": 2, "O": 3, "S": 4, "G": 5}
COLORES = ["#bfe1fb", "#3d3d3d", "#d62828", "#f4a02a", "#1d4ed8", "#16a34a"]


def leer(ruta):
    encabezado, grilla = [], []
    with open(ruta, encoding="utf-8") as archivo:
        for linea in archivo:
            linea = linea.rstrip("\n")
            # El encabezado parte con "# " (numeral y espacio). Una fila de la
            # grilla también puede partir con '#' (plataforma en la columna 0),
            # pero nunca lleva espacios.
            if linea.startswith("# "):
                encabezado.append(linea[2:].strip())
            elif linea:
                grilla.append([SIMBOLOS.get(c, 0) for c in linea])
    return encabezado, grilla


def graficar(ruta, salida):
    encabezado, grilla = leer(ruta)
    fig, ax = plt.subplots(figsize=(15, 2.4))
    ax.imshow(grilla, cmap=ListedColormap(COLORES), vmin=0, vmax=len(COLORES) - 1,
              interpolation="nearest")
    ax.set_xticks([])
    ax.set_yticks([])
    ax.set_title(" | ".join(encabezado[:4]), fontsize=8, loc="left")
    fig.tight_layout()
    fig.savefig(salida, dpi=150)
    plt.close(fig)
    print(f"Imagen guardada en {salida}")


def main():
    if len(sys.argv) < 2:
        print("Uso: python3 graficar.py <archivo_solucion | carpeta> [imagen.png]")
        sys.exit(1)
    ruta = sys.argv[1]
    if os.path.isdir(ruta):
        # Carpeta: grafica todos los *_solucion.txt que contenga
        archivos = sorted(glob.glob(os.path.join(ruta, "*_solucion.txt")))
        if not archivos:
            print(f"No hay archivos *_solucion.txt en {ruta}")
            sys.exit(1)
        for archivo in archivos:
            graficar(archivo, archivo.replace(".txt", ".png"))
        return
    salida = sys.argv[2] if len(sys.argv) > 2 else ruta.replace(".txt", ".png")
    graficar(ruta, salida)


if __name__ == "__main__":
    main()