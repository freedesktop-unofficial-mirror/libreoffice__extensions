/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: PresenterFrameworkObserver.hxx,v $
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

#ifndef SDEXT_PRESENTER_PRESENTER_FRAMEWORK_OBSERVER_HXX
#define SDEXT_PRESENTER_PRESENTER_FRAMEWORK_OBSERVER_HXX

#include <com/sun/star/drawing/framework/XConfigurationChangeListener.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase1.hxx>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {


typedef ::cppu::WeakComponentImplHelper1 <
    ::com::sun::star::drawing::framework::XConfigurationChangeListener
    > PresenterFrameworkObserverInterfaceBase;

/** Watch the drawing framework for changes and run callbacks when a certain
    change takes place.
*/
class PresenterFrameworkObserver
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public PresenterFrameworkObserverInterfaceBase
{
public:
    typedef ::boost::function<bool(void)> Predicate;
    typedef ::boost::function<void(bool)> Action;

    /** Register an action callback to be run when the specified resource is
        activated.  The action may be called synchronously when the resource
        is already active or asynchronously when the resource is actived in
        the future.
        @param rxController
            The controller gives access to the drawing framework.
        @param rxResourceId
            The id of the resource to watch for activation.
        @param rAction
            The callback that is called when the resource is activated.
    */
    static void RunOnResourceActivation (
        const css::uno::Reference<css::drawing::framework::XConfigurationController>&rxController,
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxResourceId,
        const Action& rAction);
    
    virtual void SAL_CALL disposing (void);
    virtual void SAL_CALL disposing (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);
    virtual void SAL_CALL notifyConfigurationChange (
        const css::drawing::framework::ConfigurationChangeEvent& rEvent)
        throw (css::uno::RuntimeException);

private:
    ::rtl::OUString msEventType;
    ::css::uno::Reference<css::drawing::framework::XConfigurationController> mxConfigurationController;
    Predicate maPredicate;
    Action maAction;

    PresenterFrameworkObserver (
        const css::uno::Reference<css::drawing::framework::XConfigurationController>&rxController,
        const ::rtl::OUString& rsEventName,
        const Predicate& rPredicate,
        const Action& rAction);
    virtual ~PresenterFrameworkObserver (void);

    static bool HasResource (
        const css::uno::Reference<css::drawing::framework::XConfigurationController>&rxController,
        const css::uno::Reference<css::drawing::framework::XResourceId>& rxResourceId);
};


} }  // end of namespace ::sdext::presenter

#endif
