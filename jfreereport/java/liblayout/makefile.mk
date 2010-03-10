#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..$/..

PRJNAME=jfreereport
TARGET=liblayout
VERSION=-0.2.9

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE : antsettings.mk

.IF "$(SOLAR_JAVA)" != ""
# --- Files --------------------------------------------------------
.IF "$(L10N_framework)"==""
TARFILE_NAME=$(TARGET)
TARFILE_MD5=79600e696a98ff95c2eba976f7a8dfbb
TARFILE_ROOTDIR=$(TARGET)
PATCH_FILES=$(PRJ)$/patches$/$(TARGET).patch
CONVERTFILES=build.xml

.IF "$(JAVACISGCJ)"=="yes"
JAVA_HOME=
.EXPORT : JAVA_HOME
BUILD_ACTION=$(ANT) -Dlib="../../../class" -Dbuild.label="build-$(RSCREVISION)" -Dbuild.compiler=gcj -f $(ANT_BUILDFILE) jar
.ELSE
BUILD_ACTION=$(ANT) -Dlib="../../../class" -Dbuild.label="build-$(RSCREVISION)" -f $(ANT_BUILDFILE) jar
.ENDIF

.ENDIF # $(SOLAR_JAVA)!= ""
.ENDIF
# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.IF "$(L10N_framework)"==""
.IF "$(SOLAR_JAVA)" != ""
.INCLUDE : tg_ext.mk

ALLTAR : $(CLASSDIR)$/$(TARGET)$(VERSION).jar 
$(CLASSDIR)$/$(TARGET)$(VERSION).jar : $(PACKAGE_DIR)$/$(INSTALL_FLAG_FILE)
    $(COPY) $(PACKAGE_DIR)$/$(TARFILE_ROOTDIR)$/build$/lib$/$(TARGET).jar $(CLASSDIR)$/$(TARGET)$(VERSION).jar
    
.ENDIF
.ENDIF
