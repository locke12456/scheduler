#ifndef DLL_EXPORT_H
#define DLL_EXPORT_H
#ifdef _WIN32
#ifdef DLL_BUILT_AS_STATIC
#  define DLL_EXPORT
#  define DLL_NO_EXPORT
#else
#  ifndef DLL_EXPORT
#    ifdef DLL_EXPORTS
/* We are building this library */
#      define DLL_EXPORT __declspec(dllexport)
#    else
/* We are using this library */
#      define DLL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef DLL_NO_EXPORT
#    define DLL_NO_EXPORT 
#  endif
#endif

#ifndef DLL_DEPRECATED
#  define DLL_DEPRECATED __declspec(deprecated)
#endif

#ifndef DLL_DEPRECATED_EXPORT
#  define DLL_DEPRECATED_EXPORT DLL_EXPORT DLL_DEPRECATED
#endif

#ifndef DLL_DEPRECATED_NO_EXPORT
#  define DLL_DEPRECATED_NO_EXPORT DLL_NO_EXPORT DLL_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define DLL_NO_DEPRECATED
#endif
#else 
#  define DLL_EXPORT
#endif

#endif