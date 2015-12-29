################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../http/client_data.cpp \
../http/http_server.cpp \
../http/http_worker.cpp 

OBJS += \
./http/client_data.o \
./http/http_server.o \
./http/http_worker.o 

CPP_DEPS += \
./http/client_data.d \
./http/http_server.d \
./http/http_worker.d 


# Each subdirectory must supply rules for building sources it contributes
http/%.o: ../http/%.cpp
	@echo '正在构建文件： $<'
	@echo '正在调用： GNU C++ 编译器'
	g++ -I"/root/eclipse/thread_pool/include" -I"/root/eclipse/thread_pool" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


