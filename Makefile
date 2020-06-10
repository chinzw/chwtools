#/***********************************************************************************
#* Copyright (c) 2018 Cristian Hinz Welkens.
#*
#* This file is part of chwTools (https://github.com/chinzw/chwtools).
#*
#* Permission is hereby granted, free of charge, to any person obtaining a copy of
#* this software and associated documentation files (the "Software"), to deal in
#* the Software without restriction, including without limitation the rights to 
#* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
#* of the Software, and to permit persons to whom the Software is furnished to do
#* so, subject to the following conditions:
#*
#* The above copyright notice and this permission notice shall be included in all
#* copies or substantial portions of the Software.
#*
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
#* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
#* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#***********************************************************************************/

##################################
## MAYA VERSION AND LOCATION
##################################
ifndef MAYA_VERSION
$(error MAYA_VERSION is not set)
endif

MAYA_LOCATION=/usr/autodesk/maya$(MAYA_VERSION)

MAYA_INCLUDE=-I$(MAYA_LOCATION)/include
MAYA_LIB=-L$(MAYA_LOCATION)/lib -lOpenMaya -lOpenMayaAnim

##################################
## PLUGIN OUTPUT
##################################
MAYA_MODULE_INSTALL_PATH = ~/maya/$(MAYA_VERSION)/modules
MAYA_MODULE_PATH = chwTools
MAYA_MODULE_FILE = $(MAYA_MODULE_PATH)/chwTools.mod
LIBRARY=chwTools.so

##################################
## PLUGIN SOURCES
##################################
PYTHON_MOLDULE=python
AETEMPLATES=$(shell find AETemplates/ -name '*.mel')
SOURCES=$(shell find src/ -name '*.cpp')
HEADERS=${shell find include/ -name '*.h'}
OBJECTS=$(SOURCES:.cpp=.o)

##################################
## COMPILER OPTIONS
##################################
CPP = g++
LD = $(CPP)

INCLUDE_FLAGS= $(MAYA_INCLUDE) -Iinclude/
LIB_FLAGS= $(MAYA_LIB)

CFLAGS = -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE \
		 -DLINUX_64 -fPIC \
		 -fno-strict-aliasing -DREQUIRE_IOSTREAM -Wall \
		 -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char \
		 -pthread

debug: CFLAGS += -DDEBUG -g -O0
install: CFLAGS += -O3

CPPFLAGS = $(CFLAGS) -std=c++11 -Wno-deprecated -Wno-reorder -fno-gnu-keywords

LFLAGS = $(CPPFLAGS) -Wl,-Bsymbolic -shared


##################################
## COMPILE TARGETS
##################################
.cpp.o: $(SOURCES) $(HEADERS)
	$(CPP) -c $< $(CPPFLAGS) $(INCLUDE_FLAGS) -o $@

$(LIBRARY): $(OBJECTS)
	 $(CPP) $(OBJECTS) $(LFLAGS) $(LIB_FLAGS) -o $@

install: $(LIBRARY)
	mv -f $(LIBRARY) 'chwTools/plug-ins'
	mkdir -p $(MAYA_MODULE_INSTALL_PATH)
	cp -f $(MAYA_MODULE_FILE) $(MAYA_MODULE_INSTALL_PATH)
	cp -r $(MAYA_MODULE_PATH) $(MAYA_MODULE_INSTALL_PATH)

debug: $(LIBRARY)
	mv -f $(LIBRARY) 'chwTools/plug-ins'
	mkdir -p $(MAYA_MODULE_INSTALL_PATH)
	cp -f $(MAYA_MODULE_FILE) $(MAYA_MODULE_INSTALL_PATH)
	cp -r $(MAYA_MODULE_PATH) $(MAYA_MODULE_INSTALL_PATH)

clean:
	rm -f src/*.o
	rm -f chwTools/plug-ins/*.so
