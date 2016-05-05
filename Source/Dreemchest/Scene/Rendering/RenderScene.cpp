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

#include "RenderScene.h"
#include "RenderCache.h"
#include "Rvm/RenderingContext.h"
#include "RenderSystem/RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

struct Debug_Structure_To_Track_Data_Size_Used_By_Node_Types {
    enum {
          _Light = sizeof( RenderScene::LightNode )
        , _PointCloud = sizeof( RenderScene::PointCloudNode )
        , _Camera = sizeof( RenderScene::CameraNode )
        , _StaticMesh = sizeof( RenderScene::StaticMeshNode )
        , _StateBlock = sizeof( RenderStateBlock )
        , _State = sizeof( RenderState )
        , _OpCode = sizeof( RenderCommandBuffer::OpCode )
    };
};

// ** RenderScene::CBuffer::Scene::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Scene::Layout[] = {
      { "Scene.ambient", Renderer::ConstantBufferLayout::Vec4, offsetof( RenderScene::CBuffer::Scene, ambient ) }
    , { NULL }
};

// ** RenderScene::CBuffer::Light::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Light::Layout[] = {
      { "Light.position",  Renderer::ConstantBufferLayout::Vec3,  offsetof( CBuffer::Light, position  ), }
    , { "Light.range",     Renderer::ConstantBufferLayout::Float, offsetof( CBuffer::Light, range     ), }
    , { "Light.color",     Renderer::ConstantBufferLayout::Vec3,  offsetof( CBuffer::Light, color     ), }
    , { "Light.intensity", Renderer::ConstantBufferLayout::Float, offsetof( CBuffer::Light, intensity ), }
    , { "Light.direction", Renderer::ConstantBufferLayout::Vec3,  offsetof( CBuffer::Light, direction ), }
    , { "Light.cutoff",    Renderer::ConstantBufferLayout::Float, offsetof( CBuffer::Light, cutoff    ), }
    , { NULL }
};

// ** RenderScene::CBuffer::View::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::View::Layout[] = {
      { "View.transform", Renderer::ConstantBufferLayout::Matrix4, offsetof( CBuffer::View, transform ), }
    , { "View.near",      Renderer::ConstantBufferLayout::Float,   offsetof( CBuffer::View, near      ), }
    , { "View.far",       Renderer::ConstantBufferLayout::Float,   offsetof( CBuffer::View, far       ), }
    , { NULL }
};

// ** RenderScene::CBuffer::Instance::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Instance::Layout[] = {
      { "Instance.transform", Renderer::ConstantBufferLayout::Matrix4, offsetof( CBuffer::Instance, transform ), }
    , { NULL }
};

// ** RenderScene::CBuffer::Material::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Material::Layout[] = {
      { "Material.diffuse",  Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, diffuse ),  }
    , { "Material.specular", Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, specular ), }
    , { "Material.emission", Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, emission ), }
    , { NULL }
};

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene, RenderingContextWPtr context, RenderCacheWPtr cache )
    : m_scene( scene )
    , m_context( context )
    , m_cache( cache )
{
    // Get a parent Ecs instance
    Ecs::EcsWPtr ecs = scene->ecs();

    // Create entity caches
    m_pointClouds   = ecs->createDataCache<PointCloudCache>( Ecs::Aspect::all<PointCloud, Transform>(), dcThisMethod( RenderScene::createPointCloudNode ) );
    m_lights        = ecs->createDataCache<LightCache>( Ecs::Aspect::all<Light, Transform>(), dcThisMethod( RenderScene::createLightNode ) );
    m_cameras       = ecs->createDataCache<CameraCache>( Ecs::Aspect::all<Camera, Transform>(), dcThisMethod( RenderScene::createCameraNode ) );
    m_staticMeshes  = ecs->createDataCache<StaticMeshCache>( Ecs::Aspect::all<StaticMesh, Transform>(), dcThisMethod( RenderScene::createStaticMeshNode ) );

    // Create scene constant buffer
    m_sceneConstants  = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Scene ), CBuffer::Scene::Layout );
    m_sceneParameters = DC_NEW CBuffer::Scene;
    
    // Create a default shader
    m_defaultShader = m_context->createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Null.shader" );
}

// ** RenderScene::create
RenderScenePtr RenderScene::create( SceneWPtr scene, RenderingContextWPtr context, RenderCacheWPtr cache )
{
    return DC_NEW RenderScene( scene, context, cache );
}

// ** RenderScene::scene
SceneWPtr RenderScene::scene( void ) const
{
    return m_scene;
}

// ** RenderScene::pointClouds
const RenderScene::PointClouds& RenderScene::pointClouds( void ) const
{
    return m_pointClouds->data();
}

// ** RenderScene::lights
const RenderScene::Lights& RenderScene::lights( void ) const
{
    return m_lights->data();
}

// ** RenderScene::staticMeshes
const RenderScene::StaticMeshes& RenderScene::staticMeshes( void ) const
{
    return m_staticMeshes->data();
}

// ** RenderScene::findCameraNode
const  RenderScene::CameraNode& RenderScene::findCameraNode( Ecs::EntityWPtr camera ) const
{
    return m_cameras->dataFromEntity( camera );
}

// ** RenderScene::captureFrame
RenderFrameUPtr RenderScene::captureFrame( void )
{
    RenderFrameUPtr frame( DC_NEW RenderFrame );

    // Update active constant buffers
    updateConstantBuffers( *frame.get() );

    // Get a state stack
    RenderStateStack& stateStack = frame->stateStack();

    // Gen a frame entry point command buffer
    RenderCommandBuffer& entryPoint = frame->entryPoint();

    // Push a default state block
    StateScope defaults = stateStack.newScope();
    defaults->disableAlphaTest();
    defaults->disableBlending();
    defaults->setDepthState( Renderer::LessEqual, true );
    defaults->setCullFace( Renderer::TriangleFaceBack );
    defaults->bindProgram( m_context->internShader( m_defaultShader ) );

    // Push a scene state block
    StateScope scene = stateStack.newScope();
    scene->bindConstantBuffer( m_sceneConstants, RenderState::GlobalConstants );

    // Clear all cameras
    const Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        const CameraNode& camera = cameras[i];
        entryPoint
            .renderToTarget( 0, camera.camera->viewport() )
            .clear( camera.camera->clearColor(), camera.camera->clearMask() );
    }

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( *frame.get(), entryPoint );
    }

    return frame;
}

// ** RenderScene::updateConstantBuffers
void RenderScene::updateConstantBuffers( RenderFrame& frame )
{
    // Get a frame entry point command buffer
    RenderCommandBuffer& commands = frame.entryPoint();

    // Update scene constant buffer
    m_sceneParameters->ambient = Rgba( 0.2f, 0.2f, 0.2f, 1.0f );
    commands.uploadConstantBuffer( m_sceneConstants, m_sceneParameters.get(), sizeof( CBuffer::Scene ) );

    // Update camera constant buffers
    Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        CameraNode& node = cameras[i];
        node.parameters->transform = node.camera->calculateViewProjection( node.transform->matrix() );
        node.parameters->near      = node.camera->near();
        node.parameters->far       = node.camera->far();
        commands.uploadConstantBuffer( node.constantBuffer, node.parameters.get(), sizeof CBuffer::View );
    }

    // Update light constant buffers
    Lights& lights = m_lights->data();

    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        LightNode& node = lights[i];
        node.parameters->position  = node.transform->worldSpacePosition();
        node.parameters->intensity = node.light->intensity();
        node.parameters->color     = node.light->color();
        node.parameters->range     = node.light->range();
        node.parameters->direction = node.transform->axisZ();
        node.parameters->cutoff    = cosf( radians( node.light->cutoff() ) );
        commands.uploadConstantBuffer( node.constantBuffer, node.parameters.get(), sizeof CBuffer::Light );
    }

    // Update point cloud constant buffers
    PointClouds& pointClouds = m_pointClouds->data();

    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        PointCloudNode& node = pointClouds[i];
        node.instance.parameters->transform = node.transform->matrix();
        commands.uploadConstantBuffer( node.constantBuffer, node.instance.parameters.get(), sizeof CBuffer::Instance );
    }

    // Update static mesh constant buffers
    StaticMeshes& staticMeshes = m_staticMeshes->data();

    for( s32 i = 0, n = staticMeshes.count(); i < n; i++ ) {
        StaticMeshNode& node = staticMeshes[i];
        node.instance.parameters->transform = node.transform->matrix();
        commands.uploadConstantBuffer( node.constantBuffer, node.instance.parameters.get(), sizeof CBuffer::Instance );
    }
}

// ** RenderScene::createPointCloudNode
RenderScene::PointCloudNode RenderScene::createPointCloudNode( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const PointCloud* pointCloud = entity.get<PointCloud>();

    PointCloudNode node;

    if( const AbstractRenderCache::RenderableNode* renderable = m_cache->createRenderable( pointCloud->vertices(), pointCloud->vertexCount(), pointCloud->vertexFormat() ) ) {
        node.count      = renderable->count;
        node.states     = &renderable->states;
        node.renderable = renderable;
    }

    initializeInstanceNode( entity, node, pointCloud->material() );

    return node;
}

// ** RenderScene::createLightNode
RenderScene::LightNode RenderScene::createLightNode( const Ecs::Entity& entity )
{
    LightNode light;

    light.transform         = entity.get<Transform>();
    light.matrix            = &light.transform->matrix();
    light.light             = entity.get<Light>();
    light.constantBuffer    = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Light ), CBuffer::Light::Layout );
    light.parameters        = DC_NEW CBuffer::Light;

    return light;
}

// ** RenderScene::createCameraNode
RenderScene::CameraNode RenderScene::createCameraNode( const Ecs::Entity& entity )
{
    CameraNode camera;

    camera.transform        = entity.get<Transform>();
    camera.matrix           = &camera.transform->matrix();
    camera.camera           = entity.get<Camera>();
    camera.constantBuffer   = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::View ), CBuffer::View::Layout );
    camera.parameters       = DC_NEW CBuffer::View;

    return camera;
}

// ** RenderScene::createStaticMeshNode
RenderScene::StaticMeshNode RenderScene::createStaticMeshNode( const Ecs::Entity& entity )
{
    StaticMeshNode mesh;

    mesh.mesh = entity.get<StaticMesh>();

    initializeInstanceNode( entity, mesh, mesh.mesh->material(0) );

    const MeshHandle& asset = mesh.mesh->mesh();
    const Mesh&       data  = asset.readLock();

    mesh.count = 0;
    mesh.states = NULL;

    if( const AbstractRenderCache::RenderableNode* cached = m_cache->requestMesh( mesh.mesh->mesh() ) ) {
        mesh.states = &cached->states;
        mesh.count  = cached->count;
    }

    return mesh;
}

// ** RenderScene::initializeInstanceNode
void RenderScene::initializeInstanceNode( const Ecs::Entity& entity, InstanceNode& instance, const MaterialHandle& material )
{
    instance.mask                   = ~0;
    instance.transform              = entity.get<Transform>();
    instance.matrix                 = &instance.transform->matrix();
    instance.constantBuffer         = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Instance ), CBuffer::Instance::Layout );
    instance.instance.parameters    = DC_NEW CBuffer::Instance;
    instance.material.lighting      = -1;
    instance.material.rendering     = -1;
    instance.material.states        = NULL;

    if( material.isValid() ) {
        u8 renderingMasks[] = { RenderMaskOpaque, RenderMaskCutout, RenderMaskTranslucent, RenderMaskAdditive };
        u8 lightingMasks[]  = { RenderMaskUnlit, RenderMaskAmbient, RenderMaskPhong };

        instance.material.rendering = material->renderingMode();
        instance.material.lighting  = material->lightingModel();
        instance.mask               = renderingMasks[instance.material.rendering] | lightingMasks[instance.material.lighting];
    }

    if( const AbstractRenderCache::MaterialNode* cached = m_cache->requestMaterial( material ) ) {
        instance.material.states = &cached->states;
    }
}

} // namespace Scene

DC_END_DREEMCHEST