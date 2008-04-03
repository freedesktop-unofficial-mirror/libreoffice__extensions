/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PresenterViewFactory.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2008-04-03 16:07:00 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef SDEXT_PRESENTER_VIEW_FACTORY_HXX
#define SDEXT_PRESENTER_VIEW_FACTORY_HXX

#include "PresenterController.hxx"
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/drawing/framework/XResourceFactory.hpp>
#include <com/sun/star/drawing/framework/XView.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <rtl/ref.hxx>
#include <boost/scoped_ptr.hpp>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {

class PresenterPaneContainer;

namespace {
    typedef ::cppu::WeakComponentImplHelper1 <
        css::drawing::framework::XResourceFactory
    > PresenterViewFactoryInterfaceBase;
}


/** Factory of the presenter screen specific views.  The supported set of
    views includes:
        a life view of the current slide,
        a static preview of the next slide,
        the notes of the current slide,
        a tool bar,
        a clock.
*/
class PresenterViewFactory
    : public ::cppu::BaseMutex,
      public PresenterViewFactoryInterfaceBase
{
public:
    static const ::rtl::OUString msCurrentSlidePreviewViewURL;
    static const ::rtl::OUString msNextSlidePreviewViewURL;
    static const ::rtl::OUString msNotesViewURL;
    static const ::rtl::OUString msToolBarViewURL;
    static const ::rtl::OUString msSlideSorterURL;
    static const ::rtl::OUString msClockViewURL;
    static const ::rtl::OUString msHelpViewURL;

    /** Create a new instance of this class and register it as resource
        factory in the drawing framework of the given controller.
        This registration keeps it alive.  When the drawing framework is
        shut down and releases its reference to the factory then the factory
        is destroyed.
    */
    static css::uno::Reference<css::drawing::framework::XResourceFactory> Create (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext,
        const css::uno::Reference<css::frame::XController>& rxController,
        const ::rtl::Reference<PresenterController>& rpPresenterController);
    virtual ~PresenterViewFactory (void);

    static ::rtl::OUString getImplementationName_static (void);
    static css::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_static (void);
    static css::uno::Reference<css::uno::XInterface> Create(
        const css::uno::Reference<css::uno::XComponentContext>& rxContext)
        SAL_THROW((css::uno::Exception));

    virtual void SAL_CALL disposing (void)
        throw (css::uno::RuntimeException);


    // XResourceFactory
    
    virtual css::uno::Reference<css::drawing::framework::XResource>
        SAL_CALL createResource (
            const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL
        releaseResource (
            const css::uno::Reference<css::drawing::framework::XResource>& rxPane)
        throw (css::uno::RuntimeException);

private:
    css::uno::Reference<css::uno::XComponentContext> mxComponentContext;
    css::uno::Reference<css::drawing::framework::XConfigurationController>
        mxConfigurationController;
    css::uno::WeakReference<css::frame::XController> mxControllerWeak;
    ::rtl::Reference<PresenterController> mpPresenterController;

    PresenterViewFactory (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext,
        const css::uno::Reference<css::frame::XController>& rxController,
        const ::rtl::Reference<PresenterController>& rpPresenterController);

    void Register (const css::uno::Reference<css::frame::XController>& rxController);

    css::uno::Reference<css::drawing::framework::XView> CreateSlideShowView(
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId) const;

    css::uno::Reference<css::drawing::framework::XView> CreateSlidePreviewView(
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId) const;

    css::uno::Reference<css::drawing::framework::XView> CreateNotesView(
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId) const;

    css::uno::Reference<css::drawing::framework::XView> CreateSlideSorterView(
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId) const;

    css::uno::Reference<css::drawing::framework::XView> CreateHelpView(
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxViewId) const;

    double GetSlideAspectRatio (void) const;

    void ThrowIfDisposed (void) const throw (::com::sun::star::lang::DisposedException);
};

} }

#endif
