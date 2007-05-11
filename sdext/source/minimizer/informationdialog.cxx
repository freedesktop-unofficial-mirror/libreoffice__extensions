 /*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: informationdialog.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sj $ $Date: 2007-05-11 13:56:24 $
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

#ifndef INFORMATIONDIALOG_HXX
#include "informationdialog.hxx"
#endif
#ifndef OPTIMIZATIONSTATS_HXX
#include "optimizationstats.hxx"
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#define DIALOG_WIDTH	240
#define DIALOG_HEIGHT	84
#define PAGE_POS_X		35
#define PAGE_WIDTH		( DIALOG_WIDTH - PAGE_POS_X ) - 6


// ---------------------
// - INFORMATIONDIALOG -
// ---------------------

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



// -----------------------------------------------------------------------------

rtl::OUString InsertFixedText( InformationDialog& rInformationDialog, const rtl::OUString& rControlName, const OUString& rLabel,
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

    rInformationDialog.insertFixedText( rControlName, aNames, aValues );
    return rControlName;
}

rtl::OUString InsertImage( InformationDialog& rInformationDialog, const OUString& rControlName, const OUString& rURL,
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
        Any( sal_True ),
        Any( nWidth ) };
    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rInformationDialog.insertImage( rControlName, aNames, aValues );
    return rControlName;
}

rtl::OUString InsertCheckBox( InformationDialog& rInformationDialog, const OUString& rControlName,
    const Reference< XItemListener > xItemListener, const OUString& rLabel,
        sal_Int32 nXPos, sal_Int32 nYPos, sal_Int32 nWidth, sal_Int32 nHeight, sal_Int16 nTabIndex )
{
    OUString pNames[] = {
        TKGet( TK_Enabled ),
        TKGet( TK_Height ),
        TKGet( TK_Label ),
        TKGet( TK_PositionX ),
        TKGet( TK_PositionY ),
        TKGet( TK_Step ),
        TKGet( TK_TabIndex ),
        TKGet( TK_Width ) };

    Any	pValues[] = {
        Any( sal_True ),
        Any( nHeight ),
        Any( rLabel ),
        Any( nXPos ),
        Any( nYPos ),
        Any( (sal_Int16)0 ),
        Any( nTabIndex ),
        Any( nWidth ) };

    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    Reference< XCheckBox > xCheckBox( rInformationDialog.insertCheckBox( rControlName, aNames, aValues ) );
    if ( xItemListener.is() )
        xCheckBox->addItemListener( xItemListener );
    return rControlName;
}

rtl::OUString InsertButton( InformationDialog& rInformationDialog, const OUString& rControlName, Reference< XActionListener >& xActionListener,
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
        Any( rInformationDialog.getString( nResID ) ),
        Any( nXPos ),
        Any( nYPos ),
        Any( static_cast< sal_Int16 >( PushButtonType_OK ) ),
        Any( (sal_Int16)0 ),
        Any( nTabIndex ),
        Any( nWidth ) };


    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rInformationDialog.insertButton( rControlName, xActionListener, aNames, aValues );
    return rControlName;
}


static OUString ImpValueOfInMB( const sal_Int64& rVal )
{
    double fVal( static_cast<double>( rVal ) );
    fVal /= ( 1 << 20 );
    fVal += 0.05;
    rtl::OUStringBuffer aVal( OUString::valueOf( fVal ) );
    sal_Int32 nX( OUString( aVal.getStr() ).indexOf( '.', 0 ) );
    if ( nX > 0 )
        aVal.setLength( nX + 2 );
    return aVal.makeStringAndClear();
}

void InformationDialog::InitDialog()
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
        Any( sal_Int32( DIALOG_HEIGHT ) ),
        Any( sal_True ),
        Any( sal_Int32( 113 ) ),
        Any( sal_Int32( 42 ) ),
        Any( getString( STR_ABOUT ) ),
        Any( sal_Int32( DIALOG_WIDTH ) ) };
    
    sal_Int32 nCount = sizeof( pNames ) / sizeof( OUString );

    Sequence< rtl::OUString >	aNames( pNames, nCount );
    Sequence< Any >				aValues( pValues, nCount );

    rtl::OUString sBitmapPath( getPath( TK_BitmapPath ) );
    rtl::OUString sBitmap( rtl::OUString::createFromAscii( "/aboutlogo.png" ) );
    rtl::OUString sURL( sBitmapPath += sBitmap );

    sal_Bool bOpenNewDocument = mrbOpenNewDocument;
    setControlProperty( TKGet( TK_OpenNewDocument ), TKGet( TK_State ), Any( (sal_Int16)bOpenNewDocument ) );

    mxDialogModelMultiPropertySet->setPropertyValues( aNames, aValues ); 


    OUString aInfoString( getString( STR_INFO_1 ) );
    const OUString aOldSizePlaceholder( RTL_CONSTASCII_USTRINGPARAM( "%OLDFILESIZE" ) );
    const OUString aNewSizePlaceholder( RTL_CONSTASCII_USTRINGPARAM( "%NEWFILESIZE" ) );
    sal_Int32 i = aInfoString.indexOf( aOldSizePlaceholder, 0 );
    if ( i >= 0 )
        aInfoString = aInfoString.replaceAt( i, aOldSizePlaceholder.getLength(), ImpValueOfInMB( mnSourceSize ) );
    
    sal_Int32 j = aInfoString.indexOf( aNewSizePlaceholder, 0 );
    if ( j >= 0 )
        aInfoString = aInfoString.replaceAt( j, aNewSizePlaceholder.getLength(), ImpValueOfInMB( mnDestSize ) );

    com::sun::star::uno::Reference< com::sun::star::awt::XItemListener > xItemListener;
    InsertImage( *this, rtl::OUString( rtl::OUString::createFromAscii( "aboutimage" ) ), sURL, 5, 5, 25, 25 );
    InsertFixedText( *this, rtl::OUString( rtl::OUString::createFromAscii( "fixedtext" ) ), aInfoString, PAGE_POS_X, 6, PAGE_WIDTH, 24, sal_True, 0 );
    InsertCheckBox(  *this, TKGet( TK_OpenNewDocument ), xItemListener, getString( STR_AUTOMATICALLY_OPEN ), PAGE_POS_X, 42, PAGE_WIDTH, 8, 1 );
    InsertButton( *this, rtl::OUString( rtl::OUString::createFromAscii( "button" ) ), mxActionListener, DIALOG_WIDTH / 2 - 25, DIALOG_HEIGHT - 20, 50, 14, 2, STR_OK );
}

// -----------------------------------------------------------------------------

InformationDialog::InformationDialog( const Reference< XComponentContext > &rxMSF, Reference< XFrame >& rxFrame, sal_Bool& bOpenNewDocument, sal_Int64 nSourceSize, sal_Int64 nDestSize ) :
    UnoDialog( rxMSF, rxFrame ),
    ConfigurationAccess( rxMSF, NULL ),
    mxMSF( rxMSF ),
    mxFrame( rxFrame ),
    mxActionListener( new OKActionListener( *this ) ),
    mnSourceSize( nSourceSize ),
    mnDestSize( nDestSize ),
    mrbOpenNewDocument( bOpenNewDocument )
{
    Reference< XFrame > xFrame( mxController->getFrame() );
    Reference< XWindow > xContainerWindow( xFrame->getContainerWindow() );
    Reference< XWindowPeer > xWindowPeer( xContainerWindow, UNO_QUERY_THROW );
    createWindowPeer( xWindowPeer );		

    InitDialog();
}

// -----------------------------------------------------------------------------

InformationDialog::~InformationDialog()
{
}

// -----------------------------------------------------------------------------

sal_Bool InformationDialog::execute()
{
    UnoDialog::execute();

    sal_Int16 nInt16;
    Any aAny( getControlProperty( TKGet( TK_OpenNewDocument ), TKGet( TK_State ) ) );
    if ( aAny >>= nInt16 )
    {
        sal_Bool bOpenNewDocument = static_cast< sal_Bool >( nInt16 );
        mrbOpenNewDocument = bOpenNewDocument;
    }
    return mbStatus;
}

// -----------------------------------------------------------------------------

void OKActionListener::actionPerformed( const ActionEvent& rEvent )
    throw ( com::sun::star::uno::RuntimeException )
{
    if ( rEvent.ActionCommand == rtl::OUString( rtl::OUString::createFromAscii( "button" ) ) )
    {
        mrInformationDialog.endExecute( sal_True );
    }
}
void OKActionListener::disposing( const ::com::sun::star::lang::EventObject& /* Source */ )
    throw ( com::sun::star::uno::RuntimeException )
{
}
