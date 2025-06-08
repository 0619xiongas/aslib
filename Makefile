CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Iinclude -Wall -O2

# 需要链接的第三方库
LIBS = -lmysqlclient -lboost_system -lboost_thread -luv -lprotobuf -lcjson -lpthread

BUILD_DIR = build

# 查找所有源文件
SRCS := $(shell find src -name "*.cpp")
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 输出库名
TARGET_STATIC = aslib.a
TARGET_SHARED = aslib.so

# 默认目标
all: $(BUILD_DIR) $(TARGET_STATIC) $(TARGET_SHARED)

# 创建build目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 生成静态库
$(TARGET_STATIC): $(OBJS)
	ar rcs $@ $^

# 生成动态库
$(TARGET_SHARED): $(OBJS)
	$(CXX) -shared -o $@ $^ $(LIBS)

# 编译源文件为目标文件，自动创建子目录
$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf $(BUILD_DIR) $(TARGET_STATIC) $(TARGET_SHARED)

.PHONY: all clean