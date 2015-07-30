CPPFLAGS = -std=c++11 -Werror -Wall -DGLEW_STATIC
INC_DIR = -I ./
LDLIBS = -lglfw -lGL -lGLEW -lSOIL
CPP = g++ -fdiagnostics-color=always
EXECUTABLE = multipleTextures

vpath %.cpp ./
src = multipleTextures.cpp
objects = $(patsubst %.cpp,build/%.o,$(src))

all: $(EXECUTABLE)

$(EXECUTABLE): $(objects)
	@echo $<
	@$(CPP) $(CPPFLAGS) $^ -o build/$(EXECUTABLE) $(LDLIBS)

$(objects): | build

build: 
	@mkdir -p $@

build/%.o: %.cpp
	@echo $<
	@$(CPP) $(CPPFLAGS) $(INC_DIR) -c $< -o $@

.PHONY: clean
clean:
	rm -rf build
	rm $(EXECUTABLE)
	
