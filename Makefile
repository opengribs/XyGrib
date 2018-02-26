QMAKE=/usr/bin/qmake

INSTALLDIR=$(HOME)/zyGrib
MACQTBIN=/usr/local/Qt-5.3.2/bin/
QWTDIR=src/qwt-6.1.3

all: zyGrib

SRC= src/*.h src/*.cpp src/*/*.h src/*/*.cpp \
      src/qwt*/src/*.cpp src/qwt*/src/*.h

mac: $(SRC)
	rm -f  ./zyGrib
	rm -fr ./zyGrib.app
	rm -f  src/Makefile
	cd src/g2clib; make -f makefile.osx
	cd $(QWTDIR)/src; $(MACQTBIN)/qmake; make -j6
	cd src; $(MACQTBIN)/qmake; make -j6

macclean:
	rm -f  zyGrib
	rm -fr zyGrib.app
	rm -f  src/zyGrib
	rm -f  src/release/zyGrib.exe
	rm -f  $(QWTDIR)/lib/*
	cd $(QWTDIR)/src; $(MACQTBIN)/qmake; make clean
	cd src;	$(MACQTBIN)/qmake; make clean

clean:
	rm -f  zyGrib
	rm -fr zyGrib.app
	rm -f  src/zyGrib
	rm -f  src/release/zyGrib.exe
	cd src;	$(QMAKE); make clean

veryclean: clean
	rm -f  $(QWTDIR)/lib/*
	cd $(QWTDIR)/src; $(QMAKE); make clean
	rm -f  src/g2clib/libg2c.a
	rm -f  src/g2clib/*.o

zyGrib: $(SRC)
	@ echo
	rm -f ./zyGrib
	cd $(QWTDIR)/src; $(QMAKE); make -j6
	cd src/g2clib; make
	cd src; $(QMAKE); make -j6
	@ echo "-----------------------------------"
	@ echo "#!/bin/bash" >> ./zyGrib
	@ echo "cd `pwd`" >> ./zyGrib
	@ echo 'src/zyGrib $$*' >> ./zyGrib
	@ chmod 755 ./zyGrib

install: zyGrib
	mkdir -p $(INSTALLDIR)
	mkdir -p $(INSTALLDIR)/bin
	cp -a data $(INSTALLDIR)
	cp -a grib $(INSTALLDIR)
	rm -f ./zyGrib
	rm -f $(INSTALLDIR)/bin/zyGrib
	cp  -a src/zyGrib $(INSTALLDIR)/bin
	echo "#!/bin/bash" >> ./zyGrib
	echo "cd $(INSTALLDIR)" >> ./zyGrib
	echo 'bin/zyGrib $$*' >> ./zyGrib
	chmod 755 ./zyGrib
	cp -af zyGrib $(INSTALLDIR)


