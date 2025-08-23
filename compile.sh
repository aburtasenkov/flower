SRC_DIR="src"

echo "Using following source files:"
echo "    " $SRC_DIR/*.c

gcc -o main main.c "$SRC_DIR"/*.c -lm
    
