################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../app/main.cpp 

OBJS += \
./app/main.o 

CPP_DEPS += \
./app/main.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o: ../app/%.cpp
	@echo '正在构建文件： $<'
	@echo '正在调用： GNU C++ 编译器'
	g++ -I"/root/eclipse/thread_pool/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


