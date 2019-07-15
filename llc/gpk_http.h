#include "gpk_enum.h"

#ifndef GPK_HTTP_H_23478298347928347
#define GPK_HTTP_H_23478298347928347

namespace gpk
{
	GDEFINE_ENUM_TYPE(HTTP_METHOD, int8_t);	
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNKNOWN	, 0);	// ???
	GDEFINE_ENUM_VALUE(HTTP_METHOD, GET		, 1);	// The GET method is used to retrieve information from the given server using a given URI. Requests using GET should only retrieve data and should have no other effect on the data.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, HEAD	, 2);	// Same as GET, but transfers the status line and header section only.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, POST	, 3);	// A POST request is used to send data to the server, for example, customer information, file upload, etc. using HTML forms.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PUT		, 4);	// Replaces all current representations of the target resource with the uploaded content.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, DELETE	, 5);	// Removes all current representations of the target resource given by a URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, CONNECT	, 6);	// Establishes a tunnel to the server identified by a given URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, OPTIONS	, 7);	// Describes the communication options for the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, TRACE	, 8);	// Performs a message loop-back test along the path to the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PATCH	, 9);	// The PATCH method is used to apply partial modifications to a resource.
} // namespace

#endif // GPK_HTTP_H_23478298347928347
