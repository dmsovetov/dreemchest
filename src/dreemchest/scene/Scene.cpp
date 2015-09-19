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

#include "Scene.h"

#include "Rendering/RenderingSystem.h"
#include "Rendering/Rvm.h"
#include "Assets/Assets.h"
#include "Assets/Material.h"
#include "Assets/Mesh.h"
#include "Components/Rendering.h"
#include "Components/Transform.h"

#include "Components/Input.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

IMPLEMENT_LOGGER( log )

// ** Scene::Scene
Scene::Scene( void ) : m_systems( m_entities ), m_nextEntityId( 1 )
{
	m_cameras	= Ecs::Family::create( m_entities, "Cameras", Ecs::Aspect::all<Camera>() );
	m_named		= Ecs::Family::create( m_entities, "Named Entities", Ecs::Aspect::all<Identifier>() );
}

// ** Scene::update
void Scene::update( f32 dt )
{
	m_entities.notify();
	m_systems.update( 0, dt );
}

// ** Scene::createSceneObject
SceneObjectPtr Scene::createSceneObject( void )
{
	return m_entities.registerEntity( m_nextEntityId++ );
}

// ** Scene::render
void Scene::render( const RenderingContextPtr& context )
{
	// Update all rendering systems
	for( u32 i = 0; i < ( u32 )m_renderingSystems.size(); i++ ) {
		m_renderingSystems[i]->render( context );
	}

	// Now reset internal Camera::ClearDisabled flag for all cameras in scene
	const Ecs::EntitySet& cameras = m_cameras->entities();

	for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera* camera = (*i)->get<Camera>();
		camera->setClearMask( camera->clearMask() & ~Camera::ClearDisabled );
	}

	context->rvm()->reset();
}

// ** Scene::cameras
const Ecs::FamilyPtr& Scene::cameras( void ) const
{
	return m_cameras;
}

// ** Scene::systems
Ecs::Systems& Scene::systems( void )
{
	return m_systems;
}

// ** Scene::findAllWithName
SceneObjectsList Scene::findAllWithName( const String& name ) const
{
	const Ecs::EntitySet& entities = m_named->entities();
	SceneObjectsList objects;

	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		SceneObjectPtr sceneObject = *i;

		if( sceneObject->get<Identifier>()->name() == name ) {
			objects.push_back( sceneObject );
		}
	}

	return objects;
}

// ** Scene::create
ScenePtr Scene::create( void )
{
	return ScenePtr( DC_NEW Scene );
}

// ------------------------------------------------- JsonSceneLoader ------------------------------------------------- //

// ** JsonSceneLoader::JsonSceneLoader
JsonSceneLoader::JsonSceneLoader( void )
{
	m_loaders["Transform"]	= dcThisMethod( JsonSceneLoader::readTransform );
	m_loaders["Renderer"]	= dcThisMethod( JsonSceneLoader::readRenderer );
	m_loaders["Camera"]		= dcThisMethod( JsonSceneLoader::readCamera );
	m_loaders["Light"]		= dcThisMethod( JsonSceneLoader::readLight );
}

// ** JsonSceneLoader::load
bool JsonSceneLoader::load( ScenePtr scene, const AssetBundlePtr& assets, const String& json )
{
	Json::Reader reader;

	// Parse the JSON string
	if( !reader.parse( json, m_json ) ) {
		return false;
	}

	// Set the asset bundle reference
	m_assets = assets;

	// Save the scene reference.
	m_scene = scene;

	// Read objects from JSON
	for( Json::ValueIterator i = m_json.begin(), end = m_json.end(); i != end; ++i ) {
		// Get the instance type.
		String type = i->get( "class", "" ).asString();

		// Read the scene object.
		if( type == "SceneObject" ) {
			requestSceneObject( i.key().asString() );
			continue;
		}

		// Read the component
		Ecs::ComponentPtr component = requestComponent( i.key().asString() );

		// Get the scene object to attach the component to.
		Ecs::EntityPtr entity = requestSceneObject( i->get( "sceneObject", Json::Value() ).asString() );

		// Attach the component.
		entity->attach( component.get() );
	}

	// Update the scene to populate all families and systems
	m_scene->update( 0.0f );

	return true;
}

// ** JsonSceneLoader::requestComponent
Ecs::ComponentPtr JsonSceneLoader::requestComponent( const String& id )
{
	Components::iterator i = m_components.find( id );

	if( i != m_components.end() ) {
		return i->second;
	}

	// Get the data
	Json::Value data = m_json.get( id, Json::Value() );

	// Get the component type
	String type = data.get( "class", "" ).asString();

	// Get the component loader.
	ComponentLoaders::iterator j = m_loaders.find( type );

	if( j == m_loaders.end() ) {
		log::error( "JsonSceneLoader::requestComponent : unknown object type '%s'\n", type.c_str() );
		return Ecs::ComponentPtr();
	}

	// Read the component.
	Ecs::ComponentPtr component = j->second( data );

	// Save parsed component
	m_components[id] = component;

	return component;
}

// ** JsonSceneLoader::requestSceneObject
Ecs::EntityPtr JsonSceneLoader::requestSceneObject( const String& id )
{
	SceneObjects::iterator i = m_sceneObjects.find( id );

	if( i != m_sceneObjects.end() ) {
		return i->second;
	}

	Json::Value data = m_json.get( id, Json::Value() );
	Ecs::EntityPtr sceneObject = m_scene->createSceneObject();

	sceneObject->attach<Identifier>( data["name"].asString() );
	m_sceneObjects[id] = sceneObject;

	return sceneObject;
}

// ** JsonSceneLoader::readTransform
Ecs::ComponentPtr JsonSceneLoader::readTransform( const Json::Value& value )
{
	Vec3 position = readVec3( value["position"] );
	Vec3 scale    = readVec3( value["scale"] );
	Quat rotation = readQuat( value["rotation"] );

	Transform* result = DC_NEW Transform;
	result->setPosition( Vec3( -position.x, position.y, position.z ) );
	result->setScale( scale );
	result->setRotation( Quat( -rotation.x, rotation.y, rotation.z, -rotation.w ) );

	Json::Value parent = value["parent"];

	if( parent != Json::nullValue ) {
		Ecs::ComponentPtr component = requestComponent( parent.asString() );
		result->setParent( castTo<Transform>( component.get() ) );
	}

	return result;
}

// ** JsonSceneLoader::readCamera
Ecs::ComponentPtr JsonSceneLoader::readCamera( const Json::Value& value )
{
	Camera* result = DC_NEW Camera;
	result->setFov( value["fov"].asFloat() );
	result->setNear( value["near"].asFloat() );
	result->setFar( value["far"].asFloat() );
	result->setClearColor( readRgba( value["backgroundColor"] ) );
	result->setNdc( readRect( value["ndc"] ) );

	return result;
}

// ** JsonSceneLoader::readRenderer
Ecs::ComponentPtr JsonSceneLoader::readRenderer( const Json::Value& value )
{
	StaticMesh* result = DC_NEW StaticMesh;
	String		asset  = value["asset"].asString();

	result->setMesh( m_assets->find<Mesh>( asset ) );

	Json::Value materials = value["materials"];

	for( u32 i = 0; i < materials.size(); i++ ) {
		result->setMaterial( i, m_assets->find<Material>( materials[i].asString() ) );
	}

	return result;
}

// ** JsonSceneLoader::readLight
Ecs::ComponentPtr JsonSceneLoader::readLight( const Json::Value& value )
{
	Light::Type types[] = { Light::Spot, Light::Directional, Light::Point };

	Light* result = DC_NEW Light;
	result->setColor( readRgb( value["color"] ) );
	result->setIntensity( value["intensity"].asFloat() );
	result->setRange( value["range"].asFloat() );
	result->setType( types[value["type"].asInt()] );

	return result;
}

// ** JsonSceneLoader::readVec3
Vec3 JsonSceneLoader::readVec3( const Json::Value& value )
{
	return Vec3( value[0].asFloat(), value[1].asFloat(), value[2].asFloat() );
}

// ** JsonSceneLoader::readRect
Rect JsonSceneLoader::readRect( const Json::Value& value )
{
	return Rect( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

// ** JsonSceneLoader::readRgba
Rgba JsonSceneLoader::readRgba( const Json::Value& value )
{
	return Rgba( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

// ** JsonSceneLoader::readRgb
Rgb JsonSceneLoader::readRgb( const Json::Value& value )
{
	return Rgb( value[0].asFloat(), value[1].asFloat(), value[2].asFloat() );
}

// ** JsonSceneLoader::readQuat
Quat JsonSceneLoader::readQuat( const Json::Value& value )
{
	return Quat( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

} // namespace Scene

DC_END_DREEMCHEST