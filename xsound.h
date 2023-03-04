#ifndef XLIB_SOUND
#define XLIB_SOUND

typedef struct XSound* XSound;

#define XDSOUNDCREATE(name) HRESULT WINAPI name \
(LPCGUID a, LPDIRECTSOUND *b, LPUNKNOWN c)

typedef XDSOUNDCREATE(xdsound_create);

struct XSound
{
	BYTE *bytes;
    DWORD size;
};

typedef struct
{
    s32 sampCount;
    s16 *samples;
} XSoundOutputBuffer;

typedef struct
{
    s32 sampPerSec, bytesPerSamp;
    u32 runSampIndex, bufSize, safetyCount, pc, wc, pcs[60], wcs[60], pci, wci;
    f32 tsine;
    s16 *bytes;
    
    bool soundIsValid;
    
    WAVEFORMATEX format;
    LPDIRECTSOUND dsound;
    LPDIRECTSOUNDBUFFER buffer;
	
    LARGE_INTEGER flipWallClock;
    
    xdsound_create *dsoundcreate;
} XSoundEngine;

global XSoundEngine xsound;

bool   xsoundinit    (HWND window);      // Inits xaudio2
void   xsoundfini    (void);             // Free loaded wav files, free xaudio2

XSound xloadwav (wchar_t *path);        // Loads a wave, creates source voice
void   xplay    (Sound a, float volume); // Plays a sound





void xsound_output(XSoundOutputBuffer outputBuf)
{
    s32 period = xsound.sampPerSec / 440;
    
    s16 *out = outputBuf.samples;
    for (s32 i=0; i<outputBuf.sampCount; ++i)
    {
        // f32 sineValue = sinf(xsound.tsine);
        // s16 value = (s16)(sineValue * 3000);
        s16 value = 0;
        *out++ = value;
        *out++ = value;
        
        assert(value < 3000);

        xsound.tsine += 2.0f * PIf * 1.0f / (f32)period;
        if (xsound.tsine > 2.0f * PIf)
            xsound.tsine -= 2.0f * PIf;
    }
}

void clear_sound_buffer()
{
    VOID *region1, *region2;
    DWORD region1Size, region2Size, i;
    u8 *dstSample;
    
    if(SUCCEEDED(IDirectSoundBuffer_Lock(xsound.buffer, 0, 
                                         xsound.bufSize,
                                         &region1, &region1Size,
                                         &region2, &region2Size,
                                         0)))
    {
        dstSample = (u8 *)region1;
        for(i=0; i<region1Size; ++i)
            *dstSample++ = 0;
        
        dstSample = (u8 *)region2;
        for(i=0; i<region2Size; ++i)
            *dstSample++ = 0;
        
        IDirectSoundBuffer_Unlock(xsound.buffer, region1, region1Size, 
                                  region2, region2Size);
    }
}

bool xsoundinit(HWND window)
{
    s32 samplesPerSecond = 48000;
    s32 bufferSize = samplesPerSecond*sizeof(s16)*2;
    
    HMODULE lib = LoadLibraryA("dsound.dll");
    if(lib)
    {
        xsound.dsoundcreate = (xdsound_create *)
            GetProcAddress(lib, "DirectSoundCreate");
        
        if(xsound.dsoundcreate && 
           SUCCEEDED(xsound.dsoundcreate(0, &xsound.dsound, 0)))
        {
            WAVEFORMATEX wf;
            memset(&wf, 0, sizeof(wf));
            wf.wFormatTag      = WAVE_FORMAT_PCM;
            wf.nChannels       = 2;
            wf.nSamplesPerSec  = samplesPerSecond;
            wf.wBitsPerSample  = 16;
            wf.nBlockAlign     = (wf.nChannels*wf.wBitsPerSample) / 8;
            wf.nAvgBytesPerSec = wf.nSamplesPerSec*wf.nBlockAlign;
            wf.cbSize          = 0;
            xsound.format = wf;
            
            if(SUCCEEDED(IDirectSound_SetCooperativeLevel(xsound.dsound, window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC bd;
                memset(&bd, 0, sizeof(bd));
                bd.dwSize = sizeof(bd);
                bd.dwFlags = DSBCAPS_PRIMARYBUFFER;
                
                LPDIRECTSOUNDBUFFER b;
                if(SUCCEEDED(IDirectSound_CreateSoundBuffer(xsound.dsound, &bd, &b, 0)))
                {
                    HRESULT e = IDirectSoundBuffer_SetFormat(b, &wf);
                    if(FAILED(e))
                        return false ;
                }
                else
                    return false;
            }
            else
                return false;
            
            DSBUFFERDESC bd;
            memset(&bd, 0, sizeof(bd));
            bd.dwSize        = sizeof(bd);
            bd.dwFlags       = DSBCAPS_GETCURRENTPOSITION2;
            bd.dwBufferBytes = bufferSize;
            bd.lpwfxFormat   = &wf;
            HRESULT e = IDirectSound_CreateSoundBuffer(xsound.dsound, &bd, &xsound.buffer, 0);
            if(FAILED(e))
                return false;
        }
        else
            return false;
    }
    else
        return false;
    
    
    //
    xsound.flipWallClock = xwallclock();
    
    xsound.sampPerSec = samplesPerSecond;
    xsound.bytesPerSamp = sizeof(s16)*2;
    xsound.bufSize = bufferSize;
    xsound.safetyCount = (xsound.sampPerSec * xsound.bytesPerSamp / 60) / 3;
    xsound.bytes = (s16 *)xalloc(xsound.bufSize);
    
    clear_sound_buffer();
    
    XSoundOutputBuffer soundOutputBuffer = {
        xsound.sampPerSec,
        xsound.bytes,
    };
    
//    xsound_output(soundOutputBuffer);
    
    IDirectSoundBuffer_Play(xsound.buffer, 0, 0, DSBPLAY_LOOPING);
    
    return true;
}

void xsoundfini(void)
{
}

XSound xloadwav(wchar_t* path)
{
	XSound r;
    FILE *file;
	
    r = 0;
    memset(&r, 0, sizeof(r));
    char *pathascii = xstrtoascii(path);
	file = 0;
    fopen_s(&file, pathascii, "rb");
    if (!file) {
        printf("Failed to open file %s\n", pathascii);
        return r;
    }
    xfree(pathascii);
    
    r = xalloc(sizeof(*r));
    
    // Read the WAV file header
    char chunkId[4], format[4], subchunk1Id[4];
    DWORD chunkSize, subchunk1Size, sampleRate, byteRate;
    WORD audioFormat, numChannels, bitsPerSample, blockAlign;
    fread(chunkId, 1, 4, file);
    fread(&chunkSize, 1, 4, file);
    fread(format, 1, 4, file);
    fread(subchunk1Id, 1, 4, file);
    fread(&subchunk1Size, 1, 4, file);
    fread(&audioFormat, 1, 2, file);
    fread(&numChannels, 1, 2, file);
    fread(&sampleRate, 1, 4, file);
    fread(&byteRate, 1, 4, file);
    fread(&blockAlign, 1, 2, file);
    fread(&bitsPerSample, 1, 2, file);
    
    // Check that the file format is valid
    if (strncmp(chunkId, "RIFF", 4) != 0 ||
        strncmp(format, "WAVE", 4) != 0 ||
        strncmp(subchunk1Id, "fmt ", 4) != 0 ||
        audioFormat != WAVE_FORMAT_PCM)
    {
        printf("Invalid WAV file format\n");
        fclose(file);
        return r;
    }
    
    // Read the sound data into memory
    r->bytes = xalloc(chunkSize - subchunk1Size - 36);
    fread(r->bytes, 1, chunkSize - subchunk1Size - 36, file);
    fclose(file);
    
    // Set the output variables
    r->size = chunkSize - subchunk1Size - 36;
    
    return r;
}

void xplay(Sound a, float volume)
{
}

void fill_sound_buffer(DWORD byteToLock, DWORD bytesToWrite,
                       XSoundOutputBuffer *sourceBuffer)
{
    VOID *region1, *region2;
    DWORD region1size, region2size;
    if(SUCCEEDED(IDirectSoundBuffer_Lock(xsound.buffer, byteToLock, bytesToWrite,
                                         &region1, &region1size,
                                         &region2, &region2size,
                                         0)))
    {
        DWORD region1SampleCount = region1size / xsound.bytesPerSamp;
        s16 *dstSample = (s16 *)region1;
        s16 *srcSample = sourceBuffer->samples;
        for(DWORD i=0; i<region1SampleCount; ++i)
        {
            *dstSample++ = *srcSample++;
            *dstSample++ = *srcSample++;
            ++xsound.runSampIndex;
        }
        
        DWORD region2SampleCount = region2size / xsound.bytesPerSamp;
        dstSample = (s16 *)region2;
        for(DWORD i=0; i<region2SampleCount; ++i)
        {
            *dstSample++ = *srcSample++;
            *dstSample++ = *srcSample++;
            ++xsound.runSampIndex;
        }
        
        IDirectSoundBuffer_Unlock(xsound.buffer, region1, region1size, 
                                  region2, region2size);
    }
}

void xsound_post_update()
{
    DWORD pc, wc;
    
    LARGE_INTEGER audioWallClock = xwallclock();
    f32 toAudioSeconds = xseconds(xsound.flipWallClock, audioWallClock);
    
    if(SUCCEEDED(IDirectSoundBuffer_GetCurrentPosition(xsound.buffer, &pc, &wc)))
    {
        xsound.pcs[xsound.pci++] = pc;
        xsound.wcs[xsound.wci++] = wc;
        if (xsound.pci >= narray(xsound.pcs))
            xsound.pci = 0;
        if (xsound.wci >= narray(xsound.wcs))
            xsound.wci = 0;

        if(!xsound.soundIsValid)
        {
            xsound.runSampIndex = wc / xsound.bytesPerSamp;
            xsound.soundIsValid = true;
        }
        
        DWORD byteToLock = ((xsound.runSampIndex*xsound.bytesPerSamp) %
                            xsound.bufSize);
        
        DWORD expectBytesPerFrame = (xsound.sampPerSec * xsound.bytesPerSamp) / 60;
        
        f32 targetSecondsPerFrame = 1.0f / 60;
        
        f32 secondsUntilFlip = (targetSecondsPerFrame - toAudioSeconds);
        if (secondsUntilFlip > 0 && secondsUntilFlip < 1)
        {
            DWORD expectBytesUntilFlip = (DWORD)((secondsUntilFlip / targetSecondsPerFrame) * 
                                                 (f32)expectBytesPerFrame);
            
            DWORD expectFrameBoundaryByte = pc + expectBytesPerFrame;
            
            DWORD safeWriteCursor = wc;
            if(safeWriteCursor < pc)
                safeWriteCursor += xsound.bufSize;
            
            assert(safeWriteCursor >= pc);
            safeWriteCursor += xsound.safetyCount;
            
            bool audioCardIsLowLatency = (safeWriteCursor < expectFrameBoundaryByte);                        
            
            DWORD targetCursor = 0;
            if(audioCardIsLowLatency)
                targetCursor = (expectFrameBoundaryByte + expectBytesPerFrame);
            else
                targetCursor = (wc + expectBytesPerFrame + xsound.safetyCount);
            
            targetCursor = (targetCursor % xsound.bufSize);
            
            DWORD bytesToWrite = 0;
            if(byteToLock > targetCursor)
                bytesToWrite = (xsound.bufSize - byteToLock) + targetCursor;
            else
                bytesToWrite = targetCursor - byteToLock;
            
            
            XSoundOutputBuffer soundOutputBuffer = {
                bytesToWrite / xsound.bytesPerSamp,
                xsound.bytes,
            };
            
            xsound_output(soundOutputBuffer);
            xsound.pc = pc;
            xsound.wc = wc;

            fill_sound_buffer(byteToLock, bytesToWrite, &soundOutputBuffer);
        }
    }
    else
    {
        xsound.soundIsValid = false;
    }
}


#endif