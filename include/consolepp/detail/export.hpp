
#ifndef CONSOLEPP_EXPORT_H
#define CONSOLEPP_EXPORT_H

#ifdef CONSOLEPP_STATIC_DEFINE
#  define CONSOLEPP_EXPORT
#  define CONSOLEPP_NO_EXPORT
#else
#  ifndef CONSOLEPP_EXPORT
#    ifdef consolepp_EXPORTS
        /* We are building this library */
#      define CONSOLEPP_EXPORT 
#    else
        /* We are using this library */
#      define CONSOLEPP_EXPORT 
#    endif
#  endif

#  ifndef CONSOLEPP_NO_EXPORT
#    define CONSOLEPP_NO_EXPORT 
#  endif
#endif

#ifndef CONSOLEPP_DEPRECATED
#  define CONSOLEPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CONSOLEPP_DEPRECATED_EXPORT
#  define CONSOLEPP_DEPRECATED_EXPORT CONSOLEPP_EXPORT CONSOLEPP_DEPRECATED
#endif

#ifndef CONSOLEPP_DEPRECATED_NO_EXPORT
#  define CONSOLEPP_DEPRECATED_NO_EXPORT CONSOLEPP_NO_EXPORT CONSOLEPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CONSOLEPP_NO_DEPRECATED
#    define CONSOLEPP_NO_DEPRECATED
#  endif
#endif

#endif
