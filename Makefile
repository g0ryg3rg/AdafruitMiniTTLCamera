PROJECT=camera

# Two additional CFLAGS must be used for Angstrom
# They must not be used for Debian or Ubuntu. I couldn't find out why. 
# The hint came from C:\gcc-linaro\share\doc\gcc-linaro-arm-linux-gnueabihf\README.txt 
#
# Uncomment the following line if you use Angstrom on your BeagleBone
#TARGET=angstrom

# Directory for C-Source
# vpath %.c $(CURDIR)/source
# Directory for C++ Source
vpath %.cpp $(CURDIR)/source
vpath %.c $(CURDIR)/source

# Directory for includes
CINCLUDE = $(CURDIR)/include  

# Directory for object files
OBJDIR = $(CURDIR)/object

# Other dependencies
DEPS = \
 Makefile \
 include/camera_mediator.h \
 include/bbb_uart.h

# Compiler object files 
COBJ = \
 $(OBJDIR)/camera_mediator.o \
 $(OBJDIR)/bbb_uart.o\
 $(OBJDIR)/main.o

# gcc binaries to use
CC = "C:\gcc-linaro\bin\arm-linux-gnueabihf-g++.exe"
LD = "C:\gcc-linaro\bin\arm-linux-gnueabihf-g++.exe"

# rm is part of yagarto-tools
SHELL = cmd
REMOVE = rm -f

# Compiler options
# Two additional flags neccessary for Angstrom Linux. Don't use them with Ubuntu or Debian  
CFLAGS = -marm
ifeq ($(TARGET),angstrom)
CFLAGS += -march=armv4t
CFLAGS += -mfloat-abi=soft
endif
CFLAGS += -O0 #default optimization
CFLAGS += -g3 #maximum debug info 
CFLAGS += -I.
CFLAGS += -I$(CINCLUDE)
CFLAGS += $(CDEFINE)

# for a better output
MSG_EMPTYLINE = . 
MSG_COMPILING = ---COMPILE--- 
MSG_LINKING = ---LINK--- 
MSG_SUCCESS = ---SUCCESS--- 

# Our favourite
all: $(PROJECT)

# Linker call
$(PROJECT): $(COBJ)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_LINKING)
	$(LD) -o $@ $^ $(CFLAGS)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_SUCCESS) $(PROJECT)

# Compiler call
$(COBJ): $(OBJDIR)/%.o: %.cpp $(DEPS)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_COMPILING) $<
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(REMOVE) $(OBJDIR)/*.o
	$(REMOVE) $(PROJECT)

