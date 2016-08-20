
all: .DEFAULT

clean: .DEFAULTCLEAN
	
.DEFAULT:
	$(MAKE) -C src all
	$(MAKE) -C test all
	
.DEFAULTCLEAN:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
