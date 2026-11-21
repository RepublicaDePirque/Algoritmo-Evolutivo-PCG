"""Mide la variedad de los niveles generados (observación del foro sobre patrones).

Uso (desde la carpeta del proyecto): python3 experimentos/variedad.py
Ejecuta ./generador con 10 semillas por instancia (carpeta temporal) y calcula:
  filas   : filas distintas usadas por plataformas (promedio)
  extremas: % de plataformas en las dos filas más usadas de su nivel (promedio)
  cambios : % de pares consecutivos de plataformas con altura distinta (promedio)
  sim_pos : % de columnas que coinciden en tener plataforma entre dos semillas
  sim_alt : % de columnas que coinciden en altura entre dos semillas
Imprime una fila por instancia (cifras de la sección de variedad del informe). No requiere librerías externas.
"""
import itertools
import os
import subprocess
import tempfile
from collections import Counter

SEMILLAS = range(1, 11)
INST = range(1, 7)


def leer(ruta):
    filas = [l.rstrip("\n") for l in open(ruta) if not l.startswith("# ")]
    h = [0] * 150
    for r, l in enumerate(filas):
        for x, c in enumerate(l):
            if c == "#":
                h[x] = 16 - r
    return h


def main():
    lineas = []
    with tempfile.TemporaryDirectory() as tmp:
        for i in INST:
            niveles = []
            for s in SEMILLAS:
                subprocess.run(["./generador", f"instancias/instancia{i}.txt", str(s), "-o", tmp],
                               stdout=subprocess.DEVNULL, check=True)
                niveles.append(leer(f"{tmp}/instancia{i}_semilla{s}_solucion.txt"))
            filas, ext, cam = [], [], []
            for h in niveles:
                alt = [v for v in h if v]
                c = Counter(alt)
                filas.append(len(c))
                ext.append(100 * sum(n for _, n in c.most_common(2)) / len(alt))
                cam.append(100 * sum(a != b for a, b in zip(alt, alt[1:])) / (len(alt) - 1))
            pos, alt_ = [], []
            for a, b in itertools.combinations(niveles, 2):
                pos.append(100 * sum((u > 0) == (v > 0) for u, v in zip(a, b)) / 150)
                alt_.append(100 * sum(u == v for u, v in zip(a, b)) / 150)
            m = lambda v: sum(v) / len(v)
            lineas.append((i, m(filas), m(ext), m(cam), m(pos), m(alt_)))
    print("inst filas extremas% cambios% sim_pos% sim_alt%")
    for l in lineas:
        print(l[0], *[round(v, 1) for v in l[1:]])


main()
