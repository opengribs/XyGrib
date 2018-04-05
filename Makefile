QMAKE=/usr/bin/qmake

INSTALLDIR=$(HOME)/XyGrib
MACQTBIN=/usr/local/Qt-5.3.2/bin/
QWTDIR=src/qwt-6.1.3

all: XyGrib

SRC= src/*.h src/*.cpp src/*/*.h src/*/*.cpp \
      src/qwt*/src/*.cpp src/qwt*/src/*.h

mac: $(SRC)
	rm -f  ./XyGrib
	rm -fr ./XyGrib.app
	rm -f  src/Makefile
	cd src/g2clib; make -f makefile.osx
	cd $(QWTDIR)/src; $(MACQTBIN)/qmake; make -j6
	cd src; $(MACQTBIN)/qmake; make -j6

macclean:
	rm -f  XyGrib
	rm -fr XyGrib.app
	rm -f  src/XyGrib
	rm -f  src/release/XyGrib.exe
	rm -f  $(QWTDIR)/lib/*
	cd $(QWTDIR)/src; $(MACQTBIN)/qmake; make clean
	cd src;	$(MACQTBIN)/qmake; make clean

clean:
	rm -f  XyGrib
	rm -fr XyGrib.app
	rm -f  src/XyGrib
	rm -f  src/release/XyGrib.exe
	cd src;	$(QMAKE); make clean

veryclean: clean
	rm -f  $(QWTDIR)/lib/*
	cd $(QWTDIR)/src; $(QMAKE); make clean
	rm -f  src/g2clib/libg2c.a
	rm -f  src/g2clib/*.o

XyGrib: $(SRC)
	@ echo
	rm -f ./XyGrib
	cd $(QWTDIR)/src; $(QMAKE); make -j6
	cd src/g2clib; make
	cd src; $(QMAKE); make -j6
	@ echo "-----------------------------------"
	@ echo "#!/bin/bash" >> ./XyGrib
	@ echo "cd `pwd`" >> ./XyGrib
	@ echo 'src/XyGrib $$*' >> ./XyGrib
	@ chmod 755 ./XyGrib

install: XyGrib
	mkdir -p $(INSTALLDIR)
	mkdir -p $(INSTALLDIR)/bin
	cp -a data $(INSTALLDIR)
	cp -a grib $(INSTALLDIR)
	rm -f ./XyGrib
	rm -f $(INSTALLDIR)/bin/XyGrib
	cp  -a src/XyGrib $(INSTALLDIR)/bin
	echo "#!/bin/bash" >> ./XyGrib
	echo "cd $(INSTALLDIR)" >> ./XyGrib
	echo 'bin/XyGrib $$*' >> ./XyGrib
	chmod 755 ./XyGrib
	cp -af XyGrib $(INSTALLDIR)


