MODELS = alarm code 
alarm_SPEC = spec_alarm spec_light
code_SPEC = spec_code spec_light
model_PANARGS = -m100000

all: $(patsubst %, %.verify, $(MODELS))


%.pan: %.pml
	@echo "----------- GENERATING VERIFYER $(*F) ---------------"
	spin -a $< && gcc -o $(*F).pan pan.c
	rm -f *.trail

%.verify: %.pan
	@echo "----------- VERIFYING $(*F) -------------------------"
	for f in $($(*F)_SPEC); do ./$< -a -f $($(*F)_PANARGS) -N $$f ; \
	  if [ -f *.trail ]; then \
	    spin -t -r -s -l -g $(*F).pml ; \
	  fi ; \
	done

clean:
	rm -f *.trail *.pan pan.* *.tmp *.tcl

