typedef struct {
	unsigned int screenW;
	unsigned int screenH;
	unsigned char currentScreen;
} ScreenState;

typedef struct {
	float azimuth;
	float elevation;
	float zoom;
} ViewState;

void updateViewState(ViewState *view);
void userKeyInput(ScreenState *scr, ViewState *outside, ViewState *orbital);

void drawMenu();
void drawOutside(ViewState *view, char *printViewBuf);
void drawOrbital(ViewState *view, char *printViewBuf);
void drawPanel();
void drawAero();
void drawProp();
