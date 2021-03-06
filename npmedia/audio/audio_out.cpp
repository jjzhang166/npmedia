#include "internal.h"
#include "../libav/globals.h"
#include "wave_render.h"
#include "audio_out.h"


#ifdef  __cplusplus
extern "C" {
#endif

int wave_init_audio(struct ao_context *ctx, uint32_t channels,
	uint32_t bits_per_sample, uint32_t sample_rate, int format)
{
	wave_render* wave = NULL;
	ctx->priv = (void*)(wave = new wave_render);
	return wave->init_audio((void*)ctx->priv, channels, bits_per_sample, sample_rate, format) ? 0 : -1;
}

int wave_play_audio(struct ao_context *ctx, uint8_t* data, uint32_t size)
{
	wave_render* wave = (wave_render*)ctx->priv;
	return wave->play_audio(data, size);
}

void wave_audio_control(struct ao_context *ctx, double l, double r)
{
	wave_render* wave = (wave_render*)ctx->priv;
	control_vol_t ctrl_vol = { l, r };
	wave->audio_control(CONTROL_SET_VOLUME, &ctrl_vol);
}

void wave_mute_set(struct ao_context *ctx, int s)
{
	wave_render* wave = (wave_render*)ctx->priv;
	control_vol_t ctrl_vol;
	ctrl_vol.mute = s;
	wave->audio_control(CONTROL_MUTE_SET, &ctrl_vol);
}

void wave_destory_audio(struct ao_context *ctx)
{
	wave_render* wave = (wave_render*)ctx->priv;
	if (wave)
	{
		wave->destory_audio();
		delete wave;
		ctx->priv = NULL;
	}
}


#ifdef  __cplusplus
}
#endif

