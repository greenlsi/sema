SPEC= spec
MODEL=model.pml

all: verify

pan.c: $(MODEL)
	spin -a $(MODEL)

pan: pan.c
	gcc -o pan pan.c

verify: pan
	rm -f *.trail
	for f in $(SPEC); do ./pan -a -f -N $$f ; if [ -f *.trail ]; then spin -t $(MODEL) ; fi ; done

clean:
	rm -f *.trail pan pan.* *.tmp *.tcl
