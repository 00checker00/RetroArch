#ifndef _RGLUTILS_H_
#define _RGLUTILS_H_

#include "Types.h"
#include "../export/RGL/export.h"
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSVC
   // MAX that works with side effects. Just in case.
#define MAX(A,B) ({				\
      __typeof__(A) At=(A);	\
      __typeof__(B) Bt=(B);	\
      At>Bt?At:Bt; })

#define MIN(A,B) ({				\
      __typeof__(A) At=(A);	\
      __typeof__(B) Bt=(B);	\
      At<Bt?At:Bt; })

#define RGL_LIKELY(COND) __builtin_expect((COND),1)
#define RGL_UNLIKELY(COND) __builtin_expect((COND),0)

#else
#define MAX(A,B) ((A)>(B)?(A):(B))
#define MIN(A,B) ((A)<(B)?(A):(B))
#define RGL_LIKELY(COND) (COND)
#define RGL_UNLIKELY(COND) (COND)
#endif

#define _RGL_FLOAT_AS_UINT(x) ({union {float f; unsigned int i;} u; u.f=(x); u.i;})

   static inline float rglClampf( const float value )
   {
      return MAX( MIN( value, 1.f ), 0.f );
   }

   static inline unsigned short endianSwapHalf( unsigned short v )
   {
      return ( v >> 8 & 0x00ff ) | ( v << 8 & 0xff00 );
   }

   static inline unsigned int endianSwapWord( unsigned int v )
   {
      return ( v&0xff ) << 24 | ( v&0xff00 ) << 8 |
         ( v&0xff0000 ) >> 8 | ( v&0xff000000 ) >> 24;
   }

   static inline unsigned int endianSwapWordByHalf( unsigned int v )
   {
      return ( v&0xffff ) << 16 | v >> 16;
   }

   static inline int rglLog2( unsigned int i )
   {
      int l = 0;
      while ( i )
      {
         ++l;
         i >>= 1;
      }
      return l -1;
   }

   static inline int rglIsPow2( unsigned int i )
   {
      return ( i&( i - 1 ) ) == 0;
   }

   // Pad argument x to the next multiple of argument pad.
   static inline unsigned long rglPad( unsigned long x, unsigned long pad )
   {
      return ( x + pad - 1 ) / pad*pad;
   }

   // Pad pointer x to the next multiple of argument pad.
   static inline char* rglPadPtr( const char* p, unsigned int pad )
   {
      intptr_t x = (intptr_t)p;
      x = ( x + pad - 1 ) / pad * pad;
      return ( char* )x;
   }

   // names API

   RGL_EXPORT void rglInitNameSpace (void *data);
   RGL_EXPORT void rglFreeNameSpace (void *data);
   RGL_EXPORT unsigned int rglCreateName (void *data, void* object);
   RGL_EXPORT unsigned int rglIsName( void *data, unsigned int name);
   RGL_EXPORT void rglEraseName (void *data, unsigned int name);

   static inline void *rglGetNamedValue(void *data, unsigned int name )
   {
      struct rglNameSpace *ns = (struct rglNameSpace*)data;
      return ns->data[name - 1];
   }

   void rglTexNameSpaceInit(void *data, rglTexNameSpaceCreateFunction create, rglTexNameSpaceDestroyFunction destroy );
   void rglTexNameSpaceFree(void *data);
   void rglTexNameSpaceResetNames(void *data);
   GLuint rglTexNameSpaceGetFree(void *data);
   GLboolean rglTexNameSpaceCreateNameLazy(void *data, GLuint name );
   GLboolean rglTexNameSpaceIsName(void *data, GLuint name );
   void rglTexNameSpaceGenNames(void *data, GLsizei n, GLuint *names );
   void rglTexNameSpaceDeleteNames(void *data, GLsizei n, const GLuint *names );
   void rglTexNameSpaceReinit(void *saved, void *active);


#ifdef __cplusplus
}
#endif

#ifndef RGLT_UNUSED
#ifdef MSVC
#define RGL_UNUSED(value) value;
#else
#define RGL_UNUSED(value) do { \
   __typeof__(value) rglUnused = value; \
   (void)rglUnused; \
} while(false)
#endif
#endif

#endif // _RGL_UTILS_H_
