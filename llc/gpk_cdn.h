#include "gpk_typeint.h"

#ifndef GPK_CDN_H_23627
#define GPK_CDN_H_23627

namespace gpk
{
#pragma pack(push, 1)
	// Should fit in an uint32_t
	struct SContentIdLocals	{ 
		uint32_t					Index				: 24;
		uint32_t					Block				: 8;
	};

	// Should fit in an uint32_t
	struct SContentIdSystem	{ 
		uint32_t					Bucket				: 8;
		uint32_t					Server				: 12;
		uint32_t					System				: 12;
	};

	// Should fit in an uint64_t
	struct SContentId {
		::gpk::SContentIdLocals		Locals;
		::gpk::SContentIdSystem		System;
	};

	// 80 bytes
	struct SSessionStateClient {
		::gpk::SContentId			Session		; // server_session
		::gpk::SContentId			Person		; // server_session
		::gpk::SContentId			Group		; // server_session
		::gpk::SContentId			Tenant		; // server_tenant
		::gpk::SContentId			Settings	; // server_settings
		::gpk::SContentId			Profile		; // server_profile
		::gpk::SContentId			Inventory	; // server_inventory
		::gpk::SContentId			Lobby		; // server_lobby
		::gpk::SContentId			Chatroom	; // server_chatroom
		::gpk::SContentId			Playroom	; // server_playroom
	};
#pragma pack(pop)
} // namespace 

#endif // GPK_CDN_H_23627
