#include "gpk_engine_entity.h"
#include "gpk_engine_scene.h"
#include "gpk_rigidbody.h"
#include "gpk_view_grid.h"

#ifndef GPK_ENGINE_H
#define GPK_ENGINE_H

namespace gpk 
{
	::gpk::error_t						updateEntityTransforms			
		( uint32_t								iEntity
		, const ::gpk::SVirtualEntity			& entity
		, const ::gpk::SVirtualEntityManager	& managedEntities	
		, ::gpk::SRigidBodyIntegrator			& integrator		
		, ::gpk::SRenderNodeManager				& renderNodes
		);

	struct SEngine {
		::gpk::pobj<::gpk::SEngineScene>	Scene				;
		::gpk::SVirtualEntityManager		ManagedEntities		;
		::gpk::SRigidBodyIntegrator			Integrator			;

		::gpk::error_t						Save				(::gpk::apod<ubyte_t> & output)		const	{
			gpk_necs(Scene			->Save(output));
			gpk_necs(ManagedEntities .Save(output));
			gpk_necs(Integrator		 .Save(output));
			return 0;
		}
		::gpk::error_t						Load				(::gpk::vcub & input) {
			gpk_necs(Scene			->Load(input));
			gpk_necs(ManagedEntities .Load(input));
			gpk_necs(Integrator		 .Load(input));
			return 0;
		}

		// This function messes up with the ManagedEntities array so be careful of not having any references to its contents or it may end up trying to access invalid memory
		::gpk::error_t						Clone							(uint32_t iEntitySource, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			const ::gpk::SVirtualEntity				entitySource					= ManagedEntities[iEntitySource];
			int32_t									iEntityNew						= ManagedEntities.Create();
			::gpk::SVirtualEntity					& entityNew						= ManagedEntities[iEntityNew];
			entityNew.RenderNode				= Scene->ManagedRenderNodes.Clone(entitySource.RenderNode);
			entityNew.RigidBody					= ((uint32_t)entitySource.RigidBody < Integrator.BodyFlags.size()) ? Integrator.Clone(entitySource.RigidBody) : (uint32_t)-1;
			entityNew.Parent					= entitySource.Parent;

			uint32_t								idShaderSource					= Scene->ManagedRenderNodes[entityNew.RenderNode].Shader;
			if(cloneShaders && idShaderSource < Scene->Graphics->Shaders.size()) {
				Scene->ManagedRenderNodes[entityNew.RenderNode].Shader	= Scene->Graphics->Shaders.Clone(idShaderSource);
			}

			uint32_t								idSkinSource					= Scene->ManagedRenderNodes[entityNew.RenderNode].Skin;
			if(cloneSkin && idSkinSource < Scene->Graphics->Skins.size()) {
				uint32_t								idSkin							= Scene->Graphics->Skins.Clone(idSkinSource);
				Scene->ManagedRenderNodes[entityNew.RenderNode].Skin	= idSkin;
				if(cloneSurfaces) {
					if(Scene->Graphics->Skins[idSkin]) {
						::gpk::SSkin							& newSkin						= *Scene->Graphics->Skins[idSkin];
						for(uint32_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture)
							newSkin.Textures[iTexture]			= Scene->Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
					}
				}
			}

			const ::gpk::pobj<::gpk::apod<uint32_t>>	childrenSource	= ManagedEntities.Children[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				::gpk::pobj<::gpk::apod<uint32_t>>			childrenNew		= ManagedEntities.Children[iEntityNew];
				for(uint32_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					uint32_t									entityChild		= Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces, cloneShaders);
					ManagedEntities[entityChild].Parent		= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				ManagedEntities.Children[iEntityNew]	= childrenNew;
			}
			return iEntityNew;
		}

		::gpk::error_t						SetColorDiffuse		(uint32_t iEntity, const ::gpk::SColorFloat & diffuse)				{ Scene->Graphics->Skins[Scene->ManagedRenderNodes[ManagedEntities[iEntity].RenderNode].Skin]->Material.Color.Diffuse = diffuse; return 0; }
		::gpk::error_t						SetMeshScale		(uint32_t iEntity, const ::gpk::n3<float> & scale)					{ Scene->ManagedRenderNodes.BaseTransforms[ManagedEntities[iEntity].RenderNode].World.Scale(scale, true); return 0; }
		::gpk::error_t						SetMeshPosition		(uint32_t iEntity, const ::gpk::n3<float> & position)				{ Scene->ManagedRenderNodes.BaseTransforms[ManagedEntities[iEntity].RenderNode].World.SetTranslation(position, false); return 0; }
		::gpk::error_t						SetShader			(uint32_t iEntity, const ::std::function<::gpk::TFuncPixelShader> & shader, ::gpk::vcs name) {
			const uint32_t							iShader				= Scene->ManagedRenderNodes[ManagedEntities[iEntity].RenderNode].Shader;
			Scene->Graphics->Shaders[iShader].create(shader);
			Scene->Graphics->Shaders.Names[iShader] = name;
			return 0;
		}
		::gpk::error_t						IsPhysicsActive		(uint32_t iEntity)											const	{ return Integrator.Active(ManagedEntities[iEntity].RigidBody) ? 1 : 0; }
		float								GetMass				(uint32_t iEntity)											const	{ return Integrator.GetMass		(ManagedEntities[iEntity].RigidBody);	}

		::gpk::error_t						GetRigidBody		(uint32_t iEntity)											const	{ return ManagedEntities[iEntity].RigidBody; }
		::gpk::error_t						GetRenderNode		(uint32_t iEntity)											const	{ return ManagedEntities[iEntity].RenderNode; }
		::gpk::error_t						GetParentIndex		(uint32_t iEntity)											const	{ return ManagedEntities[iEntity].Parent; }

		::gpk::error_t						GetPosition			(uint32_t iEntity, ::gpk::n3<float> & position)				const	{ Integrator.GetPosition		(ManagedEntities[iEntity].RigidBody, position);			return 0; }
		::gpk::error_t						GetVelocity			(uint32_t iEntity, ::gpk::n3<float> & velocity)				const	{ Integrator.GetVelocity		(ManagedEntities[iEntity].RigidBody, velocity);			return 0; }
		::gpk::error_t						GetAcceleration		(uint32_t iEntity, ::gpk::n3<float> & acceleration)			const	{ Integrator.GetAcceleration	(ManagedEntities[iEntity].RigidBody, acceleration);		return 0; }
		::gpk::error_t						GetRotation			(uint32_t iEntity, ::gpk::n3<float> & velocity)				const	{ Integrator.GetRotation		(ManagedEntities[iEntity].RigidBody, velocity);			return 0; }
		::gpk::error_t						GetOrientation		(uint32_t iEntity, ::gpk::quat<float> & orientation)		const	{ Integrator.GetOrientation		(ManagedEntities[iEntity].RigidBody, orientation);		return 0; }
		::gpk::error_t						SetPhysicsActive	(uint32_t iEntity, bool active)										{ Integrator.SetActive			(ManagedEntities[iEntity].RigidBody, active);			return 0; }
		::gpk::error_t						SetMass				(uint32_t iEntity, float mass)										{ Integrator.SetMass			(ManagedEntities[iEntity].RigidBody, mass);				return 0; }
		::gpk::error_t						SetPosition			(uint32_t iEntity, const ::gpk::n3<float> & position)				{ Integrator.SetPosition		(ManagedEntities[iEntity].RigidBody, position);			return 0; }
		::gpk::error_t						SetVelocity			(uint32_t iEntity, const ::gpk::n3<float> & velocity)				{ Integrator.SetVelocity		(ManagedEntities[iEntity].RigidBody, velocity);			return 0; }
		::gpk::error_t						SetAcceleration		(uint32_t iEntity, const ::gpk::n3<float> & acceleration)			{ Integrator.SetAcceleration	(ManagedEntities[iEntity].RigidBody, acceleration);		return 0; }
		::gpk::error_t						SetRotation			(uint32_t iEntity, const ::gpk::n3<float> & velocity)				{ Integrator.SetRotation		(ManagedEntities[iEntity].RigidBody, velocity);			return 0; }
		::gpk::error_t						SetOrientation		(uint32_t iEntity, const ::gpk::quat<float> & orientation)			{ Integrator.SetOrientation		(ManagedEntities[iEntity].RigidBody, orientation);		return 0; }
		::gpk::error_t						SetDampingLinear	(uint32_t iEntity, float damping)									{ Integrator.Masses[ManagedEntities[iEntity].RigidBody].LinearDamping = damping;		return 0; }
		::gpk::error_t						SetDampingAngular	(uint32_t iEntity, float damping)									{ Integrator.Masses[ManagedEntities[iEntity].RigidBody].AngularDamping = damping;		return 0; }
		::gpk::error_t						SetHidden			(uint32_t iEntity, bool hidden)										{ Scene->ManagedRenderNodes.Flags[ManagedEntities[iEntity].RenderNode].NoDraw = hidden;	return 0; }	

		::gpk::error_t						ToggleHidden			(uint32_t iEntity) {
			::gpk::SRenderNodeFlags					& flags					= Scene->ManagedRenderNodes.Flags[ManagedEntities[iEntity].RenderNode];
			flags.NoDraw						= !flags.NoDraw;
			return 0;
		}

		::gpk::error_t						CreateLight			(::gpk::LIGHT_TYPE type);
		::gpk::error_t						CreateCamera		();
		::gpk::error_t						CreateSphere		();
		::gpk::error_t						CreateCylinder		(uint16_t slices, bool reverse, float diameterRatio);
		::gpk::error_t						CreateBox			();
		::gpk::error_t						CreateCircle		();
		::gpk::error_t						CreateRing			();
		::gpk::error_t						CreateSquare		();
		::gpk::error_t						CreateTriangle		();
		::gpk::error_t						Update				(double secondsLastFrame)			{
			Integrator.Integrate(secondsLastFrame);
			for(uint32_t iEntity = 0; iEntity < ManagedEntities.size(); ++iEntity) {
				::gpk::SVirtualEntity					& entity			= ManagedEntities[iEntity];
				if(entity.Parent != -1)
					continue;

				::gpk::updateEntityTransforms(iEntity, entity, ManagedEntities, Integrator, Scene->ManagedRenderNodes);
			}
			return 0;
		}
	};

#pragma pack(push, 1)
	struct SContactResult {
		::gpk::n3<float>					ContactPosition					= {};
		::gpk::n3<float>					DistanceDirection				= {};
		double								ForceTransferRatioA				= 0;
		double								ForceTransferRatioB				= 0;

		::gpk::n3<float>					InitialVelocityA				= {};
		::gpk::n3<float>					InitialVelocityB				= {};
		::gpk::n3<float>					FinalVelocityA					= {};
		::gpk::n3<float>					FinalVelocityB					= {};

		::gpk::n3<float>					InitialRotationA				= {};
		::gpk::n3<float>					InitialRotationB				= {};
		::gpk::n3<float>					FinalRotationA					= {};
		::gpk::n3<float>					FinalRotationB					= {};
	};

	struct SContact {
		double								Time							= {};
		uint32_t							EntityA							= {};
		uint32_t							EntityB							= {};
		::gpk::n3<float>					Distance						= {};
		double								DistanceLength					= {};
		::gpk::SContactResult				Result							= {};
	};
#pragma pack(pop)

	::gpk::error_t						collisionDetect		(const ::gpk::SEngine & engine, double totalSeconds, ::gpk::apod<::gpk::SContact> & contactsDetected);
} // namespace

#endif
