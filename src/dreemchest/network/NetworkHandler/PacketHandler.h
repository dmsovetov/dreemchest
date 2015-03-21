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

#ifndef __DC_Network_PacketHandler_H__
#define __DC_Network_PacketHandler_H__

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

	//! Packet handler interface class.
	class IPacketHandler {
	public:

		virtual			~IPacketHandler( void ) {}

		//! Packet handler callback.
		virtual bool	handle( TCPSocket* sender, const NetworkPacket* packet ) = 0;
	};

	//! Template class that handles a strict-typed packets.
	template<typename T>
	class PacketHandler : public IPacketHandler {
	public:

		//! Function type to handle packets.
		typedef cClosure<bool(TCPSocket*,const T*)> Callback;

						//! Constructs GenericPacketHandler instance.
						PacketHandler( const Callback& callback )
							: m_callback( callback ) {}

		//! Casts an input network packet to a specified type and runs a callback.
		virtual bool handle( TCPSocket* sender, const NetworkPacket* packet );

	private:

		//! Packet handler callback.
		Callback	 m_callback;
	};

	// ** PacketHandler::handle
	template<typename T>
	bool PacketHandler<T>::handle( TCPSocket* sender, const NetworkPacket* packet )
	{
		const T* packetWithType = castTo<T>( packet );
		DC_BREAK_IF( packetWithType == NULL );
		return m_callback( sender, packetWithType );
	}

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PacketHandler_H__	*/