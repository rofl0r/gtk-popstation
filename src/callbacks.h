#include <gtk/gtk.h>

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

typedef struct 
{
	GtkProgressBar *pBar;
	int timer;
	gboolean isJob;
		
} pbarData, *ppBarData;

	/* Generate button is pressed */
void on_btnGenerate_activate(GtkButton *button, gpointer user_data);
	
	/* Exit button is pressed */
void on_exitBtn_clicked(GtkButton *button, gpointer user_data);

	/* Progressbar Callback */
gboolean updateBar(gpointer pData);
	
	/* Error */
void createError(const char* message);

#endif

void
on_searchEdit_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);
