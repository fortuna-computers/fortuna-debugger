TARGETS = libfdbg-server.a

all: $(TARGETS)

libfdbg-server.a:
	$(MAKE) -C src/library
	cp src/library/$@ .

clean:
	$(MAKE) -C src/library clean
	rm -f $(TARGETS)
