C ?= c++
SRC ?= main.cpp
OUT ?= kaliframe
LD_FLAGS ?= `pkg-config opencv4 --cflags --libs`

install:
	$(C) $(SRC) -o $(OUT) $(LD_FLAGS)

run:
	./$(OUT)
