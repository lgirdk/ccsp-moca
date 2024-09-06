##########################################################################
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
AM_CFLAGS = -fPIC -fno-exceptions -Os -Wall -Wextra

AM_CXXFLAGS = -std=c++11

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = CcspMoCA_gtest.bin

CcspMoCA_gtest_bin_CPPFLAGS = -I$(top_srcdir)/../hal/include \
                              -I$(top_srcdir)/../CcspCommonLibrary/source/cosa/include/ \
                              -I$(top_srcdir)/../CcspCommonLibrary/source/ccsp/components/include/ \
                              -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/ccsp \
                              -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/dbus-1.0 \
                              -I$(PKG_CONFIG_SYSROOT_DIR)$(libdir)/dbus-1.0/include \
                              -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/libsafec \
                              -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/gtest \
                              -I${top_srcdir}/gtest/include \
                              -I${top_srcdir}/source \
                              -I${top_srcdir}/source/test \
                              -I${top_srcdir}/source/TR-181/include \
                              -I${top_srcdir}/source/TR-181/middle_layer_src \
                              -I${top_srcdir}/source/TR-181/board_sbapi \
                              -DCONFIG_SYSTEM_MOCA \
                              -D_COSA_HAL_ \
                              -DUNIT_TEST_DOCKER_SUPPORT

CcspMoCA_gtest_bin_SOURCES = gtest_main.cpp \
                             $(top_srcdir)/source/TR-181/board_sbapi/cosa_moca_apis.c \
                             $(top_srcdir)/source/TR-181/board_sbapi/cosa_moca_helpers.c \
                             $(top_srcdir)/source/TR-181/board_sbapi/cosa_moca_param.c \
                             $(top_srcdir)/source/TR-181/board_sbapi/cosa_moca_webconfig_api.c \
                             $(top_srcdir)/source/TR-181/middle_layer_src/cosa_moca_dml.c \
                             $(top_srcdir)/source/TR-181/middle_layer_src/cosa_moca_internal.c \
                             $(top_srcdir)/source/TR-181/middle_layer_src/cosa_moca_network_info.c \
                             $(top_srcdir)/source/TR-181/middle_layer_src/plugin_main.c \
                             moca_mock.cpp \
                             cosa_moca_apis_test.cpp \
                             cosa_moca_webconfig_api_test.cpp \
                             cosa_moca_dml_test.cpp \
                             cosa_moca_network_info_test.cpp \
                             plugin_main_test.cpp

CcspMoCA_gtest_bin_LDFLAGS = -lgtest -lgmock -lgcov -lpthread

CcspMoCA_gtest_bin_LDADD = $(HOME)/usr/local/lib/libmock_ccsp_common.la \
                           $(HOME)/usr/local/lib/libmock_platform_hal.la \
                           $(HOME)/usr/local/lib/libmock_socket.la \
                           $(HOME)/usr/local/lib/libmock_secure_wrapper.la \
                           $(HOME)/usr/local/lib/libmock_safec_lib.la \
                           $(HOME)/usr/local/lib/libmock_psm.la \
                           $(HOME)/usr/local/lib/libmock_syscfg.la \
                           $(HOME)/usr/local/lib/libmock_utopia.la \
                           $(HOME)/usr/local/lib/libmock_telemetry.la \
                           $(HOME)/usr/local/lib/libmock_parodus.la \
                           $(HOME)/usr/local/lib/libmock_rbus.la \
                           $(HOME)/usr/local/lib/libmock_moca_hal.la \
                           $(HOME)/usr/local/lib/libmock_msgpack.la \
                           $(HOME)/usr/local/lib/libmock_webconfigframework.la \
                           $(HOME)/usr/local/lib/libmock_sysevent.la \
                           $(HOME)/usr/local/lib/libmock_base64.la

# Adding Code Coverage Flags
AM_CFLAGS += -fprofile-arcs -ftest-coverage
AM_CXXFLAGS += -fprofile-arcs -ftest-coverage
CcspMoCA_gtest_bin_LDFLAGS += --coverage