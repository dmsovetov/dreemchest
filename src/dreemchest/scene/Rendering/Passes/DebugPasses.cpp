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

#include "DebugPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** OverdrawPass::setup
void OverdrawPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Set additive blending for all render passes
	rvm.setRasterization( AllRenderModesBit, Rvm::AdditiveRasterization );

	// Set the default shader
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::ConstantColor ) );

	// Set constant color register
	rvm.setRegister( Rvm::ConstantColor, Vec4( 0.1f, 0.0f, 0.0f ) );
}

// ** WireframePass::setup
void WireframePass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	Rvm::RasterizationOptions additiveWithDepthWrite = Rvm::AdditiveRasterization.overrideDepthMask( true );

	// Set additive blending for all render passes
	rvm.setRasterization( AllRenderModesBit, additiveWithDepthWrite );

	// Set wireframe polygon mode
	rvm.setDefaultPolygonMode( Renderer::PolygonWire );

	// Set the default shader
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::ConstantColor ) );

	// Set constant color register
	rvm.setRegister( Rvm::ConstantColor, Vec4( 0.1f, 0.1f, 0.0f ) );
}

// ** BoundingBoxPass::render
void BoundingBoxPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	Bounds bounds = staticMesh.bounds() * transform.matrix();

	Vec3 x = Vec3( bounds.width(), 0.0f, 0.0f );
	Vec3 y = Vec3( 0.0f, bounds.height(), 0.0f );
	Vec3 z = Vec3( 0.0f, 0.0f, bounds.depth() );

	const Vec3& min = bounds.min();
	const Vec3& max = bounds.max();

	const Rgba color( 1.0f, 1.0f, 0.0f, 0.25f );

	renderer->line( min, min + x, color );
	renderer->line( min, min + y, color );
	renderer->line( min, min + z, color );

	renderer->line( max, max - x, color );
	renderer->line( max, max - y, color );
	renderer->line( max, max - z, color );
}

} // namespace Scene

DC_END_DREEMCHEST