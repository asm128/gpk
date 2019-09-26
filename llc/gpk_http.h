#include "gpk_enum.h"

#ifndef GPK_HTTP_H_23478298347928347
#define GPK_HTTP_H_23478298347928347

namespace gpk
{
	GDEFINE_ENUM_TYPE(HTTP_METHOD, int8_t);
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNKNOWN		, 0);	// ???
	GDEFINE_ENUM_VALUE(HTTP_METHOD, GET			, 1);	// The GET method is used to retrieve information from the given server using a given URI. Requests using GET should only retrieve data and should have no other effect on the data.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, HEAD		, 2);	// Same as GET, but transfers the status line and header section only.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, POST		, 3);	// A POST request is used to send data to the server, for example, customer information, file upload, etc. using HTML forms.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PUT			, 4);	// Replaces all current representations of the target resource with the uploaded content.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, DELETE		, 5);	// Removes all current representations of the target resource given by a URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, CONNECT		, 6);	// Establishes a tunnel to the server identified by a given URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, OPTIONS		, 7);	// Describes the communication options for the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, TRACE		, 8);	// Performs a message loop-back test along the path to the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PATCH		, 9);	// The PATCH method is used to apply partial modifications to a resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, COPY		,10);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, LINK		,11);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNLINK		,12);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, LOCK		,13);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNLOCK		,14);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PROPFIND	,15);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, VIEW		,16);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PURGE		,17);	//

#pragma pack(push, 1)
	struct SHTTPAPIRequest {
		::gpk::array_obj<::gpk::view_const_string>					QueryStringElements			= {};
		::gpk::array_obj<::gpk::TKeyValConstString>					QueryStringKeyVals			= {};
		::gpk::view_const_string									Ip							= {};
		::gpk::view_const_string									Port						= {};
		::gpk::view_const_char										Script						= {};
		::gpk::view_const_char										Path						= {};
		::gpk::view_const_char										QueryString					= {};
		::gpk::view_const_char										ContentBody					= {};
		::gpk::HTTP_METHOD											Method						= {};
	};
#pragma pack(pop)
} // namespace

#endif // GPK_HTTP_H_23478298347928347
