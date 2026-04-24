all:
	@clang *.c -o ./test/server
	@(cd test && ./server)
