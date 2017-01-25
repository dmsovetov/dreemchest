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

#ifndef __DC_Renderer_OpenGL2_H__
#define __DC_Renderer_OpenGL2_H__

#include "../Renderer.h"

#ifdef DC_DEBUG
    #ifdef DC_THREADS_ENABLED
        #define CHECK_THREAD DC_ASSERT( m_renderThread == thread::Thread::currentThread(), "Accessing OpenGL from another thread" )
    #else
        #define CHECK_THREAD
    #endif  //  DC_DEBUG

    #define DC_CHECK_GL_CONTEXT DC_ASSERT( glGetString( GL_EXTENSIONS ) != NULL, "OpenGL context should be initialized" )
    #define DC_CHECK_GL         sOpenGLErrorCheck __gl_check; CHECK_THREAD
    #define DC_CHECK_GL_ERROR   DC_EXPECT( glGetError() == GL_NO_ERROR, "Unexpected OpenGL error occured" )
#else
    #define DC_CHECK_GL_CONTEXT
    #define DC_CHECK_GL
    #define DC_CHECK_GL_ERROR
#endif

// ** OpenGL headers and libraries
#if defined( DC_PLATFORM_WINDOWS )
    #include <windows.h>

    #include <gl/gl.h>
    #include <gl/glu.h>
    #include "Windows/glext.h"
    #include "Windows/wglext.h"
#endif  //  #if defined( DC_PLATFORM_WINDOWS )

#if defined( DC_PLATFORM_ANDROID )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif  //  #if defined( DC_PLATFORM_ANDROID )

#if defined( DC_PLATFORM_HTML5 )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif  //  #if defined( DC_PLATFORM_HTML5 )

#if defined( DC_PLATFORM_IOS )
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#endif  //  #if defined( DC_PLATFORM_IOS )

#if defined( DC_PLATFORM_MACOS )
    #include <OpenGL/gl.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/glext.h>
#endif  //  #if defined( DC_PLATFORM_MACOS )

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    // ** class OpenGLView
    class OpenGLView : public RenderView {
    public:
        
    };
    
    //! An OpenGL 2 API wrapper.
    class OpenGL2
    {
    public:
        
        //! An internal class that contains all buffer object related functions.
        class Buffer
        {
        public:
            
            //! Binds a buffer object to a pipeline.
            static void     bind(GLenum type, GLuint id);
            
            //! Creates a buffer object with a specified type and returns it's identifier.
            static GLuint   create(GLenum type, const void* data, s32 size, GLenum usage);
            
            //! Updates a part of a buffer.
            static void     subData(GLenum target, GLuint id, GLintptr offset, GLsizeiptr size, const GLvoid* data);
        };
        
        //! An internal class that contains all shader related functions.
        class Program
        {
        public:
            
            //! Contains a uniform information, like type or size
            struct Uniform
            {
                s8          name[32];   //!< A uniform name.
                GLenum      type;       //!< A uniform type.
                GLint       length;     //!< A uniform name length.
                GLint       size;       //!< A uniform size.
            };
            
            //! Compiles a shader program from a source code.
            static GLuint   compileShader(GLenum type, CString source, s8* error, s32 maxErrorSize);
            
            //! Creates a program from an array of shaders.
            static GLuint   createProgram(const GLuint* shaders, s32 count, s8* error, s32 maxErrorSize);
            
            //! Destroys a program.
            static void     deleteProgram(GLuint id);
            
            //! Destroys a shader.
            static void     deleteShader(GLuint id);
            
            //! Returns a total number of active uniforms.
            static GLint    uniformCount(GLuint program);
            
            //! Returns a uniform name.
            static void     uniformAt(GLuint program, GLuint index, Uniform& uniform);
            
            //! Searches for a uniform location.
            static GLint    uniformLocation(GLuint program, CString name);
            
            //! Searches for a uniform location.
            static GLint    uniformLocation(GLuint program, const FixedString& name);
            
            //! Sets a mat4 uniform value.
            static void     uniformMatrix4(GLint location, const f32 value[16], s32 count = 1, GLboolean transpose = GL_FALSE);
            
            //! Sets an array of int uniform value.
            static void     uniform1i(GLint location, const s32* value, s32 count = 1);
            
            //! Sets an int uniform value.
            static void     uniform1i(GLint location, s32 value);
            
            //! Sets an float uniform value.
            static void     uniform1f(GLint location, f32 value);
            
            //! Sets an array of float uniform value.
            static void     uniform1f(GLint location, const f32* value, s32 count = 1);
            
            //! Sets an vec2 uniform value.
            static void     uniform2f(GLint location, const f32* value, s32 count = 1);
            
            //! Sets an vec3 uniform value.
            static void     uniform3f(GLint location, const f32* value, s32 count = 1);
            
            //! Sets an vec4 uniform value.
            static void     uniform4f(GLint location, const f32* value, s32 count = 1);

            //! Binds a shader program.
            static void     use(GLuint program);
        };
        
        //! An internal class that contains all texture related functions.
        class Texture
        {
        public:
            
            //! Creates a 2D texture object with a specified type and returns it's identifier.
            static GLuint   create2D(const void* data, u16 width, u16 height, u16 mipLevels, u32 options);

            //! Creates a cube texture object with a specified type and returns it's identifier.
            static GLuint   createCube(const void* data, u16 size, u16 mipLevels, u32 options);
            
            //! Uploads a image data to a texture.
            static GLsizei  texImage(GLenum target, const GLbyte* data, u16 width, u16 height, s32 mipLevels, PixelFormat pixelFormat);
            
            //! Constructs a depth stencil texture.
            static void     texDepthStencil(GLenum target, u16 width, u16 height, u32 options);
            
            //! Binds a texture object to a pipeline.
            static void     bind(TextureType type, GLuint id, GLuint sampler);
            
            //! Deletes a texture object by an id.
            static void     destroy(GLuint id);
        };
        
        //! An internal class that contains all framebuffer related functions.
        class Framebuffer
        {
        public:
            
            //! Creates a framebuffer.
            static GLuint   create();
            
            //! Attaches a texture target to a framebuffer.
            static void     texture2D(GLuint id, GLenum attachment, GLenum target, GLint level);
            
            //! Attaches a renderbuffer to a framebuffer.
            static GLuint   renderbuffer(GLuint id, GLsizei width, GLsizei height, GLenum attachment, GLenum internalFormat);
            
            //! Binds a framebuffer object to a pipeline.
            static void     bind(GLuint id);
            
            //! Checks the framebuffer status.
            static bool     check(GLuint id);
            
            //! Destroys a renderbuffer object.
            static void     destroyRenderBuffer(GLuint id);
            
            //! Destroys a framebuffer object.
            static void     destroy(GLuint id);
            
            //! Saves current framebuffer.
            static GLuint   save(GLint viewport[4]);
            
            //! Restores current framebuffer.
            static void     restore(GLuint id, GLint viewport[4]);
        };
        
        //! An internal class that contains all stencil buffer related functions.
        class Stencil
        {
        public:
            
            //! Sets a stencil test operations.
            static void     setOperations(StencilAction sfail, StencilAction dfail, StencilAction pass);
            
            //! Sets a stencil test functions.
            static void     setFunction(Compare function, u8 ref, u8 mask);
        };
        

        //! Initializes an OpenGL 2 wrapper.
        static bool     initialize();
        
        //! Clears an active viewport.
        static void     clear(const GLclampf* color, u8 mask, GLclampd depth, GLint stencil);
        
        //! Sets an alpha test function.
        static void     setAlphaTest(Compare function, u8 ref);
        
        //! Sets a triangle side culling
        static void     setCullFace(TriangleFace value);
        
        //! Sets a polygon offset.
        static void     setPolygonOffset(f32 factor, f32 units);
        
        //! Sets a polygon rasterization mode.
        static void     setRasterization(PolygonMode value);
        
        //! Sets a blend mode.
        static void     setBlending(BlendFactor src, BlendFactor dst);
        
        //! Sets a depth state.
        static void     setDepthState(Compare function, bool depthWrite);
        
        //! Sets a color write mask.
        static void     setColorMask(u8 value);
        
        //! Converts a blend factor constant to an OpenGL value.
        static GLenum   convertBlendFactor(BlendFactor value);
        
        //! Converts a triangle face constant to an OpenGL value.
        static GLenum   convertTriangleFace(TriangleFace value);
        
        //! Converts a comparation function constant to an OpenGL value.
        static GLenum   convertCompareFunction(Compare value);
        
        //! Converts a stencil operation constant to an OpenGL value.
        static GLenum   convertStencilAction(StencilAction value);
        
        //! Returns a texture internal format from a pixel format.
        static GLenum   textureInternalFormat(PixelFormat pixelFormat);
        
        //! Returns a texture type from a pixel format.
        static GLenum   textureType(PixelFormat pixelFormat);
        
        //! Returns a texture format from a pixel format.
        static GLenum   textureFormat(PixelFormat pixelFormat);
        
        //! Returns a texture image align from a pixel format.
        static GLenum   textureAlign(PixelFormat pixelFormat);
        
        //! Returns a texture filtering mode.
        static GLenum   textureFilter(TextureFilter filter);
        
        //! Renders an indexed batch of primitives.
        static void     drawElements(PrimitiveType primType, GLenum type, u32 firstIndex, u32 count);

        //! Renders a batch of primitives.
        static void     drawArrays(PrimitiveType primType, u32 offset, u32 count);
        
        //! Enables a vertex buffer layout.
        static void     enableInputLayout(GLbyte* pointer, const VertexBufferLayout& layout);
        
        //! Disables a vertex buffer layout.
        static void     disableInputLayout(const VertexBufferLayout& layout);
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_OpenGL2_H__  */
