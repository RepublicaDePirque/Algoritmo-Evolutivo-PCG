"""Resume los experimentos (experimentos/*.csv) en tablas LaTeX y gráficos.

Uso (desde la carpeta del proyecto): python3 experimentos/analizar.py
Requiere pandas y matplotlib. Escribe las tablas en informe/tablas/, la curva de
convergencia en experimentos/convergencia.pdf e
imprime por consola las cifras que se citan en el texto del informe.
"""
import math
import os
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

E = "experimentos"
TAB = "informe/tablas"
FIG = "informe/figuras"
os.makedirs(TAB, exist_ok=True)
os.makedirs(FIG, exist_ok=True)

INST = {  # P*, E*, O*, linealidad
    "instancia1": (75, 20, 15, "max"), "instancia2": (50, 20, 15, "max"),
    "instancia3": (75, 20, 15, "min"), "instancia4": (100, 20, 15, "min"),
    "instancia5": (50, 20, 15, "min"), "instancia6": (75, 15, 35, "min"),
}


def num(x, d=2):
    return f"{x:.{d}f}".replace(".", ",")


def pm(m, s, d=2):
    return f"{num(m, d)} $\\pm$ {num(s, d)}"


def mann_whitney(a, b):
    """Prueba U de Mann-Whitney bilateral (aproximación normal con corrección
    por empates). Devuelve (U, p). Se implementa aquí para no depender de scipy."""
    a, b = list(a), list(b)
    n1, n2 = len(a), len(b)
    datos = sorted([(v, 0) for v in a] + [(v, 1) for v in b])
    rangos = [0.0] * len(datos)
    i, empates = 0, 0.0
    while i < len(datos):
        j = i
        while j + 1 < len(datos) and datos[j + 1][0] == datos[i][0]:
            j += 1
        r = (i + j) / 2 + 1
        for k in range(i, j + 1):
            rangos[k] = r
        t = j - i + 1
        empates += t ** 3 - t
        i = j + 1
    r1 = sum(r for r, (_, g) in zip(rangos, datos) if g == 0)
    u1 = r1 - n1 * (n1 + 1) / 2
    n = n1 + n2
    var = n1 * n2 / 12 * ((n + 1) - empates / (n * (n - 1)))
    if var <= 0:
        return u1, 1.0
    z = (u1 - n1 * n2 / 2) / math.sqrt(var)
    p = math.erfc(abs(z) / math.sqrt(2))
    return u1, p


# --- Tabla 1: resultados de la configuración final + comparación inicial y aleatoria
fin = pd.read_csv(f"{E}/final.csv")
ale = pd.read_csv(f"{E}/aleatoria.csv")
filas = []
print("== Configuracion final vs busqueda aleatoria ==")
for ins, (p, e, o, lin) in INST.items():
    a = fin[fin.instancia == ins]
    r = ale[ale.instancia == ins]
    filas.append(
        f"{ins[-1]} & {p}/{e}/{o} & {lin} & "
        f"{pm(a.f_inicial.mean(), a.f_inicial.std())} & {pm(a.lin_inicial.mean(), a.lin_inicial.std())} & "
        f"{pm(r.f.mean(), r.f.std())} & {pm(r.linealidad.mean(), r.linealidad.std())} & {pm(r.fitness.mean(), r.fitness.std())} & "
        f"{pm(a.f.mean(), a.f.std())} & {pm(a.linealidad.mean(), a.linealidad.std())} & {pm(a.fitness.mean(), a.fitness.std())} & "
        f"{100 * a.completable.mean():.0f} & {num(a.segundos.mean(), 2)} \\\\"
    )
    u, pval = mann_whitney(a.fitness, r.fitness)
    print(f"{ins}: AE t={a.segundos.mean():.2f}s (max {a.segundos.max():.2f}) | "
          f"aleatoria t={r.segundos.mean():.1f}s | razon={r.segundos.mean() / a.segundos.mean():.0f}x | "
          f"lin AE={a.linealidad.mean():.3f} alea={r.linealidad.mean():.3f} | fitness AE={a.fitness.mean():.2f} alea={r.fitness.mean():.2f} | Mann-Whitney p={pval:.2e} | "
          f"f>0 AE={int((a.f > 0).sum())} | eval/s={(a.evaluaciones / a.segundos).mean():.0f}")
with open(f"{TAB}/resultados.tex", "w") as fh:
    fh.write("\n".join(filas) + "\n")
mins = [k for k, v in INST.items() if v[3] == "min"]
print(f"Linealidad media instancias min: AE={fin[fin.instancia.isin(mins)].linealidad.mean():.2f} "
      f"aleatoria={ale[ale.instancia.isin(mins)].linealidad.mean():.2f}")
print(f"Tiempo AE: {fin.segundos.mean():.2f}s medio | aleatoria: {ale.segundos.mean():.1f}s medio, "
      f"rango de medias {ale.groupby('instancia').segundos.mean().min():.0f}-"
      f"{ale.groupby('instancia').segundos.mean().max():.0f}s")
print(f"Total ejecuciones AE con f>0: {int((fin.f > 0).sum())}, no completables: {int((fin.completable == 0).sum())}")


# --- Tabla 2: sensibilidad al largo del tramo y a la probabilidad de mutación
def filas_sensibilidad(df, col, etiqueta):
    filas = []
    grupos = list(df.groupby(col))
    for idx, (valor, g) in enumerate(grupos):
        celdas = []
        for ins in INST:
            x = g[g.instancia == ins]
            celdas.append(pm(x.fitness.mean(), x.fitness.std()))
        celdas.append(num(g.gen_mejor.mean(), 0))
        celdas.append(str(int((g.f > 0).sum())))
        primera = f"\\multirow{{{len(grupos)}}}{{*}}{{{etiqueta}}}" if idx == 0 else ""
        filas.append(f"{primera} & {num(valor, 1) if isinstance(valor, float) else valor} & "
                     + " & ".join(celdas) + " \\\\")
    return filas


tr = pd.read_csv(f"{E}/tramo.csv")
mu = pd.read_csv(f"{E}/mutacion.csv")
filas = filas_sensibilidad(tr, "tramo", "$L$") + ["\\midrule"] + filas_sensibilidad(mu, "pm", "$p_m$")
with open(f"{TAB}/sensibilidad.tex", "w") as fh:
    fh.write("\n".join(filas) + "\n")
print("== Sensibilidad al tramo ==")
print(tr.groupby(["tramo", "instancia"]).linealidad.mean().unstack().round(3))
print(tr.groupby("tramo")[["f", "gen_mejor", "segundos"]].mean().round(2))
print("Ejecuciones con f>0 por tramo:", tr[tr.f > 0][["tramo", "instancia", "semilla", "f"]].values.tolist())
print("== Sensibilidad a p_m ==")
print(mu.groupby(["pm", "instancia"]).linealidad.mean().unstack().round(3))
print(mu.groupby("pm")[["f", "gen_mejor", "segundos"]].mean().round(2))
print("Ejecuciones con f>0 por p_m:", mu[mu.f > 0][["pm", "instancia", "semilla", "f"]].values.tolist())

# --- Figura: convergencia (linealidad del mejor por generación)
print("== Convergencia (semilla 1) ==")
fig, axes = plt.subplots(1, 2, figsize=(8, 1.2))
for ins, (p, e, o, lin) in INST.items():
    c = pd.read_csv(f"{E}/convergencia_{ins}.csv")
    ax = axes[0] if lin == "max" else axes[1]
    ax.plot(c.generacion, c.mejor_linealidad, label=f"Inst. {ins[-1]}", lw=1.2)
    if lin == "max":
        cero = c[c.mejor_linealidad <= 1e-9]
        print(f"{ins}: linealidad 0 desde la generacion {int(cero.generacion.min()) if len(cero) else -1}, "
              f"generaciones totales {int(c.generacion.max()) + 1}")
    else:
        g100 = c[c.generacion == min(100, int(c.generacion.max()))].mejor_linealidad.iloc[0]
        print(f"{ins}: linealidad gen 0={c.mejor_linealidad.iloc[0]:.2f}, gen 100={g100:.2f}, "
              f"final={c.mejor_linealidad.iloc[-1]:.2f}, generaciones {int(c.generacion.max()) + 1}")
for ax, t in zip(axes, ["Maximizar linealidad", "Minimizar linealidad"]):
    ax.set_title(t, fontsize=9)
    ax.set_xlabel("Generación", fontsize=8)
    ax.set_ylabel("Linealidad $\\Lambda$", fontsize=8)
    ax.tick_params(labelsize=7)
    ax.legend(fontsize=6.5, frameon=False, ncol=2)
    ax.grid(alpha=0.3)
fig.tight_layout()
fig.savefig(f"{E}/convergencia.pdf")
fig.savefig(f"{FIG}/convergencia.pdf")  # figura incluida en el informe


print("Listo")
