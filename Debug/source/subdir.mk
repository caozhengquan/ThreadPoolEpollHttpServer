################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/config.cpp \
../source/encap_epoll.cpp \
../source/tcp_server.cpp \
../source/thread_pool.cpp \
../source/worker.cpp 

OBJS += \
./source/config.o \
./source/encap_epoll.o \
./source/tcp_server.o \
./source/thread_pool.o \
./source/worker.o 

CPP_DEPS += \
./source/config.d \
./source/encap_epoll.d \
./source/tcp_server.d \
./source/thread_pool.d \
./source/worker.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo '正在构建文件： $<'
	@echo '正在调用： GNU C++ 编译器'
	g++ -I"/root/eclipse/thread_pool/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


