build/:
	mkdir build/

build/%.o: src/%.c
	@echo "CC   $*.c"
	@$(CC) -c -o $@ $(CFLAGS) $<
