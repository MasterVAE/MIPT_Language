SOURCES_MIDLEND = midlend/main.cpp midlend/optimizator.cpp \
						tree.cpp op_data.cpp tree_logger.cpp library.cpp
TARGET_MIDLEND = $(TARGET_DIR)/midlend.out

OBJECTS_MIDLEND = $(addprefix $(OBJ_DIR)/, $(SOURCES_MIDLEND:.cpp=.o))

$(TARGET_MIDLEND): $(OBJECTS_MIDLEND) | $(TARGET_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "LINKED MIDLEND"

midlend_b: $(TARGET_MIDLEND)

midlend: $(TARGET_MIDLEND)
	@./$(TARGET_MIDLEND) files/tree.tr