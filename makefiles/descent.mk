SOURCES_DESCENT = read/main.cpp read/read.cpp read/tokenizator.cpp verificator.cpp tree.cpp\
						op_data.cpp tree_logger.cpp library.cpp
TARGET_DESCENT = $(TARGET_DIR)/descent.out

OBJECTS_DESCENT = $(addprefix $(OBJ_DIR)/, $(SOURCES_DESCENT:.cpp=.o))

$(TARGET_DESCENT): $(OBJECTS_DESCENT) | $(TARGET_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "LINKED DESCENT"

descent_b: $(TARGET_DESCENT)

descent: $(TARGET_DESCENT)
	@./$(TARGET_DESCENT) files/prog.upl files/tree.tr