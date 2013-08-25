#include <hge.h>
#include <hgerect.h>
#include <algorithm>

using namespace std;

const int ARRAY_SIZE = 50;
const int EVENT_ARRAY_SIZE = 100;
const int GAME_SIZE = 400;
const int SPAWN_SIZE = 150;
const int SPAWN_BORDER = 125;

struct BoxEvent {
	float x1;
	float x2;
	float y;
	bool isTop;
	int pIndex;
};

struct NegativeBox {
	hgeQuad box;
	bool active;
};

struct CompareBoxEvents {
	bool operator() (const BoxEvent & lhs, const BoxEvent & rhs) { return lhs.y < rhs.y; }
};

HGE *hge = 0;

bool canClick = false;

DWORD positiveColor = 0xFF009496;
DWORD negativeColor = 0xFF000000;
DWORD playerColor = 0xFF00FF00;

hgeQuad pBoxes[ARRAY_SIZE];
NegativeBox nBoxes[ARRAY_SIZE];
hgeQuad playBoxes[ARRAY_SIZE];

BoxEvent bEvents[EVENT_ARRAY_SIZE];

int numPBox = 0;
int numNBox = 0;
int numPlayBox = 0;
int numBEvents = 0;

// Initialize Game for initial load
void Initialize() {
	canClick = false;
	numPBox = 0;
	numNBox = 0;
	numPlayBox = 0;
	numBEvents = 0;
	
	for(int i = 0; i < ARRAY_SIZE; i++) {
		for(int j = 0; j < 4; j++) {
			pBoxes[i].v[j].x = 0;
			pBoxes[i].v[j].y = 0;
			pBoxes[i].v[j].col = positiveColor;
			
			nBoxes[i].box.v[j].x = 0;
			nBoxes[i].box.v[j].y = 0;
			nBoxes[i].box.v[j].col = negativeColor;
			
			playBoxes[i].v[j].x = 0;
			playBoxes[i].v[j].y = 0;
			playBoxes[i].v[j].col = playerColor;
		}
		
		pBoxes[i].tex = 0;
		pBoxes[i].blend = BLEND_DEFAULT;
		
		nBoxes[i].box.tex = 0;
		nBoxes[i].box.blend = BLEND_DEFAULT;
		
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
	numBEvents = 0;
	
	for(int i = 0; i < ARRAY_SIZE; i++) {
		for(int j = 0; j < 4; j++) {
			pBoxes[i].v[j].x = 0;
			pBoxes[i].v[j].y = 0;
			
			nBoxes[i].box.v[j].x = 0;
			nBoxes[i].box.v[j].y = 0;
			nBoxes[i].active = false;
			
			playBoxes[i].v[j].x = 0;
			playBoxes[i].v[j].y = 0;
		}
	}
}

void GeneratePositiveBox() {
	int originX = hge->Random_Int(SPAWN_BORDER, SPAWN_BORDER + SPAWN_SIZE);
	int originY = hge->Random_Int(SPAWN_BORDER, SPAWN_BORDER + SPAWN_SIZE);
	
	for(int i = 0; i < 4; i++) {
		pBoxes[numPBox].v[i].x = originX;
		pBoxes[numPBox].v[i].y = originY;
	}
	
	numPBox++;
}

void GrowPositiveBoxes() {
	int growth = hge->Random_Int(10, 20);
	
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
	int numBoxes = hge->Random_Int(5, 8);
	while(numBoxes > 0) {
		GeneratePositiveBox();
		GrowPositiveBoxes();
		numBoxes--;
	}
}

void BuildBoxEvents() {
	for(int i = 0; i < numPBox; i++) {
		bEvents[numBEvents].x1 = pBoxes[i].v[0].x;
		bEvents[numBEvents].x2 = pBoxes[i].v[1].x;
		bEvents[numBEvents].y = pBoxes[i].v[0].y;
		bEvents[numBEvents].pIndex = i;
		bEvents[numBEvents].isTop = true;
		
		bEvents[numBEvents + 1].x1 = pBoxes[i].v[0].x;
		bEvents[numBEvents + 1].x2 = pBoxes[i].v[1].x;
		bEvents[numBEvents + 1].y = pBoxes[i].v[2].y;
		bEvents[numBEvents + 1].pIndex = i;
		bEvents[numBEvents + 1].isTop = false;
		
		numBEvents += 2;
	}
	
	std::sort(bEvents, bEvents + numBEvents, CompareBoxEvents());
}

bool CheckBoxAndEvent(NegativeBox* box, BoxEvent* event) {
	//TODO Return whether the box intersects with the event
	return false;
}

bool CheckPoint(float px, float py, float rx1, float ry1, float rx2, float ry2, float rx3, float ry3, float rx4, float ry4) {
	//TODO Return whether the point at px-py is contained within the rectangle
	return false;
}

void BuildNegativeBoxes() {
	float curY = 0;
	int curEvent = 0;
	nBoxes[0].active = true;
	numNBox = 1;
	int newNumNBox = 1;
	
	while(curY < GAME_SIZE && curEvent < numBEvents) {
		// Bring active boxes down to the next event
		for(int n = 0; n < numNBox; n++) {
			if(nBoxes[n].active) {
				nBoxes[n].box.v[2].y = bEvents[curEvent].y;
				nBoxes[n].box.v[3].y = bEvents[curEvent].y;
			}
		}
		
		if(bEvents[curEvent].isTop) {
			// Collided with the top of a positive box
			for(int n = 0; n < numNBox; n++) {
				if(nBoxes[n].active && CheckBoxAndEvent(&nBoxes[n], &bEvents[curEvent])) {
					bool createLeft = true;
					bool createRight = true;
					nBoxes[n].active = false;
					
					// Check Left Vertex
					for(int j = 0; j < numPBox && createLeft; j++) {
						if(j != bEvents[curEvent].pIndex &&
							CheckPoint(
								bEvents[curEvent.x1], bEvents[curEvent.y],
								pBoxes[j].v[0].x, pBoxes[j].v[0].y,
								pBoxes[j].v[1].x, pBoxes[j].v[1].y,
								pBoxes[j].v[2].x, pBoxes[j].v[2].y,
								pBoxes[j].v[3].x, pBoxes[j].v[3].y,)
						) {
							createLeft = false;
						}
					}
					
					// Check Right Vertex
					for(int j = 0; j < numPBox && createRight; j++) {
						if(j != bEvents[curEvent].pIndex &&
							CheckPoint(
								bEvents[curEvent.x2], bEvents[curEvent.y],
								pBoxes[j].v[0].x, pBoxes[j].v[0].y,
								pBoxes[j].v[1].x, pBoxes[j].v[1].y,
								pBoxes[j].v[2].x, pBoxes[j].v[2].y,
								pBoxes[j].v[3].x, pBoxes[j].v[3].y,)
						) {
							createRight = false;
						}
					}
					
					// Create a negative box on the left
					if(createLeft) {
						nBoxes[newNumNBox].box.v[0].x = nBoxes[n].box.v[0].x;
						nBoxes[newNumNBox].box.v[0].y = curY;
						
						nBoxes[newNumNBox].box.v[1].x = bEvents[curEvent].x1;
						nBoxes[newNumNBox].box.v[1].y = curY;
						
						nBoxes[newNumNBox].box.v[2].x = bEvents[curEvent].x1;
						nBoxes[newNumNBox].box.v[2].y = curY;
						
						nBoxes[newNumNBox].box.v[3].x = nBoxes[n].box.v[0].x;
						nBoxes[newNumNBox].box.v[3].y = curY;
						
						nBoxes[newNumNBox].active = true;
						
						newNumNBox++;
					}
					
					// Create a negative box on the right
					if(createRight) {
						nBoxes[newNumNBox].box.v[0].x = bEvents[curEvent].x2;
						nBoxes[newNumNBox].box.v[0].y = curY;
						
						nBoxes[newNumNBox].box.v[1].x = nBoxes[n].box.v[1].x;
						nBoxes[newNumNBox].box.v[1].y = curY;
						
						nBoxes[newNumNBox].box.v[2].x = nBoxes[n].box.v[1].x;
						nBoxes[newNumNBox].box.v[2].y = curY;
						
						nBoxes[newNumNBox].box.v[3].x = bEvents[curEvent].x2;
						nBoxes[newNumNBox].box.v[3].y = curY;
						
						nBoxes[newNumNBox].active = true;
						
						newNumNBox++;
					}
				}
			}
		} else {
			// Collided with the bottom of a positive box
		}
		
		numNBox = newNumNBox;
		curY = bEvents[curEvent].y;
		curEvent++;
	}
}

void BuildLevel() {
	Initialize();
	BuildPositiveBoxes();
	BuildBoxEvents();
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
		hge->Random_Seed();
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
