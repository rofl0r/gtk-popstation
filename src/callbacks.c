#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"
#include "convert.h"

void copyKeysBin(const char* dst, const char* src)
{
	int ouF;
	char line[512];
	(void) src;
	snprintf(line, sizeof line, "%s/KEYS.BIN", dst);

	if((ouF = open(line, O_WRONLY | O_CREAT), 0755) == -1) {
		createError("Error creating KEYS.BIN");
		return;
	}
	write(ouF, "\x4D\x42\x5C\xCE\xA9\x3A\xF2\xC2\x0C\xFB\x1F\xF1\x20\x0B\xBE\x22", 16);
	close(ouF);
}

void createError(const char* message)
{
	GtkWidget* error;
	//error = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message);
	error = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message);
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

void
on_searchEdit_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkTreeView* gameList = GTK_TREE_VIEW(lookup_widget(mainWindow, "gameList"));
	GtkTreeModel* lModel = gtk_tree_view_get_model (gameList);
	GtkTreeIter lIter;
	if(!gtk_tree_model_get_iter_first (lModel, &lIter)) return;
	char* searchtext = gtk_editable_get_chars(editable, 0, -1);
	size_t l = strlen(searchtext);
	int donext = 1;
	do {
		char* gameCode;
		char* gameName;
		gtk_tree_model_get(lModel, &lIter, COLUMN_GAMECODE, &gameCode, COLUMN_GAMENAME, &gameName, -1);
		if(!strncasecmp(searchtext, gameCode, l) || !strncasecmp(searchtext, gameName, l)) {
			GtkTreeSelection *sel = gtk_tree_view_get_selection(gameList);
			gtk_tree_selection_select_iter(sel, &lIter);
			GtkTreePath *path = gtk_tree_model_get_path(lModel, &lIter);
			gtk_tree_view_scroll_to_cell(gameList, path, NULL, FALSE, 0.0, 0.0);
			gtk_tree_path_free(path);
			donext = 0;
		}
		g_free(gameCode);
		g_free(gameName);
	} while(donext && gtk_tree_model_iter_next (lModel, &lIter));
	g_free(searchtext);
}


void
on_cdImg_file_set                      (GtkFileChooserButton *filechooserbutton,
                                        gpointer         user_data)
{
	GtkFileChooserButton *outdirbtn = GTK_FILE_CHOOSER_BUTTON(lookup_widget(mainWindow, "outputDir"));
	if(!gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(outdirbtn))) {
		char *isoname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(lookup_widget(mainWindow, "cdImg")));
		if(!isoname) return;
		char path[PATH_MAX], *p;
		p = strrchr(isoname, '/');
		if(p) {
			memcpy(path, isoname, p-isoname);
			path[p-isoname] = 0;
		} else {
			path[0] = '.';
			path[1] = 0;
		}
		strcat(path, "/out");
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(outdirbtn), path);
	}
}

