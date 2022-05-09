#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h> // gcc -o main main.c `pkg-config --libs --cflags gtk+-3.0`

GtkWidget *notebook;

typedef struct
{
	char *label;
	int sub_items;
	char sub_menu[6][15];
}MuButton;

MuButton menuList[] = {
	{"File", 6, {"New", "Open", "Save", "Save as", "Close", "Quit"}},
	{"Edit", 4, {"Cut", "Copy", "Paste", "Delete"}},
	{"View", 4, {"Full Screen", "Larger Text", "Smaller Text", "Normal Size"}},
	{"Search", 4, {"Find", "Find Next", "Find Previous", "Replace"}},
	{"Tools", 1, {"Check Spelling"}},
	{"Documents", 2, {"Close All", "Save All"}},
	{"Help", 2, {"Contents", "About"}}
};

const int mLimit = 7;

typedef struct
{
	char *file_name;
	char *label;
	char *toolTip;
}TlButton;

TlButton toolList[] = {
	{"new.png", "New", "Create New Document"},
	{"open.png", "Open", "Open File"},
	{"save.png", "Save", "Save Current Tab"},
	{"separator", "0", "0"},
	{"undo.png", "Undo", "Undo Last Action"},
	{"redo.png", "Redo", "Redo Last Undone Action"},
	{"separator", "0", "0"},
	{"cut.png", "Cut", "Cut Selection"},
	{"copy.png", "Copy", "Copy Selection"},
	{"paste.png", "Paste", "Paste From Clipboard"},
	{"separator", "0", "0"},
	{"search.png", "Search", "Search For Text"},
	{"searchreplace.png", "Search Replace", "Search For And Replace Text"}
};

int Tlimit = sizeof(toolList) / sizeof(TlButton);

void close_window();
void make_window();
void make_menu(GtkWidget*);
void button_click(GtkWidget*, gpointer);
void make_toolbar(GtkWidget*);
void make_notebook(GtkWidget*);
void close_tab(GtkWidget*, gpointer);
void add_tab(char*);
void open_file(char*);
void open_file_dialog();

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	make_window();

	gtk_main();
	return 0;
}

void close_window()
{
	gtk_main_quit();
}

void make_window()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
	gtk_window_set_title(GTK_WINDOW(window), "My text app");
	g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

	gtk_container_add(GTK_CONTAINER(window), vbox);

	make_menu(vbox);
	make_toolbar(vbox);
	make_notebook(vbox);

	gtk_widget_show_all(window);
}

void make_menu(GtkWidget *vbox)
{
	GtkWidget *menuBar = gtk_menu_bar_new();

	int index;
	for(index = 0; index < mLimit; index++)
	{
		GtkWidget *item = gtk_menu_item_new_with_label(menuList[index].label);
		GtkWidget *item_menu = gtk_menu_new();

		gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), item);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), item_menu);

		int loop, limit = menuList[index].sub_items;
		for(loop = 0; loop < limit; loop++)
		{
			GtkWidget *subItem;
			subItem = gtk_menu_item_new_with_label(menuList[index].sub_menu[loop]);
			g_signal_connect(
				GTK_WIDGET(subItem), 
				"activate", 
				G_CALLBACK(button_click), 
				menuList[index].sub_menu[loop]
			);
			gtk_menu_shell_append(GTK_MENU_SHELL(item_menu), subItem);
		}
	}

	gtk_container_add(GTK_CONTAINER(vbox), menuBar);
}

void button_click(GtkWidget *button, gpointer data)
{
	char *btn = (char*)data;

	if(strcmp(btn, "New") == 0)
		add_tab("new_tab");

	if(strcmp(btn, "Open") == 0)
		open_file_dialog();
}

void make_toolbar(GtkWidget *vbox)
{
	GtkWidget *toolbar = gtk_toolbar_new();

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_MENU);

	GtkToolItem *item;
	int index;

	for(index = 0; index < Tlimit; index++)
	{
		if(strcmp(toolList[index].file_name, "separator") == 0)
		{
			item = gtk_separator_tool_item_new();
		}
		else
		{
			GtkWidget *icon = gtk_image_new_from_file(toolList[index].file_name);
			item = gtk_tool_button_new(NULL, NULL);
			gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(item), icon);
			gtk_tool_item_set_tooltip_text(item, toolList[index].toolTip);
			g_signal_connect(
				GTK_TOOL_BUTTON(item),
				"clicked",
				G_CALLBACK(button_click),
				toolList[index].label
			);
		}

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
	}

	gtk_container_add(GTK_CONTAINER(vbox), toolbar);
}

void make_notebook(GtkWidget *vbox)
{
	notebook = gtk_notebook_new();

	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
	add_tab("untitled");
}

void close_tab(GtkWidget *button, gpointer data)
{
	int pg_num = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), data);

	gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), pg_num);
}

void add_tab(char *name)
{
	GtkWidget *textview = gtk_text_view_new();
	GtkWidget *text = gtk_label_new(name);
	GtkWidget *label = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *icon = gtk_image_new_from_file("close.png");
	GtkWidget *button = gtk_button_new();
	GtkWidget *scrollwindow = gtk_scrolled_window_new(NULL, NULL);

	gtk_button_set_image(GTK_BUTTON(button), icon);
	gtk_widget_set_tooltip_text(button, "Close tab");
	gtk_box_pack_start(GTK_BOX(label), text, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(label), button, FALSE, FALSE, 0);
//	g_signal_connect(GTK_WIDGET(button), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_container_add(GTK_CONTAINER(scrollwindow), textview);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrollwindow, label);

	g_signal_connect(GTK_WIDGET(button), "clicked", G_CALLBACK(close_tab), scrollwindow);

	gtk_widget_show_all(label);
	gtk_widget_show_all(scrollwindow);
}

void open_file(char *file_address)
{
	g_print("%s", file_address);
}

void open_file_dialog()
{
	GtkWidget *open_dialog;
	int res;

	open_dialog = gtk_file_chooser_dialog_new(
		"Open File",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		"Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	res = gtk_dialog_run(GTK_DIALOG(open_dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *file_address;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(open_dialog);
		file_address = gtk_file_chooser_get_filename(chooser);
		open_file(file_address);
		free(file_address);
	}

	gtk_widget_destroy(open_dialog);
}



