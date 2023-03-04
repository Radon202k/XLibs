void engine_init(void)
{
    srand((u32)time(0));
    
    XRendConfig rendconfig = { window_proc, .topDown = true };
    xrendinit(rendconfig);
    
    XWinConfig winconfig = { LoadCursor(NULL, IDC_ARROW), xrend.wh };
    xwininit(winconfig);
    
    engine.font  = xfont(L"fonts/Inconsolata.ttf", L"Inconsolata", 32);
    engine.white = xspritepng(L"images/white.png", false);
    engine.arrow = xspritepng(L"images/arrow128.png", false);
    engine.circle = xspritepng(L"images/circle128.png", false);
    
    engine.layer1 = xbatch(256);
    engine.meshlayer1 = xmeshbatch(256);
    
    engine.layer2 = xbatch(256);
    
    u8 *bytes = xatlasbytes();
    xatlasupdate(bytes);
}

void engine_shutdown(void)
{
    xfree(xsound.bytes);
    xfontfree(engine.font);  // Free XFont
    xrendfini();             // Free XRender resources
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
        XWNDPROC;
        
        case WM_NCHITTEST: {
            result = DefWindowProc(window, message, wParam, lParam);
            POINT p = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ScreenToClient(window, &p);
            if (result == HTCLIENT &&
                p.x < xrend.bbs.x - 50 &&
                p.y < 40) {
                return HTCAPTION;
            }
            else {
                return result;
            }
        };
        
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
}

void xrendresized(void)
{
}
