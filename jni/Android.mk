LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := memdump
LOCAL_SRC_FILES := ../main.cc
LOCAL_CPPFLAGS := -O2 -Wall -std=c++11

include $(BUILD_EXECUTABLE)
