
#ifndef __SIMPLEDIT_PREF_DIALOG_H__
#define __SIMPLEDIT_PREF_DIALOG_H__

#include "simplEdit_main.h"

#define SIMPLEDIT_PREF_DIALOG_TYPE (simpledit_pref_dialog_get_type ())
G_DECLARE_FINAL_TYPE (SimpleditPrefDialog, simpledit_pref_dialog, SIMPLEDIT, PREF_DIALOG, GtkDialog)

SimpleditPrefDialog * simpledit_pref_dialog_new(SimpleditAppWindow * pWindow);

#endif /* __SIMPLEDIT_PREF_DIALOG_H__ */
