# nRF application makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52832
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132

# Source and header files
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .
APP_SOURCE_PATHS += ../sensors_reading/
APP_SOURCE_PATHS += ../pid/
APP_SOURCE_PATHS += ../motor_driver/
APP_SOURCES = $(notdir $(wildcard ./*.c))
APP_SOURCES += $(notdir $(wildcard ../sensors_reading/*.c))
APP_SOURCES += $(notdir $(wildcard ../motor_driver/*.c))
APP_SOURCES += $(notdir $(wildcard ../pid/*.c))

# Path to base of nRF52-base repo
NRF_BASE_DIR = ../../buckler/software/nrf52x-base/

# Include board Makefile (if any)
include ../../buckler/software/boards/buckler_revB/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk
