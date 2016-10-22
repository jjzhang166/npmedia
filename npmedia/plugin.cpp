#include <windows.h>
#include <windowsx.h>

#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include "plugin.h"


#ifdef  __cplusplus
extern "C" {
#endif

int gdi_init_video(struct vo_context *ctx, int w, int h, int pix_fmt)
{
	ctx->priv = ctx->user_data;
	MediaPlugin *p = (MediaPlugin *)ctx->priv;
	if (p){

		p->m_image_width = w;
		p->m_image_height = h;

		p->m_window_aspect = (float)w / (float)h;

		p->m_swsctx = sws_getContext(p->m_image_width, p->m_image_height, PIX_FMT_YUV420P, p->m_image_width, p->m_image_height,
			PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

		p->m_framebuffer = (uint8_t*)malloc(w * h * sizeof(uint32_t));

		return 0;
	}
	return -1;
}

int gdi_render_one_frame(struct vo_context *ctx, AVFrame* data, int pix_fmt, double pts)
{
	MediaPlugin *p = (MediaPlugin *)ctx->priv;

	p->m_data = data;


	NPRect r;
	r.left = 0;
	r.top = 0;
	r.bottom = p->mWindow->height;
	r.right = p->mWindow->width;
	//NPN_InvalidateRect(p->mInstance,&r);
	NPN_ForceRedraw(p->mInstance);//强制更新
	return 0;
}

void gdi_re_size(struct vo_context *ctx, int width, int height)
{

}

void gdi_aspect_ratio(struct vo_context *ctx, int srcw, int srch, int enable_aspect)
{
	MediaPlugin *p = (MediaPlugin *)ctx->priv;
	if (enable_aspect)
	{
		p->m_keep_aspect = true;
		p->m_window_aspect = (float)srcw / (float)srch;
	}
	else
	{
		p->m_keep_aspect = false;
	}
}

int gdi_use_overlay(struct vo_context *ctx)
{
	return 0;
}

void gdi_destory_render(struct vo_context *ctx)
{
	MediaPlugin *p = (MediaPlugin *)ctx->priv;
	if (p)
	{
		if (p->m_framebuffer)
		{
			free(p->m_framebuffer);
			p->m_framebuffer = NULL;
		}
		if (p->m_swsctx)
		{
			sws_freeContext(p->m_swsctx);
			p->m_swsctx = NULL;
		}
		ctx->priv = NULL;
	}
}

#ifdef  __cplusplus
}
#endif



/* 文件下载信息. */
typedef struct src_file
{
	HANDLE fd;
} src_file;

int file_init_source(struct source_context *ctx)
{
	src_file *p = (src_file*)malloc(sizeof(src_file));
	memset(p, 0, sizeof(src_file));
	ctx->priv = p;
	//p->fd = CreateFileW(L"1.flac", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);//打开文件
	p->fd = CreateFileW(L"2.mp4", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);//打开文件
	return 0;
}
int64_t file_read_data(struct source_context *ctx, char* buff, size_t buf_size)
{
	src_file *p = (src_file*)ctx->priv;
	DWORD size;
	ReadFile(p->fd, buff, buf_size, &size, 0);

	return size;
}
int64_t file_read_seek(struct source_context *ctx, int64_t offset, int whence)
{
	src_file *p = (src_file*)ctx->priv;

	int64_t a = -1;
	LARGE_INTEGER li = { 0 }; //记得初始化
	li.QuadPart = offset/*移动的位置*/;
	switch (whence)
	{
	case SEEK_SET:	// 文件起始位置计算.
	{
						a = SetFilePointer(p->fd, li.LowPart, &li.HighPart, FILE_BEGIN);
	}
		break;
	case SEEK_CUR:	// 文件指针当前位置开始计算.
	{
						a = SetFilePointer(p->fd, li.LowPart, &li.HighPart, FILE_CURRENT);
	}
		break;
	case SEEK_END:	// 文件尾开始计算.
	{
						a = SetFilePointer(p->fd, li.LowPart, &li.HighPart, FILE_END);

	}
		break;
	default:
		break;
	}
	return a;
}
void file_close(struct source_context *ctx)
{

}
void file_destory(struct source_context *ctx)
{

}
void init_file_source(source_context *sc)
{
	sc->init_source = file_init_source;
	sc->read_data = file_read_data;
	sc->read_seek = file_read_seek;
	sc->close = file_close;
	sc->destory = file_destory;
}

void init_audio(ao_context *ao)
{

	ao->init_audio = wave_init_audio;
	ao->play_audio = wave_play_audio;
	ao->audio_control = wave_audio_control;
	ao->mute_set = wave_mute_set;
	ao->destory_audio = wave_destory_audio;
}
void init_video(vo_context *vo)
{
	gdi_init_video(vo, 10, 10, PIX_FMT_YUV420P);
	gdi_destory_render(vo);
	vo->init_video = gdi_init_video;
	vo->re_size = gdi_re_size;
	vo->aspect_ratio = gdi_aspect_ratio;
	vo->use_overlay = gdi_use_overlay;
	vo->destory_video = gdi_destory_render;
	vo->render_one_frame = gdi_render_one_frame;
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;

  MediaPlugin * plugin = new MediaPlugin(aCreateDataStruct->instance);

  // now is the time to tell Mozilla that we are windowless
  NPN_SetValue(aCreateDataStruct->instance, NPPVpluginWindowBool, NULL);

  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (MediaPlugin *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
MediaPlugin::MediaPlugin(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE),
  m_framebuffer(0),
  m_swsctx(0),
  m_image_width(0),
  m_image_height(0),
  m_keep_aspect(0),
  m_window_aspect(0),
  m_data(0)
{
	InitializeCriticalSection(&lpCriticalSection);
	m_audio = alloc_audio_render();
	m_video = alloc_video_render(this);
}

MediaPlugin::~MediaPlugin()
{
}

NPBool MediaPlugin::init(NPWindow* aWindow)
{
  mInitialized = TRUE;

  // 初始化音频和视频渲染器.
  init_audio(m_audio);
  init_video(m_video);

  // 分配播放器上下文.
  m_avplay = alloc_avplay_context();
  //分配媒体源
  m_source = alloc_media_source(MEDIA_TYPE_FILE, "E:\MVI_3981.MOV", 6 + 1, 1000000);
  // 初始化文件媒体源.
  init_file_source(m_source);
  // 初始化avplay.
  initialize(m_avplay, m_source);
  // 配置音频视频渲染器.
  configure(m_avplay, m_video, VIDEO_RENDER);
  configure(m_avplay, m_audio, AUDIO_RENDER);

  av_start(m_avplay, 0);

  return TRUE;
}

void MediaPlugin::shut()
{
  // no GUI to shut in windowless case
  mInitialized = FALSE;
}

NPBool MediaPlugin::isInitialized()
{
  return mInitialized;
}

NPError MediaPlugin::SetWindow(NPWindow* aWindow)
{
  // keep window parameters
  mWindow = aWindow;
  return NPERR_NO_ERROR;
}

uint16_t MediaPlugin::HandleEvent(void* aEvent)
{
  NPEvent * event = (NPEvent *)aEvent;
  switch (event->event) {
    case WM_PAINT: 
    {
      if(!mWindow)
        break;
	  if (m_data){
		  uint8_t* pixels[3] = { m_data->data[0],
			  m_data->data[1],
			  m_data->data[2] };

		  int linesize[3] = { m_data->linesize[0],
			  m_data->linesize[1],
			  m_data->linesize[2] };
		  //解析帧到bmp
		  AVFrame* pic = avcodec_alloc_frame();
		  avpicture_fill((AVPicture*)pic, m_framebuffer, PIX_FMT_BGR24, m_image_width, m_image_height);
		  sws_scale(m_swsctx, pixels, linesize, 0, m_image_height, pic->data, pic->linesize);
		  Bitmap bmp(m_image_width, m_image_height, pic->linesize[0], PixelFormat24bppRGB, pic->data[0]);

		  RECT rect_client = { 0 };
		  rect_client.left = 0;
		  rect_client.top = 0;
		  rect_client.bottom = mWindow->height;
		  rect_client.right = mWindow->width;
		  int width = rect_client.right - rect_client.left;
		  int height = rect_client.bottom - rect_client.top;

		  if (m_keep_aspect)
		  {
			  int win_width = width = rect_client.right - rect_client.left;
			  int win_height = height = rect_client.bottom - rect_client.top;

			  int tmpheight = ((float)width / m_window_aspect);
			  tmpheight += tmpheight % 2;
			  if (tmpheight > height)
			  {
				  width = ((float)height * m_window_aspect);
				  width += width % 2;
			  }
			  else
			  {
				  height = tmpheight;
			  }

			  // 居中对齐.
			  rect_client.left += ((win_width - width) / 2);
			  rect_client.top += ((win_height - height) / 2);
			  rect_client.bottom -= ((win_height - height) / 2);
			  rect_client.right -= ((win_width - width) / 2);
		  }
		  Graphics Gs((HDC)event->wParam);
		  Gs.DrawImage(&bmp, 0, 0, mWindow->width, mWindow->height);

		  av_free(pic);

		  m_data = 0;
	  }
      // get the dirty rectangle to update or repaint the whole window
      RECT * drc = (RECT *)event->lParam;
	  if (drc){
		//  BitBlt((HDC)event->wParam, drc->left, drc->top, drc->right - drc->left, drc->bottom - drc->top, m_hdc, drc->left, drc->top, SRCCOPY);
	  }
	  //else {
		//BitBlt((HDC)event->wParam, 0, 0, 50, 50, m_hdc, 0, 0, SRCCOPY);
      //}
	  //LeaveCriticalSection(&lpCriticalSection);
      break;
    }
    case WM_KEYDOWN:
    {
      Beep(1000,100);
      break;
    }
    default:
      return 0;
  }
  return 1;
}