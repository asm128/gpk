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
		typedef pobj<T>			TPObj;

		aobj<TPObj>				Elements		= {};
		aobj<vcst_t>			Names			= {};

		inline	const TPObj&	operator[]		(u2_t index)	const	{ return Elements[index]; }
		inline	TPObj&			operator[]		(u2_t index)			{ return Elements[index]; }
		inline	const TPObj&	operator[]		(vcst_t key)	const	{ return Elements[Names.find(key)]; }
		inline	TPObj&			operator[]		(vcst_t key)			{ return Elements[Names.find(key)]; }

		inxp	u2_t			size			()								const	{ return Names.size(); }
		inline	err_t			find			(vcst_t key, u2_t offset = 0)	const	{ return Names.find(key, offset); }

		err_t					push_back		(const pobj<T> & instance, vcst_t name = {})	{
			err_t						index			= 0;
			if_fail_fe(index = Names.push_back(name));
			cnst err_t				elementIndex	= Elements.push_back(instance);
			if(failed(elementIndex)) {
				Names.pop_back();
				return elementIndex;
			}
			return index;
		}

		inline	err_t			Create			(vcst_t name = {})									{ return push_back({}, name); }
		err_t					Delete			(u2_t index)										{
			if_true_fef(index >= size(), "Invalid index:%u. Element count:%u.", index, size());
			if_fail_fe(Names.remove_unordered(index));
			return Elements.remove_unordered(index);
		}

		err_t					Clone			(u2_t index)										{
			if_true_fef(index >= size(), "Invalid index:%u. Element count:%u.", index, size());
			s2_t							iNew;
			if_fail_fef(iNew = Create(Names[index]), "index: %i", index);
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

		err_t					Save			(au0_t & output) const {
			if_true_fef(Names.size() != Elements.size(), "Name count:%u. Element count:%u.", Names.size(), Elements.size());
			au0_t				serialized		= {};
			cnst u2_t		elementCount	= Elements.size();
			if_fail_fe(savePOD(serialized, elementCount));
			for(u2_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				if_true_fef(0 == Elements[iEntity], "Null element at index:%u.", iEntity);
				if_fail_fef(Elements[iEntity]->Save(serialized), "iEntity: %i", iEntity);
				if_fail_fef(saveView(serialized, Names[iEntity]), "iEntity: %i", iEntity);
			}
			if_fail_fe(output.append(serialized));
			info_printf("Saved %s, %i", "Elements", Elements.size());
			return 0;
		}

		err_t					Load			(vcu0_t & input) {
			vcu0_t						inputToRead		= input;
			u2_t						elementCount	= {};
			if_fail_fe(loadPOD(inputToRead, elementCount));
			aobj<TPObj>					loadedElements	= {};
			aobj<vcst_t>				loadedNames		= {};
			for(u2_t iEntity = 0; iEntity < elementCount; ++iEntity) {
				TPObj						element			= {};
				if_true_fef(0 == element.create(), "Failed to create element at index:%u.", iEntity);
				if_fail_fef(element->Load(inputToRead), "iEntity: %i", iEntity);
				vcst_t						name				= {};
				if_fail_fef(loadLabel(inputToRead, name), "iEntity: %i", iEntity);
				if_fail_fe(loadedElements.push_back(element));
				if_fail_fe(loadedNames.push_back(name));
			}
			Elements				= loadedElements;
			Names					= loadedNames;
			input					= inputToRead;
			return 0;
		}
	};

} // namespace

#endif // GPK_LINEAR_MAP_POBJ_H_23627
