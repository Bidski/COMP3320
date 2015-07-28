CPPFLAGS = -std=c++11 -Werror -Wall -DGLEW_STATIC
INC_DIR = -I ./
LDLIBS = -lglfw -lGL -lGLEW
CPP = g++
EXECUTABLE = template

vpath %.cpp ./
src = template.cpp
objects = $(patsubst %.cpp,build/%.o,$(src))

all: $(EXECUTABLE)

$(EXECUTABLE): $(objects)
	@echo $<
	@$(CPP) $(CPPFLAGS) $^ -o $(EXECUTABLE) $(LDLIBS)

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
	
