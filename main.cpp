#include <hge.h>
#include <hgerect.h>
#include <algorithm>

using namespace std;

const int ARRAY_SIZE = 50;
const int EVENT_ARRAY_SIZE = 100;
const int GAME_SIZE = 400;
const int SPAWN_SIZE = 150;
const int SPAWN_BORDER = 125;
const float GROW_SPEED = 20;

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

float negativeArea = 0;
float gameScore = 0;

bool gameWon = false;

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

bool CheckBoxAndEvent(NegativeBox& nBox, BoxEvent& event) {
	if(nBox.box.v[2].y == event.y) {
		return (nBox.box.v[3].x <= event.x1 && nBox.box.v[2].x >= event.x1) ||
			(nBox.box.v[3].x <= event.x2 && nBox.box.v[2].x >= event.x2);
	}

	return false;
}

bool CheckPoint(float px, float py, float leftX, float rightX, float topY, float bottomY) {
	return px >= leftX && px <= rightX && py >= topY && py <= bottomY;
}

bool CheckLine(float lx1, float lx2, float ly, float leftX, float rightX, float topY, float bottomY) {
	if(ly > topY && ly < bottomY) {
		return lx1 <= rightX && lx2 >= leftX;
	}
	
	return false;
}

bool CheckQuad(hgeQuad& q1, hgeQuad& q2) {
	hgeRect r1(q1.v[0].x, q1.v[0].y, q1.v[2].x, q1.v[2].y);
	hgeRect r2(q2.v[0].x, q2.v[0].y, q2.v[2].x, q2.v[2].y);
	
	return r1.Intersect(&r2);
}

void BuildNegativeBoxes() {
	float curY = 0;
	int curEvent = 0;
	nBoxes[0].active = true;
	nBoxes[0].box.v[1].x = GAME_SIZE;
	nBoxes[0].box.v[2].x = GAME_SIZE;
	numNBox = 1;
	int newNumNBox = 1;
	
	while(curY < GAME_SIZE && curEvent < numBEvents) {
		// Bring active boxes down to the next event
		curY = bEvents[curEvent].y;
		for(int n = 0; n < numNBox; n++) {
			if(nBoxes[n].active) {
				nBoxes[n].box.v[2].y = bEvents[curEvent].y;
				nBoxes[n].box.v[3].y = bEvents[curEvent].y;
			}
		}
		
		if(bEvents[curEvent].isTop) {
			// Collided with the top of a positive box
			for(int n = 0; n < numNBox; n++) {
				if(nBoxes[n].active && CheckBoxAndEvent(nBoxes[n], bEvents[curEvent])) {
					bool createLeft = true;
					bool createRight = true;
					nBoxes[n].active = false;
					
					// Check Left Vertex
					createLeft = nBoxes[n].box.v[0].x <= bEvents[curEvent].x1;
					for(int j = 0; j < numPBox && createLeft; j++) {
						if(j != bEvents[curEvent].pIndex &&
							CheckPoint(
								bEvents[curEvent].x1, bEvents[curEvent].y,
								pBoxes[j].v[0].x, pBoxes[j].v[1].x,
								pBoxes[j].v[0].y, pBoxes[j].v[2].y)
						) {
							createLeft = false;
						}
					}
					
					// Check Right Vertex
					createRight = nBoxes[n].box.v[1].x >= bEvents[curEvent].x2;
					for(int j = 0; j < numPBox && createRight; j++) {
						if(j != bEvents[curEvent].pIndex &&
							CheckPoint(
								bEvents[curEvent].x2, bEvents[curEvent].y,
								pBoxes[j].v[0].x, pBoxes[j].v[1].x,
								pBoxes[j].v[0].y, pBoxes[j].v[2].y)
						) {
							createRight = false;
						}
					}
					
					// Create a negative box on the left
					if(createLeft) {
						nBoxes[newNumNBox].box.v[0].x = nBoxes[n].box.v[0].x;
						nBoxes[newNumNBox].box.v[1].x = bEvents[curEvent].x1;
						nBoxes[newNumNBox].box.v[2].x = bEvents[curEvent].x1;
						nBoxes[newNumNBox].box.v[3].x = nBoxes[n].box.v[0].x;
						
						for(int n = 0; n < 4; n++) {
							nBoxes[newNumNBox].box.v[n].y = curY;
						}
						
						nBoxes[newNumNBox].active = true;
						
						newNumNBox++;
					}
					
					// Create a negative box on the right
					if(createRight) {
						nBoxes[newNumNBox].box.v[0].x = bEvents[curEvent].x2;
						nBoxes[newNumNBox].box.v[1].x = nBoxes[n].box.v[1].x;
						nBoxes[newNumNBox].box.v[2].x = nBoxes[n].box.v[1].x;
						nBoxes[newNumNBox].box.v[3].x = bEvents[curEvent].x2;
						
						for(int n = 0; n < 4; n++) {
							nBoxes[newNumNBox].box.v[n].y = curY;
						}
						
						nBoxes[newNumNBox].active = true;
						
						newNumNBox++;
					}
				}
			}
		} else {
			// Collided with the bottom of a positive box
			// Terminate Active boxes and determine boundaries of event
			for(int n = 0; n < numNBox; n++) {
				if(nBoxes[n].active) {
					nBoxes[n].active = false;
				}
			}
			
			float leftBoxX = -1;
			float rightBoxX = -1;
			bool collides;
			bool boxCreated = false;
			for(float x = 0; x <= GAME_SIZE; x++) {
				collides = false;
				
				for(int p = 0; p < numPBox; p++) {
					if(CheckPoint(x, curY + 1, pBoxes[p].v[0].x, pBoxes[p].v[1].x, pBoxes[p].v[0].y, pBoxes[p].v[2].y)) {
						collides = true;
					}
				}
				
				if(leftBoxX == -1 && !collides) {
					leftBoxX = x;
				}
				
				if(x == GAME_SIZE || (leftBoxX > -1 && rightBoxX == -1 && collides)) {
					rightBoxX = x;
				}
				
				if(leftBoxX > -1 && rightBoxX > -1) {
					nBoxes[newNumNBox].box.v[0].x = leftBoxX;
					nBoxes[newNumNBox].box.v[1].x = rightBoxX;
					nBoxes[newNumNBox].box.v[2].x = rightBoxX;
					nBoxes[newNumNBox].box.v[3].x = leftBoxX;
					
					for(int n = 0; n < 4; n++) {
						nBoxes[newNumNBox].box.v[n].y = curY;
					}
					
					nBoxes[newNumNBox].active = true;
					
					newNumNBox++;
					leftBoxX = -1;
					rightBoxX = -1;
					boxCreated = true;
				}
			}
			
			// There weren't any boxes on the line, just build a new big one
			if(!boxCreated) {
				nBoxes[newNumNBox].box.v[0].x = 0;
				nBoxes[newNumNBox].box.v[1].x = GAME_SIZE;
				nBoxes[newNumNBox].box.v[2].x = GAME_SIZE;
				nBoxes[newNumNBox].box.v[3].x = 0;
				
				for(int n = 0; n < 4; n++) {
					nBoxes[newNumNBox].box.v[n].y = curY;
				}
				
				nBoxes[newNumNBox].active = true;
				
				newNumNBox++;
			}
		}
		
		numNBox = newNumNBox;
		curEvent++;
	}
	
	// Extend any remaining active boxes to the bottom
	for(int n = 0; n < numNBox; n++) {
		if(nBoxes[n].active) {
			nBoxes[n].box.v[2].y = GAME_SIZE;
			nBoxes[n].box.v[3].y = GAME_SIZE;
			nBoxes[n].active = false;
		}
	}
}

void CalculateNegativeArea() {
	negativeArea = 0;
	
	for(int n = 0; n < numNBox; n++) {
		float width = nBoxes[n].box.v[1].x - nBoxes[n].box.v[0].x;
		float height = nBoxes[n].box.v[2].y - nBoxes[n].box.v[0].y;
		
		negativeArea += width * height;
	}
}

void BuildLevel() {
	Initialize();
	BuildPositiveBoxes();
	BuildBoxEvents();
	BuildNegativeBoxes();
	CalculateNegativeArea();
	canClick = true;
}

void ProcessClick() {
	if(canClick && hge->Input_KeyDown(HGEK_LBUTTON)) {
		float x;
		float y;
		hge->Input_GetMousePos(&x, &y);
		bool spaceFree = true;
		
		for(int n = 0; n < numNBox; n++) {
			if(CheckPoint(x, y, nBoxes[n].box.v[0].x, nBoxes[n].box.v[1].x, nBoxes[n].box.v[0].y, nBoxes[n].box.v[2].y)) {
				spaceFree = false;
			}
		}
		
		if(spaceFree) {
			for(int i = 0; i < 4; i++) {
				playBoxes[numPlayBox].v[i].x = x;
				playBoxes[numPlayBox].v[i].y = y;
			}
			
			numPlayBox++;
		}
	}
}

void UpdatePlayerBoxes() {
	float amount = GROW_SPEED * hge->Timer_GetDelta();
	
	for(int pl = 0; pl < numPlayBox; pl++) {
		playBoxes[pl].v[0].x -= amount;
		playBoxes[pl].v[0].y -= amount;
		
		playBoxes[pl].v[1].x += amount;
		playBoxes[pl].v[1].y -= amount;
		
		playBoxes[pl].v[2].x += amount;
		playBoxes[pl].v[2].y += amount;
		
		playBoxes[pl].v[3].x -= amount;
		playBoxes[pl].v[3].y += amount;
	}
}

void CheckPlayerCollision() {
	for(int pl = 0; pl < numPlayBox; pl++) {
		for(int n = 0; n < numNBox; n++) {
			if(CheckQuad(playBoxes[pl], nBoxes[n].box)) {
				gameWon = true;
			}
		}
	}
	
	if(gameWon) {
		int coveredArea = 0;
		float totalArea = GAME_SIZE * GAME_SIZE;
		float positiveArea = totalArea - negativeArea;
		
		for(int x = 0; x < GAME_SIZE; x++) {
			for(int y = 0; y < GAME_SIZE; y++) {
				for(int n = 0; n < numPlayBox; n++) {
					if(!CheckPoint(x, y, playBoxes[n].v[0].x, playBoxes[n].v[1].x, playBoxes[n].v[0].y, playBoxes[n].v[2].y)){
						coveredArea++;
					}
				}
			}	
		}
		
		gameScore = positiveArea / coveredArea * 100;
	}
}

void ProcessGameWinningClick() {
	if(hge->Input_KeyDown(HGEK_LBUTTON)) {
		gameWon = false;
		BuildLevel();
	}
}

bool Update() {
	if (hge->Input_GetKeyState(HGEK_ESCAPE)){
		return true;
	}
	
	if(gameWon) {
		ProcessGameWinningClick();
	} else {
		ProcessClick();
		UpdatePlayerBoxes();
		CheckPlayerCollision();
	}
	
	return false;
}

void RenderPositiveBoxes() {
	for(int i = 0; i < numPBox; i++) {
		hge->Gfx_RenderQuad(&pBoxes[i]);
	}
}

void RenderPlayerBoxes() {
	for(int i = 0; i < numPlayBox; i++) {
		hge->Gfx_RenderQuad(&playBoxes[i]);
	}
}

void RenderScore() {
	//draw gameScore
	
	
	
	//TODO
}

bool Render() {
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	
	RenderPositiveBoxes();
	RenderPlayerBoxes();
	
	if(gameWon) {
		RenderScore();
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
