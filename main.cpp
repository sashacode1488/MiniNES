#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <SDL.h>
#include "Bus.h"


#define IDM_FILE_OPEN       101
#define IDM_FILE_CLOSE      102
#define IDM_FILE_RECENT_1   103
#define IDM_FILE_RECENT_2   104
#define IDM_FILE_RECENT_3   105
#define IDM_FILE_RECENT_4   106
#define IDM_FILE_RECENT_5   107
#define IDM_FILE_EXIT       108

#define IDM_NES_RESET       201
#define IDM_NES_HARD_RESET  202
#define IDM_NES_PAUSE       203

#define IDM_OPTIONS_SOUND   301
#define IDM_OPTIONS_CHEAT   302
#define IDM_OPTIONS_SCALE_1 303
#define IDM_OPTIONS_SCALE_2 304
#define IDM_OPTIONS_SCALE_3 305
#define IDM_OPTIONS_SCALE_4 306
#define IDM_OPTIONS_SCALE_5 307

#define IDM_HELP_CONTROLS   401
#define IDM_HELP_ABOUT      402


const int SAMPLE_RATE = 44100;
const int BUFFER_SIZE = 16384;
static float audio_buffer[BUFFER_SIZE] = { 0 };
static volatile int audio_read_pos = 0;
static volatile int audio_write_pos = 0;
static bool no_sound = false;

struct HighPassFilter {
    float prev_input = 0.0f;
    float prev_output = 0.0f;
    float alpha = 0.996f;

    float process(float input) {
        float output = input - prev_input + alpha * prev_output;
        prev_input = input;
        prev_output = output;
        return output;
    }
};

static HighPassFilter hp_filter;

void audio_callback(void* userdata, Uint8* stream, int len) {
    if (no_sound) {
        memset(stream, 0, len);
        return;
    }

    float* output = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);

    for (int i = 0; i < samples; i++) {
        if (audio_read_pos == audio_write_pos) {
            output[i] = 0.0f;
        }
        else {
            float sample = audio_buffer[audio_read_pos];
            output[i] = hp_filter.process(sample);
            audio_read_pos = (audio_read_pos + 1) % BUFFER_SIZE;
        }
    }
}


class RecentFiles {
private:
    std::vector<std::string> files;
    const size_t maxFiles = 5;
    std::string configFile = "recent.txt";

public:
    RecentFiles() {
        load();
    }

    void add(const std::string& file) {
        files.erase(std::remove(files.begin(), files.end(), file), files.end());
        files.insert(files.begin(), file);
        if (files.size() > maxFiles) {
            files.resize(maxFiles);
        }
        save();
    }

    const std::vector<std::string>& getFiles() const {
        return files;
    }

    void load() {
        std::ifstream f(configFile);
        if (f.is_open()) {
            std::string line;
            while (std::getline(f, line) && files.size() < maxFiles) {
                if (!line.empty()) {
                    files.push_back(line);
                }
            }
        }
    }

    void save() {
        std::ofstream f(configFile);
        if (f.is_open()) {
            for (const auto& file : files) {
                f << file << "\n";
            }
        }
    }
};


HWND g_hwnd = NULL;
HMENU g_hmenu = NULL;
std::unique_ptr<Bus> g_nes;
RecentFiles g_recentFiles;
bool g_romLoaded = false;
bool g_paused = false;
SDL_AudioDeviceID g_audioDevice = 0;
std::string g_currentRomPath;
int g_windowScale = 3; 


SDL_Window* g_sdlWindow = NULL;
SDL_Renderer* g_sdlRenderer = NULL;
SDL_Texture* g_sdlTexture = NULL;


void ResizeWindow(int scale) {
    if (scale < 1) scale = 1;
    if (scale > 5) scale = 5;
    
    g_windowScale = scale;
    
    
    int clientWidth = 256 * scale;
    int clientHeight = 240 * scale;
    
    RECT rect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    
    SetWindowPos(g_hwnd, NULL, 0, 0, 
                 rect.right - rect.left, 
                 rect.bottom - rect.top,
                 SWP_NOMOVE | SWP_NOZORDER);
    
    
    HMENU optionsMenu = GetSubMenu(g_hmenu, 2);
    for (int i = 1; i <= 5; i++) {
        CheckMenuItem(optionsMenu, IDM_OPTIONS_SCALE_1 + i - 1, 
                     (i == scale) ? MF_CHECKED : MF_UNCHECKED);
    }
}


void UpdateRecentFilesMenu() {
    HMENU fileMenu = GetSubMenu(g_hmenu, 0);
    
    
    for (int i = 0; i < 5; i++) {
        RemoveMenu(fileMenu, IDM_FILE_RECENT_1 + i, MF_BYCOMMAND);
    }
    
    
    const auto& files = g_recentFiles.getFiles();
    int insertPos = 3; 
    
    for (size_t i = 0; i < files.size(); i++) {
        std::string filename = files[i];
        
        size_t pos = filename.find_last_of("\\/");
        if (pos != std::string::npos) {
            filename = filename.substr(pos + 1);
        }
        
        std::string menuText = "&" + std::to_string(i + 1) + " " + filename;
        InsertMenuA(fileMenu, insertPos + i, MF_BYPOSITION | MF_STRING, 
                    IDM_FILE_RECENT_1 + i, menuText.c_str());
    }
    
    DrawMenuBar(g_hwnd);
}


bool OpenROM(const std::string& path) {
    try {
        auto cart = std::make_shared<Cartridge>(path);
        g_nes = std::make_unique<Bus>();
        g_nes->insertCartridge(cart);
        g_nes->reset();
        
        
        if (g_audioDevice && !no_sound) {
            g_nes->apu.setSampleCallback([](float sample) {
                int next_write = (audio_write_pos + 1) % BUFFER_SIZE;
                if (next_write != audio_read_pos) {
                    if (sample > 1.0f) sample = 1.0f;
                    if (sample < -1.0f) sample = -1.0f;
                    audio_buffer[audio_write_pos] = sample;
                    audio_write_pos = next_write;
                }
            });
        }
        
        g_romLoaded = true;
        g_currentRomPath = path;
        g_recentFiles.add(path);
        UpdateRecentFilesMenu();
        
        
        std::string filename = path;
        size_t pos = filename.find_last_of("\\/");
        if (pos != std::string::npos) {
            filename = filename.substr(pos + 1);
        }
        std::string title = "MiniNES - " + filename;
        SetWindowTextA(g_hwnd, title.c_str());
        
        return true;
    }
    catch (const std::exception& e) {
        MessageBoxA(g_hwnd, e.what(), "Error Loading ROM", MB_OK | MB_ICONERROR);
        return false;
    }
}


void ShowOpenDialog() {
    char filename[MAX_PATH] = "";
    
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwnd;
    ofn.lpstrFilter = "NES ROM Files\0*.nes\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "nes";
    
    if (GetOpenFileNameA(&ofn)) {
        OpenROM(filename);
    }
}


void CloseROM() {
    if (g_romLoaded) {
        g_nes.reset();
        g_romLoaded = false;
        g_currentRomPath.clear();
        SetWindowTextA(g_hwnd, "MiniNES");
    }
}


HMENU CreateMainMenu() {
    HMENU hMenuBar = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hNesMenu = CreateMenu();
    HMENU hOptionsMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();
    
    
    AppendMenuA(hFileMenu, MF_STRING, IDM_FILE_OPEN, "&Open ROM...\tCtrl+O");
    AppendMenuA(hFileMenu, MF_STRING, IDM_FILE_CLOSE, "&Close ROM");
    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);
    
    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit\tAlt+F4");
    
    
    AppendMenuA(hNesMenu, MF_STRING, IDM_NES_RESET, "&Reset\tR");
    AppendMenuA(hNesMenu, MF_STRING, IDM_NES_HARD_RESET, "&Hard Reset");
    AppendMenuA(hNesMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hNesMenu, MF_STRING, IDM_NES_PAUSE, "&Pause\tP");
    
    
    AppendMenuA(hOptionsMenu, MF_STRING | MF_CHECKED, IDM_OPTIONS_SOUND, "&Sound\tM");
    AppendMenuA(hOptionsMenu, MF_STRING, IDM_OPTIONS_CHEAT, "&Cheat Console");
    AppendMenuA(hOptionsMenu, MF_SEPARATOR, 0, NULL);
    
    
    HMENU hScaleMenu = CreateMenu();
    AppendMenuA(hScaleMenu, MF_STRING, IDM_OPTIONS_SCALE_1, "&1× (256×240)");
    AppendMenuA(hScaleMenu, MF_STRING, IDM_OPTIONS_SCALE_2, "&2× (512×480)");
    AppendMenuA(hScaleMenu, MF_STRING | MF_CHECKED, IDM_OPTIONS_SCALE_3, "&3× (768×720)");
    AppendMenuA(hScaleMenu, MF_STRING, IDM_OPTIONS_SCALE_4, "&4× (1024×960)");
    AppendMenuA(hScaleMenu, MF_STRING, IDM_OPTIONS_SCALE_5, "&5× (1280×1200)");
    AppendMenuA(hOptionsMenu, MF_POPUP, (UINT_PTR)hScaleMenu, "&Window Scale");
    
    
    AppendMenuA(hHelpMenu, MF_STRING, IDM_HELP_CONTROLS, "&Controls");
    AppendMenuA(hHelpMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, "&About");
    
    
    AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
    AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hNesMenu, "&NES");
    AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hOptionsMenu, "&Options");
    AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");
    
    return hMenuBar;
}


void HandleControllerInput() {
    if (!g_romLoaded || !g_nes) return;
    
    g_nes->controller[0] = 0x00;
    
    if (GetAsyncKeyState('X') & 0x8000) g_nes->controller[0] |= 0x80; 
    if (GetAsyncKeyState('Z') & 0x8000) g_nes->controller[0] |= 0x40; 
    if (GetAsyncKeyState('A') & 0x8000) g_nes->controller[0] |= 0x20; 
    if (GetAsyncKeyState('S') & 0x8000) g_nes->controller[0] |= 0x10; 
    if (GetAsyncKeyState(VK_UP) & 0x8000) g_nes->controller[0] |= 0x08;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) g_nes->controller[0] |= 0x04;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) g_nes->controller[0] |= 0x02;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) g_nes->controller[0] |= 0x01;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_FILE_OPEN:
            ShowOpenDialog();
            break;
            
        case IDM_FILE_CLOSE:
            CloseROM();
            break;
            
        case IDM_FILE_RECENT_1:
        case IDM_FILE_RECENT_2:
        case IDM_FILE_RECENT_3:
        case IDM_FILE_RECENT_4:
        case IDM_FILE_RECENT_5: {
            int idx = LOWORD(wParam) - IDM_FILE_RECENT_1;
            const auto& files = g_recentFiles.getFiles();
            if (idx < (int)files.size()) {
                OpenROM(files[idx]);
            }
            break;
        }
            
        case IDM_FILE_EXIT:
            PostQuitMessage(0);
            break;
            
        case IDM_NES_RESET:
            if (g_romLoaded && g_nes) {
                g_nes->reset();
            }
            break;
            
        case IDM_NES_HARD_RESET:
            if (g_romLoaded && g_nes) {
                auto cart = g_nes->cart;
                g_nes.reset();
                g_nes = std::make_unique<Bus>();
                g_nes->insertCartridge(cart);
                g_nes->reset();
            }
            break;
            
        case IDM_NES_PAUSE:
            g_paused = !g_paused;
            CheckMenuItem(GetMenu(hwnd), IDM_NES_PAUSE, 
                         g_paused ? MF_CHECKED : MF_UNCHECKED);
            break;
            
        case IDM_OPTIONS_SOUND:
            no_sound = !no_sound;
            CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_SOUND, 
                         no_sound ? MF_UNCHECKED : MF_CHECKED);
            break;
            
        case IDM_OPTIONS_SCALE_1:
        case IDM_OPTIONS_SCALE_2:
        case IDM_OPTIONS_SCALE_3:
        case IDM_OPTIONS_SCALE_4:
        case IDM_OPTIONS_SCALE_5:
            ResizeWindow(LOWORD(wParam) - IDM_OPTIONS_SCALE_1 + 1);
            break;
            
        case IDM_HELP_CONTROLS:
            MessageBoxA(hwnd, 
                "Controls:\n\n"
                "Arrow Keys - D-Pad\n"
                "Z - B Button\n"
                "X - A Button\n"
                "A - Select\n"
                "S - Start\n"
                "R - Reset\n"
                "M - Toggle Sound\n"
                "P - Pause",
                "Controls", MB_OK | MB_ICONINFORMATION);
            break;
            
        case IDM_HELP_ABOUT:
            MessageBoxA(hwnd,
                "MiniNES Emulator v1.0\n\n"
                "A simple NES emulator\n"
                "Built with C++ and SDL2",
                "About", MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;
        
    case WM_KEYDOWN:
        switch (wParam) {
        case 'R':
            if (g_romLoaded && g_nes) g_nes->reset();
            break;
        case 'M':
            PostMessage(hwnd, WM_COMMAND, IDM_OPTIONS_SOUND, 0);
            break;
        case 'P':
            PostMessage(hwnd, WM_COMMAND, IDM_NES_PAUSE, 0);
            break;
        case 'O':
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                ShowOpenDialog();
            }
            break;
        }
        break;
        
    case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        char filename[MAX_PATH];
        if (DragQueryFileA(hDrop, 0, filename, MAX_PATH)) {
            OpenROM(filename);
        }
        DragFinish(hDrop);
        break;
    }
        
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        MessageBoxA(NULL, SDL_GetError(), "SDL Init Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MiniNESClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Window Registration Failed!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    g_hmenu = CreateMainMenu();
    
    
    int initialWidth = 256 * g_windowScale;
    int initialHeight = 240 * g_windowScale;
    RECT rect = { 0, 0, initialWidth, initialHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    
    g_hwnd = CreateWindowExA(
        WS_EX_ACCEPTFILES,
        "MiniNESClass",
        "MiniNES",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, g_hmenu, hInstance, NULL
    );
    
    if (!g_hwnd) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    g_sdlWindow = SDL_CreateWindowFrom((void*)g_hwnd);
    if (!g_sdlWindow) {
        MessageBoxA(NULL, SDL_GetError(), "SDL Window Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    g_sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!g_sdlRenderer) {
        MessageBoxA(NULL, SDL_GetError(), "SDL Renderer Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");  
    
    g_sdlTexture = SDL_CreateTexture(g_sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (!g_sdlTexture) {
        MessageBoxA(NULL, SDL_GetError(), "SDL Texture Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    
    SDL_AudioSpec desired, obtained;
    SDL_zero(desired);
    desired.freq = SAMPLE_RATE;
    desired.format = AUDIO_F32;
    desired.channels = 1;
    desired.samples = 512;
    desired.callback = audio_callback;
    
    g_audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 
                                        SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (g_audioDevice) {
        SDL_PauseAudioDevice(g_audioDevice, 0);
    }
    
    
    UpdateRecentFilesMenu();
    
    
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    
    MSG msg;
    bool running = true;
    
    
    const double TARGET_FPS = 60.0988;  
    const double FRAME_TIME = 1000.0 / TARGET_FPS;  
    LARGE_INTEGER perfFreq, lastTime, currentTime;
    QueryPerformanceFrequency(&perfFreq);
    QueryPerformanceCounter(&lastTime);
    double accumulator = 0.0;
    
    while (running) {
        
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        
        HandleControllerInput();
        
        
        if (g_romLoaded && g_nes && !g_paused) {
            do {
                g_nes->clock();
            } while (!g_nes->ppu.frame_complete);
            g_nes->ppu.frame_complete = false;
        }
        
        
        SDL_SetRenderDrawColor(g_sdlRenderer, 40, 40, 40, 255);
        SDL_RenderClear(g_sdlRenderer);
        
        if (g_romLoaded && g_nes) {
            SDL_Surface* screenSurface = g_nes->ppu.getScreen();
            SDL_UpdateTexture(g_sdlTexture, NULL, screenSurface->pixels, screenSurface->pitch);
            
            
            RECT clientRect;
            GetClientRect(g_hwnd, &clientRect);
            int windowWidth = clientRect.right - clientRect.left;
            int windowHeight = clientRect.bottom - clientRect.top;
            
            
            int scaleX = windowWidth / 256;
            int scaleY = windowHeight / 240;
            int scale = (scaleX < scaleY) ? scaleX : scaleY;
            if (scale < 1) scale = 1;
            
            int renderWidth = 256 * scale;
            int renderHeight = 240 * scale;
            int offsetX = (windowWidth - renderWidth) / 2;
            int offsetY = (windowHeight - renderHeight) / 2;
            
            SDL_Rect dstRect = { offsetX, offsetY, renderWidth, renderHeight };
            SDL_RenderCopy(g_sdlRenderer, g_sdlTexture, NULL, &dstRect);
        }
        
        SDL_RenderPresent(g_sdlRenderer);
        
        
        QueryPerformanceCounter(&currentTime);
        double deltaTime = ((currentTime.QuadPart - lastTime.QuadPart) * 1000.0) / perfFreq.QuadPart;
        lastTime = currentTime;
        
        accumulator += deltaTime;
        
        
        if (accumulator < FRAME_TIME) {
            double sleepTime = FRAME_TIME - accumulator;
            if (sleepTime > 1.0) {
                Sleep((DWORD)(sleepTime - 1.0)); 
            }
            
            while (accumulator < FRAME_TIME) {
                QueryPerformanceCounter(&currentTime);
                deltaTime = ((currentTime.QuadPart - lastTime.QuadPart) * 1000.0) / perfFreq.QuadPart;
                accumulator += deltaTime;
                lastTime = currentTime;
            }
        }
        
        accumulator -= FRAME_TIME;
        
        if (accumulator > FRAME_TIME * 2) {
            accumulator = 0.0;
        }
    }
    
    
    if (g_audioDevice) SDL_CloseAudioDevice(g_audioDevice);
    if (g_sdlTexture) SDL_DestroyTexture(g_sdlTexture);
    if (g_sdlRenderer) SDL_DestroyRenderer(g_sdlRenderer);
    if (g_sdlWindow) SDL_DestroyWindow(g_sdlWindow);
    SDL_Quit();
    
    return (int)msg.wParam;
}