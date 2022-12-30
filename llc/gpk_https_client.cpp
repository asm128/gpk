//#include "gpk_http_client.h"
//
//#define HOST_NAME		"www.random.org"
//#define HOST_PORT		"443"
//#define HOST_RESOURCE	"/cgi-bin/randbyte?nbytes=32&format=h"
//
////	Initialization
////	The sample program initializes the OpenSSL library with init_openssl_library. init_openssl_library calls three OpenSSL functions.
//#if (SSLEAY_VERSION_NUMBER >= 0x0907000L)
//#	include <openssl/conf.h>
//#endif
//
//void					init_openssl_library		()				{
//	(void)SSL_library_init();
//	SSL_load_error_strings();
//	// ERR_load_crypto_strings();
//	OPENSSL_config(0);
//	// Include <openssl/opensslconf.h> to get this define
//#if defined (OPENSSL_THREADS)
//	fprintf(stdout, "Warning: thread locking is not implemented\n");
//#endif
//}
//
//::gpk::error_t			httpstest					()					{
//	init_openssl_library();
//
//	const SSL_METHOD			* method					= SSLv23_method();
//	ree_if(0 == method, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(ctx != NULL) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	SSL_CTX						* ctx						= SSL_CTX_new(method);
//	ree_if(0 == ctx, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);	// Cannot fail ???
//	SSL_CTX_set_verify_depth(ctx, 4);	// Cannot fail ???
//
//	const long					flags						= SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
//	SSL_CTX_set_options(ctx, flags);	// Cannot fail ???
//
//	int64_t						res							= SSL_CTX_load_verify_locations(ctx, "random-org-chain.pem", NULL);
//	ree_if(1 != res, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	BIO							* web						= BIO_new_ssl_connect(ctx);
//	ree_if(0 == web, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//	ree_if(1 != (res = BIO_set_conn_hostname(web, HOST_NAME ":" HOST_PORT)), "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	SSL							* ssl						= NULL;
//	BIO_get_ssl(web, &ssl);
//	ree_if(0 == ssl, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	const char * const			PREFERRED_CIPHERS			= "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
//	ree_if(1 != (res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS)), "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//	ree_if(1 != (res = SSL_set_tlsext_host_name(ssl, HOST_NAME)), "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	//BIO							* out						= BIO_new_fp(stdout, BIO_NOCLOSE);
//	//ree_if(0 == out, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//	ree_if(1 != (res = BIO_do_connect	(web)), "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//	ree_if(1 != (res = BIO_do_handshake	(web)), "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	/* Step 1: verify a server certificate was presented during the negotiation */
//	X509						* cert						= SSL_get_peer_certificate(ssl);
//	if(cert)
//		X509_free(cert); // Free immediately */
//
//	ree_if(0 == cert, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	// Step 2: verify the result of chain verification. Verification performed according to RFC 4158
//	res						= SSL_get_verify_result(ssl);
//	ree_if(X509_V_OK != res, "When I grabbed this code from %s, i didn't expect to find fucked up expressions like !(1 == res) or !(method != NULL) in the example code.", "https://wiki.openssl.org/index.php/SSL/TLS_Client#SSL_BIO");
//
//	// Step 3: hostname verification. An exercise left to the reader
//	BIO_puts(web, "GET " HOST_RESOURCE " HTTP/1.1\r\n"
//				  "Host: " HOST_NAME "\r\n"
//				  "Connection: close\r\n\r\n");
//	//BIO_puts(out, "\n");
//
//	int							len							= 0;
//	::gpk::apod<byte_t>	response;
//	do {
//		char						buff	[1536]				= {};
//		len						= BIO_read(web, buff, ::gpk::size(buff));
//		if(len > 0) {
//			//BIO_write(out, buff, len);
//			response.append(buff, len);
//		}
//	} while (len > 0 || BIO_should_retry(web));
//
//	//if(out)
//	//	BIO_free(out);
//
//	if(web != NULL)
//		BIO_free_all(web);
//
//	if(NULL != ctx)
//		SSL_CTX_free(ctx);
//	return (::gpk::error_t)res;
//}
