#include "main.h"

GtkWidget *mainWindow;
pbarData progBar;
threadHandle cnvThread;
GThread* pConvertThread;

static GtkTreeModel* fillData(void)
{
	GtkListStore* store = gtk_list_store_new(COLUMN_SIZE, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter iter;

	FILE* dbFile = fopen("data/psx.db", "rt");

	if(!dbFile) {
		createError("Game Database not found!");
		return;
	}

	char codeBuffer[256];
	char nameBuffer[256];
	int bPos = 0;
	char curChar = 0;

	while(1) {
		bPos = 0;
		curChar = fgetc(dbFile);
		if(feof(dbFile))
			break;
		while(curChar != ';') {
			codeBuffer[bPos++] = curChar;
			curChar = fgetc(dbFile);
		}
		codeBuffer[bPos] = 0;
		bPos = 0;
		curChar = fgetc(dbFile);
		while(curChar != '\n') {
			nameBuffer[bPos++] = curChar;
			curChar = fgetc(dbFile);
		}
		nameBuffer[bPos] = 0;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, COLUMN_GAMECODE, codeBuffer, COLUMN_GAMENAME, nameBuffer, -1);
	}

	fclose(dbFile);

	return GTK_TREE_MODEL(store);
}

void listFillGames(void)
{
	GtkTreeView* gameList = GTK_TREE_VIEW(lookup_widget(mainWindow, "gameList"));

	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(gameList, 0, "Code", renderer, "text", COLUMN_GAMECODE, NULL);
	gtk_tree_view_insert_column_with_attributes(gameList, -1, "Name", renderer, "text", COLUMN_GAMENAME, NULL);
	gtk_tree_view_set_search_column(gameList, COLUMN_GAMENAME);

	GtkTreeModel* model = fillData();
	gtk_tree_view_set_model(gameList, model);
	g_object_unref(model);
}

void handleThread(gpointer data)
{
	pThreadHandle trHandle = (pThreadHandle)data;
	gtk_widget_set_sensitive(lookup_widget(mainWindow, "btnGenerate"), FALSE);
	progBar.isJob = TRUE;
	char *msg = convert(trHandle->input, trHandle->output, trHandle->title, trHandle->code, trHandle->complevel);
	if(msg) {
		createError(msg);
	}
	progBar.isJob = FALSE;
	gtk_widget_set_sensitive(lookup_widget(mainWindow, "btnGenerate"), TRUE);
}

int main (int argc, char *argv[])
{
	gtk_set_locale();
	gtk_init(&argc, &argv);

	g_thread_init(NULL);
	gdk_threads_init();

	mainWindow = create_mainWindow();
	gtk_widget_show(mainWindow);

	g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(mainWindow), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

	listFillGames();

	progBar.isJob = FALSE;
	progBar.pBar = GTK_PROGRESS_BAR(lookup_widget(mainWindow, "cnvProgress"));
	progBar.timer = g_timeout_add(100, updateBar, &progBar);

	GtkFileFilter* isofilter = gtk_file_filter_new();
	gtk_file_filter_add_pattern (isofilter, "*.iso");
	gtk_file_filter_add_pattern (isofilter, "*.bin");
	gtk_file_filter_set_name (isofilter, "*.iso,*.bin");
	GtkFileChooserButton *cdImg = GTK_FILE_CHOOSER_BUTTON(lookup_widget(mainWindow, "cdImg"));
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(cdImg), isofilter);

	gtk_main();
	return 0;
}
