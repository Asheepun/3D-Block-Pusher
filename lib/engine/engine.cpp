//common includes
#include "engine/engine.h"
#include "engine/strings.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include <vector>

//#include "glad/glad.h"

#include "glad/gl.h"

//linux includes
#ifdef __linux__

#include "time.h"
#include "unistd.h"

#include "X11/X.h"
#include "X11/Xlib.h"
#include "X11/extensions/Xfixes.h"
#include "X11/XKBlib.h"
#include <X11/Xutil.h>
#include "GL/glx.h"

#endif

//windows includes
#ifdef _WIN32

#include "glad/wgl.h"

#include "time.h"
#include <limits.h>
#include <winnt.h>

#include "windows.h"
#include "winuser.h"

//#include "glad/glad_wgl.h"

#endif

#ifdef __linux__
Display *dpy;
int screenNumber;
Window root;
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;
XWindowAttributes gwa;
XEvent xev;

typedef GLXContext (*glXCreateContextAttribsARBProc)
    (Display*, GLXFBConfig, GLXContext, Bool, const int*);

#endif

#ifdef _WIN32
HWND hwnd;
#endif

//GLOBAL VARIABLE DEFINITIONS

//Engine_Pixel colorBuffers[ENGINE_COLORS_LENGTH][COLOR_BUFFER_SIZE];
//int screenWidth = 800;
//int screenHeight = 450;
//Engine_Pixel *screenPixels = NULL;
//int windowWidth = 800;
//int windowHeight = 450;
int Engine_clientWidth = 800;
int Engine_clientHeight = 450;
bool Engine_isFullscreen = false;

int Engine_elapsedFrames = 0;

bool Engine_fpsModeOn = false;

Engine_Key Engine_keys[ENGINE_KEYS_LENGTH];

Engine_Pointer Engine_pointer;

std::vector<char> Engine_textInput;
//Array Engine_textInput;

#ifdef __linux__
static unsigned int OS_KEY_IDENTIFIERS[] = {

	XK_0,
	XK_1,
	XK_2,
	XK_3,
	XK_4,
	XK_5,
	XK_6,
	XK_7,
	XK_8,
	XK_9,

	XK_A,
	XK_B,
	XK_C,
	XK_D,
	XK_E,
	XK_F,
	XK_G,
	XK_H,
	XK_I,
	XK_J,
	XK_K,
	XK_L,
	XK_M,
	XK_N,
	XK_O,
	XK_P,
	XK_Q,
	XK_R,
	XK_S,
	XK_T,
	XK_U,
	XK_V,
	XK_W,
	XK_X,
	XK_Y,
	XK_Z,

	XK_Up,
	XK_Down,
	XK_Left,
	XK_Right,

	XK_space,
	XK_Escape,

	XK_Shift_L,

};
#endif

#ifdef _WIN32
static unsigned int OS_KEY_IDENTIFIERS[] = {

	0x30,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38,
	0x39,

	0x41,
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4A,
	0x4B,
	0x4C,
	0x4D,
	0x4E,
	0x4F,
	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5A,

	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,

	VK_SPACE,
	VK_ESCAPE,

	VK_SHIFT,

};
#endif

bool programShouldQuit = false;

//COMMON INITS

/*
void initPixelDrawing(){

	//init screen pixels
	screenPixels = malloc(sizeof(Engine_Pixel) * screenWidth * screenHeight);

	//init color buffers
	for(int i = 0; i < ENGINE_COLORS_LENGTH; i++){
		for(int j = 0; j < COLOR_BUFFER_SIZE; j++){
			colorBuffers[i][j] = ENGINE_COLORS[i];
		}
	}

}
*/

void initKeys(){

	for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){

		Engine_keys[i].OSIdentifier = OS_KEY_IDENTIFIERS[i];

		Engine_keys[i].down = false;
		Engine_keys[i].downed = false;
		Engine_keys[i].upped = false;
	
	}

	//Array_init(&Engine_textInput, sizeof(char) * SMALL_STRING_SIZE);
	
}

void resetKeys(){
	for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){
		Engine_keys[i].downed = false;
		Engine_keys[i].upped = false;
	}

	Engine_textInput.clear();
	//Array_clear(&Engine_textInput);
}

void initPointer(){
	Engine_pointer.pos = getVec2f(0, 0);
	Engine_pointer.down = false;
	Engine_pointer.downed = false;
	Engine_pointer.upped = false;
}

void resetPointer(){
	Engine_pointer.downed = false;
	Engine_pointer.upped = false;
}

//ENGINE ENTRY POINT

#ifdef __linux__
int main(){

	//setup window
	dpy = XOpenDisplay(NULL);

	if(dpy == NULL){
		printf("Cannot open X display!\n");
		return 0;
	}

	root = DefaultRootWindow(dpy);
	screenNumber = DefaultScreen(dpy);

	vi = glXChooseVisual(dpy, 0, att);

	if(vi == NULL){
		printf("Could not Choose X Visual\n");
		return 0;
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | StructureNotifyMask | ButtonReleaseMask | PointerMotionMask;

	win = XCreateWindow(dpy, root, 0, 0, Engine_clientWidth, Engine_clientHeight, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);

	XStoreName(dpy, win, "Untitled Engine Program");

	static int visual_attribs[] = {
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		None
    };

    int num_fbc = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &num_fbc);

    if (!fbc) {
        printf("glXChooseFBConfig() failed\n");
        exit(1);
    }

	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	static int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        None
    };

	glc = glXCreateContextAttribsARB(dpy, fbc[0], NULL, GL_TRUE, context_attribs);
	glXMakeCurrent(dpy, win, glc);

	gladLoadGL((GLADloadfunc)glXGetProcAddress);

	//int screen = DefaultScreen(dpy);

	//glXSwapIntervalEXT(1);

	Atom wmDelete = XInternAtom(dpy, "WM_DELETE_WINDOW", true);
	XSetWMProtocols(dpy, win, &wmDelete, 1);

	int autoRepeatIsAvailable;
	XkbSetDetectableAutoRepeat(dpy, true, &autoRepeatIsAvailable);

	//common inits
	//initPixelDrawing();
	initKeys();
	initPointer();

	Engine_start();

	//game loop
	size_t startTicks = 0;
	size_t endTicks = 0;

	size_t deltaTime = 0;
	size_t accumilatedTime = 0;
	size_t frameTime = 1000000 / 60;

	int fps = 60;

	size_t ticksPerFrame = CLOCKS_PER_SEC / fps;

	//bool quit = false;

	while(!programShouldQuit){

		startTicks = clock();

		//handle events
		while(XPending(dpy) > 0){

			XNextEvent(dpy, &xev);

			if(xev.type == ClientMessage
			|| xev.type == DestroyNotify){
				programShouldQuit = true;
			}

			if(xev.type == ConfigureNotify){

				XConfigureEvent xce = xev.xconfigure;

				if(xce.width != Engine_clientWidth
				|| xce.height != Engine_clientHeight){
					Engine_clientWidth = xce.width;
					Engine_clientHeight = xce.height;
				}

			}

			if(xev.type == KeyPress){

				//if(xev.xkey.keycode == XKeysymToKeycode(dpy, XK_Q)){
					//quit = true;
				//}
				char buffer[SMALL_STRING_SIZE];
				String_set(buffer, "", SMALL_STRING_SIZE);

				XLookupString((XKeyPressedEvent *)&xev, buffer, STRING_SIZE, NULL, NULL);

				if(!(strcmp(buffer, "") == 0)){
					Engine_textInput.push_back(*buffer);
					//char *text = Array_addItem(&Engine_textInput);
					//String_set(text, buffer, SMALL_STRING_SIZE);
				}

				for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){
					if(xev.xkey.keycode == XKeysymToKeycode(dpy, Engine_keys[i].OSIdentifier)){
						if(!Engine_keys[i].down){
							Engine_keys[i].downed = true;
						}
						Engine_keys[i].down = true;
					}
				}
			}

			if(xev.type == KeyRelease){

				for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){
					if(xev.xkey.keycode == XKeysymToKeycode(dpy, Engine_keys[i].OSIdentifier)){
						if(Engine_keys[i].down){
							Engine_keys[i].upped = true;
						}
						Engine_keys[i].down = false;
					}
				}

			}

			if(xev.type == ButtonPress){
				XButtonEvent *buttonEvent_p = (XButtonEvent *)&xev;

				if(buttonEvent_p->button == 1){
					Engine_pointer.down = true;
					Engine_pointer.downed = true;
					Engine_pointer.lastDownedPos = Engine_pointer.pos;
				}

			}
			if(xev.type == ButtonRelease){

				XButtonEvent *buttonEvent_p = (XButtonEvent *)&xev;

				if(buttonEvent_p->button == 1){
					Engine_pointer.down = false;
					Engine_pointer.upped = true;
					Engine_pointer.lastUppedPos = Engine_pointer.pos;
				}

			}

			if(xev.type == MotionNotify){
				XMotionEvent *motionEvent_p = (XMotionEvent *)&xev;

				Engine_pointer.pos.x = motionEvent_p->x;
				Engine_pointer.pos.y = motionEvent_p->y;

				{
					Engine_pointer.movement.x = motionEvent_p->x - Engine_clientWidth / 2;
					Engine_pointer.movement.y = motionEvent_p->y - Engine_clientHeight / 2;
				}
			}
		
		}

		//do fps magic

		if(Engine_fpsModeOn){

			int screenWidth = DisplayWidth(dpy, DefaultScreen(dpy));
			int screenHeight = DisplayHeight(dpy, DefaultScreen(dpy));

			XWarpPointer(dpy, None, root, 0, 0, 0, 0, screenWidth / 2, screenHeight / 2);

		}else{
		}

		//update

		//while(accumilatedTime > frameTime){

		Engine_update(1);

			//accumilatedTime -= frameTime;

		resetKeys();
		resetPointer();
			
		//}

		//draw

		Engine_draw();

		//glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, screenPixels);

		glXSwapBuffers(dpy, win);

		Engine_elapsedFrames++;

		endTicks = clock();

		deltaTime = (endTicks - startTicks) / (CLOCKS_PER_SEC / 1000000);

		//printf("\nframeTime: %i\n", deltaTime);

		//printf("%f\n", (float)deltaTime / 1000);

		int lag = frameTime - deltaTime;

		//printf("lag: %i\n", lag);

		if(lag < 0){
			lag = 0;
		}

		usleep(lag);

		//accumilatedTime += deltaTime;

		//printf("%i\n", deltaTime);

		//int lag = ticksPerFrame - (endTicks - startTicks);

		//if(lag < 0){
			//lag = 0;
		//}

		//float lagMilliseconds = (float)lag / (float)(CLOCKS_PER_SEC / 1000);

		//usleep(lagMilliseconds * 1000);

	
	}

	Engine_finnish();

	return 0;

}
#endif

#ifdef _WIN32
//bool QUIT_PROGRAM = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	
	//setup window
	const char CLASS_NAME[] = "Untitled Engine Program";
	
	WNDCLASS wc = {};
	
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	
	RegisterClass(&wc);
	
	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Untitled Engine Program",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		
		NULL,
		NULL,
		hInstance,
		NULL
	);
	
	if(hwnd == NULL){
		printf("Could not create Window");
		return 0;
	}
	
	HDC hdc = GetDC(hwnd);
	
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	
	int pf = ChoosePixelFormat(hdc, &pfd);
	
	if(pf == 0){
		printf("Could not choose pixel format\n");
		return 0;
	}
	
	SetPixelFormat(hdc, pf, &pfd);
	
	DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	
	HGLRC hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	gladLoaderLoadWGL(hdc);
	gladLoaderLoadGL();
	//gladLoadGL();

	wglSwapIntervalEXT(1);

	printf("%s\n", glGetString(GL_VERSION));
	//printf("%s\n", glGetString(GL_EXTENSIONS));
	//printf("%s\n", wglGetExtensionsStringARB());

	//wglSwapIntervalEXT(0);

	//common inits
	//initPixelDrawing();
	initKeys();
	initPointer();
	
	Engine_start();
	
	ShowWindow(hwnd, nCmdShow);

	LARGE_INTEGER liFrequency = {0};
	LARGE_INTEGER liStart = {0};
	LARGE_INTEGER liStop = {0};

	float deltaTime;

	float accumilatedTime = 0;

	//game loop
	while(!programShouldQuit){

		QueryPerformanceFrequency(&liFrequency);

		QueryPerformanceCounter(&liStart);
	
		//handle events
		MSG msg = {};
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		
		}

		//update
			
		while(accumilatedTime > 1000 / 60){

			Engine_update((float)(1 / 60));

			accumilatedTime -= 1000 / 60;

			resetKeys();
			resetPointer();

			//printf("hello from here %f\n", accumilatedTime);
		
		}
		
		//draw
		
		Engine_draw();
		
		SwapBuffers(hdc);
		
		//glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, screenPixels);

		Engine_elapsedFrames++;

		QueryPerformanceCounter(&liStop);

		deltaTime = (float)((liStop.QuadPart - liStart.QuadPart) * 1000000 / liFrequency.QuadPart) / 1000;

		accumilatedTime += deltaTime;
		
		//printf("%f\n", deltaTime);

	}

	Engine_finnish();
	
	return 0;
	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	
	if(uMsg == WM_CLOSE
	|| uMsg == WM_DESTROY
	|| uMsg == WM_QUIT){
		PostQuitMessage(0);
		programShouldQuit = true;
		return 0;
	}

	if(uMsg == WM_KEYDOWN){
		
		for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){
			if(wParam == Engine_keys[i].OSIdentifier){
				if(!Engine_keys[i].down){
					Engine_keys[i].downed = true;
				}
				Engine_keys[i].down = true;
			}
		}

	}

	if(uMsg == WM_KEYUP){
		
		for(int i = 0; i < ENGINE_KEYS_LENGTH; i++){
			if(wParam == Engine_keys[i].OSIdentifier){
				if(Engine_keys[i].down){
					Engine_keys[i].upped = true;
				}
				Engine_keys[i].down = false;
			}
		}

		
	}

	if(uMsg == WM_SIZE){

		Engine_clientWidth = LOWORD(lParam);
		Engine_clientHeight = HIWORD(lParam);

		//printf("%i\n", GetLastError());

		//if(result){
			//printf("%f\n", lpRect->right);
		//
		//}

		//clientWidth = (int)lpRect->right;
		//clientHeight = lpRect->bottom - lpRect->top;

	}
	
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}
#endif

//ENGINE FUNCTIONS

void Engine_quit(){
	programShouldQuit = true;
}

//WINDOW FUNCTIONS
void Engine_setFPSMode(bool setting){

	if(Engine_fpsModeOn == setting){
		return;
	}

	Engine_fpsModeOn = setting;

#ifdef __linux__
	if(Engine_fpsModeOn){
		XFixesHideCursor(dpy, root);
		XFlush(dpy);
		Engine_pointer.movement = getVec2f(0, 0);

		int screenWidth = DisplayWidth(dpy, DefaultScreen(dpy));
		int screenHeight = DisplayHeight(dpy, DefaultScreen(dpy));

		Engine_pointer.pos = getVec2f(screenWidth / 2, screenHeight / 2);
	}else{
		XFixesShowCursor(dpy, root);
		XFlush(dpy);
	}
#endif

}

void Engine_setWindowTitle(char *title){
	
#ifdef _WIN32
	SetWindowTextA(hwnd, (LPCSTR)title);
#endif
	
}

void Engine_setWindowSize(int width, int height){

	//windowWidth = width;
	//windowHeight = height;
	Engine_clientWidth = width;
	Engine_clientHeight = height;

#ifdef __linux__
	XResizeWindow(dpy, win, width, height);
#endif

#ifdef _WIN32

	
	RECT rect;
	rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
#endif

	//screenPixels = malloc(sizeof(Engine_Pixel) * width * height);

}

void Engine_centerWindow(){

#ifdef __linux__
	XMoveWindow(dpy, win, DisplayWidth(dpy, screenNumber) / 2 - Engine_clientWidth / 2, DisplayHeight(dpy, screenNumber) / 2 - Engine_clientHeight / 2);
#endif

}

void Engine_toggleFullscreen(){

#ifdef _WIN32
	if(!Engine_isFullscreen){
		SetWindowLongPtrA(hwnd, -16, WS_VISIBLE);

		SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

		Engine_isFullscreen = true;
	}else{
		SetWindowLongPtrA(hwnd, -16, WS_OVERLAPPEDWINDOW);
		Engine_setWindowSize(480 * 2, 270 * 2);

		Engine_isFullscreen = false;
	}
#endif

#ifdef __linux__
	if(!Engine_isFullscreen){

		Engine_setWindowSize(DisplayWidth(dpy, screenNumber), DisplayHeight(dpy, screenNumber));
		Engine_centerWindow();

		Engine_isFullscreen = true;
	}else{

		Engine_setWindowSize(480 * 2, 270 * 2);
		Engine_centerWindow();
	
		Engine_isFullscreen = false;
	}
#endif

}

/*
//DRAWING FUNCTIONS
unsigned int Engine_getScreenPixelIndex(int x, int y){
	return x + (screenHeight - y - 1) * screenWidth;
}

void Engine_fillRect(int x, int y, int w, int h, enum Engine_ColorEnum color){

	if(x > screenWidth
	|| y > screenHeight
	|| x + w < 0
	|| y + h < 0){
		return;
	}

	if(x + w > screenWidth){
		w = screenWidth - x;
	}
	if(y + h > screenHeight){
		h = screenHeight - y;
	}
	if(x < 0){
		w = w + x;
		x = 0;
	}
	if(y < 0){
		h = h + y;
		y = 0;
	}

	for(int i = 0; i < h; i++){

		unsigned int index = Engine_getScreenPixelIndex(x, y + i);
		Engine_Pixel *pixel_p = &screenPixels[index];

		memcpy(pixel_p, colorBuffers[color], w * sizeof(Engine_Pixel));

	}
}

void Engine_drawLine(Vec2f p1, Vec2f p2, Engine_Pixel color){
	
	Vec2f diff = p1;
	Vec2f_sub(&diff, p2);
	
	int length = getMagVec2f(diff);

	Vec2f_normalize(&diff);
		
	Vec2f pos = p2;

	for(int i = 0; i < length; i++){

		unsigned int index = Engine_getScreenPixelIndex(pos.x, pos.y);
		Engine_Pixel *pixel_p = &screenPixels[index];

		*pixel_p = color;
		
		pos.x += diff.x;
		pos.y += diff.y;

	}


}

void Engine_drawTriangle(Vec2f *verts, Engine_Pixel color){

	for(int i = 0; i < 3; i++){

		Vec2f p1 = verts[(i + 0) % 3];
		Vec2f p2 = verts[(i + 1) % 3];

		Engine_drawLine(p1, p2, color);
	
	}

}

void Engine_fillTriangle(Vec2f *verts, Engine_Pixel color){

	

}
*/
