#================================================================================#
# This makefile created by B. Jonkman on 2-Apr-2013,                             #
# adapted from Crunch (M. Buhl on 25-Jan-2013).                                  #
# (c) 2013 National Renewable Energy Laboratory                                  #
#                                                                                #
# This makefile has been tested on Windows 7 with gfortran.                      #
# This makefile works with mingw32-make.exe.                                     #
#                                                                                #
# It was designed to be used with discon.f90 for the Bladed DLL Interface        #
#================================================================================#

   # 32-bit or 64-bit?
# BITS = 32
BITS = 64
   # Location of source files for the DLL.
   # You may need to change these for your DLL.

DLL_DIR     = .

SOURCES = BaramInterf.c
SOURCE_FILE = Constants.f90 DRC_Types.f90 Functions.f90 Filters.f90 ReadSetParameters.f90 Controllers.f90 DISCON.f90

   # Name of compiler and flags for Baram interface.
PYTHON_CONFIG = python3.6-config
PYTHON_CONFIG_CFLAGS = $$($(PYTHON_CONFIG) --cflags)
PYTHON_CONFIG_LDFLAGS = $$($(PYTHON_CONFIG) --ldflags)
CC			= gcc
CFLAGS  	= -O2 -m$(BITS) $(PYTHON_CONFIG_CFLAGS)

ifneq ($(UNAME), Darwin)
   CFLAGS     := $(CFLAGS) -fPIC
endif

   # Name of compiler to use and flags to use.
FC      = gfortran
FFLAGS  = -O2 -m$(BITS) -fbacktrace -ffree-line-length-none -x f95-cpp-input -C
LDFLAGS = -shared -O2 -m$(BITS) -fbacktrace

# other useful gfortran options:
#  -fdefault-real-8 -fcheck=bounds -std=f2003 -O0 -v
# -Wl,--out-implib,DISCON.lib


   # Destination and RootName for executable

OUTPUT_NAME = DISCON
DEST_DIR    = ../DISCON

   #==========================================================#
   # You should not need to change anything beyond this point #
   #==========================================================#

   # System-specific settings.

ifeq ($(OS),Windows_NT) 

ifeq ($(CYGWIN), 1)

   FFLAGS := $(FFLAGS) -DIMPLICIT_DLLEXPORT

      # Cygwin.
   DEL_CMD   = rm
   EXE_EXT   = _gwin$(BITS).so
   INTER_DIR = Obj_win$(BITS)
   MD_CMD    = @mkdir -p
   OBJ_EXT   = .o
   PATH_SEP  = /
   SYS_FILE  = SysGnuWin
   BARAMI_LIB_EXT = baraminterface.so
else
      # Windows
   DEL_CMD   = del
   EXE_EXT   = _gwin$(BITS).dll
   INTER_DIR = Obj_win$(BITS)
   MD_CMD    = @mkdir
   OBJ_EXT   = .obj
   PATH_SEP  = \\
   SYS_FILE  = SysGnuWin
   BARAMI_LIB_EXT = baraminterface.dll
endif

else

   FFLAGS := $(FFLAGS) -DIMPLICIT_DLLEXPORT

      # Linux
   DEL_CMD   = rm -f
   EXE_EXT   = _glin$(BITS).so
   INTER_DIR = Obj_lin$(BITS)
   MD_CMD    = @mkdir -p
   OBJ_EXT   = .o
   PATH_SEP  = /
   SYS_FILE  = SysGnuLinux
   BARAMI_LIB_EXT = baraminterface.so

   UNAME := $(shell uname -s)

   ifneq ($(UNAME), Darwin)
      FFLAGS     := $(FFLAGS) -fPIC
      LDFLAGS    := $(LDFLAGS) -fPIC
   endif
endif


   # Source files (by module)

DLL_SOURCES =          \
	$(SOURCE_FILE)

vpath %.f90 $(DLL_DIR)
vpath %.mod $(INTER_DIR)
vpath %.obj $(INTER_DIR)

ALL_OBJS      = $(DLL_SOURCES:.f90=.obj)

   # Rule to do everything.

all:     default
default:  $(BARAMI_LIB_EXT) $(INTER_DIR) $(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT)

   # Build libbaraminterface shared library.
$(BARAMI_LIB_EXT): $(SOURCES)
	$(CC) $(CFLAGS) -shared -o $(BARAMI_LIB_EXT) $(SOURCES) $(PYTHON_CONFIG_LDFLAGS)

   # General rules for compliling the files.

%.obj: %.f90
	$(FC)  $(FFLAGS) -c $< -o $(INTER_DIR)/$@ -J $(INTER_DIR) -B $(INTER_DIR)


   #  Dependency rules.
#$(SYS_FILE).obj:                   $(PREC).obj


   # Make sure the destination directory for the intermediate files exist.

$(INTER_DIR):
	$(MD_CMD) $(INTER_DIR)


   # For linking DLL.

$(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT): $(ALL_OBJS) | $(INTER_DIR)
	$(FC) $(LDFLAGS) -I $(INTER_DIR) -o $(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT) \
	$(foreach src, $(ALL_OBJS), $(addprefix $(INTER_DIR)/,$(src))) $(BARAMI_LIB_EXT)

   # Cleanup afterwards.

clean:
	$(DEL_CMD) $(INTER_DIR)$(PATH_SEP)*.mod $(INTER_DIR)$(PATH_SEP)*.obj $(BARAMI_LIB_EXT)

#gfortran -shared -O2 -m32 -fbacktrace -ffree-line-length-none -x f95-cpp-input -C ../CertTest/5MW_Baseline/ServoData/Source/DISCON.f90