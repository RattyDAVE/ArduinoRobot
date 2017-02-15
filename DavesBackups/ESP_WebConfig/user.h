#ifndef USER_H
#define USER_H

typedef struct _UserConfig {
	// put your user data here
} UserConfig;

extern void userInit();
extern void userLoop();
extern void userInitConfig();
extern void userLoadConfig(JsonObject&);
extern void userSaveConfig(JsonObject&);
#endif