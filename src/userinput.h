// keyboard input set to default for now 
// (W up S down A left D right)
// W=ascend, S=descend, A=decelerate, D=accelerate
//
// credit to: https://www.raylib.com/examples/core/loader.html?name=core_input_actions

typedef enum ActionType {
	NO_ACTION = 0,
	DEV_ASCEND,
	DEV_DESCEND,
	DEV_ACCEL,
	DEV_DECEL,
	DEV_FINE,
	DEV_COARSE,
	DEV_RESET,
	PITCH_UP,
	PITCH_DOWN,
	YAW_LEFT,
	YAW_RIGHT,
	ROLL_LEFT,
	ROLL_RIGHT,
	THROTTLE_UP,
	THROTTLE_DOWN,
	MAX_ACTION
} ActionType;

typedef struct ActionInput {
	int key;
	int button;
} ActionInput;

static int gamepadIndex = 0;
static ActionInput actionInputs[MAX_ACTION] = { 0 };

static bool IsActionPressed(int action);
static bool IsActionReleased(int action);
static bool IsActionDown(int action);

static void SetActionsDefault(void);

// tried to put this all in a separate C file instead of a header in the main file but it epic failed
static bool IsActionPressed(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyPressed(actionInputs[action].key));
	return result;
}

static bool IsActionReleased(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyReleased(actionInputs[action].key));
	return result;
}

static bool IsActionDown(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyDown(actionInputs[action].key));
	return result;
}

static void SetActionsDefault() {
	actionInputs[DEV_ASCEND].key = KEY_UP;
	actionInputs[DEV_DESCEND].key = KEY_DOWN;
	actionInputs[DEV_ACCEL].key = KEY_RIGHT;
	actionInputs[DEV_DECEL].key = KEY_LEFT;
	actionInputs[DEV_FINE].key = KEY_Z;
	actionInputs[DEV_COARSE].key = KEY_X;
	actionInputs[DEV_RESET].key = KEY_TAB;
	actionInputs[PITCH_UP].key = KEY_S;
	actionInputs[PITCH_DOWN].key = KEY_W;
	actionInputs[YAW_LEFT].key = KEY_A;
	actionInputs[YAW_RIGHT].key = KEY_D;
	actionInputs[ROLL_LEFT].key = KEY_Q;
	actionInputs[ROLL_RIGHT].key = KEY_E;
	actionInputs[THROTTLE_UP].key = KEY_LEFT_SHIFT;
	actionInputs[THROTTLE_DOWN].key = KEY_LEFT_CONTROL;
}


