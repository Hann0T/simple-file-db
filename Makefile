TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./my_new_db.db -n
	./$(TARGET) -f ./my_new_db.db -a "Timmy H.,123 some address,120"
	#./$(TARGET) -f ./my_new_db.db -n # should fail, file already exists

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude # -Iinclude to search for header files in the include dir
