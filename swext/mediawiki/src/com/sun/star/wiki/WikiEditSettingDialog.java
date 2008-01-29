/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WikiEditSettingDialog.java,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: mav $ $Date: 2008-01-29 11:01:28 $
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

import com.sun.star.awt.XDialog;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.beans.XPropertySet;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import java.util.Hashtable;
import javax.net.ssl.SSLException;

import org.apache.commons.httpclient.*;
import org.apache.commons.httpclient.methods.*;

public class WikiEditSettingDialog extends WikiDialog 
{
    
    private final String sOKMethod = "OK";
    private final String sCancelMethod = "Cancel";
    private final String sHelpMethod = "Help";

    private static final String m_sNoConnectionToURL1 = "A connection to the MediaWiki system at '";
    private static final String m_sNoConnectionToURL2 = "' could not be created.";
    private static final String m_sWrongLogin = "User name or password is incorrect. Please try again, or leave the fields blank for an anonymous connection.";
    private static final String m_sInvalidURL = "A connection could not be created, because the URL is invalid.";
    private static final String m_sNoURL = "Specify a MediaWiki server by providing a URL.";
    
    String[] Methods = 
    {sOKMethod, sCancelMethod, sHelpMethod};
    private Hashtable setting;
    private boolean addMode;
    
    public WikiEditSettingDialog( XComponentContext xContext, String DialogURL ) 
    {
        super( xContext, DialogURL );
        super.setMethods( Methods );
        setting = new Hashtable();
        addMode = true;

        InitStrings( xContext );
        InitSaveCheckbox( xContext );        
    }
    
    public WikiEditSettingDialog( XComponentContext xContext, String DialogURL, Hashtable ht ) 
    {
        super( xContext, DialogURL );
        super.setMethods( Methods );
        setting = ht;
        try 
        {
            GetPropSet( "UrlField" ).setPropertyValue( "Text", ht.get( "Url" ));
            GetPropSet( "UsernameField" ).setPropertyValue( "Text", ht.get( "Username" ));
            GetPropSet( "PasswordField" ).setPropertyValue( "Text", ht.get( "Password" ));
        }
        catch ( Exception ex ) 
        {
            ex.printStackTrace();
        } 
        addMode = false;        
 
        InitStrings( xContext );
        InitSaveCheckbox( xContext );
    }

    private void InitStrings( XComponentContext xContext )
    {
        try
        {
            SetTitle( Helper.GetLocalizedString( xContext, Helper.DLG_MEDIAWIKI_TITLE ) );
            GetPropSet( "UrlLabel" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_URLLABEL ) );
            GetPropSet( "UsernameLabel" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_USERNAMELABEL ) );
            GetPropSet( "PasswordLabel" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_PASSWORDLABEL ) );
            GetPropSet( "AccountLine" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_ACCOUNTLINE ) );
            GetPropSet( "WikiLine" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_WIKILINE ) );
            GetPropSet( "SaveBox" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_EDITSETTING_SAVEBOX ) );
            GetPropSet( "OkButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_OK ) );
            GetPropSet( "HelpButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_HELP ) );
            GetPropSet( "CancelButton" ).setPropertyValue( "Label", Helper.GetLocalizedString( xContext, Helper.DLG_CANCEL ) );
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }

    private void InitSaveCheckbox( XComponentContext xContext )
    {
        XPropertySet xSaveCheck = GetPropSet( "SaveBox" );
        try
        {
            xSaveCheck.setPropertyValue( "State", new Short( (short)0 ) );
            xSaveCheck.setPropertyValue( "Enabled", new Boolean( Helper.PasswordStoringIsAllowed( xContext ) ) );
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }        
    }
    
    public boolean callHandlerMethod( XDialog xDialog, Object EventObject, String MethodName )
    {
        if ( MethodName.equals( sOKMethod ) )
        {
            String sRedirectURL = "";
            String sURL = "";
            try
            {
                sURL = ( String ) GetPropSet( "UrlField" ).getPropertyValue( "Text" );
                String sUserName = ( String ) GetPropSet( "UsernameField" ).getPropertyValue( "Text" );
                String sPassword = ( String ) GetPropSet( "PasswordField" ).getPropertyValue( "Text" );

                HostConfiguration aHostConfig = new HostConfiguration();
                boolean bInitHost = true;

                do
                {
                    if ( sRedirectURL.length() > 0 )
                    {
                        sURL = sRedirectURL;
                        sRedirectURL = "";
                    }

                    if ( sURL.length() > 0 )
                    {
                        URI aURI = new URI( sURL );
                        GetMethod aRequest = new GetMethod( aURI.getEscapedPathQuery() );
                        aRequest.setFollowRedirects( false );
                        Helper.ExecuteMethod( aRequest, aHostConfig, aURI, m_xContext, bInitHost );
                        bInitHost = false;

                        int nResultCode = aRequest.getStatusCode();
                        String sWebPage = null;
                        if ( nResultCode == 200 )
                            sWebPage = aRequest.getResponseBodyAsString();
                        else if ( nResultCode >= 301 && nResultCode <= 303 || nResultCode == 307 )
                            sRedirectURL = aRequest.getResponseHeader( "Location" ).getValue();

                        aRequest.releaseConnection();
                        
                        if ( sWebPage != null && sWebPage.length() > 0 )
                        {
                            //the URL is valid
                            String sMainURL = Helper.GetMainURL( sWebPage, sURL );
                            
                            if ( sMainURL.equals( "" ) )
                            {
                                // TODO:
                                // it's not a Wiki Page, check first whether a redirect is requested
                                // happens usually in case of https
                                sRedirectURL = Helper.GetRedirectURL( sWebPage, sURL );
                                if ( sRedirectURL.equals( "" ) )
                                {
                                    // show error
                                    Helper.ShowError( m_xContext,
                                                      m_xDialog,
                                                      Helper.DLG_MEDIAWIKI_TITLE,
                                                      Helper.NOURLCONNECTION_ERROR,
                                                      sURL,
                                                      false );
                                }
                            }
                            else
                            {
                                if ( ( sUserName.length() > 0 || sPassword.length() > 0 )
                                  && Helper.Login( new URI( sMainURL ), sUserName, sPassword, m_xContext ) == null )
                                {
                                    // a wrong login information is provided
                                    // show error
                                    Helper.ShowError( m_xContext,
                                                      m_xDialog,
                                                      Helper.DLG_MEDIAWIKI_TITLE,
                                                      Helper.WRONGLOGIN_ERROR,
                                                      null,
                                                      false );
                                }
                                else
                                {
                                    setting.put( "Url",sMainURL );
                                    setting.put( "Username", sUserName );
                                    setting.put( "Password", sPassword );
                                    if ( addMode )
                                        Settings.getSettings( m_xContext ).addWikiCon( setting );
                                    
                                    if ( Helper.PasswordStoringIsAllowed( m_xContext )
                                      && ( (Short)( GetPropSet( "SaveBox" ).getPropertyValue("State") ) ).shortValue() != (short)0 )
                                    {
                                        String[] pPasswords = { sPassword };
                                        try
                                        {
                                            Helper.GetPasswordContainer( m_xContext ).addPersistent( sMainURL, sUserName, pPasswords, Helper.GetInteractionHandler( m_xContext ) );
                                        }
                                        catch( Exception e )
                                        {
                                            e.printStackTrace();
                                        }
                                    }

                                    m_bAction = true;
                                    xDialog.endExecute();
                                }
                            }
                        }
                        else if ( sRedirectURL == null || sRedirectURL.length() == 0 )
                        {
                            // URL invalid
                            // show error
                            Helper.ShowError( m_xContext,
                                              m_xDialog,
                                              Helper.DLG_MEDIAWIKI_TITLE,
                                              Helper.INVALIDURL_ERROR,
                                              null,
                                              false );
                        }
                    }
                    else
                    {
                        // URL field empty
                        // show error
                        Helper.ShowError( m_xContext,
                                          m_xDialog,
                                          Helper.DLG_MEDIAWIKI_TITLE,
                                          Helper.NOURL_ERROR,
                                          null,
                                          false );
                    }
                } while ( sRedirectURL.length() > 0 );
            }
            catch ( SSLException essl )
            {
                Helper.ShowError( m_xContext,
                                  m_xDialog,
                                  Helper.DLG_MEDIAWIKI_TITLE,
                                  Helper.UNKNOWNCERT_ERROR,
                                  null,
                                  false );
                essl.printStackTrace();
            }
            catch ( Exception ex ) 
            {
                Helper.ShowError( m_xContext,
                                  m_xDialog,
                                  Helper.DLG_MEDIAWIKI_TITLE,
                                  Helper.NOURLCONNECTION_ERROR,
                                  sURL,
                                  false );
                ex.printStackTrace();
            } 
            return true;
        }
        else if ( MethodName.equals( sCancelMethod ) ) 
        {
            xDialog.endExecute();
            return true;
        }
        else if ( MethodName.equals( sHelpMethod ) ) 
        {
            return true;
        }

        return false;
    }
    
    
}
