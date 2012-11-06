#ifndef _RGLPrivate_h
#define _RGLPrivate_h


#include "../export/RGL/rgl.h"
#include "Types.h"
#include "Utils.h"
#include "ReportInternal.h"

#ifndef OS_VERSION_NUMERIC
#define OS_VERSION_NUMERIC 0x160
#endif

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
#define _RGL_EXTERN_C extern "C"
#else
#define _RGL_EXTERN_C
#endif

   extern RGL_EXPORT RGLcontext*	_CurrentContext;
   extern RGLdevice*	_CurrentDevice;

   extern RGL_EXPORT char*    rglVersion;


   // only for internal purpose
#define GL_UNSIGNED_BYTE_4_4 0x4A00
#define GL_UNSIGNED_BYTE_4_4_REV 0x4A01
#define GL_UNSIGNED_BYTE_6_2 0x4A02
#define GL_UNSIGNED_BYTE_2_6_REV 0x4A03
#define GL_UNSIGNED_SHORT_12_4 0x4A04
#define GL_UNSIGNED_SHORT_4_12_REV 0x4A05
#define GL_UNSIGNED_BYTE_2_2_2_2 0x4A06
#define GL_UNSIGNED_BYTE_2_2_2_2_REV 0x4A07

#define GL_FLOAT_RGBA32 0x888B
   //------------------------------------------------------------------------
   // Function prototypes
   //------------------------------------------------------------------------
   //----------------------------------------
   // RGL.c
   //----------------------------------------
   typedef void( * RGLcontextHookFunction )( RGLcontext *context );
   extern RGL_EXPORT RGLcontextHookFunction rglContextCreateHook;
   extern RGL_EXPORT RGLcontextHookFunction rglContextDestroyHook;

   extern RGLcontext*	rglContextCreate();
   extern void		rglContextFree( RGLcontext* LContext );
   extern void		rglSetError( GLenum error );
   extern GLuint	rglValidateStates( GLuint mask );
   void rglAttachContext( RGLdevice *device, RGLcontext* context );
   void rglDetachContext( RGLdevice *device, RGLcontext* context );
   void rglInvalidateAllStates( RGLcontext* context );
   void rglResetAttributeState( rglAttributeState* as );
   void rglSetFlipHandler(void (*handler)(const GLuint head), RGLdevice *device);
   void rglSetVBlankHandler(void (*handler)(const GLuint head), RGLdevice *device);

   //----------------------------------------
   // Texture.c
   //----------------------------------------
   rglTexture *rglAllocateTexture();
   void rglFreeTexture( rglTexture *texture );
   void rglTextureUnbind( RGLcontext* context, GLuint name );
   extern int	rglTextureInit( RGLcontext* context, GLuint name );
   extern void	rglTextureDelete( RGLcontext* context, GLuint name );
   extern GLboolean rglTextureHasValidLevels( const rglTexture *texture, int levels, int width, int height, int depth, GLenum format, GLenum type, GLenum internalFormat );
   extern GLboolean rglTextureIsValid( const rglTexture* texture );
   GLenum rglGetEnabledTextureMode( const rglTextureImageUnit *unit );
   extern rglTexture *rglGetCurrentTexture( const rglTextureImageUnit *unit, GLenum target );
   RGL_EXPORT void rglUpdateCurrentTextureCache( rglTextureImageUnit *unit );
   void rglReallocateImages( rglTexture *texture, GLint level, GLsizei dimension );
   extern int rglGetImage( GLenum target, GLint level, rglTexture **texture, rglImage **image, GLsizei reallocateSize );
   static inline rglTexture* rglGetTexture( RGLcontext *LContext, GLuint name )
   {
      return ( rglTexture* )LContext->textureNameSpace.data[name];
   }

   static inline rglTexture* rglGetTextureSafe( RGLcontext *LContext, GLuint name )
   {
      return rglTexNameSpaceIsName( &LContext->textureNameSpace, name ) ? ( rglTexture* )LContext->textureNameSpace.data[name] : NULL;
   }

   static inline rglFramebuffer *rglGetFramebuffer( RGLcontext *LContext, GLuint name );
   inline static void rglTextureTouchFBOs( rglTexture *texture )
   {
      RGLcontext *LContext = _CurrentContext;
      if ( !LContext ) return; // may be called in psglDestroyContext
      // check if bound to any framebuffer
      GLuint fbCount = texture->framebuffers.getCount();
      if ( fbCount > 0 )
      {
         rglFramebuffer *contextFramebuffer = LContext->framebuffer ? rglGetFramebuffer( LContext, LContext->framebuffer ) : NULL;
         for ( GLuint i = 0;i < fbCount;++i )
         {
            rglFramebuffer* framebuffer = texture->framebuffers[i];
            framebuffer->needValidate = GL_TRUE;
            if ( RGL_UNLIKELY( framebuffer == contextFramebuffer ) ) LContext->needValidate |= PSGL_VALIDATE_SCISSOR_BOX | PSGL_VALIDATE_FRAMEBUFFER;
         }
      }
   }

   //----------------------------------------
   // Image.c
   //----------------------------------------
   GLboolean rglIsType( GLenum type );
   GLboolean rglIsFormat( GLenum format );
   GLboolean rglIsValidPair( GLenum format, GLenum type );
   void rglImageAllocCPUStorage( rglImage *image );
   void rglImageFreeCPUStorage( rglImage *image );
   extern void	rglSetImage( rglImage* image, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLsizei alignment, GLenum format, GLenum type, const GLvoid* pixels );
   extern void	rglSetSubImage( GLenum target, GLint level, rglTexture *texture, rglImage* image, GLint x, GLint y, GLint z, GLsizei width, GLsizei height, GLsizei depth, GLsizei alignment, GLenum format, GLenum type, const GLvoid* pixels );
   extern int	rglGetPixelSize( GLenum format, GLenum type );
   static inline int rglGetStorageSize( GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth )
   {
      return rglGetPixelSize( format, type )*width*height*depth;
   }

   extern int rglGetTypeSize( GLenum type );
   extern int	rglGetMaxBitSize( GLenum type );
   extern int	rglGetStorageSize( GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth );
   extern void rglImageToRaster( const rglImage* image, rglRaster* raster, GLuint x, GLuint y, GLuint z );
   extern void rglRasterToImage( const rglRaster* raster, rglImage* image, GLuint x, GLuint y, GLuint z );
   extern void rglRawRasterToImage( const rglRaster* raster, rglImage* image, GLuint x, GLuint y, GLuint z );
   void rglResampleImage3D( rglImage* src, rglImage* dst );

   //----------------------------------------
   // Sync.c
   //----------------------------------------
   rglFenceObject *rglCreateFenceObject();
   void rglFreeFenceObject( rglFenceObject *fence );
   static inline rglFenceObject *rglGetFenceObject( RGLcontext *LContext, GLuint name )
   {
      return ( rglFenceObject * )LContext->fenceObjectNameSpace.data[name];
   }

   //----------------------------------------
   // FramebufferObject.c
   //----------------------------------------
   rglFramebuffer *rglCreateFramebuffer( void );
   void rglDestroyFramebuffer( rglFramebuffer *framebuffer );
   static inline rglFramebuffer *rglGetFramebuffer( RGLcontext *LContext, GLuint name )
   {
      return ( rglFramebuffer * )LContext->framebufferNameSpace.data[name];
   }

   static inline rglFramebuffer *rglGetFramebufferSafe( RGLcontext *LContext, GLuint name )
   {
      return rglTexNameSpaceIsName( &LContext->framebufferNameSpace, name ) ? ( rglFramebuffer * )LContext->framebufferNameSpace.data[name] : NULL;
   }

   void rglFramebufferGetAttachmentTexture( RGLcontext* LContext, const rglFramebufferAttachment* attachment, rglTexture** texture, GLuint* face );
   GLenum rglPlatformFramebufferCheckStatus( rglFramebuffer* framebuffer );
   GLboolean rglPlatformGenerateMipmap( rglTexture* texture, GLuint face );
   void rglPlatformFramebufferGetParameteriv( GLenum pname, GLint* params );
   void rglPlatformResolveFramebuffer();
   void rglGetFramebufferSize( GLuint* width, GLuint* height );

   //----------------------------------------
   // VertexArray.c
   //----------------------------------------
   void rglVertexAttrib1fNV( GLuint index, GLfloat x );
   void rglVertexAttrib1fvNV( GLuint index, const GLfloat* v );
   void rglVertexAttrib2fNV( GLuint index, GLfloat x, GLfloat y );
   void rglVertexAttrib2fvNV( GLuint index, const GLfloat* v );
   void rglVertexAttrib3fNV( GLuint index, GLfloat x, GLfloat y, GLfloat z );
   void rglVertexAttrib3fvNV( GLuint index, const GLfloat* v );
   void rglVertexAttrib4fNV( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
   void rglVertexAttrib4fvNV( GLuint index, const GLfloat* v );
   void rglVertexAttribPointerNV( GLuint index, GLint fsize, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer );
   void rglVertexAttribElementFunc( GLuint index, GLenum func, GLuint frequency );
   void rglEnableVertexAttribArrayNV( GLuint index );
   void rglDisableVertexAttribArrayNV( GLuint index );
   GLint rglConvertStream( rglAttributeState* asDst, const rglAttributeState* asSrc, GLuint index,
         GLint skip, GLint first, GLint count,
         const void* indices, GLenum indexType );
   void rglComputeMinMaxIndices( RGLcontext* LContext, GLuint* min, GLuint* max, const void* indices, GLenum indexType, GLsizei count );

   //----------------------------------------
   // Platform/Init.c
   //----------------------------------------
   extern void rglPlatformInit( PSGLinitOptions* options );
   extern void rglPlatformExit();

   //----------------------------------------
   // Device/Device.c
   //----------------------------------------
   extern void rglDeviceInit( PSGLinitOptions* options );
   extern void rglDeviceExit();
   extern PSGLdeviceParameters * rglShadowDeviceParameters( void );


   //----------------------------------------
   // Device/.../PlatformDevice.c
   //----------------------------------------
   extern GLboolean		rglPlatformDeviceInit( PSGLinitOptions* options );
   extern void		rglPlatformDeviceExit();
   extern int		rglPlatformDeviceSize();
   extern int		rglPlatformCreateDevice( RGLdevice* device );
   extern void		rglPlatformDestroyDevice( RGLdevice* device );
   extern void		rglPlatformMakeCurrent( void* device );
   extern void		rglPlatformSwapBuffers( RGLdevice* device );
   extern const GLvoid*	rglPlatformGetProcAddress( const char *funcName );

   //----------------------------------------
   // Raster/.../PlatformRaster.c
   //----------------------------------------
   void*	rglPlatformRasterInit();
   void	rglPlatformRasterExit( void* driver );
   void	rglPlatformRasterDestroyResources();
   void	rglPlatformDraw( rglDrawParams* dparams );
   GLboolean rglPlatformNeedsConversion( const rglAttributeState* as, GLuint index );
   // [YLIN] Try to avoid LHS inside this function.
   //   In oringinal implementation, indexType and indexCount will be stored right before this function
   //   and since we will load them right after enter this function, there are LHS.
   GLboolean rglPlatformRequiresSlowPath( rglDrawParams* dparams, const GLenum indexType, uint32_t indexCount);
   void rglPlatformRasterGetIntegerv( GLenum pname, GLint* params );
   void	rglPlatformRasterFlush();
   void	rglPlatformRasterFinish();
   void	rglValidateFragmentProgram();
   void	rglValidateFragmentProgramSharedConstants();
   void	rglValidateClipPlanes();
   void	rglInvalidateAttributes();
   GLuint	rglValidateAttributes( const GLvoid* indices, GLboolean *isMain );
   GLuint	rglValidateAttributesSlow( rglDrawParams *dparams, GLboolean *isMain );

   //----------------------------------------
   // Raster/.../PlatformTexture.c
   //----------------------------------------
   extern int	rglPlatformTextureSize();
   extern int	rglPlatformTextureMaxUnits();
   extern void	rglPlatformCreateTexture( rglTexture* texture );
   extern void	rglPlatformDestroyTexture( rglTexture* texture );
   extern void	rglPlatformValidateTextureStage( int unit, rglTexture*texture );
   void rglPlatformValidateVertexTextures();
   extern GLenum rglPlatformChooseInternalStorage( rglImage* image, GLenum internalformat );
   extern GLenum rglPlatformTranslateTextureFormat( GLenum internalFormat );
   extern void rglPlatformCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height );
   GLenum rglPlatformChooseInternalFormat( GLenum internalformat );
   void rglPlatformExpandInternalFormat( GLenum internalformat, GLenum *format, GLenum *type );
   void rglPlatformGetImageData( GLenum target, GLint level, rglTexture *texture, rglImage *image );
   extern void rglPlatformSetCompressedTexture(
         GLenum target,
         GLint level,
         GLenum internalformat,
         GLsizei width, GLsizei height, GLsizei depth,
         GLint border,
         GLsizei imageSize,
         const GLvoid* data );
   extern void rglPlatformSetCompressedTextureSub(
         GLenum target,
         GLint level,
         GLint xoffset, GLint yoffset, GLint zoffset,
         GLsizei width, GLsizei height, GLsizei depth,
         GLenum format,
         GLsizei imageSize,
         const GLvoid* data );
   extern GLboolean rglPlatformTexturePBOImage(
         rglTexture* texture,
         rglImage* image,
         GLint level,
         GLint internalformat,
         GLsizei width, GLsizei height, GLsizei depth,
         GLenum format, GLenum type,
         const GLvoid *offset );
   extern GLboolean rglPlatformTexturePBOSubImage(
         rglTexture* texture,
         rglImage* image,
         GLint level,
         GLint xoffset, GLint yoffset, GLint zoffset,
         GLsizei width, GLsizei height, GLsizei depth,
         GLenum format, GLenum type,
         const GLvoid *pixels );
   GLboolean rglPlatformTextureReference( rglTexture *texture, GLuint pitch, rglBufferObject *bufferObject, GLintptr offset );

   //----------------------------------------
   // Raster/.../PlatformFBops.c
   //----------------------------------------
   extern void rglFBClear( GLbitfield mask );
   extern void rglValidateFramebuffer( void );
   extern void rglValidateFFXVertexProgram();
   extern void rglValidateFFXFragmentProgram();
   extern void rglPlatformReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLboolean flip, GLenum format, GLenum type, GLvoid *pixels );
   extern GLboolean rglPlatformReadPBOPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLboolean flip, GLenum format, GLenum type, GLvoid *pixels );

   //----------------------------------------
   // Raster/.../PlatformTNL.c
   //----------------------------------------
   void rglValidateNormalize();
   void rglValidateVertexProgram();
   void rglValidateVertexConstants();

   //----------------------------------------
   // Raster/.../PlatformBuffer.c
   //----------------------------------------
   int rglPlatformBufferObjectSize();
   GLboolean rglPlatformCreateBufferObject( rglBufferObject* bufferObject );
   void rglPlatformDestroyBufferObject( rglBufferObject* bufferObject );
   void rglPlatformBufferObjectSetData( rglBufferObject* bufferObject, GLintptr offset, GLsizeiptr size, const GLvoid *data, GLboolean tryImmediateCopy );
   GLvoid rglPlatformBufferObjectCopyData( rglBufferObject* bufferObjectDst, rglBufferObject* bufferObjectSrc );
   // map / unmap buffers. Internally refcounted
   char *rglPlatformBufferObjectMap( rglBufferObject* bufferObject, GLenum access );
   GLboolean rglPlatformBufferObjectUnmap( rglBufferObject* bufferObject );
   void rglPlatformGetBufferParameteriv( rglBufferObject *bufferObject, GLenum pname, int *params );

   // this is shared in glBindTexture and cgGL code
   RGL_EXPORT void rglBindTextureInternal( rglTextureImageUnit *unit, GLuint name, GLenum target );
   void rglBindVertexTextureInternal( GLuint unit, GLuint name );

   //----------------------------------------
   // Raster/.../PlatformSync.c
   //----------------------------------------
   int rglPlatformFenceObjectSize();
   GLboolean rglPlatformCreateFenceObject( rglFenceObject* fenceObject );
   void rglPlatformDestroyFenceObject( rglFenceObject* fenceObject );
   void rglPlatformSetFence( rglFenceObject* fenceObject );
   GLboolean rglPlatformTestFence( rglFenceObject* fenceObject );
   void rglPlatformFinishFence( rglFenceObject* fenceObject );

#ifdef __cplusplus
}	// Close scope of 'extern "C"' declaration that encloses file.
#endif

#endif	// _RGLPrivate_h
