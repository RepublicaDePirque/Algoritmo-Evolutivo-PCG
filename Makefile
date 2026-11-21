CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
EXE      = generador
SRC      = $(wildcard src/*.cpp)
OBJ      = $(SRC:.cpp=.o)
HDR      = $(wildcard src/*.h)

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Python para graficar: usa .venv si existe, si no python3
PYTHON ?= $(shell test -x .venv/bin/python && echo .venv/bin/python || echo python3)

# Compila, ejecuta las 6 instancias con semilla 1 y grafica las soluciones
run: $(EXE)
	for i in 1 2 3 4 5 6; do ./$(EXE) instancias/instancia$$i.txt 1; done
	$(MAKE) graficar

# Genera un PNG por cada solucion de salidas/ (requiere matplotlib)
graficar:
	@for f in salidas/*_solucion.txt; do $(PYTHON) graficar.py $$f || { echo "No se pudo graficar: instale matplotlib (python3 -m venv .venv && .venv/bin/pip install matplotlib)"; exit 0; }; done

# Experimentos del informe (tablas y figuras en informe/)
experimentos: $(EXE)
	bash experimentos/correr.sh
	$(PYTHON) experimentos/analizar.py

clean:
	rm -f src/*.o $(EXE)

.PHONY: all run graficar experimentos clean
