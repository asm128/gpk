#include "gpk_event.h"
#include "gpk_lifecycle.h"

#include <functional>

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

		err_t				Save			(au0_t & output) const;
		err_t				Load			(vcu0_t & input);
	};

	struct STransferRanges {
		u3_t				Id				= 0;
		apod<STransferRange>	Ranges			= {};

		err_t				Save			(au0_t & output) const;
		err_t				Load			(vcu0_t & input);
	};

	struct STransferWrite {
		u3_t				Id				= 0;
		STransferRange		Range			= {};
		au0_t				Data			= {};

		err_t				Save			(au0_t & output) const;
		err_t				Load			(vcu0_t & input);
	};

	using EventTransfer			= SEvent<LIFECYCLE_COMMAND>;
	using PEventTransfer			= pobj<EventTransfer>;
	using TTransferQueue			= apobj<EventTransfer>;

	using FTransferResolve		= ::std::function<err_t(const STransferManifest &)>;
	using FTransferRead			= ::std::function<err_t(const STransferManifest &, const STransferRange &, au0_t &)>;
	using FTransferWrite			= ::std::function<err_t(const STransferManifest &, const STransferRange &, vcu0_t)>;
	using FTransferQuery			= ::std::function<err_t(const STransferManifest &, apod<STransferRange> &)>;
	using FTransferFinalize		= ::std::function<err_t(const STransferManifest &)>;

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
		bool				Active			= false;
		bool				Source			= false;
		STransferManifest	Manifest		= {};
		apod<STransferRange>	Durable			= {};
	};

	struct STransferState {
		u2_t				BlockSize		= 64U * 1024U;
		aobj<STransferTransaction>	Transactions	= {};
	};

	err_t	transferEventCreate		(EventTransfer & output, const STransferManifest & manifest);
	err_t	transferEventResume		(EventTransfer & output, u3_t id, view<const STransferRange> durableRanges);
	err_t	transferEventRead		(EventTransfer & output, u3_t id, view<const STransferRange> requestedRanges);
	err_t	transferEventWrite		(EventTransfer & output, u3_t id, const STransferRange & range, vcu0_t data);
	err_t	transferEventStop		(EventTransfer & output, u3_t id);
	err_t	transferEventDestroy		(EventTransfer & output, u3_t id);

	err_t	transferEnqueueCreate	(STransferState & state, const STransferManifest & manifest, TTransferQueue & output);
	err_t	transferEnqueueResume	(u3_t id, view<const STransferRange> durableRanges, TTransferQueue & output);
	err_t	transferEnqueueRead		(u3_t id, view<const STransferRange> requestedRanges, TTransferQueue & output);
	err_t	transferEnqueueWrite	(u3_t id, const STransferRange & range, vcu0_t data, TTransferQueue & output);
	err_t	transferEnqueueStop		(u3_t id, TTransferQueue & output);
	err_t	transferEnqueueDestroy	(u3_t id, TTransferQueue & output);

	err_t	transferProcess			(STransferState & state, const STransferConsumer & consumer, const EventTransfer & input, TTransferQueue & output);
}

#endif // GPK_TRANSFER_H_23627
