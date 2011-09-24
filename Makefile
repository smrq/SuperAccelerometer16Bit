APPNAME=superaccelerometerturbo
PACKAGE=net.smrq.superaccelerometerturbo
VERSION=0.1.0

SRC=main.cpp Accelerometer.cpp Animation.cpp Model.cpp Shader.cpp TransformationMatrix.cpp

LIBS=-lSDL -lSDL_image -lGLESv2 -lpdl

SRCDIR=src
RESDIR=res
BUILDDIR=build
EXECDIR=$(BUILDDIR)/exec
STAGINGDIR=$(BUILDDIR)/$(APPNAME)

OUTFILE=$(EXECDIR)/$(APPNAME)
IPKFILE=$(PACKAGE)_$(VERSION)_all.ipk

PALMPDK=/opt/PalmPDK
INCLUDEDIR=$(PALMPDK)/include
LIBDIR=$(PALMPDK)/device/lib
SYSROOT=$(PALMPDK)/arm-gcc/sysroot

PATH:=$(PATH):$(PALMPDK)/arm-gcc/bin
CC=arm-none-linux-gnueabi-gcc

DEVICEOPTS=-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
CPPFLAGS=-I$(INCLUDEDIR) -I$(INCLUDEDIR)/SDL --sysroot=$(SYSROOT)
LDFLAGS=-L$(LIBDIR) -Wl,--allow-shlib-undefined

vpath %.cpp $(SRCDIR)

###############################################################################

.PHONY : all build package install uninstall run clean clean-install

all: build

clean-install: clean install

clean:
	rm -rf $(BUILDDIR)
	rm -f *.ipk

debug:
	novacom 

run:
	palm-launch -d usb $(PACKAGE)

uninstall:
	palm-install -d usb -r $(PACKAGE)

install: $(IPKFILE)
	palm-install -d usb $(IPKFILE)

package: $(IPKFILE)

build: $(OUTFILE)

$(IPKFILE): $(OUTFILE)
	mkdir -p $(STAGINGDIR)
	cp $(OUTFILE) $(STAGINGDIR)
	cp -r $(RESDIR)/* $(STAGINGDIR)
	echo "filemode.755=$(APPNAME)" > $(STAGINGDIR)/package.properties
	palm-package $(STAGINGDIR)

$(OUTFILE): $(SRC)
	mkdir -p $(EXECDIR)
	$(CC) $(DEVICEOPTS) $(CPPFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^
