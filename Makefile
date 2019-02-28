#/*************************************************************************
#* Copyright (c) 2018 Cristian Hinz Welkens.
#*
#* This file is part of chwTools (https://github.com/chinzw/chwtools).
#*
#* chwTools is free software: you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation, version 3 of the License.
#*
#* chwTools is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with chwTools.  If not, see <https://www.gnu.org/licenses/>.
# *************************************************************************/

#MayaPlugin Makefile

##################################
## MAYA VERSION AND LOCATION
##################################
MAYA_MAJOR_VERSION=2018
MAYA_MINOR_VERSION=0.0
MAYA_LOCATION=/usr/autodesk/maya$(MAYA_MAJOR_VERSION)

MAYA_INCLUDE=-I$(MAYA_LOCATION)/include
MAYA_LIB=-L$(MAYA_LOCATION)/lib -lOpenMaya -lOpenMayaAnim

##################################
## PLUGIN OUTPUT
##################################
MAYA_MODULE_INSTALL_PATH = ~/maya/$(MAYA_MAJOR_VERSION)/modules
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
	rm -f *.so
