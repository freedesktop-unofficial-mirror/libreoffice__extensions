/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WikiOptionsEventHandlerImpl.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mav $ $Date: 2008-01-30 19:02:16 $
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

package com.sun.star.wiki;

import com.sun.star.awt.XContainerWindowEventHandler;
import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlContainer;
import com.sun.star.awt.XDialog;
import com.sun.star.awt.XDialogEventHandler;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lib.uno.helper.WeakBase;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import java.util.Hashtable;

public final class WikiOptionsEventHandlerImpl extends WeakBase
    implements XServiceInfo, XContainerWindowEventHandler, XDialogEventHandler
{
    static final String[] m_pServiceNames = { "com.sun.star.wiki.WikiOptionsEventHandler" };
    static final String m_sImplementationName = WikiOptionsEventHandlerImpl.class.getName();

    static final String sExternalEvent = "external_event";
    static final String sAdd = "Add";
    static final String sEdit = "Edit";
    static final String sRemove = "Remove";
    static final String sListStatus = "ListStatus";
    static final String sListEdit = "ListEdit";
    static final String sInitialize = "initialize";
    static final String sOk = "ok";
    static final String sBack = "back";

    private XComponentContext m_xContext;
    private XDialog m_xDialog;
    private XControlContainer m_xControlContainer;

    Settings m_aSettings;
    
    public WikiOptionsEventHandlerImpl( XComponentContext xContext )
    {
        m_xContext = xContext;
    }

    protected XPropertySet GetPropSet( String sControl )
    {
        if ( m_xControlContainer != null )
        {
            XControl xControl = m_xControlContainer.getControl(sControl);
            XPropertySet xListProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xControl.getModel() );
            return xListProps;
        }

        return null;
    }

    private void RefreshView()
    {
        if ( m_aSettings != null )
        {
            String[] pWikiList = m_aSettings.getWikiURLs();
            XPropertySet xListProps = GetPropSet( "WikiList" );
            if ( xListProps != null )
            {
                try
                {
                    xListProps.setPropertyValue( "StringItemList", pWikiList );
                }
                catch ( Exception ex )
                {
                    ex.printStackTrace();
                } 
            }
        }
    }

    private void CheckButtonState()
    {
        XPropertySet xListProps = GetPropSet( "WikiList" );
        if ( xListProps != null )
        {
            try
            {
                short [] pSel = (short []) xListProps.getPropertyValue( "SelectedItems" );
                XPropertySet xEditProps = GetPropSet( "EditButton" );
                XPropertySet xRemoveProps = GetPropSet( "RemoveButton" );
                Boolean bState = new Boolean( pSel.length != 0 );

                xEditProps.setPropertyValue( "Enabled", bState );
                xRemoveProps.setPropertyValue( "Enabled", bState );
            }
            catch ( Exception ex )
            {
                ex.printStackTrace();
            }
        }
    }

    private void AddSetting()
    {
        WikiEditSettingDialog aSettingDialog = new WikiEditSettingDialog( m_xContext, "vnd.sun.star.script:WikiEditor.EditSetting?location=application" );
        if ( aSettingDialog.show() )
            RefreshView();
    }

    private void EditSetting()
    {
        XPropertySet xListProps = GetPropSet( "WikiList" );
        if ( xListProps != null )
        {
            Hashtable ht = null;
            try
            {
                short[] pSel = (short []) xListProps.getPropertyValue( "SelectedItems" );
                String[] pItems = (String []) xListProps.getPropertyValue("StringItemList");
                if ( pSel.length > 0 && pItems.length > pSel[0] )
                {
                    String selName = pItems[pSel[0]];
                    ht = m_aSettings.getSettingByUrl( pItems[pSel[0]] );
                }
            }
            catch ( Exception ex )
            {
                ex.printStackTrace();
            }

            WikiEditSettingDialog aSettingDialog = new WikiEditSettingDialog(m_xContext, "vnd.sun.star.script:WikiEditor.EditSetting?location=application", ht, true );
            if ( aSettingDialog.show() )
                RefreshView();
        }
    }

    private void RemoveSetting()
    {
        XPropertySet xListProps = GetPropSet("WikiList");
        if ( xListProps != null )
        {
            try 
            {
                short[] pSel = (short []) xListProps.getPropertyValue("SelectedItems");
                String[] pItems = (String []) GetPropSet("WikiList").getPropertyValue("StringItemList");
                if ( pSel.length > 0 && pItems.length > pSel[0] )
                {
                    m_aSettings.removeSettingByUrl( pItems[pSel[0]] );
                    RefreshView();
                }
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
            }
        }
    }

    private void InitStrings()
    {
        try
        {
            GetPropSet( "AddButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( m_xContext, Helper.DLG_ADDBUTTON ) );
            GetPropSet( "EditButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( m_xContext, Helper.DLG_SENDBUTTON ) );
            GetPropSet( "RemoveButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( m_xContext, Helper.DLG_REMOVEBUTTON ) );
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }

    // com.sun.star.lang.XServiceInfo:
    public String getImplementationName() 
    {
         return m_sImplementationName;
    }

    public boolean supportsService( String sService ) 
    {
        int len = m_pServiceNames.length;

        for( int i=0; i < len; i++ ) 
        {
            if ( sService.equals( m_pServiceNames[i] ))
                return true;
        }
        return false;
    }

    public String[] getSupportedServiceNames() 
    {
        return m_pServiceNames;
    }

    // XContainerWindowEventHandler
    public boolean callHandlerMethod( XWindow xWindow, Object aEventObject, String sMethod )
        throws WrappedTargetException, com.sun.star.uno.RuntimeException
    {
        if ( sMethod.equals( sExternalEvent ) )
        {
            try
            {
                String sEvent = (String)AnyConverter.toString( aEventObject );
                if ( sEvent != null )
                {
                    if ( sEvent.equals( sOk ) )
                    {
                        if ( m_aSettings != null )
                            m_aSettings.storeConfiguration();
                    }
                    else if ( sEvent.equals( sInitialize ) || sEvent.equals( sBack ) )
                    {
                        if ( sEvent.equals( sInitialize ) )
                        {
                            m_xDialog = (XDialog)UnoRuntime.queryInterface( XDialog.class, xWindow );
                            m_xControlContainer = (XControlContainer)UnoRuntime.queryInterface(
                                                            XControlContainer.class, m_xDialog );
                            m_aSettings = Settings.getSettings( m_xContext );
                            InitStrings();
                        }
                        else if ( m_aSettings != null )
                            m_aSettings.loadConfiguration(); // throw away all the changes
                            
                        RefreshView();
                        CheckButtonState();
                    }
                }
            }
            catch ( com.sun.star.uno.RuntimeException r )
            {
                throw r;
            }
            catch ( com.sun.star.uno.Exception e )
            {
                throw new WrappedTargetException( sMethod, this, e );
            }
        }

        return true;
    }

    public boolean callHandlerMethod( XDialog xDialog, Object aEventObject, String sMethod )
        throws WrappedTargetException, com.sun.star.uno.RuntimeException
    {
        if ( m_xDialog != null && xDialog == m_xDialog )
        {
            if ( sMethod.equals( sAdd ) )
            {
                AddSetting();
            }
            else if ( sMethod.equals( sEdit ) || sMethod.equals( sListEdit ) )
            {
                EditSetting();
            }
            else if ( sMethod.equals( sRemove ) )
            {
                RemoveSetting();
                CheckButtonState();
            }
            else if ( sMethod.equals( sListStatus ) )
            {
                CheckButtonState();
            }
        }

        return true;
    }

    public String[] getSupportedMethodNames()
    {
        return new String[] { sExternalEvent, sAdd, sEdit, sRemove };
    }
};

