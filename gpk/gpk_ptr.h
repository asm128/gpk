#include "gpk_sync.h"
#include "gpk_log.h"

#ifndef GPK_PTR_H_0297349827348923
#define GPK_PTR_H_0297349827348923

namespace gpk 
{
	template<typename _tInstance>
	struct gpk_ref {
		typedef		_tInstance									TInstance;

					_tInstance									* Instance;
					refcount_t									References;
	};
	
	template<typename _tNCO>
						::gpk::gpk_ref<_tNCO> *				ref_acquire							(::gpk::gpk_ref<_tNCO>* gpk_reference)									noexcept	{
		if(gpk_reference)
			gpk_sync_increment(gpk_reference->References);
		return gpk_reference;
	};

	template<typename _tNCO>
						::gpk::error_t						ref_release							(::gpk::gpk_ref<_tNCO>** gpk_reference)									{
		typedef	::gpk::gpk_ref<_tNCO>								TRef;
		TRef														* oldRef							= *gpk_reference;
		*gpk_reference											= 0;
		if(oldRef)
			switch(gpk_sync_decrement(oldRef->References)) {
			case -1: error_printf("%s", "Reference count error!"); return -1;
			case  0:
				if(oldRef->Instance)
					oldRef->Instance->~_tNCO();
				else
					error_printf("%s", "Instance is NULL! At the point of writing this code it wouldn't make any sense.");
				::gpk::gpk_free(oldRef->Instance);
				::gpk::gpk_free(oldRef);
				break;
			}
		return 0;
	};

	template<typename _tOBJ>
						_tOBJ *								ref_allocate						(::gpk::gpk_ref<_tOBJ>** gpk_reference)									noexcept	{
		typedef	::gpk::gpk_ref<_tOBJ>								TRef;
		TRef														* newRef							= (TRef*)::gpk::gpk_malloc(sizeof(TRef));
		retnul_error_if(0 == newRef, "%s", "Failed to allocate reference! Out of memory?");
		if(0 == (newRef->Instance = (_tOBJ*)::gpk::gpk_malloc(sizeof(_tOBJ)))) {
			::gpk::gpk_free(newRef);
			error_printf("%s", "Failed to allocate instance! Out of memory?");
			return 0;
		}
		newRef->References										= 1;
		TRef														* oldRef							= *gpk_reference;
		*gpk_reference											= newRef;
		::gpk::ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	};

	template<typename _tOBJ, typename... _tArgs>
						_tOBJ *								ref_create							(::gpk::gpk_ref<_tOBJ>** gpk_reference, _tArgs&&... argsConstructor)				{
		typedef	::gpk::gpk_ref<_tOBJ>								TRef;
		TRef														* newRef							= 0;
		retnul_error_if(0 == ::gpk::ref_allocate(&newRef), "%s", "Failed to allocate reference");
		new (newRef->Instance) _tOBJ{argsConstructor...};

		TRef														* oldRef							= *gpk_reference;
		*gpk_reference											= newRef;
		::gpk::ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	};

	template<typename _tNCO>
	class ptr_nco {
	protected:
							::gpk::gpk_ref<_tNCO>				* Reference							= 0;
	public:
		typedef				::gpk::ptr_nco<_tNCO>				TNCOPtr;
		typedef				::gpk::gpk_ref<_tNCO>				TRef;

		inline													~ptr_nco							()															noexcept	{ ::gpk::ref_release(&Reference);													}
		inline constexpr										ptr_nco								()															noexcept	= default;
		inline													ptr_nco								(const TNCOPtr& other)										noexcept	{ Reference = ::gpk::ref_acquire(other.Reference);									}
		inline constexpr										ptr_nco								(TNCOPtr&& other)											noexcept	{ Reference = other.Reference; other.Reference = 0;									}
		inline													ptr_nco								(TRef* other)												noexcept	{ Reference = other;																}

		inline constexpr	operator							_tNCO	*							()															noexcept	{ return Reference ? Reference->Instance : 0;										}

		inline constexpr	bool								operator==							(const TNCOPtr& other)								const	noexcept	{ return Reference == other.Reference;												}
		inline constexpr	bool								operator!=							(const TNCOPtr& other)								const	noexcept	{ return !operator==(other);														}

		inline constexpr	TNCOPtr								operator =							(const TNCOPtr& other)										noexcept	{ TRef* oldInstance = Reference; Reference = ::gpk::ref_acquire(other.Reference);	::gpk::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr								operator =							(TNCOPtr&& other)											noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	::gpk::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr								operator =							(TRef* other)												noexcept	{ TRef* oldInstance = Reference; Reference = other;									::gpk::ref_release(&oldInstance); return *this; }

		inline				_tNCO*								operator->							()															noexcept	{ return Reference->Instance; }
		inline				const _tNCO*						operator->							()													const	noexcept	{ return Reference->Instance; }

		inline				TRef**								operator &							()															noexcept	{ return &Reference;	}

		inline constexpr	const TRef*							get_ref								()													const	noexcept	{ return Reference;	}
		inline constexpr	const TRef*							set_ref								(TRef* ref)													noexcept	{ TRef* oldInstance = Reference; Reference = ref; ::gpk::ref_release(&oldInstance); return Reference;	}

		template<typename _tNCOOther>
		inline				_tNCO*								as									(_tNCOOther** other)										noexcept	{ return *other = (Reference ? dynamic_cast<_tNCOOther*>(Reference->Instance) : 0);				}

		template<typename _tNCOOther>
		inline				_tNCO*								as									(::gpk::ptr_nco<_tNCOOther>& other)							noexcept	{ other = (::gpk::gpk_ref<_tNCOOther>*)::gpk::ref_acquire(Reference); return 0;									}
	};
	
	template<typename _tOBJ>
	class ptr_obj : public ::gpk::ptr_nco<_tOBJ> {
	public:
		typedef				::gpk::ptr_nco<_tOBJ>				TNCOPtr;
		typedef				::gpk::ptr_obj<_tOBJ>				TOBJPtr;
		typedef				::gpk::gpk_ref<_tOBJ>				TRef;
		using				TNCOPtr								::Reference;

		inline				const _tOBJ*						operator->							()													const	noexcept	{ return Reference;																	}
		inline				_tOBJ*								operator->							()															noexcept	{ return (0 == Reference) ? ::gpk::ref_create(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tOBJ*								create								(_tArgsConstructor&&... argsConstructor)					noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline				_tOBJ*								allocate							()															noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};

	template<typename _tPOD>
	class ptr_pod : public ::gpk::ptr_nco<_tPOD> {
	public:
		typedef				::gpk::ptr_nco<_tPOD>				TNCOPtr;
		typedef				::gpk::ptr_pod<_tPOD>				TPODPtr;
		typedef				::gpk::gpk_ref<_tPOD>				TRef;
		using				TNCOPtr								::Reference;

		inline				const _tPOD*						operator->							()													const	noexcept	{ return Reference->Instance;														}
		inline				_tPOD*								operator->							()															noexcept	{ return (0 == Reference) ? ::gpk::ref_allocate(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tPOD*								create								(_tArgsConstructor&&... argsConstructor)					noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline				_tPOD*								allocate							()															noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};
} // namespace

#endif // GPK_PTR_H_0297349827348923
