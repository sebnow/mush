TEST_CFLAGS := $(CFLAGS) -Isrc -Ibuild/cmockery/include/google

TEST_OBJ = build/test_all.o \
           build/test_builtin.o \
           build/test_command.o \
           build/test_exec.o \
           build/test_parser.o \
           build/test_queue.o

build/test_%.o: tests/test_%.c
	@@echo "CC   test_$*.c"
	@$(CC) -c -o $@ $(TEST_CFLAGS) $<

build/cmockery/lib/libcmockery.a:
	sh build-test-deps.sh

tests: all build/cmockery/lib/libcmockery.a $(TEST_OBJ)
	$(LINK.cc) -o run_tests $(TEST_OBJ) $(OBJ) build/cmockery/lib/libcmockery.a
