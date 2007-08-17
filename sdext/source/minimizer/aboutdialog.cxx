 /*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: aboutdialog.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: sj $ $Date: 2007-08-17 15:10:57 $
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

#ifndef ABOUTDIALOG_HXX
#include "aboutdialog.hxx"
#endif
#ifndef OPTIMIZATIONSTATS_HXX
#include "optimizationstats.hxx"
#endif
#ifndef _FILEOPEN_DIALOG_HXX_
#include "fileopendialog.hxx"
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif

// ---------------
// - ABOUTDIALOG -
// ---------------

using namespace ::rtl;
using namespace ::com::sun::star::ui;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::container;

#define ABOUT_DIALOG_WIDTH  200
#define ABOUT_DIALOG_HEIGHT 221

// -----------------------------------------------------------------------------


rtl::OUString InsertFixedText( AboutDialog& rAboutDialog, const rtl::OUString& rControlName, const OUString& rLabel,
                                sal_Int32 nXPos, sal_Int32 nYPos, sal_Int32 nWidth, sal_Int32 nHeight, sal_Bool bMultiLine, sal_Int16 nTabIndex )
{
    OUString pNames[] = {
        TKGet( TK_Height ),
        TKGet( TK_Label ),
        TKGet( TK_MultiLine ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_Step ),
        TKGet( TK_TabIndex ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( nHeight ),
        Any( rLabel ),
        Any( bMultiLine ),
        Any( nXPos ),
        Any( nYPos ),
        Any( (sal_Int16)0 ),
        Any( nTabIndex ),
        Any( nWidth ) };

    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rAboutDialog.insertFixedText( rControlName, aNames, aValues );
    return rControlName;
}

rtl::OUString InsertSeparator( AboutDialog& rAboutDialog, const OUString& rControlName, sal_Int32 nOrientation,
                        sal_Int32 nPosX, sal_Int32 nPosY, sal_Int32 nWidth, sal_Int32 nHeight )
{
    OUString pNames[] = {
        TKGet( TK_Height ),
        TKGet( TK_Orientation ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_Step ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( nHeight ),
        Any( nOrientation ),
        Any( nPosX ),
        Any( nPosY ),
        Any( sal_Int16( 0 ) ),
        Any( nWidth ) };

    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rAboutDialog.insertControlModel( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlFixedLineModel" ) ),
        rControlName, aNames, aValues );
    return rControlName;
}

rtl::OUString InsertImage( AboutDialog& rAboutDialog, const OUString& rControlName, const OUString& rURL,
                        sal_Int32 nPosX, sal_Int32 nPosY, sal_Int32 nWidth, sal_Int32 nHeight )
{
    OUString pNames[] = {
        TKGet( TK_Border ),
        TKGet( TK_Height ),
        TKGet( TK_ImageURL ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_ScaleImage ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( sal_Int16( 1 ) ),
        Any( nHeight ),
        Any( rURL ),
        Any( nPosX ),
        Any( nPosY ),
        Any( sal_False ),
        Any( nWidth ) };
    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rAboutDialog.insertImage( rControlName, aNames, aValues );
    return rControlName;
}

rtl::OUString InsertButton( AboutDialog& rAboutDialog, const OUString& rControlName, Reference< XActionListener >& xActionListener,
    sal_Int32 nXPos, sal_Int32 nYPos, sal_Int32 nWidth, sal_Int32 nHeight, sal_Int16 nTabIndex, PPPOptimizerTokenEnum nResID )
{
    OUString pNames[] = {
        TKGet( TK_Enabled ),
        TKGet( TK_Height ),
        TKGet( TK_Label ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_PushButtonType ),
        TKGet( TK_Step ),
        TKGet( TK_TabIndex ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( sal_True ),
        Any( nHeight ),
        Any( rAboutDialog.getString( nResID ) ),
        Any( nXPos ),
        Any( nYPos ),
        Any( static_cast< sal_Int16 >( PushButtonType_OK ) ),
        Any( (sal_Int16)0 ),
        Any( nTabIndex ),
        Any( nWidth ) };


    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rAboutDialog.insertButton( rControlName, xActionListener, aNames, aValues );
    return rControlName;
}

void AboutDialog::InitDialog()
{
   // setting the dialog properties
    OUString pNames[] = {
        TKGet( TK_Closeable ),
        TKGet( TK_Height ),
        TKGet( TK_Moveable ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_Title ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( sal_True ),
        Any( sal_Int32( ABOUT_DIALOG_HEIGHT ) ),
        Any( sal_True ),
        Any( sal_Int32( 113 ) ),
        Any( sal_Int32( 42 ) ),
        Any( getString( STR_ABOUT_VERSION ) ),
        Any( sal_Int32( ABOUT_DIALOG_WIDTH ) ) };
    
    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rtl::OUString sBitmapPath( getPath( TK_BitmapPath ) );
    rtl::OUString sBitmap( rtl::OUString::createFromAscii( "/aboutlogo.png" ) );
    rtl::OUString sURL( sBitmapPath += sBitmap );

    mxDialogModelMultiPropertySet->setPropertyValues( aNames, aValues ); 
    sal_Int32 nWidth = getMapsFromPixels( 387 );
    if ( nWidth )
        mxDialogModelPropertySet->setPropertyValue( TKGet( TK_Width ), Any( nWidth ) );
    else
        nWidth = ABOUT_DIALOG_WIDTH;

//    int nHeight = (int) getMapsFromPixels( 95 );

    InsertImage( *this, rtl::OUString( rtl::OUString::createFromAscii( "aboutimage" ) ), sURL, 0, 0, nWidth, 60 );
    InsertFixedText( *this, rtl::OUString( rtl::OUString::createFromAscii( "fixedtext" ) ), getString( STR_ABOUT_PRN ), 9, 66, nWidth - 18, 127, sal_True, 0 );
    InsertSeparator( *this, rtl::OUString( rtl::OUString::createFromAscii( "separator" ) ), 0, 0, 196, nWidth, 8 );
    InsertButton( *this, rtl::OUString( rtl::OUString::createFromAscii( "button" ) ), mxActionListener, ( nWidth / 2 ) - 25, 204, 50, 14, 1, STR_OK );
}

// -----------------------------------------------------------------------------

AboutDialog::AboutDialog( const Reference< XComponentContext > &rxMSF, Reference< XFrame >& rxFrame ) :
    UnoDialog( rxMSF, rxFrame ),
    ConfigurationAccess( rxMSF, NULL ),
    mxMSF( rxMSF ),
    mxFrame( rxFrame ),
    mxActionListener( new AboutActionListener( *this ) )
{
    Reference< XFrame > xFrame( mxController->getFrame() );
    Reference< XWindow > xContainerWindow( xFrame->getContainerWindow() );
    Reference< XWindowPeer > xWindowPeer( xContainerWindow, UNO_QUERY_THROW );
    createWindowPeer( xWindowPeer );		

    InitDialog();
}

// -----------------------------------------------------------------------------

AboutDialog::~AboutDialog()
{
}

// -----------------------------------------------------------------------------

sal_Bool AboutDialog::execute()
{
    UnoDialog::execute();
    return mbStatus;
}

// -----------------------------------------------------------------------------

void AboutActionListener::actionPerformed( const ActionEvent& rEvent )
    throw ( com::sun::star::uno::RuntimeException )
{
    if ( rEvent.ActionCommand == rtl::OUString( rtl::OUString::createFromAscii( "button" ) ) )
    {
        mrAboutDialog.endExecute( sal_True );
    }
}
void AboutActionListener::disposing( const ::com::sun::star::lang::EventObject& /* Source */ )
    throw ( com::sun::star::uno::RuntimeException )
{
}
