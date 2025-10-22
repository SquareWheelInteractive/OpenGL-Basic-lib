CXX := gcc
CXXFLAGS := -Wall -I. -I./external
LDFLAGS := -lglfw -lGL -lm -lX11

SRC := $(wildcard *.c) \
		glad/glad.c \
		mplib/mplib.c \

OBJ := $(SRC:.c=.o)

TARGET := app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
