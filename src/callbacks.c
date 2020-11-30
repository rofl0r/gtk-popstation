#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"
#include "convert.h"

void copyKeysBin(const char* dst, const char* src)
{
	int inF, ouF;
	char line[512];
	int bytes;
		
	if((inF = open("data/KEYS.BIN", O_RDONLY, 0755)) == -1) {
		createError("Error opening KEYS.BIN");
		return;
	}
		
	strcpy(line, dst);
	strcat(line, "/KEYS.BIN");
	if((ouF = open(line, O_WRONLY | O_CREAT), 0755) == -1) {
		createError("Error creating KEYS.BIN");
		return;
	}
		
	while((bytes = read(inF, line, sizeof(line))) > 0)
		write(ouF, line, bytes);
		
	close(inF);
	close(ouF);
}

void createError(const char* message)
{
	GtkWidget* error;
	error = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message);
	gtk_dialog_run(GTK_DIALOG(error));
	gtk_widget_destroy(error);
}

gboolean updateBar(gpointer pData)
{
	ppBarData cpBar = (ppBarData)pData;
	if(cpBar->isJob)
		gtk_progress_bar_pulse(cpBar->pBar);
	else
		gtk_progress_bar_set_fraction(cpBar->pBar, 0.0);
}

void on_btnGenerate_activate(GtkButton *button, gpointer user_data)
{
	GtkSpinButton* compRate = GTK_SPIN_BUTTON(lookup_widget(mainWindow, "compRate"));
	GtkTreeView* gameList = GTK_TREE_VIEW(lookup_widget(mainWindow, "gameList"));
	GtkTreeSelection* listSelection = gtk_tree_view_get_selection(gameList);
	GtkTreeModel* lModel;
	GtkTreeIter lIter;
		
	char* gameCode;
    char* gameName;
	int compressionRate = gtk_spin_button_get_value_as_int(compRate);
	char* isoName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(lookup_widget(mainWindow, "cdImg")));
	char* outputDirName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(lookup_widget(mainWindow, "outputDir")));
		
	if(!isoName) {
		createError("Error: No CD Image selected!");
		return;
	}
		
	if(gtk_tree_selection_get_selected(listSelection, &lModel, &lIter))
		gtk_tree_model_get(lModel, &lIter, COLUMN_GAMECODE, &gameCode, COLUMN_GAMENAME, &gameName, -1);
	else {
		createError("Error: No game selected!");
		return;
	}
		
	strcpy(cnvThread.input, isoName);
	strcpy(cnvThread.output, outputDirName);
	strcat(cnvThread.output, "/");
	strcat(cnvThread.output, gameCode);
	mkdir(cnvThread.output, 0755);
	copyKeysBin(cnvThread.output, "data/KEYS.BIN");
	strcat(cnvThread.output, "/EBOOT.PBP");
	strcpy(cnvThread.title, gameName);
	strcpy(cnvThread.code, gameCode);
	cnvThread.complevel = compressionRate;
		
	GError *err1 = NULL;
	if((pConvertThread = g_thread_create((GThreadFunc)handleThread, (void*)&cnvThread, TRUE, &err1)) == NULL) {
		createError("Thread creation failed!");
		g_error_free(err1);
	}
		
	g_free(gameCode);
	g_free(gameName);
}

void on_exitBtn_clicked(GtkButton *button, gpointer user_data)
{
	gtk_main_quit();
}
