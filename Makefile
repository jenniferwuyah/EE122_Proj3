objects = source destination

all: $(objects)

$(objects): %: %.c
	gcc -std=c99 -o $@ -Wno-implicit-function-declaration $<

clean: 
	rm $(objects)
