APPNAME=superaccelerometerturbo
PACKAGE=net.smrq.superaccelerometerturbo
VERSION=0.1.0

SRCDIR=src
RESDIR=res
LIBDIR=lib
BUILDDIR=build
EXECDIR=$(BUILDDIR)/exec
STAGINGDIR=$(BUILDDIR)/$(APPNAME)

LIBS=-lSDL -lSDL_image -lGLESv2 -lpdl
SRC=$(SRCDIR)/*.cpp $(LIBDIR)/jsoncpp-0.5.0/src/*.cpp

OUTFILE=$(EXECDIR)/$(APPNAME)
IPKFILE=$(PACKAGE)_$(VERSION)_all.ipk

PALMPDK=/opt/PalmPDK
SYSROOT=$(PALMPDK)/arm-gcc/sysroot

CC=$(PALMPDK)/arm-gcc/bin/arm-none-linux-gnueabi-gcc

DEVICEOPTS=-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
CPPFLAGS=-I$(PALMPDK)/include -I$(PALMPDK)/include/SDL -I$(LIBDIR)/jsoncpp-0.5.0/include --sysroot=$(SYSROOT)
LDFLAGS=-L$(PALMPDK)/device/lib -Wl,--allow-shlib-undefined

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
