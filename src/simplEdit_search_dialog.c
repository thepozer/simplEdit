#include "simplEdit_window.h"
#include "simplEdit_search_dialog.h"

struct _SimpleditSearchDialog {
	GtkDialog parent;
		
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

G_DEFINE_TYPE(SimpleditSearchDialog, simpledit_search_dialog, GTK_TYPE_DIALOG);

static void simpledit_search_dialog_init (SimpleditSearchDialog *pDialog) {
	gtk_widget_init_template(GTK_WIDGET(pDialog));
}

static void simpledit_search_dialog_class_init (SimpleditSearchDialogClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditSearchDialog.glade");
/*	
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
*/
}

SimpleditSearchDialog * simpledit_search_dialog_new (SimpleditAppWindow *pWindow) {
	SimpleditSearchDialog * pDialog = g_object_new(SIMPLEDIT_SEARCH_DIALOG_TYPE, "transient-for", pWindow, NULL);
	
	return pDialog;
}

void smpldt_searchdlg_clbk_close (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWin = GTK_WINDOW(user_data);

	gtk_window_close(pWin);
}

