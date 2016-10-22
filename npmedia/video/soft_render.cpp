#include "internal.h"

// 使用gdi+渲染.


#include "soft_render.h"


soft_render::soft_render()
   : m_hwnd(NULL)
   , m_hdc(NULL)
   , m_image_width(0)
   , m_image_height(0)
   , m_keep_aspect(false)
   , m_window_aspect(0.0f)
   , m_framebuffer(NULL)
   , m_swsctx(NULL)
{

}

soft_render::~soft_render()
{

}

bool soft_render::init_render(void* ctx, int w, int h, int pix_fmt)
{

}

bool soft_render::render_one_frame(AVFrame* data, int pix_fmt)
{

}

void soft_render::re_size(int width, int height)
{

}

void soft_render::aspect_ratio(int srcw, int srch, bool enable_aspect)
{

}

void soft_render::destory_render()
{

}

