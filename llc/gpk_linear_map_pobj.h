#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_LINEAR_MAP_POBJ_H_23627
#define GPK_LINEAR_MAP_POBJ_H_23627

namespace gpk
{
	tplt <tpnm _tElementWithSaveLoad>
	struct gpk_linear_map_pobj {
		typedef _tElementWithSaveLoad	T;
		typedef ::gpk::pobj<T>			TPObj;

		::gpk::aobj<TPObj>			Elements		= {};
		::gpk::avcsc_t				Names			= {};

		inline	const TPObj&		operator[]		(::gpk::u2_t index)	const	{ return Elements[index]; }
		inline	TPObj&				operator[]		(::gpk::u2_t index)			{ return Elements[index]; }
		inline	const TPObj&		operator[]		(::gpk::vcst_t key)	const	{ return Elements[Names.find(key)]; }
		inline	TPObj&				operator[]		(::gpk::vcst_t key)			{ return Elements[Names.find(key)]; }

		inlcxpr	::gpk::u2_t			size			()											const	{ return Names.size(); }
		inline	::gpk::error_t		find			(::gpk::vcst_t key, ::gpk::u2_t offset = 0)	const	{ return Names.find(key, offset); }

		::gpk::error_t				push_back		(const ::gpk::pobj<T> & instance, ::gpk::vcst_t name = {})	{
			::gpk::err_t				index			= 0;
			gpk_necs(index = Names.push_back(name));
			cnst ::gpk::err_t		elementIndex	= Elements.push_back(instance);
			if(::gpk::failed(elementIndex)) {
				Names.pop_back();
				return elementIndex;
			}
			return index;
		}

		inline	::gpk::error_t		Create			(const ::gpk::vcst_t name = {})									{ return push_back({}, name); }
		::gpk::error_t				Delete			(::gpk::u2_t index)												{
			if_true_fef(index >= size(), "Invalid index:%u. Element count:%u.", index, size());
			gpk_necs(Names.remove_unordered(index));
			return Elements.remove_unordered(index);
		}

		::gpk::error_t				Clone			(::gpk::u2_t index)												{
			if_true_fef(index >= size(), "Invalid index:%u. Element count:%u.", index, size());
			int32_t							iNew;
			gpk_necall(iNew = Create(Names[index]), "index: %i", index);
			const TPObj						& srcElement	= Elements[index];
			if(srcElement) {
				TPObj							& newElement	= Elements[iNew];
				if(!newElement && 0 == newElement.create()) {
					Delete(iNew);
					return -1;
				}
				*newElement					= *srcElement;
			}
			return iNew;
		}

		::gpk::error_t				Save			(::gpk::au0_t & output) const {
			if_true_fef(Names.size() != Elements.size(), "Name count:%u. Element count:%u.", Names.size(), Elements.size());
			::gpk::au0_t				serialized		= {};
			cnst ::gpk::u2_t		elementCount	= Elements.size();
			gpk_necs(::gpk::savePOD(serialized, elementCount));
			for(::gpk::u2_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				if_true_fef(0 == Elements[iEntity], "Null element at index:%u.", iEntity);
				gpk_necall(Elements[iEntity]->Save(serialized), "iEntity: %i", iEntity);
				gpk_necall(gpk::saveView(serialized, Names[iEntity]), "iEntity: %i", iEntity);
			}
			gpk_necs(output.append(serialized));
			info_printf("Saved %s, %i", "Elements", Elements.size());
			return 0;
		}

		::gpk::error_t				Load			(::gpk::vcu0_t & input) {
			::gpk::vcu0_t			inputToRead		= input;
			::gpk::u2_t			elementCount	= {};
			gpk_necs(::gpk::loadPOD(inputToRead, elementCount));
			::gpk::aobj<TPObj>		loadedElements	= {};
			::gpk::avcsc_t			loadedNames		= {};
			for(::gpk::u2_t iEntity = 0; iEntity < elementCount; ++iEntity) {
				TPObj						element			= {};
				if_true_fef(0 == element.create(), "Failed to create element at index:%u.", iEntity);
				gpk_necall(element->Load(inputToRead), "iEntity: %i", iEntity);
				::gpk::vcst_t			name				= {};
				gpk_necall(gpk::loadLabel(inputToRead, name), "iEntity: %i", iEntity);
				gpk_necs(loadedElements.push_back(element));
				gpk_necs(loadedNames.push_back(name));
			}
			Elements				= loadedElements;
			Names					= loadedNames;
			input					= inputToRead;
			return 0;
		}
	};

} // namespace

#endif // GPK_LINEAR_MAP_POBJ_H_23627


