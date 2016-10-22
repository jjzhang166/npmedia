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
  ao_context *m_audio;//��Ƶ��Ⱦ��
  vo_context *m_video;//��Ƶ��Ⱦ��
  source_context *m_source;//ý��Դ

  CRITICAL_SECTION lpCriticalSection;
  AVFrame* m_data;

  uint8_t* m_framebuffer;
  // ����ת��֡���ݸ�ʽ.
  SwsContext* m_swsctx;
  // ��Ƶ��.
  int m_image_width;
  // ��Ƶ��.
  int m_image_height;
  // �Ƿ����ÿ�߱�.
  bool m_keep_aspect;
  // ��߱�.
  float m_window_aspect;

};

#endif // __PLUGIN_H__
