CC=emcc
WEBCORE_DIR=../WebCore
SOURCES=main.cpp
LDFLAGS=-O2 --llvm-opts 2
OUTPUT=out/pthread_turing_example.js
USE_IMGUI=-I$(WEBCORE_DIR)/imgui/
USE_VORBIS=-s USE_VORBIS=1
USE_LIBPNG=-s USE_LIBPNG=1 -s USE_ZLIB=1
USE_WEBGL=-s FULL_ES3=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -lGL
USE_WEBGL_MOB=-s FULL_ES2=1 -s USE_GLFW=3 -lGL
USE_ENGINE_WEBCORE=-I$(WEBCORE_DIR)/include/  $(wildcard $(WEBCORE_DIR)/src/*.cpp) $(wildcard $(WEBCORE_DIR)/imgui/*.cpp)
USE_ASYNC_FILES=-s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1

USE_PTHREADPOOL=-Ipthreadpool_master/include/ -Ipthreadpool_master/src/ ./pthreadpool_master/src/pthreads.c 
USE_TASKFLOW=-Itaskflow-master

#cmd source ~/emsdk/emsdk_env.sh
#ource ~/Documents/dev/emsdk/emsdk_env.sh

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) -Isrc -I/usr/local/include/ $(SOURCES) $(USE_IMGUI) -std=c++1z $(USE_VORBIS) $(USE_LIBPNG) $(USE_WEBGL) $(USE_ENGINE_WEBCORE) $(USE_TASKFLOW) $(LDFLAGS) -o $(OUTPUT) -s WASM=1 -s ASSERTIONS=1 --preload-file data -s ALLOW_MEMORY_GROWTH=1 --emrun -s USE_PTHREADS=1


clean:
	rm $(OUTPUT)

#g++ simple.cpp -Itaskflow-master/ -std=c++17 -O2 -lpthread -o simple