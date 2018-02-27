#ifndef __SIMPLEDIT_APP_H__
#define __SIMPLEDIT_APP_H__

#include "simplEdit_main.h"


#define SIMPLEDIT_APP_TYPE (simpledit_app_get_type ())
G_DECLARE_FINAL_TYPE (SimpleditApp, simpledit_app, SIMPLEDIT, APP, GtkApplication)

SimpleditApp * simpledit_app_new (void);

GSettings * simpledit_app_get_settings (SimpleditApp * pApp);

#endif /* __SIMPLEDIT_APP_H__ */
