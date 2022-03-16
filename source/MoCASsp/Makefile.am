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
AM_CFLAGS =

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspMoCA

CcspMoCA_CPPFLAGS = -I$(top_srcdir)/source/TR-181/include -I$(top_srcdir)/source/TR-181/middle_layer_src

CcspMoCA_DEPENDENCIES= \
	$(EXTRA_DEPENDENCIES) \
	${top_builddir}/source/TR-181/middle_layer_src/libCcspMoCA_middle_layer_src.la \
	${top_builddir}/source/TR-181/board_sbapi/libCcspMoCA_board_sbapi.la

CcspMoCA_CFLAGS = $(SYSTEMD_CFLAGS)
CcspMoCA_SOURCES = ssp_messagebus_interface.c ssp_main.c ssp_action.c dm_pack_datamodel.c
CcspMoCA_LDFLAGS = -rdynamic $(SYSTEMD_LDFLAGS) -pthread -lsecure_wrapper
CcspMoCA_LDADD = $(CcspMoCA_DEPENDENCIES) -lsyscfg -lsysevent -lccsp_common -ldl -lmsgpackc -lwebconfig_framework
