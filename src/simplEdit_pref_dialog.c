#include "simplEdit_window.h"
#include "simplEdit_pref_dialog.h"

struct _SimpleditPrefDialog
{
	GtkDialog parent;
	
	GSettings * pSettings;
	
	GtkWidget * checkLineNumbers;
};

G_DEFINE_TYPE(SimpleditPrefDialog, simpledit_pref_dialog, GTK_TYPE_DIALOG);

static void simpledit_pref_dialog_init (SimpleditPrefDialog *pDialog) {
	gtk_widget_init_template(GTK_WIDGET(pDialog));
	
	//pDialog->pSettings = g_settings_new ("net.thepozer.simpledit");
	
	//g_settings_bind (pDialog->pSettings, "show-line-numbers", pDialog->checkLineNumbers, "active", G_SETTINGS_BIND_DEFAULT);
	
}

static void simpledit_pref_dialog_dispose (GObject * pObject) {
	SimpleditPrefDialog *pDialog = SIMPLEDIT_PREF_DIALOG(pObject);
	
	//g_clear_object (&pDialog->pSettings);

	G_OBJECT_CLASS(simpledit_pref_dialog_parent_class)->dispose(pObject);
}

static void simpledit_pref_dialog_class_init (SimpleditPrefDialogClass *pClass) {
	G_OBJECT_CLASS(pClass)->dispose = simpledit_pref_dialog_dispose;
	
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditPrefDialog.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkLineNumbers);
}

SimpleditPrefDialog * simpledit_pref_dialog_new (SimpleditAppWindow *pWindow) {
	SimpleditPrefDialog * pDialog = g_object_new(SIMPLEDIT_PREF_DIALOG_TYPE, "transient-for", pWindow, "use-header-bar", TRUE, NULL);
	
	return pDialog;
}

