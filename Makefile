OUTPUT := out
# Always create output folder when running make
_CREATE_OUTPUT := $(shell mkdir -p $(OUTPUT))

ifeq ($(DEBUG), 1)
	cmake_build_type = Debug
else
	cmake_build_type = Release
endif

CXX ?= c++

fname := main

all: build test

$(OUTPUT)/Makefile:
	(cd $(OUTPUT) && cmake -DCMAKE_BUILD_TYPE=$(cmake_build_type) -DCMAKE_CXX_COMPILER=$(CXX) ..)

cmake: $(OUTPUT)/Makefile

build: cmake
	@(cd $(OUTPUT) && $(MAKE) --no-print-directory -j)

clean:
	@rm -Rf $(OUTPUT)

test: build
	check-algolab $(OUTPUT)/$(fname) .

.PHONY: all clean test build
