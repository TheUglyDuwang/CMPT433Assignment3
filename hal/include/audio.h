#include <alsa/asoundlib.h>

#ifndef AUDIO_H
#define AUDIO_H

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
snd_pcm_t *Audio_openDevice();

#endif