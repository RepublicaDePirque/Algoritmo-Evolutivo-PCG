#!/bin/bash
# Ejecuta los experimentos del informe. Uso (desde la carpeta del proyecto):
#   bash experimentos/correr.sh            (usa nproc ejecuciones en paralelo)
#   PARALELO=8 bash experimentos/correr.sh (fija la cantidad de ejecuciones en paralelo)
# Genera experimentos/*.csv; luego: python3 experimentos/analizar.py
# Cada archivo se escribe primero en un temporal y se reemplaza solo al terminar,
# para que una ejecucion interrumpida no deje CSV incompletos.
set -e
make -s
OUT=experimentos
CAB="instancia,semilla,P,E,O,f,completable,linealidad,fitness,f_inicial,lin_inicial,generaciones,gen_mejor,evaluaciones,segundos"
NPROC=${PARALELO:-$(nproc)}
echo "Ejecuciones en paralelo: $NPROC"

# A) Configuracion final: 6 instancias x 30 semillas
echo "A) Configuracion final"
echo "$CAB" > $OUT/final.tmp
for i in 1 2 3 4 5 6; do for s in $(seq 1 30); do echo "$i $s"; done; done |
  xargs -P $NPROC -n 2 sh -c './generador instancias/instancia$0.txt $1 -q | grep ^RESULTADO' |
  sed 's/^RESULTADO,//' | sort -t, -k1,1 -k2,2n >> $OUT/final.tmp
mv $OUT/final.tmp $OUT/final.csv

# B) Sensibilidad al largo del tramo: 6 instancias x 10 semillas x 5 largos
echo "B) Sensibilidad al tramo"
echo "tramo,$CAB" > $OUT/tramo.tmp
for t in 2 5 15 30 50; do for i in 1 2 3 4 5 6; do for s in $(seq 1 10); do echo "$t $i $s"; done; done; done |
  xargs -P $NPROC -n 3 sh -c './generador instancias/instancia$1.txt $2 -t $0 -q | grep ^RESULTADO | sed "s/^RESULTADO/$0/"' >> $OUT/tramo.tmp
mv $OUT/tramo.tmp $OUT/tramo.csv

# C) Sensibilidad a la probabilidad de mutacion: 6 instancias x 10 semillas x 3 valores
echo "C) Sensibilidad a la mutacion"
echo "pm,$CAB" > $OUT/mutacion.tmp
for m in 0.2 0.5 0.8; do for i in 1 2 3 4 5 6; do for s in $(seq 1 10); do echo "$m $i $s"; done; done; done |
  xargs -P $NPROC -n 3 sh -c './generador instancias/instancia$1.txt $2 -m $0 -q | grep ^RESULTADO | sed "s/^RESULTADO/$0/"' >> $OUT/mutacion.tmp
mv $OUT/mutacion.tmp $OUT/mutacion.csv

# D) Linea base: busqueda aleatoria con el mismo presupuesto (48.050 niveles
#    aleatorios, sin generaciones), 6 instancias x 10 semillas
echo "D) Busqueda aleatoria"
echo "$CAB" > $OUT/aleatoria.tmp
for i in 1 2 3 4 5 6; do for s in $(seq 1 10); do echo "$i $s"; done; done |
  xargs -P $NPROC -n 2 sh -c './generador instancias/instancia$0.txt $1 -p 48050 -g 0 -q | grep ^RESULTADO' |
  sed 's/^RESULTADO,//' | sort -t, -k1,1 -k2,2n >> $OUT/aleatoria.tmp
mv $OUT/aleatoria.tmp $OUT/aleatoria.csv

# E) Curvas de convergencia (semilla 1)
echo "E) Convergencia"
for i in 1 2 3 4 5 6; do
  ./generador instancias/instancia$i.txt 1 -q -l $OUT/convergencia_instancia$i.csv > /dev/null
done
echo "Listo"
