/*******************************************************************************
 * Copyright (c) 2009, Rockwell Automation, Inc.
 * Copyright (c) 2016-2018, SoftPLC Corporation.
 *
 ******************************************************************************/

#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_

#include <string>

#include "sockaddr.h"
#include "../cip/ciptypes.h"

extern uint64_t g_current_usecs;


/**
 * Function NetworkHandlerInitialize
 * starts a TCP/UDP listening socket to accept connections.
 */
EipStatus NetworkHandlerInitialize();

EipStatus NetworkHandlerProcessOnce();

EipStatus NetworkHandlerFinish();

/**
 * Function strerrno
 * returns a string containing text generated by the OS for the last value
 * of errno or WSAGetLastError()
 */
std::string strerrno();

/**
 * Function CloseSocket
 * closes @a aSocket
 */
void CloseSocket( int aSocket );

bool SocketAsync( int aSocket, bool isAsync = true );


/**
 * Function SendUdpData
 * sends the bytes provided in @a aOutput to the UDP node given by @a aSockAddr
 * using @a aSocket.
 *
 * @param aSockAddr is the "send to" address
 * @param aSocket is the socket descriptor to send on
 * @param aOutput is the data to send and its length
 * @return  EipStatus - EipStatusSuccess on success
 */
EipStatus SendUdpData( const SockAddr& aSockAddr, int aSocket, BufReader aOutput );


class UdpSocket
{
    friend class UdpSocketMgr;

public:
    UdpSocket( const SockAddr& aSockAddr, int aSocket ) :
        m_sockaddr( aSockAddr ),
        m_socket( aSocket ),
        m_ref_count( 1 )
    {
    }

    ~UdpSocket()
    {
        if( m_socket != kSocketInvalid )
        {
            CloseSocket( m_socket );
            m_socket = kSocketInvalid;
        }
    }

    EipStatus Send( const SockAddr& aAddr, const BufReader& aReader )
    {
        return ::SendUdpData( aAddr, m_socket, aReader );
    }

    int Recv( SockAddr* aAddr, const BufWriter& aWriter )
    {
        socklen_t   from_address_length = SADDRZ;

        return recvfrom( m_socket, (char*) aWriter.data(), aWriter.capacity(), 0,
                    *aAddr, &from_address_length );
    }

    int h() const   { return m_socket; }

private:
    SockAddr    m_sockaddr; // what this socket is bound to with bind()
    int         m_socket;
    int         m_ref_count;
};


/**
 * Class UdpSocketMgr
 * manages UDP sockets.  Since a UDP socket can be used for multiple inbound
 * and outbound UDP frames, without regard to who the recipient or sender is,
 * we re-use UDP sockets accross multiple I/O connections.  This way we do
 * not create more than the minimum number of sockets, and we should not have
 * to use SO_REUSEADDR.
 */
class UdpSocketMgr
{
public:

    typedef std::vector<UdpSocket*>     sockets;
    typedef sockets::iterator           sock_iter;
    typedef sockets::const_iterator     sock_citer;

    UdpSocketMgr()          { m_sockets.reserve( 10 ); }

    static UdpSocket*       GrabSocket( const SockAddr& aSockAddr );
    static bool             ReleaseSocket( UdpSocket* aUdpSocket );
    static sockets&         GetAllSockets()  { return m_sockets; }

protected:

    /**
     * Function createSocket
     * creates a UDP socket and binds it to aSockAddr.
     *
     * @param aSockAddr tells how to setup the socket.
     * @return int - socket on success or kSocketInvalid on error
     */
    static int createSocket( const SockAddr& aSockAddr );

    // Allocate and initialize a new UdpSocket
    static UdpSocket*  alloc( const SockAddr& aSockAddr, int aSocket );

    static void free( UdpSocket* aUdpSocket )
    {
        m_free.push_back( aUdpSocket );
    }

    static sockets      m_sockets;
    static sockets      m_free;         // recycling bin
};


#endif  // NETWORKHANDLER_H_
