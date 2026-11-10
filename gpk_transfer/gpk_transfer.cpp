#include "gpk_transfer.h"

#include <utility>

::gpk::err_t	gpk::STransferManifest::Save	(::gpk::au0_t & output) const {
	if(::gpk::failed(::gpk::savePOD(output, Id)))
		return -1;
	if(::gpk::failed(::gpk::savePOD(output, Size)))
		return -1;
	if(::gpk::failed(::gpk::saveView(output, Resource)))
		return -1;
	return ::gpk::saveView(output, Digest);
}

::gpk::err_t	gpk::STransferManifest::Load	(::gpk::vcu0_t & input) {
	::gpk::vcu0_t	resource	= {};
	::gpk::vcu0_t	digest		= {};
	if(::gpk::failed(::gpk::loadPOD(input, Id)))
		return -1;
	if(::gpk::failed(::gpk::loadPOD(input, Size)))
		return -1;
	if(::gpk::failed(::gpk::loadView(input, resource)))
		return -1;
	if(::gpk::failed(::gpk::loadView(input, digest)))
		return -1;
	Resource	= resource;
	Digest		= digest;
	return 0;
}

::gpk::err_t	gpk::STransferRanges::Save	(::gpk::au0_t & output) const {
	if(::gpk::failed(::gpk::savePOD(output, Id)))
		return -1;
	return ::gpk::saveView(output, Ranges);
}

::gpk::err_t	gpk::STransferRanges::Load	(::gpk::vcu0_t & input) {
	::gpk::view<const ::gpk::STransferRange>	ranges	= {};
	if(::gpk::failed(::gpk::loadPOD(input, Id)))
		return -1;
	if(::gpk::failed(::gpk::loadView(input, ranges)))
		return -1;
	Ranges	= ranges;
	return 0;
}

::gpk::err_t	gpk::STransferWrite::Save	(::gpk::au0_t & output) const {
	if(::gpk::failed(::gpk::savePOD(output, Id)))
		return -1;
	if(::gpk::failed(::gpk::savePOD(output, Range)))
		return -1;
	return ::gpk::saveView(output, Data);
}

::gpk::err_t	gpk::STransferWrite::Load	(::gpk::vcu0_t & input) {
	::gpk::vcu0_t	data	= {};
	if(::gpk::failed(::gpk::loadPOD(input, Id)))
		return -1;
	if(::gpk::failed(::gpk::loadPOD(input, Range)))
		return -1;
	if(::gpk::failed(::gpk::loadView(input, data)))
		return -1;
	Data	= data;
	return 0;
}

static ::gpk::err_t	queueEvent	(const ::gpk::EventTransfer & eventToQueue, ::gpk::TTransferQueue & output) {
	::gpk::PEventTransfer	queued	= {};
	queued->Type	= eventToQueue.Type;
	queued->Data	= eventToQueue.Data;
	const ::gpk::err_t indexEvent	= output.push_back(queued);
	return ::gpk::failed(indexEvent) ? indexEvent : 0;
}

static ::gpk::err_t	saveId	(::gpk::EventTransfer & output, ::gpk::LIFECYCLE_COMMAND type, ::gpk::u3_t id) {
	output.Type	= {};
	output.Data.clear();
	output.Type	= type;
	return ::gpk::savePOD(output.Data, id);
}

::gpk::err_t	gpk::transferEventCreate	(::gpk::EventTransfer & output, const ::gpk::STransferManifest & manifest) {
	output.Type	= {};
	output.Data.clear();
	output.Type	= ::gpk::LIFECYCLE_COMMAND_CREATE;
	return manifest.Save(output.Data);
}

::gpk::err_t	gpk::transferEventResume	(::gpk::EventTransfer & output, ::gpk::u3_t id, ::gpk::view<const ::gpk::STransferRange> durableRanges) {
	output.Type	= {};
	output.Data.clear();
	output.Type	= ::gpk::LIFECYCLE_COMMAND_RESUME;
	::gpk::STransferRanges payload	= {};
	payload.Id		= id;
	payload.Ranges	= durableRanges;
	return payload.Save(output.Data);
}

static ::gpk::err_t	transferEventCommand	(::gpk::EventTransfer & output, ::gpk::COMMAND command, ::gpk::vcu0_t payload) {
	::gpk::SEventCommand	operation	= {command, payload};
	output.Type	= {};
	output.Data.clear();
	output.Type	= ::gpk::LIFECYCLE_COMMAND_UPDATE;
	return operation.Save(output.Data);
}

::gpk::err_t	gpk::transferEventRead	(::gpk::EventTransfer & output, ::gpk::u3_t id, ::gpk::view<const ::gpk::STransferRange> requestedRanges) {
	::gpk::STransferRanges payload	= {};
	payload.Id		= id;
	payload.Ranges	= requestedRanges;
	::gpk::au0_t serialized;
	if(::gpk::failed(payload.Save(serialized)))
		return -1;
	return ::transferEventCommand(output, ::gpk::COMMAND_Read, serialized);
}

::gpk::err_t	gpk::transferEventWrite	(::gpk::EventTransfer & output, ::gpk::u3_t id, const ::gpk::STransferRange & range, ::gpk::vcu0_t data) {
	::gpk::STransferWrite payload	= {};
	payload.Id		= id;
	payload.Range	= range;
	payload.Data	= data;
	::gpk::au0_t serialized;
	if(::gpk::failed(payload.Save(serialized)))
		return -1;
	return ::transferEventCommand(output, ::gpk::COMMAND_Write, serialized);
}

::gpk::err_t	gpk::transferEventStop	(::gpk::EventTransfer & output, ::gpk::u3_t id) {
	return ::saveId(output, ::gpk::LIFECYCLE_COMMAND_STOP, id);
}

::gpk::err_t	gpk::transferEventDestroy	(::gpk::EventTransfer & output, ::gpk::u3_t id) {
	return ::saveId(output, ::gpk::LIFECYCLE_COMMAND_DESTROY, id);
}

static ::gpk::s2_t	transactionFind	(const ::gpk::STransferState & state, ::gpk::u3_t id) {
	for(::gpk::u2_t iTransaction = 0; iTransaction < state.Transactions.size(); ++iTransaction)
		if(state.Transactions[iTransaction].Active && state.Transactions[iTransaction].Manifest.Id == id)
			return (::gpk::s2_t)iTransaction;
	return -1;
}

static ::gpk::STransferTransaction * transactionEnsure	(::gpk::STransferState & state, const ::gpk::STransferManifest & manifest, bool source) {
	const ::gpk::s2_t indexFound	= ::transactionFind(state, manifest.Id);
	if(0 <= indexFound) {
		::gpk::STransferTransaction & transaction	= state.Transactions[indexFound];
		transaction.Active	= true;
		transaction.Source	= source;
		transaction.Manifest	= manifest;
		return &transaction;
	}
	for(::gpk::u2_t iTransaction = 0; iTransaction < state.Transactions.size(); ++iTransaction)
		if(false == state.Transactions[iTransaction].Active) {
			state.Transactions[iTransaction]			= {};
			state.Transactions[iTransaction].Active	= true;
			state.Transactions[iTransaction].Source	= source;
			state.Transactions[iTransaction].Manifest	= manifest;
			return &state.Transactions[iTransaction];
		}
	const ::gpk::err_t indexNew	= state.Transactions.push_back({true, source, manifest, {}});
	return ::gpk::failed(indexNew) ? nullptr : &state.Transactions[indexNew];
}

static void	transactionRelease	(::gpk::STransferState & state, ::gpk::u3_t id) {
	const ::gpk::s2_t indexTransaction	= ::transactionFind(state, id);
	if(0 <= indexTransaction)
		state.Transactions[indexTransaction]	= {};
}

static void	rangesNormalize	(::gpk::apod<::gpk::STransferRange> & ranges, ::gpk::u3_t sizeTotal) {
	::gpk::u2_t countValid	= 0;
	for(::gpk::u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		::gpk::STransferRange range	= ranges[iRange];
		if(0 == range.Count || range.Offset >= sizeTotal)
			continue;
		if(range.Count > sizeTotal - range.Offset)
			range.Count	= sizeTotal - range.Offset;
		ranges[countValid++]	= range;
	}
	ranges.resize(countValid);
	for(::gpk::u2_t iRange = 1; iRange < ranges.size(); ++iRange) {
		const ::gpk::STransferRange value	= ranges[iRange];
		::gpk::u2_t iInsert	= iRange;
		while(iInsert && ranges[iInsert - 1].Offset > value.Offset) {
			ranges[iInsert]	= ranges[iInsert - 1];
			--iInsert;
		}
		ranges[iInsert]	= value;
	}
	::gpk::u2_t countMerged	= 0;
	for(::gpk::u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		if(0 == countMerged) {
			ranges[countMerged++]	= ranges[iRange];
			continue;
		}
		::gpk::STransferRange & previous	= ranges[countMerged - 1];
		const ::gpk::u3_t previousEnd	= previous.Offset + previous.Count;
		const ::gpk::u3_t currentEnd	= ranges[iRange].Offset + ranges[iRange].Count;
		if(ranges[iRange].Offset <= previousEnd) {
			if(currentEnd > previousEnd)
				previous.Count	= currentEnd - previous.Offset;
		}
		else
			ranges[countMerged++]	= ranges[iRange];
	}
	ranges.resize(countMerged);
}

static bool	rangesCover	(::gpk::apod<::gpk::STransferRange> ranges, ::gpk::u3_t sizeTotal) {
	if(0 == sizeTotal)
		return true;
	::rangesNormalize(ranges, sizeTotal);
	return 1 == ranges.size() && 0 == ranges[0].Offset && ranges[0].Count == sizeTotal;
}

static ::gpk::err_t	missingAppend	(::gpk::apod<::gpk::STransferRange> & output, ::gpk::u3_t begin, ::gpk::u3_t end, ::gpk::u2_t blockSize) {
	if(0 == blockSize)
		return -1;
	while(begin < end) {
		const ::gpk::u3_t count	= ::gpk::min((::gpk::u3_t)blockSize, end - begin);
		const ::gpk::err_t indexRange	= output.push_back({begin, count});
		if(::gpk::failed(indexRange))
			return indexRange;
		begin	+= count;
	}
	return 0;
}

static ::gpk::err_t	missingBuild	(::gpk::u3_t sizeTotal, ::gpk::apod<::gpk::STransferRange> durable, ::gpk::u2_t blockSize, ::gpk::apod<::gpk::STransferRange> & output) {
	output.clear();
	::rangesNormalize(durable, sizeTotal);
	::gpk::u3_t cursor	= 0;
	for(::gpk::u2_t iRange = 0; iRange < durable.size(); ++iRange) {
		if(cursor < durable[iRange].Offset && ::gpk::failed(::missingAppend(output, cursor, durable[iRange].Offset, blockSize)))
			return -1;
		cursor	= durable[iRange].Offset + durable[iRange].Count;
	}
	return cursor < sizeTotal ? ::missingAppend(output, cursor, sizeTotal, blockSize) : 0;
}

::gpk::err_t	gpk::transferEnqueueCreate	(::gpk::STransferState & state, const ::gpk::STransferManifest & manifest, ::gpk::TTransferQueue & output) {
	if(nullptr == ::transactionEnsure(state, manifest, true))
		return -1;
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventCreate(eventToQueue, manifest)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

::gpk::err_t	gpk::transferEnqueueResume	(::gpk::u3_t id, ::gpk::view<const ::gpk::STransferRange> durableRanges, ::gpk::TTransferQueue & output) {
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventResume(eventToQueue, id, durableRanges)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

::gpk::err_t	gpk::transferEnqueueRead	(::gpk::u3_t id, ::gpk::view<const ::gpk::STransferRange> requestedRanges, ::gpk::TTransferQueue & output) {
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventRead(eventToQueue, id, requestedRanges)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

::gpk::err_t	gpk::transferEnqueueWrite	(::gpk::u3_t id, const ::gpk::STransferRange & range, ::gpk::vcu0_t data, ::gpk::TTransferQueue & output) {
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventWrite(eventToQueue, id, range, data)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

::gpk::err_t	gpk::transferEnqueueStop	(::gpk::u3_t id, ::gpk::TTransferQueue & output) {
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventStop(eventToQueue, id)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

::gpk::err_t	gpk::transferEnqueueDestroy	(::gpk::u3_t id, ::gpk::TTransferQueue & output) {
	::gpk::EventTransfer eventToQueue;
	if(::gpk::failed(::gpk::transferEventDestroy(eventToQueue, id)))
		return -1;
	return ::queueEvent(eventToQueue, output);
}

static ::gpk::err_t	transferFail	(::gpk::STransferState & state, const ::gpk::STransferConsumer & consumer, const ::gpk::STransferManifest & manifest, ::gpk::TTransferQueue & output, ::gpk::err_t result) {
	if(consumer.RollbackProvisional)
		consumer.RollbackProvisional(manifest);
	::gpk::transferEnqueueStop(manifest.Id, output);
	::transactionRelease(state, manifest.Id);
	return ::gpk::failed(result) ? result : -1;
}

static ::gpk::err_t	transferCommit	(::gpk::STransferState & state, const ::gpk::STransferConsumer & consumer, const ::gpk::STransferManifest & manifest, ::gpk::TTransferQueue & output) {
	if(false == (bool)consumer.ValidateProvisional || false == (bool)consumer.CommitProvisional)
		return ::transferFail(state, consumer, manifest, output, -1);
	::gpk::err_t result	= consumer.ValidateProvisional(manifest);
	if(::gpk::failed(result))
		return ::transferFail(state, consumer, manifest, output, result);
	result	= consumer.CommitProvisional(manifest);
	if(::gpk::failed(result))
		return ::transferFail(state, consumer, manifest, output, result);
	result	= ::gpk::transferEnqueueDestroy(manifest.Id, output);
	::transactionRelease(state, manifest.Id);
	return result;
}

static ::gpk::err_t	transferSendRanges	(::gpk::STransferState & state, const ::gpk::STransferConsumer & consumer, ::gpk::u3_t id, ::gpk::view<const ::gpk::STransferRange> ranges, ::gpk::TTransferQueue & output) {
	const ::gpk::s2_t indexTransaction	= ::transactionFind(state, id);
	if(0 > indexTransaction || false == state.Transactions[indexTransaction].Source || false == (bool)consumer.ReadSourceRange)
		return -1;
	const ::gpk::STransferManifest & manifest	= state.Transactions[indexTransaction].Manifest;
	for(::gpk::u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		const ::gpk::STransferRange & range	= ranges[iRange];
		if(0 == range.Count || range.Offset >= manifest.Size || range.Count > manifest.Size - range.Offset)
			return -1;
		::gpk::au0_t data;
		const ::gpk::err_t resultRead	= consumer.ReadSourceRange(manifest, range, data);
		if(::gpk::failed(resultRead) || data.size() != range.Count)
			return ::gpk::failed(resultRead) ? resultRead : -1;
		const ::gpk::err_t resultQueue	= ::gpk::transferEnqueueWrite(id, range, data, output);
		if(::gpk::failed(resultQueue))
			return resultQueue;
	}
	return 0;
}

::gpk::err_t	gpk::transferProcess	(::gpk::STransferState & state, const ::gpk::STransferConsumer & consumer, const ::gpk::EventTransfer & input, ::gpk::TTransferQueue & output) {
	switch(input.Type) {
	case ::gpk::LIFECYCLE_COMMAND_CREATE: {
		::gpk::vcu0_t inputData	= input.Data;
		::gpk::STransferManifest manifest;
		if(::gpk::failed(manifest.Load(inputData)))
			return -1;
		::gpk::STransferTransaction * transaction	= ::transactionEnsure(state, manifest, false);
		if(nullptr == transaction)
			return -1;
		if(false == (bool)consumer.ResolveCommitted || false == (bool)consumer.ResolveProvisional || false == (bool)consumer.QueryDurableRanges)
			return ::transferFail(state, consumer, manifest, output, -1);
		::gpk::err_t result	= consumer.ResolveCommitted(manifest);
		if(::gpk::failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		result	= consumer.ResolveProvisional(manifest);
		if(::gpk::failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		result	= consumer.QueryDurableRanges(manifest, transaction->Durable);
		if(::gpk::failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		::rangesNormalize(transaction->Durable, manifest.Size);
		if(::rangesCover(transaction->Durable, manifest.Size))
			return ::transferCommit(state, consumer, manifest, output);
		return ::gpk::transferEnqueueResume(manifest.Id, transaction->Durable, output);
	}
	case ::gpk::LIFECYCLE_COMMAND_RESUME: {
		::gpk::vcu0_t inputData	= input.Data;
		::gpk::STransferRanges durable;
		if(::gpk::failed(durable.Load(inputData)))
			return -1;
		const ::gpk::s2_t indexTransaction	= ::transactionFind(state, durable.Id);
		if(0 > indexTransaction || false == state.Transactions[indexTransaction].Source)
			return -1;
		const ::gpk::STransferManifest & manifest	= state.Transactions[indexTransaction].Manifest;
		::gpk::apod<::gpk::STransferRange> missing;
		if(::gpk::failed(::missingBuild(manifest.Size, durable.Ranges, state.BlockSize, missing)))
			return -1;
		return ::transferSendRanges(state, consumer, durable.Id, missing, output);
	}
	case ::gpk::LIFECYCLE_COMMAND_UPDATE: {
		::gpk::vcu0_t inputData	= input.Data;
		::gpk::SEventCommand operation;
		if(::gpk::failed(operation.Load(inputData)))
			return -1;
		if(::gpk::COMMAND_Read == operation.Type) {
			::gpk::vcu0_t operationData	= operation.Data;
			::gpk::STransferRanges requested;
			if(::gpk::failed(requested.Load(operationData)))
				return -1;
			return ::transferSendRanges(state, consumer, requested.Id, requested.Ranges, output);
		}
		if(::gpk::COMMAND_Write == operation.Type) {
			::gpk::vcu0_t operationData	= operation.Data;
			::gpk::STransferWrite write;
			if(::gpk::failed(write.Load(operationData)))
				return -1;
			const ::gpk::s2_t indexTransaction	= ::transactionFind(state, write.Id);
			if(0 > indexTransaction || state.Transactions[indexTransaction].Source)
				return -1;
			::gpk::STransferTransaction & transaction	= state.Transactions[indexTransaction];
			if(0 == write.Range.Count || write.Range.Offset >= transaction.Manifest.Size || write.Range.Count > transaction.Manifest.Size - write.Range.Offset || write.Data.size() != write.Range.Count)
				return ::transferFail(state, consumer, transaction.Manifest, output, -1);
			if(false == (bool)consumer.WriteProvisionalRange || false == (bool)consumer.QueryDurableRanges)
				return ::transferFail(state, consumer, transaction.Manifest, output, -1);
			::gpk::err_t result	= consumer.WriteProvisionalRange(transaction.Manifest, write.Range, write.Data);
			if(::gpk::failed(result))
				return ::transferFail(state, consumer, transaction.Manifest, output, result);
			result	= consumer.QueryDurableRanges(transaction.Manifest, transaction.Durable);
			if(::gpk::failed(result))
				return ::transferFail(state, consumer, transaction.Manifest, output, result);
			::rangesNormalize(transaction.Durable, transaction.Manifest.Size);
			if(::rangesCover(transaction.Durable, transaction.Manifest.Size))
				return ::transferCommit(state, consumer, transaction.Manifest, output);
			return 0;
		}
		return -1;
	}
	case ::gpk::LIFECYCLE_COMMAND_STOP: {
		::gpk::vcu0_t inputData	= input.Data;
		::gpk::u3_t id	= 0;
		if(::gpk::failed(::gpk::loadPOD(inputData, id)))
			return -1;
		const ::gpk::s2_t indexTransaction	= ::transactionFind(state, id);
		if(0 <= indexTransaction && false == state.Transactions[indexTransaction].Source && consumer.RollbackProvisional)
			consumer.RollbackProvisional(state.Transactions[indexTransaction].Manifest);
		::transactionRelease(state, id);
		return 0;
	}
	case ::gpk::LIFECYCLE_COMMAND_DESTROY: {
		::gpk::vcu0_t inputData	= input.Data;
		::gpk::u3_t id	= 0;
		if(::gpk::failed(::gpk::loadPOD(inputData, id)))
			return -1;
		::transactionRelease(state, id);
		return 0;
	}
	case ::gpk::LIFECYCLE_COMMAND_PAUSE:
		return 0;
	default:
		return -1;
	}
}
