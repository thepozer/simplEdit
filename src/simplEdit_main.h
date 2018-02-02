
#ifndef __SMPLDT_MAIN_H__
#define __SMPLDT_MAIN_H__

#include <gtk/gtk.h>

#include <gtksourceview/gtksource.h>

typedef struct {
	GtkBuilder *builder;
	gpointer user_data;
} SGlobalData;

#ifndef __SIMPLEDIT_MAIN_PART__

extern GtkApplication * gpApp;
extern GtkBuilder * gpBuilder;

#endif /* __SIMPLEDIT_MAIN_PART__ */

#endif /* __SMPLDT_MAIN_H__ */
