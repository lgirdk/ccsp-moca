##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
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
#                                              -*- Autoconf -*-
# Process this file with autoconf to produce configure script.
#

AC_PREREQ([2.65])
AC_INIT([tr181], [1.0], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_PREFIX_DEFAULT(`pwd`)
AC_ENABLE_SHARED
AC_DISABLE_STATIC

AC_ARG_ENABLE([unitTestDockerSupport],
             AS_HELP_STRING([--enable-unitTestDockerSupport],[enable docker support for unit testing (default is no)]),
             [
              UNIT_TEST_DOCKER_SUPPORT=""
               case "${enableval}" in
                yes) UNIT_TEST_DOCKER_SUPPORT=true
                     m4_if(m4_sysval,[0],[AC_CONFIG_FILES([source/test/Makefile])]);;
                no) UNIT_TEST_DOCKER_SUPPORT=false;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-unitTestDockerSupport ]);;
               esac
             ],
             [echo "Docker support is disabled"])
AM_CONDITIONAL([UNIT_TEST_DOCKER_SUPPORT], [test x$UNIT_TEST_DOCKER_SUPPORT = xtrue])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_LIBTOOL(libtool)

SYSTEMD_CFLAGS=" "
SYSTEMD_LDFLAGS=" "
AC_ARG_ENABLE([notify],
	AS_HELP_STRING([--enable-notify],[enable systemd notify]),
	[
	  case "${enableval}" in
  	   yes) SYSTEMD_CFLAGS="-DENABLE_SD_NOTIFY"
                SYSTEMD_LDFLAGS="-lsystemd" ;;
           no) AC_MSG_ERROR([systemd notify is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-notify ]) ;;
           esac
           ],
	 [echo "headless build is enable"])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_SUBST(SYSTEMD_CFLAGS)
AC_SUBST(SYSTEMD_LDFLAGS)

AC_CONFIG_FILES(
	source/TR-181/board_sbapi/Makefile
	source/TR-181/middle_layer_src/Makefile
	source/TR-181/Makefile
	source/Makefile
	Makefile
	source/MoCASsp/Makefile
	source/MRD/Makefile
)

AC_SUBST(UNIT_TEST_DOCKER_SUPPORT)
AC_OUTPUT

