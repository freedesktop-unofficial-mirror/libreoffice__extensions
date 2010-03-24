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

#ifndef SDEXT_PRESENTER_PRESENTER_UI_PAINTER_HXX
#define SDEXT_PRESENTER_PRESENTER_UI_PAINTER_HXX

#include "PresenterTheme.hxx"
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XBitmap.hpp>
#include <boost/noncopyable.hpp>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {


/** Functions for painting UI elements.
*/
class PresenterUIPainter
    : ::boost::noncopyable
{
public:
    PresenterUIPainter (void);
    ~PresenterUIPainter (void);
    
    static void PaintHorizontalBitmapComposite (
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::awt::Rectangle& rBoundingBox,
        const css::uno::Reference<css::rendering::XBitmap>& rxLeftBitmap,
        const css::uno::Reference<css::rendering::XBitmap>& rxRepeatableCenterBitmap,
        const css::uno::Reference<css::rendering::XBitmap>& rxRightBitmap);

    static void PaintVerticalBitmapComposite (
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::awt::Rectangle& rBoundingBox,
        const css::uno::Reference<css::rendering::XBitmap>& rxTopBitmap,
        const css::uno::Reference<css::rendering::XBitmap>& rxRepeatableCenterBitmap,
        const css::uno::Reference<css::rendering::XBitmap>& rxBottomBitmap);
};

} }

#endif
