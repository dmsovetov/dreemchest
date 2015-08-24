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

#ifndef __DC_Mvvm_H__
#define __DC_Mvvm_H__

#include "../Dreemchest.h"

#ifndef FOO_INCLUDED
	#include <Foo/Foo.h>
#endif

DC_BEGIN_DREEMCHEST

namespace mvvm {

    template<typename T> class GenericProperty;
    template<typename T> class GenericArrayProperty;

    class View;
    class Binding;
    class Property;
    class ActionHandler;
    class Data;

    typedef StrongPtr<Binding> BindingPtr;

    typedef GenericProperty<bool>           BoolProperty;
    typedef GenericProperty<s32>            IntProperty;
    typedef GenericProperty<f32>			FloatProperty;
    typedef GenericProperty<String>         StringProperty;
    typedef GenericArrayProperty<String>    StringArrayProperty;
    typedef List<BindingPtr>                BindingsList;

} // namespace mvvm

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "View.h"
    #include "Data.h"
    #include "ActionHandler.h"
    #include "Property.h"
    #include "Validation.h"

    #ifdef DC_MVVM_MYGUI_ENABLED
        #include "MyGUI/MyGUIView.h"
        #include "MyGUI/BindingsMyGUI.h"
    #endif
#endif

#endif  /*  !__DC_Mvvm_H__   */