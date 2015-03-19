/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef		__DC_Network_H__
#define		__DC_Network_H__

#include	"../Dreemchest.h"

#ifndef DC_UTILS_INCLUDED
    #include <utils/Utils.h>
#endif

#include "../io/Io.h"

DC_BEGIN_DREEMCHEST

namespace net {

	DECLARE_LOG(log)

	class TCPSocketDelegate;
	class TCPSocket;
	class UDPSocketDelegate;
	class UDPSocket;
	class TCPSocketListenerDelegate;
	class TCPSocketListener;
	class SocketDescriptor;
	class TCPStream;

    class INetwork;

	//! TCP socket strong ptr.
	typedef StrongPtr<TCPSocket> TCPSocketPtr;

	//! TCP socket event delegate ptr.
	typedef StrongPtr<TCPSocketDelegate> TCPSocketDelegatePtr;

	//! UDP socket strong ptr.
	typedef StrongPtr<UDPSocket> UDPSocketPtr;

	//! UDP socket event delegate ptr.
	typedef StrongPtr<UDPSocketDelegate> UDPSocketDelegatePtr;

	//! TCP stream strong ptr.
	typedef StrongPtr<TCPStream> TCPStreamPtr;

	//! TCP socket listener ptr.
	typedef StrongPtr<TCPSocketListener> TCPSocketListenerPtr;

	//! TCP socket listener delegate ptr.
	typedef StrongPtr<TCPSocketListenerDelegate> TCPSocketListenerDelegatePtr;

	//! Socket list type.
	typedef List<TCPSocketPtr> TCPSocketList;

    //! A helper class to represent a network address.
    class NetworkAddress {
    public:

        static NetworkAddress           Null;		//!< Empty address.
        static NetworkAddress           Localhost;	//!< Localhost address.

    public:

										//! Constructs NetworkAddress from a 32-bit integer.
                                        NetworkAddress( u32 address = 0 );
										
										//! Constructs a NetworkAddress from string.
        explicit                        NetworkAddress( CString address );

		//! Converts a network address to unsigned 32-bit integer.
        operator                        u32( void ) const;

		//! Converts a network address to string.
        CString							toString( void ) const;

    private:

		//! Actual network address.
        u32								m_address;
    };

	//! Array of network addresses.
    typedef Array<NetworkAddress> NetworkAddressArray;

    //! Network interface class.
    class Network {
    public:

										//! Constructs Network instance.
                                        Network( void );
        virtual                         ~Network( void );

		//! Returns a current host IP address.
        const NetworkAddress&           hostIP( void ) const;

		//! Returns a broadcast IP address.
        const NetworkAddress&           broadcastIP( void ) const;

		//! Returns current host name.
        CString							hostName( void ) const;

    private:

		//! Network implementation.
        INetwork*                       m_impl;
    };

    // ** class INetwork
    class INetwork {
    friend class Network;
    public:


        virtual                         ~INetwork( void ) {}

        virtual const NetworkAddress&   hostIP( void ) const = 0;
        virtual const NetworkAddress&   broadcastIP( void ) const = 0;
        virtual CString					hostName( void ) const = 0;

    protected:

        Network*                        m_parent;
    };
    
} // namespace net
    
DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Sockets/TCPSocketListener.h"
	#include "Sockets/TCPSocket.h"
	#include "Sockets/UDPSocket.h"
	#include "Sockets/TCPStream.h"
#endif

#endif	/*	!__DC_Network_H__	*/