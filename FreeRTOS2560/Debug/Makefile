################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../debugPrint.c \
../FreeRTOS2560.c \
../heap_1.c \
../list.c \
../myHcSonar.c \
../myMaxSonar.c \
../myADC.c \
../mySharpIR.c \
../myTimer.c \
../myUSART.c \
../port.c \
../queue.c \
../ringBuffer.c \
../tasks.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
debugPrint.o \
FreeRTOS2560.o \
heap_1.o \
list.o \
myHcSonar.o \
myMaxSonar.o \
myADC.o \
mySharpIR.o \
myTimer.o \
myUSART.o \
port.o \
queue.o \
ringBuffer.o \
tasks.o

OBJS_AS_ARGS +=  \
debugPrint.o \
FreeRTOS2560.o \
heap_1.o \
list.o \
myHcSonar.o \
myMaxSonar.o \
myADC.o \
mySharpIR.o \
myTimer.o \
myUSART.o \
port.o \
queue.o \
ringBuffer.o \
tasks.o

C_DEPS +=  \
debugPrint.d \
FreeRTOS2560.d \
heap_1.d \
list.d \
myHcSonar.d \
myMaxSonar.d \
myADC.d \
mySharpIR.d \
myTimer.d \
myUSART.d \
port.d \
queue.d \
ringBuffer.d \
tasks.d

C_DEPS_AS_ARGS +=  \
debugPrint.d \
FreeRTOS2560.d \
heap_1.d \
list.d \
myHcSonar.d \
myMaxSonar.d \
myADC.d \
mySharpIR.d \
myTimer.d \
myUSART.d \
port.d \
queue.d \
ringBuffer.d \
tasks.d

OUTPUT_FILE_PATH +=FreeRTOS2560.elf

OUTPUT_FILE_PATH_AS_ARGS +=FreeRTOS2560.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+=  \
C:/Users/tienlong/Desktop/CG3002/Project/arduino/Arduino/Debug/libarduino.a

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler





























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino" -I"C:\Program Files (x86)\Arduino\hardware\arduino\avr\variants\mega" -I"C:\Users\tienlong\Desktop\CG3002\FreeRTOS V8.1.2\FREERTOS\Demo\Common\include" -I"C:\Users\tienlong\Desktop\CG3002\FreeRTOS V8.1.2\FREERTOS\Source\include" -I"C:\Users\tienlong\Desktop\CG3002\FreeRTOS V8.1.2\FREERTOS\Source\portable\GCC\ATMega2560" -I"C:\Users\tienlong\Desktop\CG3002\Project\FreeRTOS2560\FreeRTOS2560"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=atmega2560 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="FreeRTOS2560.map" -Wl,--start-group -Wl,-lm -Wl,-larduino  -Wl,--end-group -Wl,-L"C:\Users\tienlong\Desktop\CG3002\Project\arduino\Arduino\Debug"  -Wl,--gc-sections -mrelax -mmcu=atmega2560  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "FreeRTOS2560.elf" "FreeRTOS2560.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "FreeRTOS2560.elf" "FreeRTOS2560.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "FreeRTOS2560.elf" > "FreeRTOS2560.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "FreeRTOS2560.elf" "FreeRTOS2560.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-size.exe" "FreeRTOS2560.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "FreeRTOS2560.elf" "FreeRTOS2560.a" "FreeRTOS2560.hex" "FreeRTOS2560.lss" "FreeRTOS2560.eep" "FreeRTOS2560.map" "FreeRTOS2560.srec" "FreeRTOS2560.usersignatures"
	