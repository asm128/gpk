#include "gpk_apod_serialize.h"
#include "gpk_vox.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(VOX_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, TAG_MATCH			, 0, "SVOXChunkHeader::IsType() did not match exactly four tag bytes.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, LOAD_RESULT			, 1, "SVOXData::Load() rejected a valid SIZE chunk.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, LOAD_CONSUMPTION	, 2, "SVOXData::Load() did not consume the valid input.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, LOAD_CHUNK			, 3, "SVOXData::Load() did not retain the SIZE chunk.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, DIMENSIONS			, 4, "SVOXData::GetDimensions() did not apply the established YZX coordinate order.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, TRUNCATED_HEADER	, 5, "SVOXData::Load() accepted a truncated chunk header.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, TRUNCATED_CHUNK	, 6, "SVOXData::Load() accepted a chunk whose declared payload was missing.");
GDEFINE_ENUM_VALUED(VOX_TEST_RESULT, TRUNCATED_DICT		, 7, "voxDictLoad() accepted a truncated dictionary count.");

sttc void setTag(::gpk::SVOXChunkHeader & header, cnst char (&tag)[5]) {
	::memcpy(header.Type.Storage, tag, 4);
}

::gpk::err_t test_vox(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::SVOXChunkHeader typeHeader = {};
	setTag(typeHeader, "SIZE");
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_TAG_MATCH, false == typeHeader.IsType("SIZE") || typeHeader.IsType("PACK"));

	::gpk::SVOXFileHeader fileHeader = {};
	::memcpy(fileHeader.MagicNumber.Storage, "VOX ", 4);
	fileHeader.Version = 150;
	::gpk::SVOXChunkHeader sizeHeader = {};
	setTag(sizeHeader, "SIZE");
	sizeHeader.ChunkSize = szof(::gpk::n3u2_t);
	cnst ::gpk::n3u2_t sourceDimensions = {2, 3, 4};
	::gpk::au0_t encoded = {};
	if_fail_fe(::gpk::savePOD(encoded, fileHeader));
	if_fail_fe(::gpk::savePOD(encoded, sizeHeader));
	if_fail_fe(::gpk::savePOD(encoded, sourceDimensions));
	::gpk::vcu0_t input = encoded;
	::gpk::SVOXData vox = {};
	cnst ::gpk::err_t loadResult = vox.Load(input);
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_LOAD_RESULT, loadResult < 0);
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_LOAD_CONSUMPTION, input.size());
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_LOAD_CHUNK, vox.Chunks.size() != 1 || false == vox.Chunks[0].Header.IsType("SIZE"));
	cnst ::gpk::n3u0_t expectedDimensions = {3, 4, 2};
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_DIMENSIONS, vox.GetDimensions() != expectedDimensions);

	::gpk::au0_t truncatedHeader = {};
	if_fail_fe(::gpk::savePOD(truncatedHeader, fileHeader));
	cnst ::gpk::u2_t partialChunkHeader = 0;
	if_fail_fe(::gpk::savePOD(truncatedHeader, partialChunkHeader));
	::gpk::vcu0_t truncatedHeaderInput = truncatedHeader;
	::gpk::SVOXData truncatedHeaderVox = {};
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_TRUNCATED_HEADER, 0 <= truncatedHeaderVox.Load(truncatedHeaderInput));

	::gpk::SVOXChunkHeader missingPayloadHeader = {};
	setTag(missingPayloadHeader, "JUNK");
	missingPayloadHeader.ChunkSize = 8;
	::gpk::au0_t truncatedChunk = {};
	if_fail_fe(::gpk::savePOD(truncatedChunk, fileHeader));
	if_fail_fe(::gpk::savePOD(truncatedChunk, missingPayloadHeader));
	::gpk::vcu0_t truncatedChunkInput = truncatedChunk;
	::gpk::SVOXData truncatedChunkVox = {};
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_TRUNCATED_CHUNK, 0 <= truncatedChunkVox.Load(truncatedChunkInput));

	cnst ::gpk::u0_t oneByte[] = {0};
	::gpk::vcu0_t truncatedDictInput = oneByte;
	::gpk::SVOXDict attributes = {};
	GPK_TEST_CHECK(errors, VOX_TEST_RESULT_TRUNCATED_DICT, 0 <= ::gpk::voxDictLoad(attributes, truncatedDictInput));
	return 0;
}
