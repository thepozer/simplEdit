AM_CFLAGS = $(SIMPLEDIT_CFLAGS)
AM_CPPFLAGS = -DLOCALEDIR=\""$(localedir)"\" -rdynamic
bin_PROGRAMS = simplEdit

simplEdit_LDADD = $(SIMPLEDIT_LIBS) 
simplEdit_SOURCES = simplEdit_main.c \
simplEdit_app.c \
simplEdit_window.c \
simplEdit_content.c \
simplEdit_pref_dialog.c \
simplEdit_search_dialog.c \
simplEdit.gresources.h \
simplEdit.gresources.c


simplEdit.gresources.c: simplEdit.gresources.xml simplEdit.SimpleditAppWindow.glade simplEdit.SimpleditPrefDialog.glade simplEdit.SimpleditSearchDialog.glade
	$(AM_V_GEN) glib-compile-resources --generate-source $<

simplEdit.gresources.h: simplEdit.gresources.xml simplEdit.SimpleditAppWindow.glade simplEdit.SimpleditPrefDialog.glade simplEdit.SimpleditSearchDialog.glade
	$(AM_V_GEN) glib-compile-resources --generate-header $<

gsettings_SCHEMAS = net.thepozer.simpledit.gschema.xml
EXTRA_DIST = $(gsettings_SCHEMAS)

@GSETTINGS_RULES@
