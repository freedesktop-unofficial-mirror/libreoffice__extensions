/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WikiProtocolSocketFactory.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: mav $ $Date: 2007-11-28 11:15:42 $
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

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.KeyStore;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import org.apache.commons.httpclient.ConnectTimeoutException;
import org.apache.commons.httpclient.HttpClientError;
import org.apache.commons.httpclient.params.HttpConnectionParams;
import org.apache.commons.httpclient.protocol.SecureProtocolSocketFactory;

class WikiProtocolSocketFactory implements SecureProtocolSocketFactory
{
    private SSLContext m_aSSLContext;

    public WikiProtocolSocketFactory()
    {
        super();
    }

    public synchronized SSLContext GetNotSoSecureSSLContext()
    {
        if ( m_aSSLContext == null )
        {
            TrustManager[] pTrustUnknownCerts = new TrustManager[]
            {
                new X509TrustManager() {
                    private X509TrustManager m_aOrgTrustManager;
                
                    private X509TrustManager GetOrgTrustManager()
                    {
                        if ( m_aOrgTrustManager == null )
                        {
                            try
                            {
                                TrustManagerFactory aFactory = TrustManagerFactory.getInstance( TrustManagerFactory.getDefaultAlgorithm() );
                                aFactory.init( (KeyStore)null );
                                TrustManager[] pTrustmanagers = aFactory.getTrustManagers();
                                if ( pTrustmanagers.length != 0 && pTrustmanagers[0] != null )
                                    m_aOrgTrustManager = (X509TrustManager)pTrustmanagers[0];
                            }
                            catch( Exception e )
                            {
                                throw new RuntimeException( "No access to the default trust manager!" );
                            }
                        }
                        
                        return m_aOrgTrustManager;
                    }
                    
                    public X509Certificate[] getAcceptedIssuers()
                    {
                        return GetOrgTrustManager().getAcceptedIssuers();
                    }
                    
                    public void checkClientTrusted(X509Certificate[] certs, String authType) throws CertificateException
                    {
                        GetOrgTrustManager().checkClientTrusted( certs, authType );
                    }
                    
                    public void checkServerTrusted(X509Certificate[] certs, String authType) throws CertificateException
                    {
                        if ( certs == null || certs.length == 0 )
                            GetOrgTrustManager().checkServerTrusted( certs, authType );
                        else
                            for ( int nInd = 0; nInd < certs.length; nInd++ )
                                certs[nInd].checkValidity();
                    }
                }
            };

            try
            {
                SSLContext aContext = SSLContext.getInstance("SSL");
                if ( aContext != null )
                {
                    aContext.init( null, pTrustUnknownCerts, null );
                    m_aSSLContext = aContext;
                }
            }
            catch ( Exception e )
            {
            }
        }

        if ( m_aSSLContext == null )
            throw new HttpClientError();

        return m_aSSLContext;
    }

    public Socket createSocket( String sHost, int nPort, InetAddress clientHost, int clientPort )
        throws IOException, UnknownHostException
    {
        return GetNotSoSecureSSLContext().getSocketFactory().createSocket( sHost, nPort, clientHost, clientPort );
    }

    public Socket createSocket( final String sHost, final int nPort, final InetAddress aLocalAddress, final int nLocalPort, final HttpConnectionParams params )
        throws IOException, UnknownHostException, ConnectTimeoutException
    {
        if ( params == null )
            return createSocket( sHost, nPort, aLocalAddress, nLocalPort );

        int nTimeout = params.getConnectionTimeout();
        Socket aSocket = GetNotSoSecureSSLContext().getSocketFactory().createSocket();
        aSocket.bind( new InetSocketAddress( aLocalAddress, nLocalPort ) );
        aSocket.connect( new InetSocketAddress( sHost, nPort ), nTimeout );
        return aSocket;
    }

    public Socket createSocket( String sHost, int nPort )
        throws IOException, UnknownHostException
    {
        return GetNotSoSecureSSLContext().getSocketFactory().createSocket( sHost, nPort );
    }

    public Socket createSocket( Socket aSocket, String sHost, int nPort, boolean bAutoClose )
        throws IOException, UnknownHostException
    {
        return GetNotSoSecureSSLContext().getSocketFactory().createSocket( aSocket, sHost, nPort, bAutoClose );
    }

    public boolean equals(Object obj)
    {
        return ((obj != null) && obj.getClass().equals(WikiProtocolSocketFactory.class));
    }

    public int hashCode()
    {
        return WikiProtocolSocketFactory.class.hashCode();
    }
};

