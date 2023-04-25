static void plat_init()
{
    XD11Config xconfig = {0};
    xconfig.wndproc = window_proc;
    xstrcpy(xconfig.wndTitle, "List Networked");;
    v2_copy((v2){1300-800,100}, xconfig.wndP);
    v2_copy((v2){800,600}, xconfig.wndDim);
    
    /* Initialization must happen in this order */
    xd11_initialize(xconfig);
    xwin_initialize((XWindowConfig){
                        LoadCursor(NULL, IDC_ARROW), 
                        xd11.wndHandle
                    });
    xrender2d_initialize((v4){.02f,.02f,.02f,1});
    xaudio_initialize();
    xsocket_initialize();
    
    /* Load images */
    {
        char imageFullPath[260];
        xwin_path_abs(imageFullPath, 260, "images\\symbol1.png");
        plat.images[plat.imageIndex++] = xrender2d_sprite_from_png(imageFullPath);
    }
    
    /* Load fonts */
    {
        char fontFullPath[260];
        xwin_path_abs(fontFullPath, 260, "fonts\\Inconsolata.ttf");
        plat.fonts[plat.fontIndex++] = xrender2d_font(fontFullPath, "Inconsolata", 84);
    }
    {
        char fontFullPath[260];
        xwin_path_abs(fontFullPath, 260, "fonts\\Inconsolata.ttf");
        plat.fonts[plat.fontIndex++] = xrender2d_font(fontFullPath, "Inconsolata", 64);
    }
    {
        char fontFullPath[260];
        xwin_path_abs(fontFullPath, 260, "fonts\\Inconsolata.ttf");
        plat.fonts[plat.fontIndex++] = xrender2d_font(fontFullPath, "Inconsolata", 20);
    }
    
    
    /* Generate mip maps */
    xd11_generate_mips(&xrender2d.textures.tex);
    
    /* Load sounds */
    plat.sounds[plat.soundIndex++] = xaudio_load_mp3("sounds\\move.mp3");
    
    /* Account for layer */
    plat.layerIndex = 4;
}
