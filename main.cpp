#include <hge.h>
#include <hgerect.h>

HGE *hge = 0;

hgeRect positiveBoxes[50];
hgeRect negativeBoxes[50];
hgeRect playerBoxes[50];

struct BoxEvent {
	float x1;
	float x2;
	float y;
	bool isTop;
};

void BuildLevel() {
	
}

bool Update() {
	if (hge->Input_GetKeyState(HGEK_ESCAPE)){
		return true;
	}
	
	return false;
}

bool Render() {
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	hge = hgeCreate(HGE_VERSION);
	
	hge->System_SetState(HGE_LOGFILE, "Log.log");
	hge->System_SetState(HGE_FRAMEFUNC, Update);
	hge->System_SetState(HGE_RENDERFUNC, Render);
	hge->System_SetState(HGE_TITLE, "Filling in the Music");
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_SCREENWIDTH, 400);
	hge->System_SetState(HGE_SCREENHEIGHT, 400);
	hge->System_SetState(HGE_HIDEMOUSE, false);

	if(hge->System_Initiate()) {
		BuildLevel();
		hge->System_Start();
	} else {	
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	hge->System_Shutdown();
	hge->Release();

	return 0;
}
