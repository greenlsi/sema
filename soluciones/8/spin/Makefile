MODELS = alarm code
alarm_SPEC = spec
code_SPEC = spec

all: $(patsubst %, %.verify, $(MODELS))


%.pan: %.pml
	@echo "----------- GENERATING VERIFYER $(*F) ---------------"
	spin -a $< && gcc -o $(*F).pan pan.c
	rm -f *.trail

%.verify: %.pan
	@echo "----------- VERIFYING $(*F) -------------------------"
	for f in $($(*F)_SPEC); do ./$< -a -f -N $$f ; \
	  if [ -f *.trail ]; then \
	    spin -t -r -s -l -g $(*F).pml ; \
	  fi ; \
	done

clean:
	rm -f *.trail *.pan pan.* *.tmp *.tcl

