#ifndef XLIB_SOUND
#define XLIB_SOUND

typedef ma_sound * XSound;

typedef struct {
    ma_engine engine;
} XAudio;

global XAudio xaudio;

static void
xaudio_shutdown(void) {
    ma_engine_uninit(&xaudio.engine);
}

static void
xaudio_free_sound(XSound sound) {
    ma_sound_uninit(sound);
    xfree(sound);
}

static XSound
xaudio_load_mp3(char *path) {
    ma_result error;
    
    XSound result = xalloc(sizeof(ma_sound));
    
    error = ma_sound_init_from_file(&xaudio.engine, path,
                                    MA_SOUND_FLAG_DECODE, 
                                    NULL, NULL, result);
    if (error != MA_SUCCESS) {
        xfree(result);
        result = 0;
    }
    
    return result;
}

static bool
xaudio_initialize(void) {
    ma_result result;
    
    result = ma_engine_init(NULL, &xaudio.engine);
    if (result != MA_SUCCESS) {
        return false;  // Failed to initialize the engine.
    }
    
    return true;
}

static void
xaudio_play(XSound sound) {
    ma_sound_start(sound);
}

#endif