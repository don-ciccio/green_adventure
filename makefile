COMPILER = clang
SOURCE_LIBS = -I/opt/homebrew/Cellar/raylib/5.5/include
OSX_OPT = -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
OSX_OUT = -o "bin/build_osx"
CFILES = src/*.c

build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT) && ./bin/build_osx