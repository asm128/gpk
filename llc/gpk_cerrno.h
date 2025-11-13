#include "gpk_enum.h"

#ifndef GPK_CERRNO_H
#define GPK_CERRNO_H

#ifdef GPK_ATMEL
#	include <errno.h>
#else
#	include <cerrno>
#endif
namespace gpk
{
	GDEFINE_ENUM_TYPE(ERRNO, int32_t);
	GDEFINE_ENUM_VALUED(ERRNO, OK              ,    0, "No error"                                          );
	GDEFINE_ENUM_VALUED(ERRNO, EPERM           , -  1, "Operation not permitted"                           );
	GDEFINE_ENUM_VALUED(ERRNO, ENOENT          , -  2, "No such file or directory"                         );
	GDEFINE_ENUM_VALUED(ERRNO, ESRCH           , -  3, "No such process"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, EINTR           , -  4, "Interrupted system call"                           );
	GDEFINE_ENUM_VALUED(ERRNO, EIO             , -  5, "Input/output error"                                );
	GDEFINE_ENUM_VALUED(ERRNO, ENXIO           , -  6, "No such device or address"                         );
	GDEFINE_ENUM_VALUED(ERRNO, E2BIG           , -  7, "Argument list too long"                            );
	GDEFINE_ENUM_VALUED(ERRNO, ENOEXEC         , -  8, "Exec format error"                                 );
	GDEFINE_ENUM_VALUED(ERRNO, EBADF           , -  9, "Bad file descriptor"                               );
	GDEFINE_ENUM_VALUED(ERRNO, ECHILD          , - 10, "No child processes"                                );
	GDEFINE_ENUM_VALUED(ERRNO, EAGAIN          , - 11, "Resource temporarily unavailable"                  );
	GDEFINE_ENUM_VALUED(ERRNO, ENOMEM          , - 12, "Cannot allocate memory"                            );
	GDEFINE_ENUM_VALUED(ERRNO, EACCES          , - 13, "Permission denied"                                 );
	GDEFINE_ENUM_VALUED(ERRNO, EFAULT          , - 14, "Bad address"                                       );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTBLK         , - 15, "Block device required"                             );
	GDEFINE_ENUM_VALUED(ERRNO, EBUSY           , - 16, "Device or resource busy"                           );
	GDEFINE_ENUM_VALUED(ERRNO, EEXIST          , - 17, "File exists"                                       );
	GDEFINE_ENUM_VALUED(ERRNO, EXDEV           , - 18, "Invalid cross-device link"                         );
	GDEFINE_ENUM_VALUED(ERRNO, ENODEV          , - 19, "No such device"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTDIR         , - 20, "Not a directory"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, EISDIR          , - 21, "Is a directory"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EINVAL          , - 22, "Invalid argument"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, ENFILE          , - 23, "Too many open files in system"                     );
	GDEFINE_ENUM_VALUED(ERRNO, EMFILE          , - 24, "Too many open files"                               );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTTY          , - 25, "Inappropriate ioctl for device"                    );
	GDEFINE_ENUM_VALUED(ERRNO, ETXTBSY         , - 26, "Text file busy"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EFBIG           , - 27, "File too large"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, ENOSPC          , - 28, "No space left on device"                           );
	GDEFINE_ENUM_VALUED(ERRNO, ESPIPE          , - 29, "Illegal seek"                                      );
	GDEFINE_ENUM_VALUED(ERRNO, EROFS           , - 30, "Read-only file system"                             );
	GDEFINE_ENUM_VALUED(ERRNO, EMLINK          , - 31, "Too many links"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EPIPE           , - 32, "Broken pipe"                                       );
	GDEFINE_ENUM_VALUED(ERRNO, EDOM            , - 33, "Numerical argument out of domain"                  );
	GDEFINE_ENUM_VALUED(ERRNO, ERANGE          , - 34, "Numerical result out of range"                     );
	GDEFINE_ENUM_VALUED(ERRNO, EDEADLK         , - 35, "Resource deadlock avoided"                         );
	GDEFINE_ENUM_VALUED(ERRNO, ENAMETOOLONG    , - 36, "File name too long"                                );
	GDEFINE_ENUM_VALUED(ERRNO, ENOLCK          , - 37, "No locks available"                                );
	GDEFINE_ENUM_VALUED(ERRNO, ENOSYS          , - 38, "Function not implemented"                          );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTEMPTY       , - 39, "Directory not empty"                               );
	GDEFINE_ENUM_VALUED(ERRNO, ELOOP           , - 40, "Too many levels of symbolic links"                 );
	GDEFINE_ENUM_VALUED(ERRNO, ENOMSG          , - 42, "No message of desired type"                        );
	GDEFINE_ENUM_VALUED(ERRNO, EIDRM           , - 43, "Identifier removed"                                );
	GDEFINE_ENUM_VALUED(ERRNO, ECHRNG          , - 44, "Channel number out of range"                       );
	GDEFINE_ENUM_VALUED(ERRNO, EL2NSYNC        , - 45, "Level 2 not synchronized"                          );
	GDEFINE_ENUM_VALUED(ERRNO, EL3HLT          , - 46, "Level 3 halted"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EL3RST          , - 47, "Level 3 reset"                                     );
	GDEFINE_ENUM_VALUED(ERRNO, ELNRNG          , - 48, "Link number out of range"                          );
	GDEFINE_ENUM_VALUED(ERRNO, EUNATCH         , - 49, "Protocol driver not attached"                      );
	GDEFINE_ENUM_VALUED(ERRNO, ENOCSI          , - 50, "No CSI structure available"                        );
	GDEFINE_ENUM_VALUED(ERRNO, EL2HLT          , - 51, "Level 2 halted"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EBADE           , - 52, "Invalid exchange"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, EBADR           , - 53, "Invalid request descriptor"                        );
	GDEFINE_ENUM_VALUED(ERRNO, EXFULL          , - 54, "Exchange full"                                     );
	GDEFINE_ENUM_VALUED(ERRNO, ENOANO          , - 55, "No anode"                                          );
	GDEFINE_ENUM_VALUED(ERRNO, EBADRQC         , - 56, "Invalid request code"                              );
	GDEFINE_ENUM_VALUED(ERRNO, EBADSLT         , - 57, "Invalid slot"                                      );
	GDEFINE_ENUM_VALUED(ERRNO, EBFONT          , - 59, "Bad font file format"                              );
	GDEFINE_ENUM_VALUED(ERRNO, ENOSTR          , - 60, "Device not a stream"                               );
	GDEFINE_ENUM_VALUED(ERRNO, ENODATA         , - 61, "No data available"                                 );
	GDEFINE_ENUM_VALUED(ERRNO, ETIME           , - 62, "Timer expired"                                     );
	GDEFINE_ENUM_VALUED(ERRNO, ENOSR           , - 63, "Out of streams resources"                          );
	GDEFINE_ENUM_VALUED(ERRNO, ENONET          , - 64, "Machine is not on the network"                     );
	GDEFINE_ENUM_VALUED(ERRNO, ENOPKG          , - 65, "Package not installed"                             );
	GDEFINE_ENUM_VALUED(ERRNO, EREMOTE         , - 66, "Object is remote"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, ENOLINK         , - 67, "Link has been severed"                             );
	GDEFINE_ENUM_VALUED(ERRNO, EADV            , - 68, "Advertise error"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, ESRMNT          , - 69, "Srmount error"                                     );
	GDEFINE_ENUM_VALUED(ERRNO, ECOMM           , - 70, "Communication error on send"                       );
	GDEFINE_ENUM_VALUED(ERRNO, EPROTO          , - 71, "Protocol error"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, EMULTIHOP       , - 72, "Multihop attempted"                                );
	GDEFINE_ENUM_VALUED(ERRNO, EDOTDOT         , - 73, "RFS specific error"                                );
	GDEFINE_ENUM_VALUED(ERRNO, EBADMSG         , - 74, "Bad message"                                       );
	GDEFINE_ENUM_VALUED(ERRNO, EOVERFLOW       , - 75, "Value too large for defined data type"             );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTUNIQ        , - 76, "Name not unique on network"                        );
	GDEFINE_ENUM_VALUED(ERRNO, EBADFD          , - 77, "File descriptor in bad state"                      );
	GDEFINE_ENUM_VALUED(ERRNO, EREMCHG         , - 78, "Remote address changed"                            );
	GDEFINE_ENUM_VALUED(ERRNO, ELIBACC         , - 79, "Can not access a needed shared library"            );
	GDEFINE_ENUM_VALUED(ERRNO, ELIBBAD         , - 80, "Accessing a corrupted shared library"              );
	GDEFINE_ENUM_VALUED(ERRNO, ELIBSCN         , - 81, ".lib section in a.out corrupted"                   );
	GDEFINE_ENUM_VALUED(ERRNO, ELIBMAX         , - 82, "Attempting to link in too many shared libraries"   );
	GDEFINE_ENUM_VALUED(ERRNO, ELIBEXEC        , - 83, "Cannot exec a shared library directly"             );
	GDEFINE_ENUM_VALUED(ERRNO, EILSEQ          , - 84, "Invalid or incomplete multibyte or wide character" );
	GDEFINE_ENUM_VALUED(ERRNO, ERESTART        , - 85, "Interrupted system call should be restarted"       );
	GDEFINE_ENUM_VALUED(ERRNO, ESTRPIPE        , - 86, "Streams pipe error"                                );
	GDEFINE_ENUM_VALUED(ERRNO, EUSERS          , - 87, "Too many users"                                    );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTSOCK        , - 88, "Socket operation on non-socket"                    );
	GDEFINE_ENUM_VALUED(ERRNO, EDESTADDRREQ    , - 89, "Destination address required"                      );
	GDEFINE_ENUM_VALUED(ERRNO, EMSGSIZE        , - 90, "Message too long"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, EPROTOTYPE      , - 91, "Protocol wrong type for socket"                    );
	GDEFINE_ENUM_VALUED(ERRNO, ENOPROTOOPT     , - 92, "Protocol not available"                            );
	GDEFINE_ENUM_VALUED(ERRNO, EPROTONOSUPPORT , - 93, "Protocol not supported"                            );
	GDEFINE_ENUM_VALUED(ERRNO, ESOCKTNOSUPPORT , - 94, "Socket type not supported"                         );
	GDEFINE_ENUM_VALUED(ERRNO, EOPNOTSUPP      , - 95, "Operation not supported"                           );
	GDEFINE_ENUM_VALUED(ERRNO, EPFNOSUPPORT    , - 96, "Protocol family not supported"                     );
	GDEFINE_ENUM_VALUED(ERRNO, EAFNOSUPPORT    , - 97, "Address family not supported by protocol"          );
	GDEFINE_ENUM_VALUED(ERRNO, EADDRINUSE      , - 98, "Address already in use"                            );
	GDEFINE_ENUM_VALUED(ERRNO, EADDRNOTAVAIL   , - 99, "Cannot assign requested address"                   );
	GDEFINE_ENUM_VALUED(ERRNO, ENETDOWN        , -100, "Network is down"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, ENETUNREACH     , -101, "Network is unreachable"                            );
	GDEFINE_ENUM_VALUED(ERRNO, ENETRESET       , -102, "Network dropped connection on reset"               );
	GDEFINE_ENUM_VALUED(ERRNO, ECONNABORTED    , -103, "Software caused connection abort"                  );
	GDEFINE_ENUM_VALUED(ERRNO, ECONNRESET      , -104, "Connection reset by peer"                          );
	GDEFINE_ENUM_VALUED(ERRNO, ENOBUFS         , -105, "No buffer space available"                         );
	GDEFINE_ENUM_VALUED(ERRNO, EISCONN         , -106, "Transport endpoint is already connected"           );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTCONN        , -107, "Transport endpoint is not connected"               );
	GDEFINE_ENUM_VALUED(ERRNO, ESHUTDOWN       , -108, "Cannot send after transport endpoint shutdown"     );
	GDEFINE_ENUM_VALUED(ERRNO, ETOOMANYREFS    , -109, "Too many references: cannot splice"                );
	GDEFINE_ENUM_VALUED(ERRNO, ETIMEDOUT       , -110, "Connection timed out"                              );
	GDEFINE_ENUM_VALUED(ERRNO, ECONNREFUSED    , -111, "Connection refused"                                );
	GDEFINE_ENUM_VALUED(ERRNO, EHOSTDOWN       , -112, "Host is down"                                      );
	GDEFINE_ENUM_VALUED(ERRNO, EHOSTUNREACH    , -113, "No route to host"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, EALREADY        , -114, "Operation already in progress"                     );
	GDEFINE_ENUM_VALUED(ERRNO, EINPROGRESS     , -115, "Operation now in progress"                         );
	GDEFINE_ENUM_VALUED(ERRNO, ESTALE          , -116, "Stale file handle"                                 );
	GDEFINE_ENUM_VALUED(ERRNO, EUCLEAN         , -117, "Structure needs cleaning"                          );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTNAM         , -118, "Not a XENIX named type file"                       );
	GDEFINE_ENUM_VALUED(ERRNO, ENAVAIL         , -119, "No XENIX semaphores available"                     );
	GDEFINE_ENUM_VALUED(ERRNO, EISNAM          , -120, "Is a named type file"                              );
	GDEFINE_ENUM_VALUED(ERRNO, EREMOTEIO       , -121, "Remote I/O error"                                  );
	GDEFINE_ENUM_VALUED(ERRNO, EDQUOT          , -122, "Disk quota exceeded"                               );
	GDEFINE_ENUM_VALUED(ERRNO, ENOMEDIUM       , -123, "No medium found"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, EMEDIUMTYPE     , -124, "Wrong medium type"                                 );
	GDEFINE_ENUM_VALUED(ERRNO, ECANCELED       , -125, "Operation canceled"                                );
	GDEFINE_ENUM_VALUED(ERRNO, ENOKEY          , -126, "Required key not available"                        );
	GDEFINE_ENUM_VALUED(ERRNO, EKEYEXPIRED     , -127, "Key has expired"                                   );
	GDEFINE_ENUM_VALUED(ERRNO, EKEYREVOKED     , -128, "Key has been revoked"                              );
	GDEFINE_ENUM_VALUED(ERRNO, EKEYREJECTED    , -129, "Key was rejected by service"                       );
	GDEFINE_ENUM_VALUED(ERRNO, EOWNERDEAD      , -130, "Owner died"                                        );
	GDEFINE_ENUM_VALUED(ERRNO, ENOTRECOVERABLE , -131, "State not recoverable"                             );
	GDEFINE_ENUM_VALUED(ERRNO, ERFKILL         , -132, "Operation not possible due to RF-kill"             );
	GDEFINE_ENUM_VALUED(ERRNO, EHWPOISON       , -133, "Memory page has hardware error"                    );
} // namespace

#endif // GPK_CERRNO_H
