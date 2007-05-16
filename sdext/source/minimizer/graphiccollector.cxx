 /*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: graphiccollector.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: sj $ $Date: 2007-05-16 15:07:46 $
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

#ifndef GRAPHICCOLLECTOR_HXX
#include "graphiccollector.hxx"
#endif

#ifndef _COM_SUN_STAR_AWT_XDEVICE_HPP_
#include <com/sun/star/awt/XDevice.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMESSUPPLIER_HPP_
#include <com/sun/star/frame/XFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FILLSTYLE_HPP_
#include <com/sun/star/drawing/FillStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_BITMAPMODE_HPP_
#include <com/sun/star/drawing/BitmapMode.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_XPRESENTATIONPAGE_HPP_
#include <com/sun/star/presentation/XPresentationPage.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#endif

#include "impoptimizer.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::graphic;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::presentation;

const DeviceInfo& GraphicCollector::GetDeviceInfo( const Reference< XComponentContext >& rxFact )
{
    static DeviceInfo aDeviceInfo;
    if( !aDeviceInfo.Width )
    {
        try
        {
            Reference< XFramesSupplier > xDesktop( rxFact->getServiceManager()->createInstanceWithContext(
                    OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ), rxFact ), UNO_QUERY_THROW );
            Reference< XFrame > xFrame( xDesktop->getActiveFrame() );
            Reference< XWindow > xWindow( xFrame->getContainerWindow() );
            Reference< XDevice > xDevice( xWindow, UNO_QUERY_THROW );
            aDeviceInfo = xDevice->getInfo();
        }
        catch( Exception& )
        {
        }
    }
    return aDeviceInfo;
}

void ImpAddEntity( std::vector< GraphicCollector::GraphicEntity >& rGraphicEntities, Reference< XGraphic >& rxGraphic, const GraphicSettings& rGraphicSettings, const GraphicCollector::GraphicUser& rUser )
{
    const rtl::OUString aGraphicURL( rUser.maGraphicURL );
    const rtl::OUString sPackageURL( OUString::createFromAscii( "vnd.sun.star.GraphicObject:" ) );

    if ( rGraphicSettings.mbEmbedLinkedGraphics || ( !aGraphicURL.getLength() || aGraphicURL.match( sPackageURL, 0 ) ) )
    {
        std::vector< GraphicCollector::GraphicEntity >::iterator aIter( rGraphicEntities.begin() );
        while( aIter != rGraphicEntities.end() )
        {
            if ( aIter->maUser[ 0 ].maGraphicURL == aGraphicURL )
            {
                if ( rUser.maLogicalSize.Width > aIter->maLogicalSize.Width )
                    aIter->maLogicalSize.Width = rUser.maLogicalSize.Width;
                if ( rUser.maLogicalSize.Height > aIter->maLogicalSize.Height )
                    aIter->maLogicalSize.Height = rUser.maLogicalSize.Height;
                aIter->maUser.push_back( rUser );
                break;
            }
            aIter++;
        }
        if ( aIter == rGraphicEntities.end() )
        {
            GraphicCollector::GraphicEntity aEntity( rxGraphic, rUser );
            rGraphicEntities.push_back( aEntity );
        }
    }
}

void ImpAddGraphicEntity( const Reference< XComponentContext >& rxMSF, Reference< XShape >& rxShape, const GraphicSettings& rGraphicSettings, std::vector< GraphicCollector::GraphicEntity >& rGraphicEntities )
{
    Reference< XGraphic > xGraphic;
    Reference< XPropertySet > xShapePropertySet( rxShape, UNO_QUERY_THROW );
    if ( xShapePropertySet->getPropertyValue( TKGet( TK_Graphic ) ) >>= xGraphic )
    {
        text::GraphicCrop aGraphicCropLogic( 0, 0, 0, 0 );

        GraphicCollector::GraphicUser aUser;
        aUser.mxShape = rxShape;
        aUser.mbFillBitmap = sal_False;
        xShapePropertySet->getPropertyValue( TKGet( TK_GraphicURL ) ) >>= aUser.maGraphicURL;
        xShapePropertySet->getPropertyValue( TKGet( TK_GraphicStreamURL ) ) >>= aUser.maGraphicStreamURL;
        xShapePropertySet->getPropertyValue( TKGet( TK_GraphicCrop ) ) >>= aGraphicCropLogic;
        awt::Size aLogicalSize( rxShape->getSize() );

        // calculating the logical size, as if there were no cropping
        if ( aGraphicCropLogic.Left || aGraphicCropLogic.Right || aGraphicCropLogic.Top || aGraphicCropLogic.Bottom )
        {
            awt::Size aSize100thMM( GraphicCollector::GetOriginalSize( rxMSF, xGraphic ) );
            if ( aSize100thMM.Width && aSize100thMM.Height )
            {
                awt::Size aCropSize( aSize100thMM.Width - ( aGraphicCropLogic.Left + aGraphicCropLogic.Right ),
                                     aSize100thMM.Height - ( aGraphicCropLogic.Top + aGraphicCropLogic.Bottom ));
                if ( aCropSize.Width && aCropSize.Height )
                {
                    awt::Size aNewLogSize( static_cast< sal_Int32 >( static_cast< double >( aSize100thMM.Width * aLogicalSize.Width ) / aCropSize.Width ),
                        static_cast< sal_Int32 >( static_cast< double >( aSize100thMM.Height * aLogicalSize.Height ) / aCropSize.Height ) );
                    aLogicalSize = aNewLogSize;
                }
            }
        }
        aUser.maGraphicCropLogic = aGraphicCropLogic;
        aUser.maLogicalSize = aLogicalSize;
        ImpAddEntity( rGraphicEntities, xGraphic, rGraphicSettings, aUser );
    }
}

void ImpAddFillBitmapEntity( const Reference< XComponentContext >& rxMSF, const Reference< XPropertySet >& rxPropertySet, const awt::Size& rLogicalSize,
    std::vector< GraphicCollector::GraphicEntity >& rGraphicEntities, const GraphicSettings& rGraphicSettings, const Reference< XPropertySet >& rxPagePropertySet )
{
    try
    {
        FillStyle eFillStyle;
        if ( rxPropertySet->getPropertyValue( TKGet( TK_FillStyle ) ) >>= eFillStyle )
        {
            if ( eFillStyle == FillStyle_BITMAP )
            {
                rtl::OUString aFillBitmapURL;
                Reference< XBitmap > xFillBitmap;
                if ( rxPropertySet->getPropertyValue( TKGet( TK_FillBitmap ) ) >>= xFillBitmap )
                {
                    Reference< XGraphic > xGraphic( xFillBitmap, UNO_QUERY_THROW );
                    if ( xGraphic.is() )
                    {
                        awt::Size aLogicalSize( rLogicalSize );
                        Reference< XPropertySetInfo > axPropSetInfo( rxPropertySet->getPropertySetInfo() );
                        if ( axPropSetInfo.is() )
                        {
                            if ( axPropSetInfo->hasPropertyByName( TKGet( TK_FillBitmapMode ) ) )
                            {
                                BitmapMode eBitmapMode;
                                if ( rxPropertySet->getPropertyValue( TKGet( TK_FillBitmapMode ) ) >>= eBitmapMode )
                                {
                                    if ( ( eBitmapMode == BitmapMode_REPEAT ) || ( eBitmapMode == BitmapMode_NO_REPEAT ) )
                                    {
                                        sal_Bool bLogicalSize;
                                        awt::Size aSize( 0, 0 );
                                        if ( ( rxPropertySet->getPropertyValue( TKGet( TK_FillBitmapLogicalSize ) ) >>= bLogicalSize )
                                          && ( rxPropertySet->getPropertyValue( TKGet( TK_FillBitmapSizeX ) ) >>= aSize.Width )
                                          && ( rxPropertySet->getPropertyValue( TKGet( TK_FillBitmapSizeY ) ) >>= aSize.Height ) )
                                        {
                                            if ( bLogicalSize )
                                            {
                                                if ( !aSize.Width || !aSize.Height )
                                                {
                                                    awt::Size aSize100thMM( GraphicCollector::GetOriginalSize( rxMSF, xGraphic ) );
                                                    if ( aSize100thMM.Width && aSize100thMM.Height )
                                                        aLogicalSize = aSize100thMM;
                                                }
                                                else
                                                    aLogicalSize = aSize;
                                            }
                                            else
                                            {
                                                aLogicalSize.Width = sal::static_int_cast< sal_Int32 >( ( static_cast< double >( aLogicalSize.Width ) * aSize.Width ) / -100.0 );
                                                aLogicalSize.Height = sal::static_int_cast< sal_Int32 >( ( static_cast< double >( aLogicalSize.Height ) * aSize.Height ) / -100.0 );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        GraphicCollector::GraphicUser aUser;
                        aUser.mxPropertySet = rxPropertySet;
                        rxPropertySet->getPropertyValue( TKGet( TK_FillBitmapURL ) ) >>= aUser.maGraphicURL;
                        aUser.mbFillBitmap = sal_True;
                        aUser.maLogicalSize = aLogicalSize;
                        aUser.mxPagePropertySet = rxPagePropertySet;
                        ImpAddEntity( rGraphicEntities, xGraphic, rGraphicSettings, aUser );
                    }
                }
            }
        }
    }
    catch( Exception& )
    {
    }
}

void ImpCollectBackgroundGraphic( const Reference< XComponentContext >& rxMSF, const Reference< XDrawPage >& rxDrawPage, const GraphicSettings& rGraphicSettings, std::vector< GraphicCollector::GraphicEntity >& rGraphicEntities )
{
    try
    {
        awt::Size aLogicalSize( 28000, 21000 );
        Reference< XPropertySet > xPropertySet( rxDrawPage, UNO_QUERY_THROW );
        xPropertySet->getPropertyValue( TKGet( TK_Width ) ) >>= aLogicalSize.Width;
        xPropertySet->getPropertyValue( TKGet( TK_Height ) ) >>= aLogicalSize.Height;
        
        Reference< XPropertySet > xBackgroundPropSet;
        if ( xPropertySet->getPropertyValue( TKGet( TK_Background ) ) >>= xBackgroundPropSet )
            ImpAddFillBitmapEntity( rxMSF, xBackgroundPropSet, aLogicalSize, rGraphicEntities, rGraphicSettings, xPropertySet );
    }
    catch( Exception& )
    {
    }
}

void ImpCollectGraphicObjects( const Reference< XComponentContext >& rxMSF, const Reference< XShapes >& rxShapes, const GraphicSettings& rGraphicSettings, std::vector< GraphicCollector::GraphicEntity >& rGraphicEntities )
{
    for ( sal_Int32 i = 0; i < rxShapes->getCount(); i++ )
    {
        try
        {
            const OUString sGraphicObjectShape( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.GraphicObjectShape" ) );
            const OUString sGroupShape( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.GroupShape" ) );
            Reference< XShape > xShape( rxShapes->getByIndex( i ), UNO_QUERY_THROW );
            const OUString sShapeType( xShape->getShapeType() );
            if ( sShapeType == sGroupShape )
            {
                Reference< XShapes > xShapes( xShape, UNO_QUERY_THROW );
                ImpCollectGraphicObjects( rxMSF, xShapes, rGraphicSettings, rGraphicEntities );
                continue;
            }

            if ( sShapeType == sGraphicObjectShape )
                ImpAddGraphicEntity( rxMSF, xShape, rGraphicSettings, rGraphicEntities );

            // now check for a fillstyle
            Reference< XPropertySet > xEmptyPagePropSet;
            Reference< XPropertySet > xShapePropertySet( xShape, UNO_QUERY_THROW );
            awt::Size aLogicalSize( xShape->getSize() );
            ImpAddFillBitmapEntity( rxMSF, xShapePropertySet, aLogicalSize, rGraphicEntities, rGraphicSettings, xEmptyPagePropSet );
        }
        catch( Exception& )
        {
        }
    }
}

awt::Size GraphicCollector::GetOriginalSize( const Reference< XComponentContext >& rxMSF, const Reference< XGraphic >& rxGraphic )
{
    awt::Size aSize100thMM( 0, 0 );
    Reference< XPropertySet > xGraphicPropertySet( rxGraphic, UNO_QUERY_THROW );
    if ( xGraphicPropertySet->getPropertyValue( TKGet( TK_Size100thMM ) ) >>= aSize100thMM )
    {
        if ( !aSize100thMM.Width && !aSize100thMM.Height )
        {	// MAPMODE_PIXEL USED :-(
            awt::Size aSourceSizePixel( 0, 0 );
            if ( xGraphicPropertySet->getPropertyValue( TKGet( TK_SizePixel ) ) >>= aSourceSizePixel )
            {
                const DeviceInfo& rDeviceInfo( GraphicCollector::GetDeviceInfo( rxMSF ) );
                if ( rDeviceInfo.PixelPerMeterX && rDeviceInfo.PixelPerMeterY )
                {
                    aSize100thMM.Width = static_cast< sal_Int32 >( ( aSourceSizePixel.Width * 100000.0 ) / rDeviceInfo.PixelPerMeterX );
                    aSize100thMM.Height = static_cast< sal_Int32 >( ( aSourceSizePixel.Height * 100000.0 ) / rDeviceInfo.PixelPerMeterY );
                }
            }
        }
    }
    return aSize100thMM;
}

void GraphicCollector::CollectGraphics( const Reference< XComponentContext >& rxMSF, const Reference< XModel >& rxModel,
        const GraphicSettings& rGraphicSettings, std::vector< GraphicCollector::GraphicEntity >& rGraphicList )
{
    try
    {
        sal_Int32 i;
        Reference< XDrawPagesSupplier > xDrawPagesSupplier( rxModel, UNO_QUERY_THROW );
        Reference< XDrawPages > xDrawPages( xDrawPagesSupplier->getDrawPages(), UNO_QUERY_THROW );
        for ( i = 0; i < xDrawPages->getCount(); i++ )
        {
            Reference< XDrawPage > xDrawPage( xDrawPages->getByIndex( i ), UNO_QUERY_THROW );
            ImpCollectBackgroundGraphic( rxMSF, xDrawPage, rGraphicSettings, rGraphicList );
            Reference< XShapes > xDrawShapes( xDrawPage, UNO_QUERY_THROW );
            ImpCollectGraphicObjects( rxMSF, xDrawShapes, rGraphicSettings, rGraphicList );

            Reference< XPresentationPage > xPresentationPage( xDrawPage, UNO_QUERY_THROW );
            Reference< XDrawPage > xNotesPage( xPresentationPage->getNotesPage() );
            ImpCollectBackgroundGraphic( rxMSF, xNotesPage, rGraphicSettings, rGraphicList );
            Reference< XShapes > xNotesShapes( xNotesPage, UNO_QUERY_THROW );
            ImpCollectGraphicObjects( rxMSF, xNotesShapes, rGraphicSettings, rGraphicList );
        }
        Reference< XMasterPagesSupplier > xMasterPagesSupplier( rxModel, UNO_QUERY_THROW );
        Reference< XDrawPages > xMasterPages( xMasterPagesSupplier->getMasterPages(), UNO_QUERY_THROW );
        for ( i = 0; i < xMasterPages->getCount(); i++ )
        {
            Reference< XDrawPage > xMasterPage( xMasterPages->getByIndex( i ), UNO_QUERY_THROW );
            ImpCollectBackgroundGraphic( rxMSF, xMasterPage, rGraphicSettings, rGraphicList );
            Reference< XShapes > xMasterPageShapes( xMasterPage, UNO_QUERY_THROW );
            ImpCollectGraphicObjects( rxMSF, xMasterPageShapes, rGraphicSettings, rGraphicList );
        }

        std::vector< GraphicCollector::GraphicEntity >::iterator aGraphicIter( rGraphicList.begin() );
        std::vector< GraphicCollector::GraphicEntity >::iterator aGraphicIEnd( rGraphicList.end() );
        while( aGraphicIter != aGraphicIEnd )
        {
            // check if it is possible to remove the crop area
            aGraphicIter->mbRemoveCropArea = rGraphicSettings.mbRemoveCropArea;
            if ( aGraphicIter->mbRemoveCropArea )
            {
                std::vector< GraphicCollector::GraphicUser >::iterator aGUIter( aGraphicIter->maUser.begin() );
                while( aGraphicIter->mbRemoveCropArea && ( aGUIter != aGraphicIter->maUser.end() ) )
                {
                    if ( aGUIter->maGraphicCropLogic.Left || aGUIter->maGraphicCropLogic.Top
                        || aGUIter->maGraphicCropLogic.Right || aGUIter->maGraphicCropLogic.Bottom )
                    {
                        if ( aGUIter == aGraphicIter->maUser.begin() )
                            aGraphicIter->maGraphicCropLogic = aGUIter->maGraphicCropLogic;
                        else if ( ( aGraphicIter->maGraphicCropLogic.Left != aGUIter->maGraphicCropLogic.Left )
                            || ( aGraphicIter->maGraphicCropLogic.Top != aGUIter->maGraphicCropLogic.Top )
                            || ( aGraphicIter->maGraphicCropLogic.Right != aGUIter->maGraphicCropLogic.Right )
                            || ( aGraphicIter->maGraphicCropLogic.Bottom != aGUIter->maGraphicCropLogic.Bottom ) )
                        {
                            aGraphicIter->mbRemoveCropArea = sal_False;
                        }
                    }
                    else
                        aGraphicIter->mbRemoveCropArea = sal_False;
                    aGUIter++;
                }
            }
            if ( !aGraphicIter->mbRemoveCropArea )
                aGraphicIter->maGraphicCropLogic = text::GraphicCrop( 0, 0, 0, 0 );
            aGraphicIter++;
        }
    }
    catch ( Exception& )
    {
    }
}

void ImpCountGraphicObjects( const Reference< XComponentContext >& rxMSF, const Reference< XShapes >& rxShapes, const GraphicSettings& rGraphicSettings, sal_Int32& rnGraphics )
{
    for ( sal_Int32 i = 0; i < rxShapes->getCount(); i++ )
    {
        try
        {
            const OUString sGraphicObjectShape( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.GraphicObjectShape" ) );
            const OUString sGroupShape( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.GroupShape" ) );
            Reference< XShape > xShape( rxShapes->getByIndex( i ), UNO_QUERY_THROW );
            const OUString sShapeType( xShape->getShapeType() );
            if ( sShapeType == sGroupShape )
            {
                Reference< XShapes > xShapes( xShape, UNO_QUERY_THROW );
                ImpCountGraphicObjects( rxMSF, xShapes, rGraphicSettings, rnGraphics );
                continue;
            }

            if ( sShapeType == sGraphicObjectShape )
            {
                rnGraphics++;
            }

            // now check for a fillstyle
            Reference< XPropertySet > xEmptyPagePropSet;
            Reference< XPropertySet > xShapePropertySet( xShape, UNO_QUERY_THROW );
            awt::Size aLogicalSize( xShape->getSize() );

            FillStyle eFillStyle;
            if ( xShapePropertySet->getPropertyValue( TKGet( TK_FillStyle ) ) >>= eFillStyle )
            {
                if ( eFillStyle == FillStyle_BITMAP )
                {
                    rnGraphics++;
                }
            }
        }
        catch( Exception& )
        {
        }
    }
}

void ImpCountBackgroundGraphic( const Reference< XComponentContext >& /* rxMSF */, const Reference< XDrawPage >& rxDrawPage,
                               const GraphicSettings& /* rGraphicSettings */, sal_Int32& rnGraphics )
{
    try
    {
        awt::Size aLogicalSize( 28000, 21000 );
        Reference< XPropertySet > xPropertySet( rxDrawPage, UNO_QUERY_THROW );
        xPropertySet->getPropertyValue( TKGet( TK_Width ) ) >>= aLogicalSize.Width;
        xPropertySet->getPropertyValue( TKGet( TK_Height ) ) >>= aLogicalSize.Height;
        
        Reference< XPropertySet > xBackgroundPropSet;
        if ( xPropertySet->getPropertyValue( TKGet( TK_Background ) ) >>= xBackgroundPropSet )
        {
            FillStyle eFillStyle;
            if ( xBackgroundPropSet->getPropertyValue( TKGet( TK_FillStyle ) ) >>= eFillStyle )
            {
                if ( eFillStyle == FillStyle_BITMAP )
                {
                    rnGraphics++;
                }
            }
        }
    }
    catch( Exception& )
    {
    }
}

void GraphicCollector::CountGraphics( const Reference< XComponentContext >& rxMSF, const Reference< XModel >& rxModel,
        const GraphicSettings& rGraphicSettings, sal_Int32& rnGraphics )
{
    try
    {
        sal_Int32 i;
        Reference< XDrawPagesSupplier > xDrawPagesSupplier( rxModel, UNO_QUERY_THROW );
        Reference< XDrawPages > xDrawPages( xDrawPagesSupplier->getDrawPages(), UNO_QUERY_THROW );
        for ( i = 0; i < xDrawPages->getCount(); i++ )
        {
            Reference< XDrawPage > xDrawPage( xDrawPages->getByIndex( i ), UNO_QUERY_THROW );
            ImpCountBackgroundGraphic( rxMSF, xDrawPage, rGraphicSettings, rnGraphics );
            Reference< XShapes > xDrawShapes( xDrawPage, UNO_QUERY_THROW );
            ImpCountGraphicObjects( rxMSF, xDrawShapes, rGraphicSettings, rnGraphics );

            Reference< XPresentationPage > xPresentationPage( xDrawPage, UNO_QUERY_THROW );
            Reference< XDrawPage > xNotesPage( xPresentationPage->getNotesPage() );
            ImpCountBackgroundGraphic( rxMSF, xNotesPage, rGraphicSettings, rnGraphics );
            Reference< XShapes > xNotesShapes( xNotesPage, UNO_QUERY_THROW );
            ImpCountGraphicObjects( rxMSF, xNotesShapes, rGraphicSettings, rnGraphics );
        }
        Reference< XMasterPagesSupplier > xMasterPagesSupplier( rxModel, UNO_QUERY_THROW );
        Reference< XDrawPages > xMasterPages( xMasterPagesSupplier->getMasterPages(), UNO_QUERY_THROW );
        for ( i = 0; i < xMasterPages->getCount(); i++ )
        {
            Reference< XDrawPage > xMasterPage( xMasterPages->getByIndex( i ), UNO_QUERY_THROW );
            ImpCountBackgroundGraphic( rxMSF, xMasterPage, rGraphicSettings, rnGraphics );
            Reference< XShapes > xMasterPageShapes( xMasterPage, UNO_QUERY_THROW );
            ImpCountGraphicObjects( rxMSF, xMasterPageShapes, rGraphicSettings, rnGraphics );
        }
    }
    catch ( Exception& )
    {
    }
}

