#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "interface.h"
#include "support.h"
#include "callbacks.h"

#ifndef __MAIN_H__
#define __MAIN_H__

enum {
	COLUMN_GAMECODE,
	COLUMN_GAMENAME,
	COLUMN_SIZE,
};

typedef struct 
{
	char input[256];
	char output[256]; 
	char title[256];
	char code[256];
	int complevel;
		
} threadHandle, *pThreadHandle;
		
extern GtkWidget *mainWindow;
extern pbarData progBar;
extern GThread* pConvertThread;
extern threadHandle cnvThread;

void handleThread(gpointer data);

#endif
