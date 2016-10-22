#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "pluginbase.h"

#include "libav\avplay.h"
#include "audio\audio_out.h"
#include "video\video_out.h"
#include "libav\globals.h"

class MediaPlugin : public nsPluginInstanceBase
{
public:
	MediaPlugin(NPP aInstance);
  ~MediaPlugin();

  NPBool init(NPWindow* aWindow);
  void shut();
  NPBool isInitialized();

  NPError SetWindow(NPWindow* aWindow);
  uint16_t HandleEvent(void* aEvent);


  NPP mInstance;
  NPBool mInitialized;
  NPWindow * mWindow;

  avplay *m_avplay;//
  ao_context *m_audio;//音频渲染器
  vo_context *m_video;//视频渲染器
  source_context *m_source;//媒体源

  CRITICAL_SECTION lpCriticalSection;
  AVFrame* m_data;

  uint8_t* m_framebuffer;
  // 用于转换帧数据格式.
  SwsContext* m_swsctx;
  // 视频宽.
  int m_image_width;
  // 视频高.
  int m_image_height;
  // 是否启用宽高比.
  bool m_keep_aspect;
  // 宽高比.
  float m_window_aspect;

};

#endif // __PLUGIN_H__
