#include "gpk_event.h"
#include "gpk_lifecycle.h"
#include "gpk_event_device.h"

usns  gpk;

#ifndef GPK_TRANSFER_H_23627
#define GPK_TRANSFER_H_23627

namespace gpk
{
	struct STransferRange {
		u3_t				Offset			= 0;
		u3_t				Count			= 0;
	};

	struct STransferManifest {
		u3_t				Id				= 0;
		u3_t				Size			= 0;
		au0_t				Resource		= {};
		au0_t				Digest			= {};

		err_t				Save			(au0_t & output) cnst;
		err_t				Load			(vcu0_t & input);
	};

	struct STransferRanges {
		u3_t				Id				= 0;
		apod<STransferRange>	Ranges			= {};

		err_t				Save			(au0_t & output) cnst;
		err_t				Load			(vcu0_t & input);
	};

	struct STransferWrite {
		u3_t				Id				= 0;
		STransferRange		Range			= {};
		au0_t				Data			= {};

		err_t				Save			(au0_t & output) cnst;
		err_t				Load			(vcu0_t & input);
	};

	using EventTransfer			= SEvent<LIFECYCLE_COMMAND>;
	using PEventTransfer			= pobj<EventTransfer>;
	using TTransferQueue			= apobj<EventTransfer>;

	using FTransferResolve		= ::gpk::function<err_t(cnst STransferManifest &)>;
	using FTransferRead			= ::gpk::function<err_t(cnst STransferManifest &, cnst STransferRange &, au0_t &)>;
	using FTransferWrite			= ::gpk::function<err_t(cnst STransferManifest &, cnst STransferRange &, vcu0_t)>;
	using FTransferQuery			= ::gpk::function<err_t(cnst STransferManifest &, apod<STransferRange> &)>;
	using FTransferFinalize		= ::gpk::function<err_t(cnst STransferManifest &)>;

	struct STransferConsumer {
		FTransferResolve		ResolveCommitted		= {};
		FTransferResolve		ResolveProvisional	= {};
		FTransferRead		ReadSourceRange		= {};
		FTransferWrite		WriteProvisionalRange	= {};
		FTransferQuery		QueryDurableRanges	= {};
		FTransferFinalize	ValidateProvisional	= {};
		FTransferFinalize	CommitProvisional	= {};
		FTransferFinalize	RollbackProvisional	= {};
	};

	struct STransferTransaction {
		b8_t				Active			= false;
		b8_t				Source			= false;
		STransferManifest	Manifest		= {};
		apod<STransferRange>	Durable			= {};
	};

	struct STransferState {
		u2_t				BlockSize		= 64U * 1024U;
		aobj<STransferTransaction>	Transactions	= {};
	};

	err_t	transferEventCreate		(EventTransfer & output, cnst STransferManifest & manifest);
	err_t	transferEventResume		(EventTransfer & output, u3_t id, view<cnst STransferRange> durableRanges);
	err_t	transferEventRead		(EventTransfer & output, u3_t id, view<cnst STransferRange> requestedRanges);
	err_t	transferEventWrite		(EventTransfer & output, u3_t id, cnst STransferRange & range, vcu0_t data);
	err_t	transferEventStop		(EventTransfer & output, u3_t id);
	err_t	transferEventDestroy		(EventTransfer & output, u3_t id);

	err_t	transferEnqueueCreate	(STransferState & state, cnst STransferManifest & manifest, TTransferQueue & output);
	err_t	transferEnqueueResume	(u3_t id, view<cnst STransferRange> durableRanges, TTransferQueue & output);
	err_t	transferEnqueueRead		(u3_t id, view<cnst STransferRange> requestedRanges, TTransferQueue & output);
	err_t	transferEnqueueWrite	(u3_t id, cnst STransferRange & range, vcu0_t data, TTransferQueue & output);
	err_t	transferEnqueueStop		(u3_t id, TTransferQueue & output);
	err_t	transferEnqueueDestroy	(u3_t id, TTransferQueue & output);

	err_t	transferProcess			(STransferState & state, cnst STransferConsumer & consumer, cnst EventTransfer & input, TTransferQueue & output);
}

#endif // GPK_TRANSFER_H_23627
