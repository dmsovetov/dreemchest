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

#ifndef __DC_Network_ClientHandler_H__
#define __DC_Network_ClientHandler_H__

#include "NetworkHandler.h"
#include "../Sockets/TCPSocket.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    // ** class ClientHandler
    class ClientHandler : public NetworkHandler {
	friend class ClientSocketDelegate;
    public:

		//! ConnectionClosed event is emitted when a connection was closed.
		struct ConnectionClosed {
		};

		virtual					~ClientHandler( void );

		//! Return current connection.
		const ConnectionPtr&	connection( void ) const;
		ConnectionPtr&			connection( void );

		//! Creates a new NetworkClientHandler instance and connects to server.
		static ClientHandlerPtr	create( const Address& address, u16 port );

		//! Does a broadcast request to detect a running servers.
		static bool				detectServers( u16 port );

        // ** NetworkHandler
        virtual void			update( u32 dt );
		virtual ConnectionList	eventListeners( void ) const;

	protected:

								//! Constructs ClientHandler instance.
								ClientHandler( TCPSocketPtr socket );

		//! Handles the connection closed event.
		void			        handleConnectionClosed( const Connection::Closed& e );

	private:

		ConnectionPtr			m_connection;	//!< Client connection.
    };
    
} // namespace Network
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkClientHandler_H__	*/