/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WikiPropDialog.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mav $ $Date: 2007-12-14 09:40:43 $
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

import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlModel;
import com.sun.star.awt.XDialog;
import com.sun.star.awt.XThrobber;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

public class WikiPropDialog extends WikiDialog{
 
    private static final String m_sCancelSending = "The transfer has been interrupted. Please check the integrity of the wiki article.";
    WikiEditorImpl m_aWikiEditor;

    private final String sSendMethod = "Send";
    private final String sCancelMethod = "Cancel";
    private final String sHelpMethod = "Help";
    private final String sLoadMethod = "Load";
    private final String sWikiListMethod = "WikiListChange";
    private final String sArticleTextMethod = "ArticleTextChange";
    private final String sAddWikiMethod = "AddWiki";
    
    String[] Methods = {sSendMethod, sCancelMethod, sHelpMethod, sLoadMethod, sWikiListMethod, sArticleTextMethod, sAddWikiMethod};
    
    protected String m_sWikiEngineURL = "";
    protected String m_sWikiTitle = "";
    protected String m_sWikiComment = "";
    protected boolean m_bWikiMinorEdit = false;
    protected boolean m_bWikiShowBrowser = false;
 
    private Thread m_aSendingThread;
    
    /** Creates a new instance of WikiPropDialog */
    public WikiPropDialog(XComponentContext c, String DialogURL, WikiEditorImpl aWikiEditorForThrobber )
    {
        super(c, DialogURL);
        super.setMethods(Methods);

        if ( aWikiEditorForThrobber != null )
        {
            InsertThrobber();
            m_aWikiEditor = aWikiEditorForThrobber;
        }
    }

        
    public void fillWikiList()
    {
        String [] WikiList = m_aSettings.getWikiURLs();
        
        try
        {
            XPropertySet xPS = getPropSet("WikiList");
            xPS.setPropertyValue("StringItemList", WikiList);
            // short [] nSel = new short[1];
            // nSel[0] = (short) m_aSettings.getLastUsedWikiServer();
            // xPS.setPropertyValue("SelectedItems", sel);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        } 
    }
    
    public void fillDocList()
    {
        XPropertySet xPS = getPropSet("ArticleText");
        try
        {
            short [] sel = (short[]) getPropSet("WikiList").getPropertyValue("SelectedItems");
            xPS.setPropertyValue("StringItemList", m_aSettings.getWikiDocList(sel[0], 5));
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }
    
    
    public void setm_sWikiTitle(String sArticle)
    {
        this.m_sWikiTitle = sArticle;
        try
        {
            XPropertySet xPS = getPropSet("ArticleText");
            xPS.setPropertyValue("Text", sArticle);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        } 
    }
    

    public void switchSendButtonIfNecessary()
    {
        XPropertySet xSendButton = getPropSet( "SendButton" );
        if ( xSendButton != null )
        {
            XPropertySet xWikiListProps = getPropSet( "WikiList" );
            XPropertySet xArticleProps = getPropSet( "ArticleText" );
            if ( xWikiListProps != null && xArticleProps != null )
            {
                try
                {
                    short [] pSel = (short[]) getPropSet("WikiList").getPropertyValue("SelectedItems");
                    String sArticle = (String)xArticleProps.getPropertyValue( "Text" );
                    if ( pSel != null && pSel.length > 0 && sArticle != null && sArticle.length() != 0 )
                        xSendButton.setPropertyValue( "Enabled", Boolean.TRUE );
                    else
                        xSendButton.setPropertyValue( "Enabled", Boolean.FALSE );
                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                }
            }
        }
    }


    public boolean callHandlerMethod( XDialog xDialog, Object EventObject, String MethodName )
    {
        if ( MethodName.equals( sSendMethod ) )
        {
            try
            {
                XPropertySet aWikiListProps = getPropSet( "WikiList" );
                XPropertySet aArticleTextProps = getPropSet( "ArticleText" );
                XPropertySet aCommentTextProps = getPropSet( "CommentText" );
                XPropertySet aMinorCheckProps = getPropSet( "MinorCheck" );
                XPropertySet aBrowserCheckProps = getPropSet( "BrowserCheck" );
                XPropertySet aHelpButtonProps = getPropSet( "HelpButton" );
                XPropertySet aSendButtonProps = getPropSet( "SendButton" );
                XPropertySet aAddButtonProps = getPropSet( "AddButton" );

                short [] sel = (short[]) aWikiListProps.getPropertyValue("SelectedItems");
                String [] items = (String []) aWikiListProps.getPropertyValue("StringItemList");
                m_sWikiEngineURL = items[sel[0]];
                m_aSettings.setLastUsedWikiServer(sel[0]);
                m_sWikiTitle = (String) aArticleTextProps.getPropertyValue("Text");
                m_sWikiComment = (String) aCommentTextProps.getPropertyValue("Text");

                short minorState = ((Short) aMinorCheckProps.getPropertyValue("State")).shortValue();
                if (minorState != 0)
                    m_bWikiMinorEdit = true;
                else 
                    m_bWikiMinorEdit = false;
                
                short nBrowserState = ((Short) aBrowserCheckProps.getPropertyValue("State")).shortValue();
                if ( nBrowserState != 0 )
                    m_bWikiShowBrowser = true;
                else
                    m_bWikiShowBrowser = false;

                XPropertySet[] aToDisable = { aWikiListProps, aArticleTextProps, aCommentTextProps, aMinorCheckProps, aBrowserCheckProps, aHelpButtonProps, aSendButtonProps, aAddButtonProps };
                for ( int nInd = 0; nInd < aToDisable.length; nInd++ )
                    aToDisable[nInd].setPropertyValue( "Enabled", Boolean.FALSE );
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
            }

            // TODO: In future the result of storing will be interesting
            // TODO: do not do it in OOo2.2
            final WikiPropDialog aThisDialog = this;
            final XDialog xDialogToClose = xDialog;

            boolean bAllowThreadUsage = false;
            try
            {
                XMultiComponentFactory xFactory = m_xContext.getServiceManager();
                if ( xFactory == null )
                    throw new com.sun.star.uno.RuntimeException();

                Object oCheckCallback = xFactory.createInstanceWithContext( "com.sun.star.awt.AsyncCallback", m_xContext );
                bAllowThreadUsage = ( oCheckCallback != null );
            }
            catch( Exception e )
            {
                e.printStackTrace();
            }

            // start spinning
            SetThrobberActive( true );
            
            if ( bAllowThreadUsage )
            {
                m_aSendingThread = new Thread( "com.sun.star.thread.WikiEditorSendingThread" )
                {
                    public void run()
                    {
                        try
                        {
                            if ( m_aWikiEditor != null )
                            {
                                Thread.yield();
                                m_aWikiEditor.SendArticleImpl( aThisDialog );
                                m_bAction = true;
                            }
                        } catch( java.lang.Exception e )
                        {}
                        finally
                        {
                            xDialogToClose.endExecute();
                            Helper.AllowConnection( true );
                        }
                    }
                };

                m_aSendingThread.start();
            }
            else
            {
                try
                {
                    if ( m_aWikiEditor != null )
                    {
                        m_aWikiEditor.SendArticleImpl( aThisDialog );
                        m_bAction = true;
                    }
                } catch( java.lang.Exception e )
                {}
                finally
                {
                    xDialogToClose.endExecute();
                    Helper.AllowConnection( true );
                }
            }
                       
            return true;
        }
        else if ( MethodName.equals( sLoadMethod ) )
        {
            try
            {
                short [] sel = (short[]) getPropSet("WikiList").getPropertyValue("SelectedItems");
                String [] items = (String []) getPropSet("WikiList").getPropertyValue("StringItemList");
                m_sWikiEngineURL = items[sel[0]];
                m_aSettings.setLastUsedWikiServer(sel[0]);
                m_sWikiTitle = (String) getPropSet("ArticleText").getPropertyValue("Text");
            }
            catch (UnknownPropertyException ex)
            {
                ex.printStackTrace();
            }
            catch (WrappedTargetException ex)
            {
                ex.printStackTrace();
            }
            m_bAction = true;
            xDialog.endExecute();
            return true;
        }
        else if ( MethodName.equals( sCancelMethod ) )
        {
            // disallow any connection till the dialog is closed
            Helper.AllowConnection( false );

            if ( m_aSendingThread == null )
            {
                m_bAction = false;
                xDialog.endExecute();
            }
            else
            {
                Helper.ShowError( m_xContext,
                                  (XWindowPeer)UnoRuntime.queryInterface( XWindowPeer.class, m_xDialog ),
                                  m_sCancelSending );
            }

            return true;
        }
        else if ( MethodName.equals( sHelpMethod ) )
        {
            m_bAction = false;
            //xDialog.endExecute();
            return true;
        }
        else if ( MethodName.equals( sWikiListMethod ) )
        {
            fillDocList();
            switchSendButtonIfNecessary();
            return true;
        }
        else if ( MethodName.equals( sArticleTextMethod ) )
        {
            switchSendButtonIfNecessary();
            return true;
        }
        else if ( MethodName.equals( sAddWikiMethod ) )
        {
            WikiEditSettingDialog xAddDialog = new WikiEditSettingDialog(m_xContext, "vnd.sun.star.script:WikiEditor.EditSetting?location=application");
            if ( xAddDialog.show() )
                fillWikiList();

            return true;
        }

        return false;
    }
    
    private void InsertThrobber()
    {
        try
        {
            XControl xDialogControl = ( XControl ) UnoRuntime.queryInterface( XControl.class, m_xDialog );
            XControlModel xDialogModel = null;
            if ( xDialogControl != null )
                xDialogModel = xDialogControl.getModel();

            XMultiServiceFactory xDialogFactory = ( XMultiServiceFactory ) UnoRuntime.queryInterface( XMultiServiceFactory.class, xDialogModel );
            if ( xDialogFactory != null )
            {
                XControlModel xThrobberModel = (XControlModel)UnoRuntime.queryInterface( XControlModel.class, xDialogFactory.createInstance( "com.sun.star.awt.UnoThrobberControlModel" ) );
                XPropertySet xThrobberProps = (XPropertySet)UnoRuntime.queryInterface( XPropertySet.class, xThrobberModel );
                if ( xThrobberProps != null )
                {
                    xThrobberProps.setPropertyValue( "Name", "WikiThrobber" ); 
                    xThrobberProps.setPropertyValue( "PositionX", new Integer( 242 ) );
                    xThrobberProps.setPropertyValue( "PositionY", new Integer( 42 ) );
                    xThrobberProps.setPropertyValue( "Height", new Integer( 16 ) );
                    xThrobberProps.setPropertyValue( "Width", new Integer( 16 ) );

                    XNameContainer xDialogContainer = (XNameContainer)UnoRuntime.queryInterface( XNameContainer.class, xDialogModel );
                    xDialogContainer.insertByName( "WikiThrobber", xThrobberModel );
                }
            }
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }

    public void SetThrobberActive( boolean bActive )
    {
        if ( m_xControlContainer != null )
        {
            try
            {
                XThrobber xThrobber = (XThrobber)UnoRuntime.queryInterface( XThrobber.class, m_xControlContainer.getControl( "WikiThrobber" ) );
                if ( xThrobber != null )
                {
                    if ( bActive )
                        xThrobber.start();
                    else
                        xThrobber.stop();
                }
            }
            catch( Exception e )
            {
                e.printStackTrace();
            }
        }
    }

}

