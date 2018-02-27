#include "simplEdit_window.h"
#include "simplEdit_pref_dialog.h"

struct _SimpleditPrefDialog {
	GtkDialog parent;
	
	GSettings * pSettings;
	
	GtkWidget * checkLineNumbers;
	GtkWidget * checkLineMarks;
	GtkWidget * checkHighlightCurrentLine;
	GtkWidget * btnFontSelect;
	GtkWidget * checkRightMargin;
	GtkWidget * spinPositionRightMargin;

	GtkWidget * checkAutoIndent;
	GtkWidget * checkIndentOnTab;
	GtkWidget * checkSmartBackspace;
	GtkWidget * checkInsertSpacesInsteadTabs;
	GtkWidget * spinIndentWidth;
	GtkWidget * spinTabWidth;
};

G_DEFINE_TYPE(SimpleditPrefDialog, simpledit_pref_dialog, GTK_TYPE_DIALOG);

static void simpledit_pref_dialog_init (SimpleditPrefDialog *pDialog) {
	gtk_widget_init_template(GTK_WIDGET(pDialog));
}

static void simpledit_pref_dialog_class_init (SimpleditPrefDialogClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditPrefDialog.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkLineNumbers);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkLineMarks);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkHighlightCurrentLine);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, btnFontSelect);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkRightMargin);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, spinPositionRightMargin);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkAutoIndent);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkIndentOnTab);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkSmartBackspace);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, checkInsertSpacesInsteadTabs);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, spinIndentWidth);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditPrefDialog, spinTabWidth);
}

SimpleditPrefDialog * simpledit_pref_dialog_new (SimpleditAppWindow *pWindow, GSettings * pSettings) {
	SimpleditPrefDialog * pDialog = g_object_new(SIMPLEDIT_PREF_DIALOG_TYPE, "transient-for", pWindow, "use-header-bar", TRUE, NULL);
	
	pDialog->pSettings = pSettings;
	
	g_settings_bind (pDialog->pSettings, "show-line-numbers", pDialog->checkLineNumbers, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "show-line-marks", pDialog->checkLineMarks, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "highlight-current-line", pDialog->checkHighlightCurrentLine, "active", G_SETTINGS_BIND_DEFAULT);
	//g_settings_bind (pDialog->pSettings, "font", pDialog->btnFontSelect, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "show-right-margin", pDialog->checkRightMargin, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "right-margin-position", pDialog->spinPositionRightMargin, "value", G_SETTINGS_BIND_DEFAULT);
	
	g_settings_bind (pDialog->pSettings, "auto-indent", pDialog->checkAutoIndent, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "indent-on-tab", pDialog->checkIndentOnTab, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "smart-backspace", pDialog->checkSmartBackspace, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "insert-spaces-instead-of-tabs", pDialog->checkInsertSpacesInsteadTabs, "active", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "indent-width", pDialog->spinIndentWidth, "value", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (pDialog->pSettings, "tab-width", pDialog->spinTabWidth, "value", G_SETTINGS_BIND_DEFAULT);
	
	return pDialog;
}

void smpldt_prefdlg_clbk_close (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWin = GTK_WINDOW(user_data);

	gtk_window_close(pWin);
}

