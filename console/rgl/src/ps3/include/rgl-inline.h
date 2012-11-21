// Clamp a float to [0,1]:
//   The order of clamp comparisons are arranged so that NaN maps to min
//   when compiled with VisualC or gcc.  IEEE says comparisons with NaN
//   should always fail (gcc implements this) but VisualC sometimes uses
//   signed integer compares rather than floating-point compares which is
//   incorrect in the case of NaN.
#define RGLGCM_CLAMPF_01(x) ((x) >= 0.0f ? ((x) > 1.0f ? 1.0f : (x)) : 0.0f)

static inline GLboolean rglIsDrawableColorFormat( GLenum colorFormat )
{
   switch ( colorFormat )
   {
      case RGLGCM_ARGB8:
      case RGLGCM_FLOAT_R32:
         return GL_TRUE;
      default:
         return GL_FALSE;
   }
}

static inline GLuint rglPlatformGetBitsPerPixel( GLenum internalFormat )
{
   switch ( internalFormat )
   {
      case RGLGCM_ALPHA16:
      case RGLGCM_HILO8:
      case RGLGCM_RGB5_A1_SCE:
      case RGLGCM_RGB565_SCE:
         return 16;
      case RGLGCM_ALPHA8:
         return 8;
      case RGLGCM_RGBX8:
      case RGLGCM_RGBA8:
      case RGLGCM_ABGR8:
      case RGLGCM_ARGB8:
      case RGLGCM_BGRA8:
      case RGLGCM_FLOAT_R32:
      case RGLGCM_HILO16:
      case RGLGCM_XBGR8:
         return 32;
      default:
         return 0;
   }
}


#define SUBPIXEL_BITS 12
#define SUBPIXEL_ADJUST (0.5/(1<<SUBPIXEL_BITS))

/* the affine transformation of x and y from normalized device coordinates to
 ** window coordinates.
 **
 ** notes:
 **   - 1:1 port of glViewport (see spec for details)
 **   - x, y, width, height are clamped to the surface limits
 **   - initially, x, y width and height are zero!!
 **   - (0,0) is in the lower left [yInverted == TRUE]
 **   - important: because there is no concept of a window it is the callers
 **     responsibility to set the viewport to non zero values
 **   - [Jun] Two new paramenters zNear and zFar with default values to make
 **     this function compatible with cellGcmSetViewport
 */
static inline void rglGcmFifoGlViewport( GLint x, GLint y, GLsizei width, GLsizei height, GLclampf zNear = 0.0f, GLclampf zFar = 1.0f )
{
   rglGcmViewportState *vp = &rglGcmState_i.state.viewport;
   rglGcmRenderTarget *rt = &rglGcmState_i.renderTarget;
   GLint clipX0, clipX1, clipY0, clipY1;

   // keep for yInverted handling/disable+enable
   vp->x = x;
   vp->y = y;
   vp->w = width;
   vp->h = height;

   // clamp to hw limits
   clipX0 = x;
   clipX1 = x + width;
   if ( rt->yInverted )
   {
      clipY0 = rt->gcmRenderTarget.height - ( y + height );
      clipY1 = rt->gcmRenderTarget.height - y;
   }
   else
   {
      clipY0 = y;
      clipY1 = y + height;
   }

   if ( clipX0 < 0 )
      clipX0 = 0;
   if ( clipY0 < 0 )
      clipY0 = 0;

   if ( clipX1 >= RGLGCM_MAX_RT_DIMENSION )
      clipX1 = RGLGCM_MAX_RT_DIMENSION;

   if ( clipY1 >= RGLGCM_MAX_RT_DIMENSION )
      clipY1 = RGLGCM_MAX_RT_DIMENSION;

   if (( clipX1 <= clipX0 ) || ( clipY1 <= clipY0 ) )
      clipX0 = clipY0 = clipX1 = clipY1 = 0;

   // update viewport info
   vp->xScale = width * 0.5f;
   vp->xCenter = ( GLfloat )( x + vp->xScale + RGLGCM_SUBPIXEL_ADJUST );

   if ( rt->yInverted )
   {
      vp->yScale = height * -0.5f;
      vp->yCenter = ( GLfloat )( rt->gcmRenderTarget.height - RGLGCM_VIEWPORT_EPSILON - y +  vp->yScale + RGLGCM_SUBPIXEL_ADJUST );
   }
   else
   {
      vp->yScale = height * 0.5f;
      vp->yCenter = ( GLfloat )( y +  vp->yScale + RGLGCM_SUBPIXEL_ADJUST );
   }

   // Clamp depth range to legal values
   zNear = RGLGCM_CLAMPF_01( zNear );
   zFar  = RGLGCM_CLAMPF_01( zFar );

   // compute viewport values for hw [no doubles, so we might loose a few lsb]
   GLfloat z_scale = ( GLfloat )( 0.5f * ( zFar - zNear ) );
   GLfloat z_center = ( GLfloat )( 0.5f * ( zFar + zNear ) );

   // hw zNear/zFar clipper
   if ( zNear > zFar )
   {
      GLclampf tmp = zNear;
      zNear = zFar;
      zFar = tmp;
   }

   float scale[4] = {  vp->xScale,  vp->yScale,  z_scale, 0.0f };
   float offset[4] = {   vp->xCenter,  vp->yCenter,  z_center, 0.0f };

   GCM_FUNC( cellGcmSetViewport, clipX0, clipY0, clipX1 - clipX0,
         clipY1 - clipY0, zNear, zFar, scale, offset );
}

/* When you call glDrawArrays, count sequential elements from each enabled
 ** array are used to construct a sequence of geometric primitives, beginning
 ** with the first element. The mode parameter specifies what kind of primitive
 ** to construct and how to use the array elements to construct the primitives.
 **
 ** note:
 **   - 1:1 port of glDrawArrays (see spec for details)
 **   - legal values for mode are RGLGCM_POINTS, RGLGCM_LINES, RGLGCM_LINE_LOOP, RGLGCM_LINE_STRIP,
 **     RGLGCM_TRIANGLES, RGLGCM_TRIANGLE_STRIP, RGLGCM_TRIANGLE_FAN, RGLGCM_QUADS, RGLGCM_QUAD_STRIP
 **     and RGLGCM_POLYGON
 **   - legal values for first, first+Count are [0, 0xfffff]
 **   - if vertex reusage exists, glDrawElements is preferred
 */
static inline void rglGcmFifoGlDrawArrays( rglGcmEnum mode, GLint first, GLsizei count )
{
   uint8_t gcmMode = 0;
   switch ( mode )
   {
      case RGLGCM_POINTS:
         gcmMode = CELL_GCM_PRIMITIVE_POINTS;
         break;
      case RGLGCM_LINES:
         gcmMode = CELL_GCM_PRIMITIVE_LINES;
         break;
      case RGLGCM_LINE_LOOP:
         gcmMode = CELL_GCM_PRIMITIVE_LINE_LOOP;
         break;
      case RGLGCM_LINE_STRIP:
         gcmMode = CELL_GCM_PRIMITIVE_LINE_STRIP;
         break;
      case RGLGCM_TRIANGLES:
         gcmMode = CELL_GCM_PRIMITIVE_TRIANGLES;
         break;
      case RGLGCM_TRIANGLE_STRIP:
         gcmMode = CELL_GCM_PRIMITIVE_TRIANGLE_STRIP;
         break;
      case RGLGCM_TRIANGLE_FAN:
         gcmMode = CELL_GCM_PRIMITIVE_TRIANGLE_FAN;
         break;
      case RGLGCM_QUADS:
         gcmMode = CELL_GCM_PRIMITIVE_QUADS;
         break;
      case RGLGCM_QUAD_STRIP:
         gcmMode = CELL_GCM_PRIMITIVE_QUAD_STRIP;
         break;
      case RGLGCM_POLYGON:
         gcmMode = CELL_GCM_PRIMITIVE_POLYGON;
         break;
      default:
         break;
   }

   if ( count )
   {
      GCM_FUNC_SAFE( cellGcmSetDrawArrays, gcmMode, first, count );
   }
}

   static inline void rglGcmFifoGlTransferDataVidToVid
(
 GLuint dstVidId,   
 GLuint dstVidIdOffset,
 GLuint dstPitch,
 GLuint dstX,
 GLuint dstY,
 GLuint srcVidId, 
 GLuint srcVidIdOffset,
 GLuint srcPitch,
 GLuint srcX,
 GLuint srcY,
 GLuint width,            // size in pixel
 GLuint height,
 GLuint bytesPerPixel
 )
{
   GLuint dstOffset = gmmIdToOffset(dstVidId) + dstVidIdOffset;
   GLuint srcOffset = gmmIdToOffset(srcVidId) + srcVidIdOffset;

   GCM_FUNC( cellGcmSetTransferImage, CELL_GCM_TRANSFER_LOCAL_TO_LOCAL, dstOffset, dstPitch, dstX, dstY, srcOffset, srcPitch, srcX, srcY, width, height, bytesPerPixel );
}

static inline GLuint rglGcmMapMinTextureFilter( GLenum filter )
{
   switch (filter)
   {
      case GL_NEAREST:
         return CELL_GCM_TEXTURE_NEAREST;
         break;
      case GL_LINEAR:
         return CELL_GCM_TEXTURE_LINEAR;
         break;
      case GL_NEAREST_MIPMAP_NEAREST:
         return CELL_GCM_TEXTURE_NEAREST_NEAREST;
         break;
      case GL_NEAREST_MIPMAP_LINEAR:
         return CELL_GCM_TEXTURE_NEAREST_LINEAR;
         break;
      case GL_LINEAR_MIPMAP_NEAREST:
         return CELL_GCM_TEXTURE_LINEAR_NEAREST;
         break;
      case GL_LINEAR_MIPMAP_LINEAR:
         return CELL_GCM_TEXTURE_LINEAR_LINEAR;
         break;
      default:
         return 0;
   }
   return filter;
}

static inline GLuint rglGcmMapMagTextureFilter( GLenum filter )
{
   switch ( filter )
   {
      case GL_NEAREST:
         return CELL_GCM_TEXTURE_NEAREST;
         break;
      case GL_LINEAR:
         return CELL_GCM_TEXTURE_LINEAR;
         break;
      default:
         return 0;
   }
   return filter;
}

static inline GLuint rglGcmMapWrapMode( GLuint mode )
{
   switch ( mode )
   {
      case RGLGCM_CLAMP:
         return CELL_GCM_TEXTURE_CLAMP;
         break;
      case RGLGCM_REPEAT:
         return CELL_GCM_TEXTURE_WRAP;
         break;
      case RGLGCM_CLAMP_TO_EDGE:
         return CELL_GCM_TEXTURE_CLAMP_TO_EDGE;
         break;
      case RGLGCM_CLAMP_TO_BORDER:
         return CELL_GCM_TEXTURE_BORDER;
         break;
      case RGLGCM_MIRRORED_REPEAT:
         return CELL_GCM_TEXTURE_MIRROR;
         break;
      case RGLGCM_MIRROR_CLAMP_TO_EDGE:
         return CELL_GCM_TEXTURE_MIRROR_ONCE_CLAMP_TO_EDGE;
         break;
      case RGLGCM_MIRROR_CLAMP_TO_BORDER:
         return CELL_GCM_TEXTURE_MIRROR_ONCE_BORDER;
         break;
      case RGLGCM_MIRROR_CLAMP:
         return CELL_GCM_TEXTURE_MIRROR_ONCE_CLAMP;
         break;
      default:
         return 0;
         break;
   }
   return 0;
}


static inline void rglGcmMapTextureFormat( GLuint internalFormat, uint8_t & gcmFormat, uint32_t & remap )
{
   gcmFormat = 0;

   switch ( internalFormat )
   {
      case RGLGCM_ALPHA8:                 // in_rgba = xxAx, out_rgba = 000A
         {
            gcmFormat =  CELL_GCM_TEXTURE_B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_ZERO,
                  CELL_GCM_TEXTURE_REMAP_ZERO,
                  CELL_GCM_TEXTURE_REMAP_ZERO );

         }
         break;
      case RGLGCM_ALPHA16:                // in_rgba = xAAx, out_rgba = 000A
         {
            gcmFormat =  CELL_GCM_TEXTURE_X16;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_ZERO,
                  CELL_GCM_TEXTURE_REMAP_ZERO,
                  CELL_GCM_TEXTURE_REMAP_ZERO );

         }
         break;
      case RGLGCM_HILO8:                  // in_rgba = HLxx, out_rgba = HL11
         {
            gcmFormat =  CELL_GCM_TEXTURE_COMPRESSED_HILO8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_ONE );

         }
         break;
      case RGLGCM_HILO16:                 // in_rgba = HLxx, out_rgba = HL11
         {
            gcmFormat =  CELL_GCM_TEXTURE_Y16_X16;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_ONE );

         }
         break;
      case RGLGCM_ARGB8:                  // in_rgba = RGBA, out_rgba = RGBA
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_BGRA8:                  // in_rgba = GRAB, out_rgba = RGBA ** NEEDS TO BE TESTED
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_RGBA8:                  // in_rgba = GBAR, out_rgba = RGBA ** NEEDS TO BE TESTED
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );
         }
         break;
      case RGLGCM_ABGR8:                  // in_rgba = BGRA, out_rgba = RGBA  ** NEEDS TO BE TESTED
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_RGBX8:                  // in_rgba = BGRA, out_rgba = RGB1  ** NEEDS TO BE TESTED
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_XBGR8:                  // in_rgba = BGRA, out_rgba = RGB1  ** NEEDS TO BE TESTED
         {
            gcmFormat =  CELL_GCM_TEXTURE_A8R8G8B8;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_FLOAT_R32:              // in_rgba = Rxxx, out_rgba = R001
         {
            gcmFormat =  CELL_GCM_TEXTURE_X32_FLOAT;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XYXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_ZERO,
                  CELL_GCM_TEXTURE_REMAP_ZERO );

         }
         break;
      case RGLGCM_RGB5_A1_SCE:          // in_rgba = RGBA, out_rgba = RGBA
         {
            gcmFormat =  CELL_GCM_TEXTURE_A1R5G5B5;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XXXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      case RGLGCM_RGB565_SCE:          // in_rgba = RGBA, out_rgba = RGBA
         {
            gcmFormat =  CELL_GCM_TEXTURE_R5G6B5;
            remap = CELL_GCM_REMAP_MODE(
                  CELL_GCM_TEXTURE_REMAP_ORDER_XXXY,
                  CELL_GCM_TEXTURE_REMAP_FROM_A,
                  CELL_GCM_TEXTURE_REMAP_FROM_R,
                  CELL_GCM_TEXTURE_REMAP_FROM_G,
                  CELL_GCM_TEXTURE_REMAP_FROM_B,
                  CELL_GCM_TEXTURE_REMAP_ONE,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP,
                  CELL_GCM_TEXTURE_REMAP_REMAP );

         }
         break;
      default:
         break;
   };

   return;
}

// Explicitly invalidate the L2 texture cache
static inline void rglGcmFifoGlInvalidateTextureCache( void )
{
   GCM_FUNC( cellGcmSetInvalidateTextureCache, CELL_GCM_INVALIDATE_TEXTURE );
}

// Fast conversion for values between 0.0 and 65535.0
GLuint inline static RGLGCM_QUICK_FLOAT2UINT( const GLfloat f )
{
   union
   {
      GLfloat f;
      GLuint ui;
   } t;
   t.f = f + RGLGCM_F0_DOT_0;
   return t.ui & 0xffff;
}

// construct a packed unsigned int ARGB8 color
void inline static RGLGCM_CALC_COLOR_LE_ARGB8( GLuint *color0, const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a )
{
   GLuint r2 = RGLGCM_QUICK_FLOAT2UINT( r * 255.0f );
   GLuint g2 = RGLGCM_QUICK_FLOAT2UINT( g * 255.0f );
   GLuint b2 = RGLGCM_QUICK_FLOAT2UINT( b * 255.0f );
   GLuint a2 = RGLGCM_QUICK_FLOAT2UINT( a * 255.0f );
   *color0 = ( a2 << 24 ) | ( r2 << 16 ) | ( g2 << 8 ) | ( b2 << 0 );
}

// Wait for the gpu to pass the given fence in the command buffer.
static inline void rglGcmFifoGlFinishFenceRef( const GLuint ref )
{
   rglGcmFifo *fifo = &rglGcmState_i.fifo;

   // wait for completion
   for ( ;; )
   {
      // gpu passed reference ?
      if ( !rglGcmFifoReferenceInUse( fifo, ref ) )
         break;

      // avoid polling on bus (interrupts memory traffic)
      sys_timer_usleep(10);
   }
}

#define RGLGCM_UTIL_LABEL_INDEX 253

// Utility to let RSX wait for complete RSX pipeline idle
static inline void rglGcmUtilWaitForIdle()
{
   // set write label command in push buffer, and wait
   // NOTE: this is for RSX to wailt
   GCM_FUNC( cellGcmSetWriteBackEndLabel, RGLGCM_UTIL_LABEL_INDEX, rglGcmState_i.labelValue );
   GCM_FUNC( cellGcmSetWaitLabel, RGLGCM_UTIL_LABEL_INDEX, rglGcmState_i.labelValue );

   // increment label value for next time. 
   rglGcmState_i.labelValue++; 

   // make sure the entire pipe in clear not just the front end 
   // Utility function that does GPU 'finish'.
   GCM_FUNC( cellGcmSetWriteBackEndLabel, RGLGCM_UTIL_LABEL_INDEX, 
         rglGcmState_i.labelValue );
   cellGcmFlush();

   while( *(cellGcmGetLabelAddress( RGLGCM_UTIL_LABEL_INDEX)) != rglGcmState_i.labelValue)
      sys_timer_usleep(30);

   rglGcmState_i.labelValue++;
}

// Sets the source and destination factor used for blending.
static inline void rglGcmFifoGlBlendFunc( rglGcmEnum sf, rglGcmEnum df, rglGcmEnum sfAlpha, rglGcmEnum dfAlpha )
{
   // syntax check
   switch ( sf )
   {
      case RGLGCM_ZERO:
      case RGLGCM_ONE:
      case RGLGCM_SRC_COLOR:
      case RGLGCM_ONE_MINUS_SRC_COLOR:
      case RGLGCM_SRC_ALPHA:
      case RGLGCM_ONE_MINUS_SRC_ALPHA:
      case RGLGCM_DST_ALPHA:
      case RGLGCM_ONE_MINUS_DST_ALPHA:
      case RGLGCM_DST_COLOR:
      case RGLGCM_ONE_MINUS_DST_COLOR:
      case RGLGCM_SRC_ALPHA_SATURATE:
      case RGLGCM_CONSTANT_COLOR:
      case RGLGCM_ONE_MINUS_CONSTANT_COLOR:
      case RGLGCM_CONSTANT_ALPHA:
      case RGLGCM_ONE_MINUS_CONSTANT_ALPHA:
         break;
      default:
         break;
   }
   switch ( sfAlpha )
   {
      case RGLGCM_ZERO:
      case RGLGCM_ONE:
      case RGLGCM_SRC_COLOR:
      case RGLGCM_ONE_MINUS_SRC_COLOR:
      case RGLGCM_SRC_ALPHA:
      case RGLGCM_ONE_MINUS_SRC_ALPHA:
      case RGLGCM_DST_ALPHA:
      case RGLGCM_ONE_MINUS_DST_ALPHA:
      case RGLGCM_DST_COLOR:
      case RGLGCM_ONE_MINUS_DST_COLOR:
      case RGLGCM_SRC_ALPHA_SATURATE:
      case RGLGCM_CONSTANT_COLOR:
      case RGLGCM_ONE_MINUS_CONSTANT_COLOR:
      case RGLGCM_CONSTANT_ALPHA:
      case RGLGCM_ONE_MINUS_CONSTANT_ALPHA:
         break;
      default:
         break;
   }

   switch ( df )
   {
      case RGLGCM_ZERO:
      case RGLGCM_ONE:
      case RGLGCM_SRC_COLOR:
      case RGLGCM_ONE_MINUS_SRC_COLOR:
      case RGLGCM_SRC_ALPHA:
      case RGLGCM_ONE_MINUS_SRC_ALPHA:
      case RGLGCM_DST_ALPHA:
      case RGLGCM_ONE_MINUS_DST_ALPHA:
      case RGLGCM_DST_COLOR:
      case RGLGCM_ONE_MINUS_DST_COLOR:
      case RGLGCM_SRC_ALPHA_SATURATE:
      case RGLGCM_CONSTANT_COLOR:
      case RGLGCM_ONE_MINUS_CONSTANT_COLOR:
      case RGLGCM_CONSTANT_ALPHA:
      case RGLGCM_ONE_MINUS_CONSTANT_ALPHA:
         break;
      default:
         break;
   }
   switch ( dfAlpha )
   {
      case RGLGCM_ZERO:
      case RGLGCM_ONE:
      case RGLGCM_SRC_COLOR:
      case RGLGCM_ONE_MINUS_SRC_COLOR:
      case RGLGCM_SRC_ALPHA:
      case RGLGCM_ONE_MINUS_SRC_ALPHA:
      case RGLGCM_DST_ALPHA:
      case RGLGCM_ONE_MINUS_DST_ALPHA:
      case RGLGCM_DST_COLOR:
      case RGLGCM_ONE_MINUS_DST_COLOR:
      case RGLGCM_SRC_ALPHA_SATURATE:
      case RGLGCM_CONSTANT_COLOR:
      case RGLGCM_ONE_MINUS_CONSTANT_COLOR:
      case RGLGCM_CONSTANT_ALPHA:
      case RGLGCM_ONE_MINUS_CONSTANT_ALPHA:
         break;
      default:
         break;
   }

   GCM_FUNC( cellGcmSetBlendFunc, sf, df, sfAlpha, dfAlpha );
}

// Prints out an int in hexedecimal and binary, broken into bytes.
// Can be used for printing out macro and constant values.
// example: rglPrintIt( RGLGCM_3DCONST(SET_SURFACE_FORMAT, COLOR, LE_A8R8G8B8) );
//          00 00 00 08 : 00000000 00000000 00000000 00001000 */
void static inline rglPrintIt( unsigned int v )
{
   // HEX (space between bytes)
   printf( "%02x %02x %02x %02x : ", ( v >> 24 )&0xff, ( v >> 16 )&0xff, ( v >> 8 )&0xff, v&0xff );

   // BINARY (space between bytes)
   for ( unsigned int mask = ( 0x1 << 31 ), i = 1; mask != 0; mask >>= 1, i++ )
      printf( "%d%s", ( v & mask ) ? 1 : 0, ( i % 8 == 0 ) ? " " : "" );
   printf( "\n" );
}

// prints the last numWords of the command fifo
void static inline rglPrintFifoFromPut( unsigned int numWords ) 
{
   for ( int i = -numWords; i <= -1; i++ )
      rglPrintIt((( uint32_t* )rglGcmState_i.fifo.current )[i] );
}

// prints the last numWords of the command fifo
void static inline rglPrintFifoFromGet( unsigned int numWords ) 
{
   for ( int i = -numWords; i <= -1; i++ )
      rglPrintIt((( uint32_t* )rglGcmState_i.fifo.lastGetRead )[i] );
}

// Determine whether a given location in a command buffer has been passed, by 
// using reference markers.
GLboolean static inline rglGcmFifoGlTestFenceRef( const GLuint ref )
{
   rglGcmFifo *fifo = &rglGcmState_i.fifo;
   return rglGcmFifoReferenceInUse( fifo, ref );
}

// Add a reference marker to the command buffer to determine whether a location 
// in the command buffer has been passed
void static inline rglGcmFifoGlIncFenceRef( GLuint *ref )
{
   rglGcmFifo *fifo = &rglGcmState_i.fifo;
   *ref = rglGcmFifoPutReference( fifo );
}

// Flush the current FIFO.
void static inline rglGcmFifoGlFlush( void )
{
   GCM_FUNC_NO_ARGS( cellGcmSetInvalidateVertexCache );
   rglGcmFifoFlush( &rglGcmState_i.fifo );
}

// Set blending constant, used for certain blending modes.
static inline void rglGcmFifoGlBlendColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   rglGcmBlendState *blend = &rglGcmState_i.state.blend;
   GLuint hwColor;

   // syntax check
   blend->r = r;
   blend->g = g;
   blend->b = b;
   blend->a = a;

   // program hw
   switch ( rglGcmState_i.renderTarget.colorFormat )
   {
      case RGLGCM_ARGB8:
         r = RGLGCM_CLAMPF_01( r );
         g = RGLGCM_CLAMPF_01( g );
         b = RGLGCM_CLAMPF_01( b );
         a = RGLGCM_CLAMPF_01( a );
         RGLGCM_CALC_COLOR_LE_ARGB8( &hwColor, r, g, b, a );
         GCM_FUNC( cellGcmSetBlendColor, hwColor, hwColor );
         break;
      case RGLGCM_NONE:
      case RGLGCM_FLOAT_R32:
         // no native support support
         break;
      default:
         break;
   }
}

// Set the current blend equation.
static inline void rglGcmFifoGlBlendEquation( rglGcmEnum mode, rglGcmEnum modeAlpha )
{
   // syntax check
   switch ( mode )
   {
      case RGLGCM_FUNC_ADD:
      case RGLGCM_MIN:
      case RGLGCM_MAX:
      case RGLGCM_FUNC_SUBTRACT:
      case RGLGCM_FUNC_REVERSE_SUBTRACT:
         break;
      default:
         break;
   }
   switch ( modeAlpha )
   {
      case RGLGCM_FUNC_ADD:
      case RGLGCM_MIN:
      case RGLGCM_MAX:
      case RGLGCM_FUNC_SUBTRACT:
      case RGLGCM_FUNC_REVERSE_SUBTRACT:
         break;
      default:
         break;
   }

   GCM_FUNC( cellGcmSetBlendEquation, mode, modeAlpha );
}

static inline void rglGcmFifoGlVertexAttribPointer
(
 GLuint          index,
 GLint           size,
 rglGcmEnum       type,
 GLboolean       normalized,
 GLsizei         stride,
 GLushort        frequency,
 GLboolean       isMain,
 GLuint          offset
 )
{
   // syntax check
   switch ( size )
   {
      case 0: // disable
         stride = 0;
         normalized = 0;
         type = RGLGCM_FLOAT;
         offset = 0;
         break;
      case 1:
      case 2:
      case 3:
      case 4:
         // valid
         break;
      default:
         break;
   }

   // mapping to native types
   uint8_t gcmType = 0;
   switch ( type )
   {
      case RGLGCM_UNSIGNED_BYTE:
         if (normalized)
            gcmType = CELL_GCM_VERTEX_UB;
         else
            gcmType = CELL_GCM_VERTEX_UB256;
         break;

      case RGLGCM_SHORT:
         gcmType = normalized ? CELL_GCM_VERTEX_S1 : CELL_GCM_VERTEX_S32K;
         break;

      case RGLGCM_FLOAT:
         gcmType = CELL_GCM_VERTEX_F;
         break;

      case RGLGCM_HALF_FLOAT:
         gcmType = CELL_GCM_VERTEX_SF;
         break;

      case RGLGCM_CMP:
         size = 1;   // required for this format
         gcmType = CELL_GCM_VERTEX_CMP;
         break;

      default:
         break;
   }

   GCM_FUNC( cellGcmSetVertexDataArray, index, frequency, stride, size, gcmType, CELL_GCM_LOCATION_LOCAL, offset );
}

// set the vertex attribute to the specified value.
void static inline rglGcmFifoGlVertexAttrib4fv( GLuint index, const GLfloat v[4] )
{
   GCM_FUNC( cellGcmSetVertexData4f, index, v );
}

static inline void rglGcmFifoGlClearColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   GCM_FUNC( cellGcmSetClearColor, 0 );
}

static inline void rglGcmFifoGlClear( GLbitfield mask )
{
   GLuint hwMask = 0;

   if ( mask & RGLGCM_COLOR_BUFFER_BIT && rglGcmState_i.renderTarget.colorFormat)
      hwMask = CELL_GCM_CLEAR_R | CELL_GCM_CLEAR_G | CELL_GCM_CLEAR_B | CELL_GCM_CLEAR_A;

   if ( hwMask )
   {
      GCM_FUNC( cellGcmSetClearSurface, hwMask );
   }
}

static inline void rglGcmFifoGlEnable( rglGcmEnum cap )
{
   switch ( cap )
   {
      case RGLGCM_BLEND:
         GCM_FUNC( cellGcmSetBlendEnable,  RGLGCM_TRUE );
         GCM_FUNC( cellGcmSetBlendEnableMrt, RGLGCM_TRUE, RGLGCM_TRUE, RGLGCM_TRUE );
         break;
      case RGLGCM_DITHER:
         GCM_FUNC( cellGcmSetDitherEnable, RGLGCM_TRUE );
         break;
      case RGLGCM_PSHADER_SRGB_REMAPPING:
         GCM_FUNC( cellGcmSetFragmentProgramGammaEnable, RGLGCM_TRUE );
         break;
      default:
         break;
   }
}

static inline void rglGcmFifoGlDisable( rglGcmEnum cap )
{

   switch ( cap )
   {
      case RGLGCM_BLEND:
         GCM_FUNC( cellGcmSetBlendEnable, RGLGCM_FALSE );
         GCM_FUNC( cellGcmSetBlendEnableMrt, RGLGCM_FALSE, RGLGCM_FALSE, RGLGCM_FALSE );
         break;
      case RGLGCM_DITHER:
         GCM_FUNC( cellGcmSetDitherEnable, RGLGCM_FALSE );
         break;
      case RGLGCM_PSHADER_SRGB_REMAPPING:
         GCM_FUNC( cellGcmSetFragmentProgramGammaEnable, RGLGCM_FALSE );
         break;
      default:
         break;
   }
}

static inline void rglFifoGlProgramParameterfvVP( const _CGprogram *program, const CgParameterEntry *parameterEntry, const GLfloat *value )
{
   const CgParameterResource *parameterResource = rglGetParameterResource( program, parameterEntry );
   if ( parameterResource->resource != ( unsigned short ) - 1 )
   {
      switch ( parameterResource->type )
      {
         case CG_FLOAT:
         case CG_FLOAT1:
         case CG_FLOAT2:
         case CG_FLOAT3:
         case CG_FLOAT4:
            GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 1, value ); // GCM_PORT_TESTED [Cedric]
            break;
         case CG_FLOAT4x4:
            // set 4 consts
            {
               GLfloat v2[16];
               v2[0] = value[0];v2[1] = value[4];v2[2] = value[8];v2[3] = value[12];
               v2[4] = value[1];v2[5] = value[5];v2[6] = value[9];v2[7] = value[13];
               v2[8] = value[2];v2[9] = value[6];v2[10] = value[10];v2[11] = value[14];
               v2[12] = value[3];v2[13] = value[7];v2[14] = value[11];v2[15] = value[15];
               GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 4, v2 ); // GCM_PORT_TESTED [Cedric]
            }
            break;
         case CG_FLOAT3x3:
            // set 3 consts
            {
               GLfloat v2[12];
               v2[0] = value[0];v2[1] = value[3];v2[2] = value[6];v2[3] = 0;
               v2[4] = value[1];v2[5] = value[4];v2[6] = value[7];v2[7] = 0;
               v2[8] = value[2];v2[9] = value[5];v2[10] = value[8];v2[11] = 0;
               GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 3, v2 );
            }
            break;
         case CG_HALF:
         case CG_HALF1:
         case CG_HALF2:
         case CG_HALF3:
         case CG_HALF4:
         case CG_INT:
         case CG_INT1:
         case CG_INT2:
         case CG_INT3:
         case CG_INT4:
         case CG_BOOL:
         case CG_BOOL1:
         case CG_BOOL2:
         case CG_BOOL3:
         case CG_BOOL4:
         case CG_FIXED:
         case CG_FIXED1:
         case CG_FIXED2:
         case CG_FIXED3:
         case CG_FIXED4:
            GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 1, value ); // GCM_PORT_TESTED [Cedric]
            break;
         case CG_HALF4x4:
         case CG_INT4x4:
         case CG_BOOL4x4:
         case CG_FIXED4x4:
            // set 4 consts
            {
               GLfloat v2[16];
               v2[0] = value[0];
               v2[1] = value[4];
               v2[2] = value[8];
               v2[3] = value[12];
               v2[4] = value[1];
               v2[5] = value[5];
               v2[6] = value[9];
               v2[7] = value[13];
               v2[8] = value[2];
               v2[9] = value[6];
               v2[10] = value[10];
               v2[11] = value[14];
               v2[12] = value[3];
               v2[13] = value[7];
               v2[14] = value[11];
               v2[15] = value[15];
               GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 4, v2 ); // GCM_PORT_TESTED [Cedric]
            }
            break;
         case CG_HALF3x3:
         case CG_INT3x3:
         case CG_BOOL3x3:
         case CG_FIXED3x3:
            // set 3 consts
            {
               GLfloat v2[12];
               v2[0] = value[0];v2[1] = value[3];v2[2] = value[6];v2[3] = 0;
               v2[4] = value[1];v2[5] = value[4];v2[6] = value[7];v2[7] = 0;
               v2[8] = value[2];v2[9] = value[5];v2[10] = value[8];v2[11] = 0;
               GCM_FUNC( cellGcmSetVertexProgramParameterBlock, parameterResource->resource, 3, v2 );
            }
            break;
         default:
            break;
      }
   }
}

// Look up the memory location of a buffer object (VBO, PBO)
static inline GLuint rglGcmGetBufferObjectOrigin( GLuint buffer )
{
   rglBufferObject *bufferObject = (rglBufferObject*)_CurrentContext->bufferObjectNameSpace.data[buffer];
   rglGcmBufferObject *gcmBuffer = ( rglGcmBufferObject * ) & bufferObject->platformBufferObject;
   return gcmBuffer->bufferId;
}

static inline rglGcmDriver *rglGetGcmDriver(void)
{
   return ( rglGcmDriver * )( _CurrentDevice->rasterDriver );
}

