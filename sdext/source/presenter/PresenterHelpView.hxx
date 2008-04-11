/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: PresenterHelpView.hxx,v $
 *
 * $Revision: 1.3 $
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

#ifndef SDEXT_PRESENTER_HELP_VIEW_HXX
#define SDEXT_PRESENTER_HELP_VIEW_HXX

#include "PresenterController.hxx"
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase3.hxx>
#include <com/sun/star/awt/XPaintListener.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/drawing/framework/XView.hpp>
#include <com/sun/star/drawing/framework/XResourceId.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/rendering/XSpriteCanvas.hpp>

namespace css = ::com::sun::star;

namespace {
    typedef cppu::WeakComponentImplHelper3< 
        css::drawing::framework::XView,
        css::awt::XWindowListener,
        css::awt::XPaintListener
        > PresenterHelpViewInterfaceBase;
}

namespace sdext { namespace presenter {

/** Experimental.  Do not use (yet).
*/
class PresenterHelpView
    : private ::cppu::BaseMutex,
      public PresenterHelpViewInterfaceBase
{
public:
    explicit PresenterHelpView (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext,
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId,
        const css::uno::Reference<css::frame::XController>& rxController,
        const ::rtl::Reference<PresenterController>& rpPresenterController);
    virtual ~PresenterHelpView (void);

    virtual void SAL_CALL disposing (void);

    // lang::XEventListener

    virtual void SAL_CALL 
        disposing (const css::lang::EventObject& rEventObject)
        throw (css::uno::RuntimeException);


    // XWindowListener
    
    virtual void SAL_CALL windowResized (const css::awt::WindowEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowMoved (const css::awt::WindowEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowShown (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowHidden (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);


    // XPaintListener

    virtual void SAL_CALL windowPaint (const css::awt::PaintEvent& rEvent)
        throw (css::uno::RuntimeException);


    // XResourceId

    virtual css::uno::Reference<css::drawing::framework::XResourceId> SAL_CALL getResourceId (void)
        throw (css::uno::RuntimeException);

    virtual sal_Bool SAL_CALL isAnchorOnly (void)
        throw (com::sun::star::uno::RuntimeException);

private:
    css::uno::Reference<css::uno::XComponentContext> mxComponentContext;
    css::uno::Reference<css::drawing::framework::XResourceId> mxViewId;
    css::uno::Reference<css::drawing::framework::XPane> mxPane;
    css::uno::Reference<css::awt::XWindow> mxWindow;
    ::rtl::Reference<PresenterController> mpPresenterController;
    css::uno::Reference<css::rendering::XSpriteCanvas> mxCanvas;
    css::uno::Reference<css::rendering::XCanvasFont> mxFont;

    void ProvideCanvas (void);
    void Resize (void);
    void Paint (const css::awt::Rectangle& rRedrawArea);
    
    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void)
        throw (css::lang::DisposedException);
};

} } // end of namespace ::sdext::presenter

#endif
