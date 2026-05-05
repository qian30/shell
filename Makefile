CC     = gcc
CFLAGS = -Wall -Wextra -g
TEST   = test
SRC    = src
CSRC   = csrc
BIN    = bin
SRCS   = $(wildcard $(SRC)/*.c)

helper_DEPS = $(TEST)/test_helper.c $(SRC)/helper.c
parse_DEPS  = $(TEST)/test_parse.c  $(SRC)/env.c    $(SRC)/helper.c	$(SRC)/parse.c $(SRC)/builtin.c
env_DEPS    = $(TEST)/test_env.c    $(SRC)/env.c    $(SRC)/helper.c	$(SRC)/parse.c $(SRC)/builtin.c

TEST_BINS = $(BIN)/test_helper \
            $(BIN)/test_parse  \
            $(BIN)/test_env \
			$(BIN)/test_shell 

all: shell

.PHONY: shell
shell: $(SRCS) | $(BIN)
	$(CC) $(CFLAGS) -I include $^ -o $@

# =====================
# External Executables
# =====================
.PHONY: number
number: $(CSRC)/number.c | $(BIN)
	$(CC) $(CFLAGS) $^ -o $(BIN)/number

# =============================
# Unit Test: parse, helper, env
# =============================
.PHONY: parse helper env
parse helper env:
	@$(MAKE) clean
	@$(MAKE) -s $(BIN)/test_$@
	@./$(BIN)/test_$@

.SECONDEXPANSION:
$(BIN)/test_%: $$($*_DEPS) | $(BIN)
	$(CC) $(CFLAGS) -I include -I test $($*_DEPS) -o $@

# ================
# Integrated Test
# ================
.PHONY: test
test: $(SRCS) | $(BIN)
	@$(MAKE) clean
	$(CC) $(CFLAGS) -I include -DTEST_MODE $^ -o $(BIN)/test_shell
	./script/gen_out.sh
	./script/run.sh

# =======
# Others
# =======
$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf $(TEST_BINS) shell **/*.dSYM *.dSYM

.PHONY: indent
indent:
	clang-format -i **/*.[ch]