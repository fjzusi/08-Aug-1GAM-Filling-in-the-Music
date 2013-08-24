#include <hge.h>
#include <hgerect.h>
#include <stdlib.h>
#include <time.h>

const int ARRAY_SIZE = 50;
const int GAME_SIZE = 400;
const int SPAWN_SIZE = 150;
const int SPAWN_BORDER = 125;

HGE *hge = 0;

bool canClick = false;

DWORD positiveColor = 0xFF009496;
DWORD negativeColor = 0xFF000000;
DWORD playerColor = 0xFF00FF00;

hgeQuad pBoxes[ARRAY_SIZE];
hgeQuad nBoxes[ARRAY_SIZE];
hgeQuad playBoxes[ARRAY_SIZE];

int numPBox = 0;
int numNBox = 0;
int numPlayBox = 0;

struct BoxEvent {
	float x1;
	float x2;
	float y;
	bool isTop;
};

// Initialize Game for initial load
void Initialize() {
	canClick = false;
	numPBox = 0;
	numNBox = 0;
	numPlayBox = 0;
	
	for(int i = 0; i < ARRAY_SIZE; i++) {
		for(int j = 0; j < 4; j++) {
			pBoxes[i].v[j].x = 0;
			pBoxes[i].v[j].y = 0;
			pBoxes[i].v[j].col = positiveColor;
			
			nBoxes[i].v[j].x = 0;
			nBoxes[i].v[j].y = 0;
			nBoxes[i].v[j].col = negativeColor;
			
			playBoxes[i].v[j].x = 0;
			playBoxes[i].v[j].y = 0;
			playBoxes[i].v[j].col = playerColor;
		}
		
		pBoxes[i].tex = 0;
		pBoxes[i].blend = BLEND_DEFAULT;
		
		nBoxes[i].tex = 0;
		nBoxes[i].blend = BLEND_DEFAULT;
		
		playBoxes[i].tex = 0;
		playBoxes[i].blend = BLEND_DEFAULT;
	}
}

// Initialize for a new level
void ReInitialize() {
	canClick = false;
	numPBox = 0;
	numNBox = 0;
	numPlayBox = 0;
	
	for(int i = 0; i < ARRAY_SIZE; i++) {
		for(int j = 0; j < 4; j++) {
			pBoxes[i].v[j].x = 0;
			pBoxes[i].v[j].y = 0;
			
			nBoxes[i].v[j].x = 0;
			nBoxes[i].v[j].y = 0;
			
			playBoxes[i].v[j].x = 0;
			playBoxes[i].v[j].y = 0;
		}
	}
}

void GeneratePositiveBox() {
	int originX = rand() % (SPAWN_SIZE) + SPAWN_BORDER;
	int originY = rand() % (SPAWN_SIZE) + SPAWN_BORDER;
	
	for(int i = 0; i < 4; i++) {
		pBoxes[numPBox].v[i].x = originX;
		pBoxes[numPBox].v[i].y = originY;
	}
	
	numPBox++;
}

void GrowPositiveBoxes() {
	int growth = rand() % 10 + 10;
	
	for(int i = 0; i < numPBox; i++) {
		pBoxes[i].v[0].x = pBoxes[i].v[0].x - growth;
		pBoxes[i].v[0].y = pBoxes[i].v[0].y - growth;
		
		pBoxes[i].v[1].x = pBoxes[i].v[1].x + growth;
		pBoxes[i].v[1].y = pBoxes[i].v[1].y - growth;
		
		pBoxes[i].v[2].x = pBoxes[i].v[2].x + growth;
		pBoxes[i].v[2].y = pBoxes[i].v[2].y + growth;
		
		pBoxes[i].v[3].x = pBoxes[i].v[3].x - growth;
		pBoxes[i].v[3].y = pBoxes[i].v[3].y + growth;
	}
}

void BuildPositiveBoxes() {
	int numBoxes = rand() % 3 + 5;
	while(numBoxes > 0) {
		GeneratePositiveBox();
		GrowPositiveBoxes();
		numBoxes--;
	}
}

void BuildNegativeBoxes() {
	
}

void BuildLevel() {
	Initialize();
	BuildPositiveBoxes();
	BuildNegativeBoxes();
	canClick = true;
}

bool Update() {
	if (hge->Input_GetKeyState(HGEK_ESCAPE)){
		return true;
	}
	
	if(hge->Input_KeyDown(HGEK_LBUTTON)) {
		BuildLevel();
	}
	
	return false;
}

bool Render() {
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	
	for(int i = 0; i < numPBox; i++) {
		hge->Gfx_RenderQuad(&pBoxes[i]);
	}
	
	hge->Gfx_EndScene();
	
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
	hge->System_SetState(HGE_SCREENWIDTH, GAME_SIZE);
	hge->System_SetState(HGE_SCREENHEIGHT, GAME_SIZE);
	hge->System_SetState(HGE_HIDEMOUSE, false);

	if(hge->System_Initiate()) {
		srand(time(NULL));
		Initialize();
		BuildLevel();
		hge->System_Start();
	} else {	
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	hge->System_Shutdown();
	hge->Release();

	return 0;
}
