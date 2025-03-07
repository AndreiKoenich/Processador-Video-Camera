# FUNDAMENTOS DE PROCESSAMENTO DE IMAGENS

# Andrei Pochmann Koenich - Matrícula 00308680

# Makefile

# Variáveis de flags de compilação
CXXFLAGS := -g -Wall -Wno-unknown-pragmas $(shell pkg-config --cflags opencv4) -o
LDFLAGS := $(shell pkg-config --libs opencv4)

# Comando para compilar e gerar o executável
link:
	g++ $(CXXFLAGS) processadorCamera processadorCamera.cpp $(LDFLAGS)

# Comando padrão do Makefile
.DEFAULT_GOAL := link