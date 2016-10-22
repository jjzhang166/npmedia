#ifndef __VIDEO_OUT_H__
#define __VIDEO_OUT_H__

#ifdef _MSC_VER
#ifdef VIDEO_EXPORTS
#define __declspec(dllexport)
#else
#define __declspec(dllimport)
#endif
#endif

#ifdef  __cplusplus
extern "C" {
#endif


int gdi_init_video(struct vo_context *ctx, int w, int h, int pix_fmt);
int gdi_render_one_frame(struct vo_context *ctx, AVFrame* data, int pix_fmt, double pts);
void gdi_re_size(struct vo_context *ctx, int width, int height);
void gdi_aspect_ratio(struct vo_context *ctx, int srcw, int srch, int enable_aspect);
int gdi_use_overlay(struct vo_context *ctx);
void gdi_destory_render(struct vo_context *ctx);


#ifdef  __cplusplus
}
#endif

#endif // __VIDEO_OUT_H__

