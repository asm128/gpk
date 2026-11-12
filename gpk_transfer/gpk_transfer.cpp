#include "gpk_transfer.h"

#include <utility>

usns gpk;


gpk::err_t	gpk::STransferManifest::Save	(au0_t & output) const {
	if_fail_fe(savePOD(output, Id));
	if_fail_fe(savePOD(output, Size));
	if_fail_fe(saveView(output, Resource));
	return saveView(output, Digest);
}

gpk::err_t	gpk::STransferManifest::Load	(vcu0_t & input) {
	vcu0_t	resource	= {};
	vcu0_t	digest		= {};
	if_fail_fe(loadPOD(input, Id));
	if_fail_fe(loadPOD(input, Size));
	if_fail_fe(loadView(input, resource));
	if_fail_fe(loadView(input, digest));
	Resource	= resource;
	Digest		= digest;
	return 0;
}

gpk::err_t	gpk::STransferRanges::Save	(au0_t & output) const {
	if_fail_fe(savePOD(output, Id));
	return saveView(output, Ranges);
}

gpk::err_t	gpk::STransferRanges::Load	(vcu0_t & input) {
	view<const STransferRange>	ranges	= {};
	if_fail_fe(loadPOD(input, Id));
	if_fail_fe(loadView(input, ranges));
	Ranges	= ranges;
	return 0;
}

gpk::err_t	gpk::STransferWrite::Save	(au0_t & output) const {
	if_fail_fe(savePOD(output, Id));
	if_fail_fe(savePOD(output, Range));
	return saveView(output, Data);
}

gpk::err_t	gpk::STransferWrite::Load	(vcu0_t & input) {
	vcu0_t	data	= {};
	if_fail_fe(loadPOD(input, Id));
	if_fail_fe(loadPOD(input, Range));
	if_fail_fe(loadView(input, data));
	Data	= data;
	return 0;
}

static gpk::err_t	queueEvent	(const EventTransfer & eventToQueue, TTransferQueue & output) {
	PEventTransfer	queued	= {};
	queued->Type	= eventToQueue.Type;
	queued->Data	= eventToQueue.Data;
	const gpk::err_t indexEvent	= output.push_back(queued);
	return gpk::failed(indexEvent) ? indexEvent : 0;
}

static gpk::err_t	saveId	(gpk::EventTransfer & output, LIFECYCLE_COMMAND type, u3_t id) {
	output.Data.clear();
	output.Type	= type;
	return savePOD(output.Data, id);
}

gpk::err_t	gpk::transferEventCreate	(EventTransfer & output, const STransferManifest & manifest) {
	output = LIFECYCLE_COMMAND_CREATE;
	return manifest.Save(output.Data);
}

gpk::err_t	gpk::transferEventResume	(EventTransfer & output, u3_t id, view<const STransferRange> durableRanges) {
	output = LIFECYCLE_COMMAND_RESUME;
	STransferRanges payload	= {};
	payload.Id		= id;
	payload.Ranges	= durableRanges;
	return payload.Save(output.Data);
}

static gpk::err_t	transferEventCommand	(EventTransfer & output, gpk::COMMAND command, gpk::vcu0_t payload) {
	SEventCommand	operation	= {command, payload};
	output = LIFECYCLE_COMMAND_UPDATE;
	return operation.Save(output.Data);
}

gpk::err_t	gpk::transferEventRead	(EventTransfer & output, u3_t id, view<const STransferRange> requestedRanges) {
	STransferRanges payload	= {};
	payload.Id		= id;
	payload.Ranges	= requestedRanges;
	au0_t serialized;
	if_fail_fe(payload.Save(serialized));
	return ::transferEventCommand(output, COMMAND_Read, serialized);
}

gpk::err_t	gpk::transferEventWrite	(EventTransfer & output, u3_t id, const STransferRange & range, vcu0_t data) {
	STransferWrite payload	= {};
	payload.Id		= id;
	payload.Range	= range;
	payload.Data	= data;
	au0_t serialized;
	if_fail_fe(payload.Save(serialized));
	return ::transferEventCommand(output, COMMAND_Write, serialized);
}

gpk::err_t	gpk::transferEventStop	(EventTransfer & output, u3_t id) {
	return ::saveId(output, LIFECYCLE_COMMAND_STOP, id);
}

gpk::err_t	gpk::transferEventDestroy	(EventTransfer & output, u3_t id) {
	return ::saveId(output, LIFECYCLE_COMMAND_DESTROY, id);
}

static s2_t	transactionFind	(const gpk::STransferState & state, u3_t id) {
	for(u2_t iTransaction = 0; iTransaction < state.Transactions.size(); ++iTransaction)
		if(state.Transactions[iTransaction].Active && state.Transactions[iTransaction].Manifest.Id == id)
			return (s2_t)iTransaction;
	return -1;
}

static STransferTransaction * transactionEnsure	(STransferState & state, const STransferManifest & manifest, bool source) {
	const s2_t indexFound	= ::transactionFind(state, manifest.Id);
	if(0 <= indexFound) {
		STransferTransaction & transaction	= state.Transactions[indexFound];
		transaction.Active	= true;
		transaction.Source	= source;
		transaction.Manifest	= manifest;
		return &transaction;
	}
	for(u2_t iTransaction = 0; iTransaction < state.Transactions.size(); ++iTransaction)
		if(false == state.Transactions[iTransaction].Active) {
			state.Transactions[iTransaction]			= {};
			state.Transactions[iTransaction].Active	= true;
			state.Transactions[iTransaction].Source	= source;
			state.Transactions[iTransaction].Manifest	= manifest;
			return &state.Transactions[iTransaction];
		}
	const gpk::err_t indexNew	= state.Transactions.push_back({true, source, manifest, {}});
	return failed(indexNew) ? nullptr : &state.Transactions[indexNew];
}

static void	transactionRelease	(STransferState & state, u3_t id) {
	const s2_t indexTransaction	= ::transactionFind(state, id);
	if(0 <= indexTransaction)
		state.Transactions[indexTransaction]	= {};
}

static void	rangesNormalize	(apod<STransferRange> & ranges, u3_t sizeTotal) {
	u2_t countValid	= 0;
	for(u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		STransferRange range	= ranges[iRange];
		if(0 == range.Count || range.Offset >= sizeTotal)
			continue;
		if(range.Count > sizeTotal - range.Offset)
			range.Count	= sizeTotal - range.Offset;
		ranges[countValid++]	= range;
	}
	ranges.resize(countValid);
	for(u2_t iRange = 1; iRange < ranges.size(); ++iRange) {
		const STransferRange value	= ranges[iRange];
		u2_t iInsert	= iRange;
		while(iInsert && ranges[iInsert - 1].Offset > value.Offset) {
			ranges[iInsert]	= ranges[iInsert - 1];
			--iInsert;
		}
		ranges[iInsert]	= value;
	}
	u2_t countMerged	= 0;
	for(u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		if(0 == countMerged) {
			ranges[countMerged++]	= ranges[iRange];
			continue;
		}
		STransferRange & previous	= ranges[countMerged - 1];
		const u3_t previousEnd	= previous.Offset + previous.Count;
		const u3_t currentEnd	= ranges[iRange].Offset + ranges[iRange].Count;
		if(ranges[iRange].Offset <= previousEnd) {
			if(currentEnd > previousEnd)
				previous.Count	= currentEnd - previous.Offset;
		}
		else
			ranges[countMerged++]	= ranges[iRange];
	}
	ranges.resize(countMerged);
}

static bool	rangesCover	(apod<STransferRange> ranges, u3_t sizeTotal) {
	if(0 == sizeTotal)
		return true;
	::rangesNormalize(ranges, sizeTotal);
	return 1 == ranges.size() && 0 == ranges[0].Offset && ranges[0].Count == sizeTotal;
}

static gpk::err_t	missingAppend	(apod<STransferRange> & output, u3_t begin, u3_t end, u2_t blockSize) {
	if(0 == blockSize)
		return -1;
	while(begin < end) {
		const u3_t count	= min((u3_t)blockSize, end - begin);
		const gpk::err_t indexRange	= output.push_back({begin, count});
		if(failed(indexRange))
			return indexRange;
		begin	+= count;
	}
	return 0;
}

static gpk::err_t	missingBuild	(u3_t sizeTotal, apod<STransferRange> durable, u2_t blockSize, apod<STransferRange> & output) {
	output.clear();
	::rangesNormalize(durable, sizeTotal);
	u3_t cursor	= 0;
	for(u2_t iRange = 0; iRange < durable.size(); ++iRange) {
		if(cursor < durable[iRange].Offset && failed(::missingAppend(output, cursor, durable[iRange].Offset, blockSize)))
			return -1;
		cursor	= durable[iRange].Offset + durable[iRange].Count;
	}
	return cursor < sizeTotal ? ::missingAppend(output, cursor, sizeTotal, blockSize) : 0;
}

gpk::err_t	gpk::transferEnqueueCreate	(STransferState & state, const STransferManifest & manifest, TTransferQueue & output) {
	if(nullptr == ::transactionEnsure(state, manifest, true))
		return -1;
	EventTransfer eventToQueue;
	if_fail_fe(transferEventCreate(eventToQueue, manifest));
	return ::queueEvent(eventToQueue, output);
}

gpk::err_t	gpk::transferEnqueueResume	(u3_t id, view<const STransferRange> durableRanges, TTransferQueue & output) {
	EventTransfer eventToQueue;
	if_fail_fe(transferEventResume(eventToQueue, id, durableRanges));
	return ::queueEvent(eventToQueue, output);
}

gpk::err_t	gpk::transferEnqueueRead	(u3_t id, view<const STransferRange> requestedRanges, TTransferQueue & output) {
	EventTransfer eventToQueue;
	if_fail_fe(transferEventRead(eventToQueue, id, requestedRanges));
	return ::queueEvent(eventToQueue, output);
}

gpk::err_t	gpk::transferEnqueueWrite	(u3_t id, const STransferRange & range, vcu0_t data, TTransferQueue & output) {
	EventTransfer eventToQueue;
	if_fail_fe(transferEventWrite(eventToQueue, id, range, data));
	return ::queueEvent(eventToQueue, output);
}

gpk::err_t	gpk::transferEnqueueStop	(u3_t id, TTransferQueue & output) {
	EventTransfer eventToQueue;
	if_fail_fe(transferEventStop(eventToQueue, id));
	return ::queueEvent(eventToQueue, output);
}

gpk::err_t	gpk::transferEnqueueDestroy	(u3_t id, TTransferQueue & output) {
	EventTransfer eventToQueue;
	if_fail_fe(transferEventDestroy(eventToQueue, id));
	return ::queueEvent(eventToQueue, output);
}

static gpk::err_t	transferFail	(STransferState & state, const STransferConsumer & consumer, const STransferManifest & manifest, TTransferQueue & output, gpk::err_t result) {
	if(consumer.RollbackProvisional)
		consumer.RollbackProvisional(manifest);
	transferEnqueueStop(manifest.Id, output);
	::transactionRelease(state, manifest.Id);
	return failed(result) ? result : -1;
}

static gpk::err_t	transferCommit	(STransferState & state, const STransferConsumer & consumer, const STransferManifest & manifest, TTransferQueue & output) {
	if(false == (bool)consumer.ValidateProvisional || false == (bool)consumer.CommitProvisional)
		return ::transferFail(state, consumer, manifest, output, -1);
	gpk::err_t result	= consumer.ValidateProvisional(manifest);
	if(failed(result))
		return ::transferFail(state, consumer, manifest, output, result);
	result	= consumer.CommitProvisional(manifest);
	if(failed(result))
		return ::transferFail(state, consumer, manifest, output, result);
	result	= transferEnqueueDestroy(manifest.Id, output);
	::transactionRelease(state, manifest.Id);
	return result;
}

static gpk::err_t	transferSendRanges	(STransferState & state, const STransferConsumer & consumer, u3_t id, view<const STransferRange> ranges, TTransferQueue & output) {
	const s2_t indexTransaction	= ::transactionFind(state, id);
	if(0 > indexTransaction || false == state.Transactions[indexTransaction].Source || false == (bool)consumer.ReadSourceRange)
		return -1;
	const STransferManifest & manifest	= state.Transactions[indexTransaction].Manifest;
	for(u2_t iRange = 0; iRange < ranges.size(); ++iRange) {
		const STransferRange & range	= ranges[iRange];
		if(0 == range.Count || range.Offset >= manifest.Size || range.Count > manifest.Size - range.Offset)
			return -1;
		au0_t data;
		const gpk::err_t resultRead	= consumer.ReadSourceRange(manifest, range, data);
		if(failed(resultRead) || data.size() != range.Count)
			return failed(resultRead) ? resultRead : -1;
		const gpk::err_t resultQueue	= transferEnqueueWrite(id, range, data, output);
		if(failed(resultQueue))
			return resultQueue;
	}
	return 0;
}

gpk::err_t	gpk::transferProcess	(STransferState & state, const STransferConsumer & consumer, const EventTransfer & input, TTransferQueue & output) {
	switch(input.Type) {
	case LIFECYCLE_COMMAND_CREATE: {
		vcu0_t inputData	= input.Data;
		STransferManifest manifest;
		if_fail_fe(manifest.Load(inputData));
		STransferTransaction * transaction	= ::transactionEnsure(state, manifest, false);
		if(nullptr == transaction)
			return -1;
		if(false == (bool)consumer.ResolveCommitted || false == (bool)consumer.ResolveProvisional || false == (bool)consumer.QueryDurableRanges)
			return ::transferFail(state, consumer, manifest, output, -1);
		gpk::err_t result	= consumer.ResolveCommitted(manifest);
		if(failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		result	= consumer.ResolveProvisional(manifest);
		if(failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		result	= consumer.QueryDurableRanges(manifest, transaction->Durable);
		if(failed(result))
			return ::transferFail(state, consumer, manifest, output, result);
		::rangesNormalize(transaction->Durable, manifest.Size);
		if(::rangesCover(transaction->Durable, manifest.Size))
			return ::transferCommit(state, consumer, manifest, output);
		return transferEnqueueResume(manifest.Id, transaction->Durable, output);
	}
	case LIFECYCLE_COMMAND_RESUME: {
		vcu0_t inputData	= input.Data;
		STransferRanges durable;
		if_fail_fe(durable.Load(inputData));
		const s2_t indexTransaction	= ::transactionFind(state, durable.Id);
		if(0 > indexTransaction || false == state.Transactions[indexTransaction].Source)
			return -1;
		const STransferManifest & manifest	= state.Transactions[indexTransaction].Manifest;
		apod<STransferRange> missing;
		if_fail_fe(::missingBuild(manifest.Size, durable.Ranges, state.BlockSize, missing));
		return ::transferSendRanges(state, consumer, durable.Id, missing, output);
	}
	case LIFECYCLE_COMMAND_UPDATE: {
		vcu0_t inputData	= input.Data;
		SEventCommand operation;
		if_fail_fe(operation.Load(inputData));
		if(COMMAND_Read == operation.Type) {
			vcu0_t operationData	= operation.Data;
			STransferRanges requested;
			if_fail_fe(requested.Load(operationData));
			return ::transferSendRanges(state, consumer, requested.Id, requested.Ranges, output);
		}
		if(COMMAND_Write == operation.Type) {
			vcu0_t operationData	= operation.Data;
			STransferWrite write;
			if_fail_fe(write.Load(operationData));
			const s2_t indexTransaction	= ::transactionFind(state, write.Id);
			if(0 > indexTransaction || state.Transactions[indexTransaction].Source)
				return -1;
			STransferTransaction & transaction	= state.Transactions[indexTransaction];
			if(0 == write.Range.Count || write.Range.Offset >= transaction.Manifest.Size || write.Range.Count > transaction.Manifest.Size - write.Range.Offset || write.Data.size() != write.Range.Count)
				return ::transferFail(state, consumer, transaction.Manifest, output, -1);
			if(false == (bool)consumer.WriteProvisionalRange || false == (bool)consumer.QueryDurableRanges)
				return ::transferFail(state, consumer, transaction.Manifest, output, -1);
			gpk::err_t result	= consumer.WriteProvisionalRange(transaction.Manifest, write.Range, write.Data);
			if(failed(result))
				return ::transferFail(state, consumer, transaction.Manifest, output, result);
			result	= consumer.QueryDurableRanges(transaction.Manifest, transaction.Durable);
			if(failed(result))
				return ::transferFail(state, consumer, transaction.Manifest, output, result);
			::rangesNormalize(transaction.Durable, transaction.Manifest.Size);
			if(::rangesCover(transaction.Durable, transaction.Manifest.Size))
				return ::transferCommit(state, consumer, transaction.Manifest, output);
			return 0;
		}
		return -1;
	}
	case LIFECYCLE_COMMAND_STOP: {
		vcu0_t inputData	= input.Data;
		u3_t id	= 0;
		if_fail_fe(loadPOD(inputData, id));
		const s2_t indexTransaction	= ::transactionFind(state, id);
		if(0 <= indexTransaction && false == state.Transactions[indexTransaction].Source && consumer.RollbackProvisional)
			consumer.RollbackProvisional(state.Transactions[indexTransaction].Manifest);
		::transactionRelease(state, id);
		return 0;
	}
	case LIFECYCLE_COMMAND_DESTROY: {
		vcu0_t inputData	= input.Data;
		u3_t id	= 0;
		if_fail_fe(loadPOD(inputData, id));
		::transactionRelease(state, id);
		return 0;
	}
	case LIFECYCLE_COMMAND_PAUSE:
		return 0;
	default:
		return -1;
	}
}
