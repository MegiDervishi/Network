MAIN=pong

PACKAGES=unix,graphics
#PACKAGES=unix,graphics,curses
#PACKAGES=unix,graphics,sdl

all: $(MAIN)

$(MAIN): $(MAIN).ml
	ocamlfind ocamlopt -o $@ \
	    -thread -linkpkg -package $(PACKAGES) $(MAIN).ml

$(MAIN).byte: $(MAIN).ml
	ocamlfind ocamlc -o $@ \
	    -thread -linkpkg -package $(PACKAGES) $(MAIN).ml

clean:
	rm -f $(MAIN).cmi $(MAIN).cmx $(MAIN).cmo $(MAIN).o

realclean: clean
	rm -f *~ $(MAIN) $(MAIN).byte

cleanall: realclean

