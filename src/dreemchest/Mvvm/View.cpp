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

#include "View.h"
#include "ActionHandler.h"
#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// ** View::notify
void View::notify( const String& event )
{
    ActionHandlers::Parts& actionHandlers = m_actionHandlers.parts();

    for( ActionHandlers::Parts::iterator i = actionHandlers.begin(), end = actionHandlers.end(); i != end; ++i ) {
        i->second->handleEvent( event );
    }
}

// ** View::clear
void View::clear( void )
{
    m_bindings.clear();
    m_data.clear();
    m_actionHandlers.clear();
}

// ** View::addBinding
void View::addBinding( Binding* instance )
{
    instance->refresh();
    m_bindings.push_back( instance );
}

} // namespace mvvm

DC_END_DREEMCHEST
