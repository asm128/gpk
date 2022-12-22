#include "gpk_engine_d3d_shader.h"

#if defined(GPK_WINDOWS)
//
//int32_t												gpk::shaderD3DWireframe
//	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
//	, ::gpk::view_grid<uint32_t>			backBufferDepth
//	, ::gpk::SEngineRenderCache				& renderCache
//	, const ::gpk::SEngineScene				& scene
//	, const ::gpk::SEngineSceneConstants	& constants
//	, int32_t								iRenderNode
//	) {
//	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
//	const ::gpk::SGeometryMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
//	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];
//
//	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());
//
//	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
//	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
//	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
//	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
//	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
//	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
//	const ::gpk::SRenderMaterial							& material					= skin.Material;
//	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
//
//	return 0;//drawBuffersWireframe(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, material, worldTransform, constants);
//}

#endif