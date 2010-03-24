/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package com.sun.star.report;

import java.io.InputStream;
import java.awt.Dimension;

/**
 *
 * @author oj93728
 */
public interface ImageService
{

    /**
     * @return the mime-type of the image as string.
     */
    String getMimeType(final InputStream image) throws ReportExecutionException;

    /**
     * @return the mime-type of the image as string.
     */
    String getMimeType(final byte[] image) throws ReportExecutionException;

    /**
     * @returns the dimension in 100th mm.
     **/
    Dimension getImageSize(final InputStream image) throws ReportExecutionException;

    /**
     * @returns the dimension in 100th mm.
     **/
    Dimension getImageSize(final byte[] image) throws ReportExecutionException;
}

