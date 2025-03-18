CC ?= c++
SRC ?= main.cpp
OUT ?= kaliframe
LD_FLAGS ?= `pkg-config --cflags --libs opencv4 hdf5` -lstdc++

install:
	$(CC) $(SRC) -o $(OUT) $(LD_FLAGS)

run:
	./$(OUT)
