#include "gpk_view.h"
#include "gpk_n2.h"

#ifndef GPK_GRID_H_23627
#define GPK_GRID_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm TCell>
	clss grid {
	protected:
		TCell			* Data	= {};
		n2u2_t			Size	= {};
	public:
		tydf TCell		T;

		inxp				grid		() nxpt = default;
		inln				grid		(T * data, u2_t width, u2_t height) : Data{data}, Size{width, height} {
			if_true_tef(0 == data && Size.x && Size.y, "Invalid grid size:{%u, %u}, pointer:%p.", width, height, data);
		}
		inln				grid		(T * data, n2u2_c & metrics) : grid(data, metrics.x, metrics.y) {}
		tplt<size_t width, size_t height>
		inxp				grid		(T (&data)[height][width]) nxpt : Data{&data[0][0]}, Size{width, height} {}

		inxp	oper		grid<cnst T>	() cnst nxpt { rtrn {Data, Size}; }

		view<T>			oper[]		(u2_t row) {
			if_null_te(Data);
			if_true_tef(row >= Size.y, "Invalid row:%u.", row);
			rtrn {&Data[row * Size.x], Size.x};
		}
		view<cnst T>		oper[]		(u2_t row) cnst {
			if_null_te(Data);
			if_true_tef(row >= Size.y, "Invalid row:%u.", row);
			rtrn {&Data[row * Size.x], Size.x};
		}
		T&				oper[]		(n2u2_c & cell) {
			if_null_te(Data);
			if_true_tef(cell.y >= Size.y, "Invalid row:%u.", cell.y);
			if_true_tef(cell.x >= Size.x, "Invalid column:%u.", cell.x);
			rtrn Data[cell.y * Size.x + cell.x];
		}
		cnst T&			oper[]		(n2u2_c & cell) cnst {
			if_null_te(Data);
			if_true_tef(cell.y >= Size.y, "Invalid row:%u.", cell.y);
			if_true_tef(cell.x >= Size.x, "Invalid column:%u.", cell.x);
			rtrn Data[cell.y * Size.x + cell.x];
		}

		inxp	cnst T*		begin		() cnst nxpt { rtrn Data; }
		inxp	cnst T*		end			() cnst nxpt { rtrn Data + size(); }
		inxp	T*			begin		() nxpt { rtrn Data; }
		inxp	T*			end			() nxpt { rtrn Data + size(); }
		inxp	n2u2_c&		metrics		() cnst nxpt { rtrn Size; }
		inxp	n2u1_t		metrics16	() cnst nxpt { rtrn Size.u1(); }
		inxp	u2_t			size		() cnst nxpt { rtrn area(); }
		inxp	u2_t			byte_count	() cnst nxpt { rtrn area() * szof(T); }
		inxp	u2_t			area		() cnst nxpt { rtrn Size.x * Size.y; }

		err_t				fill		(cnst T & value, u2_t offset = 0, u2_t count = (u2_t)-1) {
			if_true_fef(count > size() && count != (u2_t)-1, "Count:%u, grid size:%u.", count, size());
			for(u2_t i = offset, stop = min(size(), count); i < stop; ++i)
				Data[i] = value;
			rtrn 0;
		}
	};
#pragma pack(pop)

	tydf grid<sc_t>	gsc_t;
	tydf grid<uc_t>	guc_t;
	tydf grid<u0_t>	gu0_t;
	tydf grid<u1_t>	gu1_t;
	tydf grid<u2_t>	gu2_t;
	tydf grid<u3_t>	gu3_t;
	tydf grid<s0_t>	gs0_t;
	tydf grid<s1_t>	gs1_t;
	tydf grid<s2_t>	gs2_t;
	tydf grid<s3_t>	gs3_t;
	tydf grid<f2_t>	gf2_t;
	tydf grid<f3_t>	gf3_t;

	enum GRID_ROTATION : u0_t {
		GRID_ROTATION_DEFAULT	= 0,
		GRID_ROTATION_0			= 0x1,
		GRID_ROTATION_90		= 0x2,
		GRID_ROTATION_180		= 0x4,
		GRID_ROTATION_270		= 0x8,
	};
} // namespace gpk

#endif // GPK_GRID_H_23627


