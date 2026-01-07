SOURCES_COMPILATOR = compile/main.cpp compile/compilator.cpp tree.cpp op_data.cpp tree_logger.cpp\
						library.cpp
TARGET_COMPILATOR = $(TARGET_DIR)/compilator.out

OBJECTS_COMPILATOR = $(addprefix $(OBJ_DIR)/, $(SOURCES_COMPILATOR:.cpp=.o))

$(TARGET_COMPILATOR): $(OBJECTS_COMPILATOR) | $(TARGET_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "LINKED COMPILATOR"

compilator_b: $(TARGET_COMPILATOR)

compilator: $(TARGET_COMPILATOR)
	@./$(TARGET_COMPILATOR) files/tree.tr files/code.asm