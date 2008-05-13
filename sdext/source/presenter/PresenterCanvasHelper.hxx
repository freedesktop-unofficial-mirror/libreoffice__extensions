/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: PresenterCanvasHelper.hxx,v $
 *
 * $Revision: 1.2 $
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

#ifndef SDEXT_PRESENTER_PRESENTER_CANVAS_HELPER_HXX
#define SDEXT_PRESENTER_PRESENTER_CANVAS_HELPER_HXX

#include "PresenterTheme.hxx"
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XCanvasFont.hpp>
#include <com/sun/star/rendering/XPolyPolygon2D.hpp>
#include <rtl/ref.hxx>
#include <boost/noncopyable.hpp>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {

class PresenterController;

/** Collection of functions to ease the life of a canvas user.
*/
class PresenterCanvasHelper
    : ::boost::noncopyable
{
public:
    PresenterCanvasHelper (void);
    ~PresenterCanvasHelper (void);
    
    void Paint (
        const SharedBitmapDescriptor& rpBitmap,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::awt::Rectangle& rBackgroundBoundingBox,
        const css::awt::Rectangle& rContentBoundingBox) const;

    void PaintTexture (
        const css::uno::Reference<css::rendering::XBitmap>& rxTexture,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::uno::Reference<css::rendering::XPolyPolygon2D>& rxPolygon) const;

    void PaintTiledBitmap (
        const css::uno::Reference<css::rendering::XBitmap>& rxTexture,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::uno::Reference<css::rendering::XPolyPolygon2D>& rxPolygon,
        const css::awt::Rectangle& rHole) const;

    void PaintBitmap (
        const css::uno::Reference<css::rendering::XBitmap>& rxBitmap,
        const css::awt::Point& rLocation,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::uno::Reference<css::rendering::XPolyPolygon2D>& rxPolygon) const;

    void PaintColor (
        const css::util::Color nColor,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::awt::Rectangle& rRepaintBox,
        const css::uno::Reference<css::rendering::XPolyPolygon2D>& rxPolygon) const;

    static void SetDeviceColor(
        css::rendering::RenderState& rRenderState,
        const css::util::Color aColor);

    static css::geometry::RealSize2D GetTextSize (
        const css::uno::Reference<css::rendering::XCanvasFont>& rxFont,
        const ::rtl::OUString& rsText);

private:
    const css::rendering::ViewState maDefaultViewState;
    const css::rendering::RenderState maDefaultRenderState;
};

} }

#endif
