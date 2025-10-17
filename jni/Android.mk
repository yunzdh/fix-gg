
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := 5.so.fix

#正在使用的参数
LOCAL_CPPFLAGS += -fno-rtti -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++14

LOCAL_C_INCLUDES += $(LOCAL_PATH)/

LOCAL_CPP_EXTENSION := .cpp .c

LOCAL_SRC_FILES = library.cpp

LOCAL_LDLIBS := -llog

LOCAL_LDLIBS += $(LOCAL_PATH)/driver.a

include $(BUILD_SHARED_LIBRARY)
#动态库





