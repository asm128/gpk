#include "gpk_transfer.h"

#include <cstdio>
#include <cstring>

usns  gpk;

struct SMemoryTransferConsumer {
	::gpk::au0_t				Source			= {};
	::gpk::au0_t				Committed		= {};
	::gpk::au0_t				Provisional		= {};
	::gpk::apod<::gpk::STransferRange>	Durable	= {};
	::gpk::u3_t				Transaction		= 0;
	uint32_t					Commits			= 0;
	uint32_t					Rollbacks			= 0;
};

sttc b8_t bytesEqual	(cnst ::gpk::vcu0_t & a, cnst ::gpk::vcu0_t & b) {
	return a.size() == b.size() && (0 == a.size() || 0 == ::memcmp(a.begin(), b.begin(), a.size()));
}

sttc ::gpk::STransferConsumer senderCallbacks	(SMemoryTransferConsumer & memory) {
	::gpk::STransferConsumer consumer;
	consumer.ReadSourceRange	= [&memory](cnst ::gpk::STransferManifest &, cnst ::gpk::STransferRange & range, ::gpk::au0_t & output) {
		if(range.Offset > memory.Source.size() || range.Count > memory.Source.size() - range.Offset)
			return (::gpk::err_t)-1;
		output.resize((::gpk::u2_t)range.Count);
		if(range.Count)
			::memcpy(output.begin(), memory.Source.begin() + range.Offset, (size_t)range.Count);
		return (::gpk::err_t)0;
	};
	return consumer;
}

sttc ::gpk::STransferConsumer receiverCallbacks	(SMemoryTransferConsumer & memory, cnst ::gpk::vcu0_t expected) {
	::gpk::STransferConsumer consumer;
	consumer.ResolveCommitted	= [](cnst ::gpk::STransferManifest &) { return (::gpk::err_t)0; };
	consumer.ResolveProvisional	= [&memory](cnst ::gpk::STransferManifest & manifest) {
		if(memory.Transaction != manifest.Id) {
			memory.Transaction	= manifest.Id;
			memory.Provisional.resize((::gpk::u2_t)manifest.Size);
			memory.Durable.clear();
			if(memory.Provisional.size())
				::memset(memory.Provisional.begin(), 0, memory.Provisional.size());
		}
		return (::gpk::err_t)0;
	};
	consumer.WriteProvisionalRange	= [&memory](cnst ::gpk::STransferManifest &, cnst ::gpk::STransferRange & range, ::gpk::vcu0_t data) {
		if(range.Offset > memory.Provisional.size() || range.Count > memory.Provisional.size() - range.Offset || data.size() != range.Count)
			return (::gpk::err_t)-1;
		::memcpy(memory.Provisional.begin() + range.Offset, data.begin(), data.size());
		memory.Durable.push_back(range);
		return (::gpk::err_t)0;
	};
	consumer.QueryDurableRanges	= [&memory](cnst ::gpk::STransferManifest &, ::gpk::apod<::gpk::STransferRange> & output) {
		output	= memory.Durable;
		return (::gpk::err_t)0;
	};
	consumer.ValidateProvisional	= [&memory, expected](cnst ::gpk::STransferManifest & manifest) {
		return manifest.Size == expected.size() && ::bytesEqual(memory.Provisional, expected) ? (::gpk::err_t)0 : (::gpk::err_t)-1;
	};
	consumer.CommitProvisional	= [&memory](cnst ::gpk::STransferManifest &) {
		memory.Committed	= memory.Provisional;
		++memory.Commits;
		return (::gpk::err_t)0;
	};
	consumer.RollbackProvisional	= [&memory](cnst ::gpk::STransferManifest &) {
		memory.Provisional.clear();
		memory.Durable.clear();
		memory.Transaction	= 0;
		++memory.Rollbacks;
		return (::gpk::err_t)0;
	};
	return consumer;
}

sttc int check	(b8_t condition, cnst sc_t * text) {
	::printf("[%s] %s\n", condition ? "PASS" : "FAIL", text);
	return condition ? 0 : 1;
}

int main() {
	int failures	= 0;
	SMemoryTransferConsumer senderMemory;
	SMemoryTransferConsumer receiverMemory;
	senderMemory.Source.resize(150000);
	for(::gpk::u2_t iByte = 0; iByte < senderMemory.Source.size(); ++iByte)
		senderMemory.Source[iByte]	= (::gpk::u0_t)((iByte * 31U + 7U) & 0xFFU);
	receiverMemory.Committed.resize(11);
	::memcpy(receiverMemory.Committed.begin(), "old-version", 11);
	cnst ::gpk::au0_t originalCommitted	= receiverMemory.Committed;

	::gpk::STransferManifest manifest;
	manifest.Id		= 1;
	manifest.Size	= senderMemory.Source.size();
	cnst sc_t resourceName[]	= "stash/example.bin";
	manifest.Resource.resize(sizeof(resourceName) - 1);
	::memcpy(manifest.Resource.begin(), resourceName, sizeof(resourceName) - 1);
	manifest.Digest.push_back(0xA5);

	::gpk::STransferState senderState;
	::gpk::STransferState receiverState;
	senderState.BlockSize	= 32768;
	cnst ::gpk::STransferConsumer sender	= ::senderCallbacks(senderMemory);
	cnst ::gpk::STransferConsumer receiver	= ::receiverCallbacks(receiverMemory, senderMemory.Source);
	::gpk::TTransferQueue toReceiver;
	::gpk::TTransferQueue toSender;

	failures += ::check(0 == ::gpk::transferEnqueueCreate(senderState, manifest, toReceiver), "CREATE event queued");
	{
		::gpk::au0_t wire;
		failures += ::check(0 == toReceiver[0]->Save(wire), "transfer event serialized by SEvent");
		::gpk::vcu0_t wireView	= wire;
		::gpk::EventTransfer decoded;
		failures += ::check(0 == decoded.Load(wireView) && decoded.Type == ::gpk::LIFECYCLE_COMMAND_CREATE, "transfer event restored by SEvent");
		::gpk::SEventCommand outer	= {::gpk::COMMAND_Write};
		failures += ::check(0 == decoded.Save(outer.Data), "transfer event nested in another SEvent");
		::gpk::vcu0_t nestedData	= outer.Data;
		::gpk::EventTransfer nested;
		failures += ::check(0 == nested.Load(nestedData) && nested.Type == decoded.Type, "nested transfer event extracted");
	}

	failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[0], toSender), "receiver resolved committed and provisional resources");
	toReceiver.clear();
	failures += ::check(::bytesEqual(receiverMemory.Committed, originalCommitted), "committed resource unchanged after CREATE");
	failures += ::check(1 == toSender.size() && toSender[0]->Type == ::gpk::LIFECYCLE_COMMAND_RESUME, "receiver reported durable ranges with RESUME");

	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "sender produced missing ranges");
	toSender.clear();
	failures += ::check(2 < toReceiver.size(), "source was split into multiple ordinary SEvent messages");

	for(::gpk::u2_t iEvent = toReceiver.size(); iEvent; --iEvent) {
		failures += ::check(::bytesEqual(receiverMemory.Committed, originalCommitted), "committed resource preserved before final range");
		failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[iEvent - 1], toSender), "out-of-order range accepted");
	}
	toReceiver.clear();
	failures += ::check(1 == receiverMemory.Commits, "consumer committed exactly once after complete coverage");
	failures += ::check(::bytesEqual(receiverMemory.Committed, senderMemory.Source), "committed bytes equal the transferred source");
	failures += ::check(1 == toSender.size() && toSender[0]->Type == ::gpk::LIFECYCLE_COMMAND_DESTROY, "receiver acknowledged committed transaction with DESTROY");
	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "sender released committed transaction state");
	toSender.clear();

	cnst ::gpk::au0_t committedBeforeRollback	= receiverMemory.Committed;
	manifest.Id	= 2;
	failures += ::check(0 == ::gpk::transferEnqueueCreate(senderState, manifest, toReceiver), "second transaction created");
	failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[0], toSender), "second provisional resource resolved");
	toReceiver.clear();
	toSender.clear();
	cnst ::gpk::STransferRange partialRange	= {0, 1024};
	failures += ::check(0 == ::gpk::transferEnqueueWrite(manifest.Id, partialRange, {senderMemory.Source.begin(), (::gpk::u2_t)partialRange.Count}, toReceiver), "partial WRITE event queued");
	failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[0], toSender), "partial range written only to provisional resource");
	toReceiver.clear();
	failures += ::check(::bytesEqual(receiverMemory.Committed, committedBeforeRollback), "partial transfer did not mutate committed resource");
	failures += ::check(0 == ::gpk::transferEnqueueStop(manifest.Id, toReceiver), "STOP event queued");
	failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[0], toSender), "STOP rolled back provisional resource");
	failures += ::check(1 == receiverMemory.Rollbacks, "consumer rollback called exactly once");
	failures += ::check(::bytesEqual(receiverMemory.Committed, committedBeforeRollback), "rollback preserved committed resource");
	toReceiver.clear();
	toSender.clear();

	manifest.Id	= 3;
	receiverMemory.Transaction	= manifest.Id;
	receiverMemory.Provisional.resize((::gpk::u2_t)manifest.Size);
	::memset(receiverMemory.Provisional.begin(), 0, receiverMemory.Provisional.size());
	::memcpy(receiverMemory.Provisional.begin(), senderMemory.Source.begin(), senderState.BlockSize);
	receiverMemory.Durable.clear();
	receiverMemory.Durable.push_back({0, senderState.BlockSize});
	receiverMemory.Committed.resize(10);
	::memcpy(receiverMemory.Committed.begin(), "resume-old", 10);
	cnst ::gpk::au0_t committedBeforeResume	= receiverMemory.Committed;
	failures += ::check(0 == ::gpk::transferEnqueueCreate(senderState, manifest, toReceiver), "resumed transaction created");
	failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[0], toSender), "receiver recovered consumer-owned durable ranges");
	toReceiver.clear();
	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "sender skipped the already durable range");
	toSender.clear();
	failures += ::check(4 == toReceiver.size(), "resume emitted only the four missing ranges");
	for(::gpk::u2_t iEvent = 0; iEvent < toReceiver.size(); ++iEvent) {
		failures += ::check(::bytesEqual(receiverMemory.Committed, committedBeforeResume), "resume preserved committed bytes before the next range");
		failures += ::check(0 == ::gpk::transferProcess(receiverState, receiver, *toReceiver[iEvent], toSender), "resumed range accepted");
	}
	toReceiver.clear();
	failures += ::check(::bytesEqual(receiverMemory.Committed, senderMemory.Source), "resumed transaction committed the complete resource");
	failures += ::check(false == ::bytesEqual(receiverMemory.Committed, committedBeforeResume), "resumed commit replaced the prior committed bytes only at completion");
	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "sender released resumed transaction state");
	toReceiver.clear();
	toSender.clear();

	manifest.Id	= 4;
	::gpk::au0_t wrongExpected	= senderMemory.Source;
	wrongExpected[0]	^= 0xFF;
	cnst ::gpk::STransferConsumer rejectingReceiver	= ::receiverCallbacks(receiverMemory, wrongExpected);
	cnst ::gpk::au0_t committedBeforeValidationFailure	= receiverMemory.Committed;
	cnst uint32_t rollbacksBeforeValidationFailure	= receiverMemory.Rollbacks;
	failures += ::check(0 == ::gpk::transferEnqueueCreate(senderState, manifest, toReceiver), "validation-failure transaction created");
	failures += ::check(0 == ::gpk::transferProcess(receiverState, rejectingReceiver, *toReceiver[0], toSender), "validation-failure provisional resource resolved");
	toReceiver.clear();
	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "validation-failure source ranges produced");
	toSender.clear();
	for(::gpk::u2_t iEvent = 0; iEvent < toReceiver.size(); ++iEvent) {
		cnst ::gpk::err_t result	= ::gpk::transferProcess(receiverState, rejectingReceiver, *toReceiver[iEvent], toSender);
		failures += ::check((iEvent + 1 == toReceiver.size()) ? ::gpk::failed(result) : (0 == result), "range handled with validation failure deferred until completion");
	}
	toReceiver.clear();
	failures += ::check(rollbacksBeforeValidationFailure + 1 == receiverMemory.Rollbacks, "validation failure rolled back provisional resource");
	failures += ::check(::bytesEqual(receiverMemory.Committed, committedBeforeValidationFailure), "validation failure preserved committed resource");
	failures += ::check(1 == toSender.size() && toSender[0]->Type == ::gpk::LIFECYCLE_COMMAND_STOP, "validation failure returned STOP");
	failures += ::check(0 == ::gpk::transferProcess(senderState, sender, *toSender[0], toReceiver), "sender released rejected transaction state");

	::printf("Transfer tests: %s (%d failure%s)\n", failures ? "FAILED" : "PASSED", failures, 1 == failures ? "" : "s");
	return failures ? 1 : 0;
}
