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

#pragma pack(push, 1)
	struct SContactResult {
		::gpk::SCoord3<float>				ContactPosition					= {};
		::gpk::SCoord3<float>				DistanceDirection				= {};
		double								ForceTransferRatioA				= 0;
		double								ForceTransferRatioB				= 0;

		::gpk::SCoord3<float>				InitialVelocityA				= {};
		::gpk::SCoord3<float>				InitialVelocityB				= {};
		::gpk::SCoord3<float>				FinalVelocityA					= {};
		::gpk::SCoord3<float>				FinalVelocityB					= {};

		::gpk::SCoord3<float>				InitialRotationA				= {};
		::gpk::SCoord3<float>				InitialRotationB				= {};
		::gpk::SCoord3<float>				FinalRotationA					= {};
		::gpk::SCoord3<float>				FinalRotationB					= {};
	};

	struct SContact {
		uint32_t							EntityA							= (uint32_t)-1;
		uint32_t							EntityB							= (uint32_t)-1;
		::gpk::SCoord3<float>				Distance						= {};
		double								DistanceLength					= {};
		::gpk::SContactResult				Result							= {};
	};
#pragma pack(pop)

	struct SEngine {
		::gpk::ptr_obj<::gpk::SEngineScene>	Scene				;
		::gpk::SVirtualEntityManager		ManagedEntities		;
		::gpk::SRigidBodyIntegrator			Integrator			;

		::gpk::error_t						Clone				(uint32_t iEntitySource, bool cloneSkin, bool cloneSurfaces) {
			const ::gpk::SVirtualEntity				entitySource		= ManagedEntities.Entities[iEntitySource];
			int32_t									iEntityNew			= ManagedEntities.Create();
			::gpk::SVirtualEntity					& entityNew			= ManagedEntities.Entities[iEntityNew];
			entityNew.RenderNode				= Scene->ManagedRenderNodes	.Clone(entitySource.RenderNode);
			entityNew.RigidBody					= ((uint32_t)entitySource.RigidBody < Integrator.BodyFlags.size()) ? Integrator.Clone(entitySource.RigidBody) : (uint32_t)-1;
			entityNew.Parent					= entitySource.Parent;

			uint32_t								idSkinSource		= Scene->ManagedRenderNodes.RenderNodes[entityNew.RenderNode].Skin;
			if(cloneSkin && idSkinSource < Scene->Graphics->Skins.size()) {
				uint32_t								idSkin						= Scene->Graphics->Skins.Clone(idSkinSource);
				Scene->ManagedRenderNodes.RenderNodes[entityNew.RenderNode].Skin	= idSkin;
				if(cloneSurfaces) {
					if(Scene->Graphics->Skins[idSkin]) {
						::gpk::SSkin							& newSkin					= *Scene->Graphics->Skins[idSkin];
						for(uint32_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture) {
							newSkin.Textures[iTexture]			= Scene->Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
						}
					}
				}
			}

			const ::gpk::ptr_obj<::gpk::array_pod<uint32_t>>	childrenSource	= ManagedEntities.Children[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				::gpk::ptr_obj<::gpk::array_pod<uint32_t>>			childrenNew		= ManagedEntities.Children[iEntityNew];
				for(uint32_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					uint32_t entityChild = Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces);
					ManagedEntities.Entities[entityChild].Parent	= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				ManagedEntities.Children[iEntityNew]		= childrenNew;
			}
			return iEntityNew;
		}

		::gpk::error_t						SetShader			(uint32_t iEntity, const ::std::function<::gpk::TFuncEffect> & shader) {
			Scene->Graphics->Shaders[Scene->ManagedRenderNodes.RenderNodes[ManagedEntities.Entities[iEntity].RenderNode].Shader].create(shader);
			return 0;
		}
		::gpk::error_t						SetMeshScale		(uint32_t iEntity, const ::gpk::SCoord3<float> & scale) {
			Scene->ManagedRenderNodes.BaseTransforms[ManagedEntities.Entities[iEntity].RenderNode].World.Scale(scale, false);
			return 0;
		}

		::gpk::error_t						SetMeshPosition		(uint32_t iEntity, const ::gpk::SCoord3<float> & position) {
			Scene->ManagedRenderNodes.BaseTransforms[ManagedEntities.Entities[iEntity].RenderNode].World.SetTranslation(position, false);
			return 0;
		}

		::gpk::error_t						SetPosition			(uint32_t iEntity, const ::gpk::SCoord3<float> & position) {
			Integrator.SetPosition(ManagedEntities.Entities[iEntity].RigidBody, position);
			return 0;
		}

		::gpk::error_t						SetVelocity			(uint32_t iEntity, const ::gpk::SCoord3<float> & velocity) {
			Integrator.SetVelocity(ManagedEntities.Entities[iEntity].RigidBody, velocity);
			return 0;
		}

		::gpk::error_t						SetAcceleration		(uint32_t iEntity, const ::gpk::SCoord3<float> & acceleration) {
			Integrator.SetAcceleration(ManagedEntities.Entities[iEntity].RigidBody, acceleration);
			return 0;
		}

		::gpk::error_t						SetRotation			(uint32_t iEntity, const ::gpk::SCoord3<float> & velocity) {
			Integrator.SetRotation(ManagedEntities.Entities[iEntity].RigidBody, velocity);
			return 0;
		}

		::gpk::error_t						SetDampingLinear		(uint32_t iEntity, float damping) {
			Integrator.Masses[ManagedEntities.Entities[iEntity].RigidBody].LinearDamping = damping;
			return 0;
		}

		::gpk::error_t						SetDampingAngular		(uint32_t iEntity, float damping) {
			Integrator.Masses[ManagedEntities.Entities[iEntity].RigidBody].AngularDamping = damping;
			return 0;
		}

		::gpk::error_t						SetOrientation			(uint32_t iEntity, const ::gpk::SQuaternion<float> & orientation) {
			Integrator.SetOrientation(ManagedEntities.Entities[iEntity].RigidBody, orientation);
			return 0;
		}

		::gpk::error_t						SetHidden				(uint32_t iEntity, bool hidden) {
			Scene->ManagedRenderNodes.Flags[ManagedEntities.Entities[iEntity].RenderNode].NoDraw = hidden;
			return 0;
		}

		::gpk::error_t						ToggleHidden			(uint32_t iEntity) {
			::gpk::SRenderNodeFlags					& flags					= Scene->ManagedRenderNodes.Flags[ManagedEntities.Entities[iEntity].RenderNode];
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
			for(uint32_t iEntity = 0; iEntity < ManagedEntities.Entities.size(); ++iEntity) {
				::gpk::SVirtualEntity					& entity			= ManagedEntities.Entities[iEntity];
				if(entity.Parent != -1)
					continue;

				::gpk::updateEntityTransforms(iEntity, entity, ManagedEntities, Integrator, Scene->ManagedRenderNodes);
			}
			return 0;
		}
	};

	::gpk::error_t						collisionDetect		(const ::gpk::SEngine & engine, ::gpk::array_pod<::gpk::SContact> & contactsDetected);
} // namespace

#endif
