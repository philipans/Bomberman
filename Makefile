TARGET = Bomberman.exe
HEADS = gamefunctions.h renderfunctions.h
SRC = main.c
CFLAGS = -std=c99
LFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm

build: $(TARGET)

all: $(TARGET)
	./$<

$(TARGET): $(SRC) $(HEADS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LFLAGS)

clean:
	$(RM) $(TARGET)
