/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2013 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2013 - Daniel De Matteis
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <crtdefs.h>
#include <tchar.h>
#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>

#include "rmenu_xui.h"

#include "utils/file_browser.h"

#include "../../console/rarch_console.h"

#include "../../gfx/gfx_common.h"
#include "../../gfx/gfx_context.h"

#include "../../message.h"

#include "../../general.h"

enum {
   MENU_XUI_ITEM_LOAD_STATE = 0,
   MENU_XUI_ITEM_SAVE_STATE,
   MENU_XUI_ITEM_ASPECT_RATIO,
   MENU_XUI_ITEM_ORIENTATION,
   MENU_XUI_ITEM_RESIZE_MODE,
   MENU_XUI_ITEM_FRAME_ADVANCE,
   MENU_XUI_ITEM_RESET,
   MENU_XUI_ITEM_RETURN_TO_GAME,
   MENU_XUI_ITEM_QUIT_RARCH,
};

enum
{
   S_LBL_ASPECT_RATIO = 0,
   S_LBL_RARCH_VERSION,
   S_LBL_ROTATION,
   S_LBL_LOAD_STATE_SLOT,
   S_LBL_SAVE_STATE_SLOT,
   S_LBL_REWIND_GRANULARITY,
};

class CRetroArch : public CXuiModule
{
   public:
      HXUIOBJ hMainScene;
      HXUIOBJ hControlsMenu;
      HXUIOBJ hFileBrowser;
      HXUIOBJ hCoreBrowser;
      HXUIOBJ hShaderBrowser;
      HXUIOBJ hQuickMenu;
      HXUIOBJ hRetroArchSettings;
   protected:
      virtual HRESULT RegisterXuiClasses();
      virtual HRESULT UnregisterXuiClasses();
};

class CRetroArchMain: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_filebrowser;
      HXUIOBJ m_quick_menu;
      HXUIOBJ m_controls;
      HXUIOBJ m_settings;
      HXUIOBJ m_change_libretro_core;
      HXUIOBJ m_quit;
      HXUIOBJ m_title;
      HXUIOBJ m_core;
      HXUIOBJ m_logoimage;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchMain, L"RetroArchMain", XUI_CLASS_SCENE)
};

class CRetroArchFileBrowser: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_back;
      HXUIOBJ m_dir_game;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchFileBrowser, L"RetroArchFileBrowser", XUI_CLASS_SCENE)
};

class CRetroArchCoreBrowser: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_back;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchCoreBrowser, L"RetroArchCoreBrowser", XUI_CLASS_SCENE)
};

class CRetroArchShaderBrowser: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_back;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchShaderBrowser, L"RetroArchShaderBrowser", XUI_CLASS_SCENE)
};

class CRetroArchQuickMenu: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_quickmenulist;
      HXUIOBJ m_back;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );
      HRESULT OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled);

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_CONTROL_NAVIGATE( OnControlNavigate )
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchQuickMenu, L"RetroArchQuickMenu", XUI_CLASS_SCENE)
};

class CRetroArchSettings: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_settingslist;
      HXUIOBJ m_back;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );
      HRESULT OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled);

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_CONTROL_NAVIGATE( OnControlNavigate )
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchSettings, L"RetroArchSettings", XUI_CLASS_SCENE)
};

class CRetroArchControls: public CXuiSceneImpl
{
   protected:
      HXUIOBJ m_controlslist;
      HXUIOBJ m_back;
      HXUIOBJ m_controlnoslider;
   public:
      HRESULT OnInit( XUIMessageInit* pInitData, int & bHandled );
      HRESULT OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled );
      HRESULT OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled);

      XUI_BEGIN_MSG_MAP()
         XUI_ON_XM_INIT( OnInit)
         XUI_ON_XM_CONTROL_NAVIGATE( OnControlNavigate )
         XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
         XUI_END_MSG_MAP();

      XUI_IMPLEMENT_CLASS(CRetroArchControls, L"RetroArchControls", XUI_CLASS_SCENE)
};

CRetroArch app;
HXUIOBJ m_list;
HXUIOBJ m_list_path;
HXUIOBJ hCur;

wchar_t strw_buffer[PATH_MAX];
char str_buffer[PATH_MAX];

static int process_input_ret = 0;
static unsigned input_loop = 0;

enum
{
   RMENU_DEVICE_NAV_UP = 0,
   RMENU_DEVICE_NAV_DOWN,
   RMENU_DEVICE_NAV_LEFT,
   RMENU_DEVICE_NAV_RIGHT,
   RMENU_DEVICE_NAV_UP_ANALOG_L,
   RMENU_DEVICE_NAV_DOWN_ANALOG_L,
   RMENU_DEVICE_NAV_LEFT_ANALOG_L,
   RMENU_DEVICE_NAV_RIGHT_ANALOG_L,
   RMENU_DEVICE_NAV_UP_ANALOG_R,
   RMENU_DEVICE_NAV_DOWN_ANALOG_R,
   RMENU_DEVICE_NAV_LEFT_ANALOG_R,
   RMENU_DEVICE_NAV_RIGHT_ANALOG_R,
   RMENU_DEVICE_NAV_B,
   RMENU_DEVICE_NAV_A,
   RMENU_DEVICE_NAV_X,
   RMENU_DEVICE_NAV_Y,
   RMENU_DEVICE_NAV_START,
   RMENU_DEVICE_NAV_SELECT,
   RMENU_DEVICE_NAV_L1,
   RMENU_DEVICE_NAV_R1,
   RMENU_DEVICE_NAV_L2,
   RMENU_DEVICE_NAV_R2,
   RMENU_DEVICE_NAV_L3,
   RMENU_DEVICE_NAV_R3,
   RMENU_DEVICE_NAV_LAST
};

/* Register custom classes */
HRESULT CRetroArch::RegisterXuiClasses (void)
{
   CRetroArchMain::Register();
   CRetroArchFileBrowser::Register();
   CRetroArchCoreBrowser::Register();
   CRetroArchShaderBrowser::Register();
   CRetroArchQuickMenu::Register();
   CRetroArchControls::Register();
   CRetroArchSettings::Register();

   return 0;
}

/* Unregister custom classes */
HRESULT CRetroArch::UnregisterXuiClasses (void)
{
   CRetroArchMain::Unregister();
   CRetroArchCoreBrowser::Unregister();
   CRetroArchShaderBrowser::Unregister();
   CRetroArchFileBrowser::Unregister();
   CRetroArchQuickMenu::Register();
   CRetroArchControls::Register();
   CRetroArchSettings::Unregister();

   return 0;
}

static void menu_settings_create_menu_item_label_w(wchar_t *strwbuf, unsigned setting, size_t size)
{
   char str[PATH_MAX];

   switch (setting)
   {
      case S_LBL_ASPECT_RATIO:
         snprintf(str, size, "Aspect Ratio: %s", aspectratio_lut[g_settings.video.aspect_ratio_idx].name);
         break;
      case S_LBL_RARCH_VERSION:
         snprintf(str, size, "RetroArch %s", PACKAGE_VERSION);
         break;
      case S_LBL_ROTATION:
         snprintf(str, size, "Rotation: %s", rotation_lut[g_extern.console.screen.orientation]);
         break;
      case S_LBL_LOAD_STATE_SLOT:
         snprintf(str, size, "Load State #%d", g_extern.state_slot);
         break;
      case S_LBL_SAVE_STATE_SLOT:
         snprintf(str, size, "Save State #%d", g_extern.state_slot);
         break;
      case S_LBL_REWIND_GRANULARITY:
         snprintf(str, size, "Rewind granularity: %d", g_settings.rewind_granularity);
         break;
   }

   mbstowcs(strwbuf, str, size / sizeof(wchar_t));
}

void filebrowser_fetch_directory_entries(uint64_t action)
{
   filebrowser_update(rgui->browser, action, rgui->browser->current_dir.extensions); 

   mbstowcs(strw_buffer, rgui->browser->current_dir.directory_path, sizeof(strw_buffer) / sizeof(wchar_t));
   XuiTextElementSetText(m_list_path, strw_buffer);

   XuiListDeleteItems(m_list, 0, XuiListGetItemCount(m_list));
   XuiListInsertItems(m_list, 0, rgui->browser->list->size);

   for(unsigned i = 0; i < rgui->browser->list->size; i++)
   {
      char fname_tmp[256];
      fill_pathname_base(fname_tmp, rgui->browser->list->elems[i].data, sizeof(fname_tmp));
      mbstowcs(strw_buffer, fname_tmp, sizeof(strw_buffer) / sizeof(wchar_t));
      XuiListSetText(m_list, i, strw_buffer);
   }
}

HRESULT CRetroArchFileBrowser::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiRomList", &m_list);
   GetChildById(L"XuiBackButton1", &m_back);
   GetChildById(L"XuiTxtRomPath", &m_list_path);
   GetChildById(L"XuiBtnGameDir", &m_dir_game);

   filebrowser_set_root_and_ext(rgui->browser, g_extern.system.valid_extensions,
      default_paths.filebrowser_startup_dir);

   uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
   filebrowser_fetch_directory_entries(action);

   return 0;
}

HRESULT CRetroArchFileBrowser::OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled )
{
   char path[PATH_MAX];
   process_input_ret = 0;

   if(hObjPressed == m_list)
   {
      int index = XuiListGetCurSel(m_list, NULL);
      wcstombs(str_buffer, (const wchar_t *)XuiListGetText(m_list, index), sizeof(str_buffer));
      if (path_file_exists(rgui->browser->list->elems[index].data))
      {
         snprintf(path, sizeof(path), "%s\\%s", rgui->browser->current_dir.directory_path, str_buffer);
         strlcpy(g_extern.fullpath, path, sizeof(g_extern.fullpath));
         g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME);
      }
      else if(rgui->browser->list->elems[index].attr.b)
      {
         snprintf(path, sizeof(path), "%s\\%s", rgui->browser->current_dir.directory_path, str_buffer);
         uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
         filebrowser_set_root_and_ext(rgui->browser, g_extern.system.valid_extensions, path);
         filebrowser_fetch_directory_entries(action);
      }
   }
   else if (hObjPressed == m_dir_game)
   {
      filebrowser_set_root_and_ext(rgui->browser, g_extern.system.valid_extensions,
            g_settings.rgui_browser_directory);
      uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
      filebrowser_fetch_directory_entries(action);
   }

   bHandled = TRUE;

   return 0;
}

static void set_dpad_emulation_label(unsigned port, char *str, size_t sizeof_str)
{
   switch(g_settings.input.dpad_emulation[port])
   {
      case ANALOG_DPAD_NONE:
         snprintf(str, sizeof_str, "D-Pad Emulation: None");
         break;
      case ANALOG_DPAD_LSTICK:
         snprintf(str, sizeof_str, "D-Pad Emulation: Left Stick");
         break;
      case ANALOG_DPAD_RSTICK:
         snprintf(str, sizeof_str, "D-Pad Emulation: Right Stick");
         break;
   }
}

HRESULT CRetroArchControls::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   unsigned i;
   int controlno;
   char buttons[RARCH_FIRST_META_KEY][128];

   GetChildById(L"XuiControlsList", &m_controlslist);
   GetChildById(L"XuiBackButton", &m_back);
   GetChildById(L"XuiControlNoSlider", &m_controlnoslider);

   XuiSliderSetValue(m_controlnoslider, 0);
   XuiSliderGetValue(m_controlnoslider, &controlno);

   for(i = 0; i < RARCH_FIRST_META_KEY; i++)
   {
      struct platform_bind key_label;
      strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
      key_label.joykey = g_settings.input.binds[controlno][i].joykey;

      if (driver.input->set_keybinds)
         driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));

      snprintf(buttons[i], sizeof(buttons[i]), "%s #%d: %s", 
            g_settings.input.binds[controlno][i].desc, controlno, key_label.desc);
      mbstowcs(strw_buffer, buttons[i], sizeof(strw_buffer) / sizeof(wchar_t));
      XuiListSetText(m_controlslist, i, strw_buffer);
   }

   set_dpad_emulation_label(controlno, buttons[0], sizeof(buttons[0]));
   mbstowcs(strw_buffer, buttons[0], sizeof(strw_buffer) / sizeof(wchar_t));
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DPAD_EMULATION, strw_buffer);
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DEFAULT_ALL, L"Reset all buttons to default");

   return 0;
}

HRESULT CRetroArchControls::OnControlNavigate(
      XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled)
{
   char button[128];
   char buttons[RARCH_FIRST_META_KEY][128];
   int controlno, i, current_index;

   current_index = XuiListGetCurSel(m_controlslist, NULL);
   XuiSliderGetValue(m_controlnoslider, &controlno);

   for(i = 0; i < RARCH_FIRST_META_KEY; i++)
   {
      struct platform_bind key_label;
      strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
      key_label.joykey = g_settings.input.binds[controlno][i].joykey;

      if (driver.input->set_keybinds)
         driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));

      snprintf(buttons[i], sizeof(buttons[i]), "%s #%d: %s", 
            g_settings.input.binds[controlno][i].desc, controlno, 
            key_label.desc);
      mbstowcs(strw_buffer, buttons[i], sizeof(strw_buffer) / sizeof(wchar_t));
      XuiListSetText(m_controlslist, i, strw_buffer);
   }

   switch(pControlNavigateData->nControlNavigate)
   {
      case XUI_CONTROL_NAVIGATE_LEFT:
         switch(current_index)
         {
            case SETTING_CONTROLS_DPAD_EMULATION:
               if (driver.input->set_keybinds)
               {
                  unsigned keybind_action = 0;

                  switch(g_settings.input.dpad_emulation[controlno])
                  {
                     case ANALOG_DPAD_NONE:
                        break;
                     case ANALOG_DPAD_LSTICK:
                        keybind_action = (1ULL << KEYBINDS_ACTION_SET_ANALOG_DPAD_NONE);
                        break;
                     case ANALOG_DPAD_RSTICK:
                        keybind_action = (1ULL << KEYBINDS_ACTION_SET_ANALOG_DPAD_LSTICK);
                        break;
                  }

                  if (keybind_action)
                     driver.input->set_keybinds(driver.input_data, g_settings.input.device[controlno], controlno, 0, keybind_action);
               }
               break;
            case SETTING_CONTROLS_DEFAULT_ALL:
               break;
            default:
               {
                  struct platform_bind key_label;
                  strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
                  key_label.joykey = g_settings.input.binds[controlno][current_index].joykey;

                  if (driver.input->set_keybinds)
                     driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));

                  if (driver.input->set_keybinds)
                     driver.input->set_keybinds(driver.input_data, g_settings.input.device[controlno],
                           controlno, current_index, (1ULL << KEYBINDS_ACTION_DECREMENT_BIND));

                  snprintf(button, sizeof(button), "%s #%d: %s",
                        g_settings.input.binds[controlno][current_index].desc, controlno, key_label.desc);
                  mbstowcs(strw_buffer, button, sizeof(strw_buffer) / sizeof(wchar_t));
                  XuiListSetText(m_controlslist, current_index, strw_buffer);
               }
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_RIGHT:
         switch(current_index)
         {
            case SETTING_CONTROLS_DPAD_EMULATION:
               if (driver.input->set_keybinds)
               {
                  unsigned keybind_action = 0;

                  switch(g_settings.input.dpad_emulation[controlno])
                  {
                     case ANALOG_DPAD_NONE:
                        keybind_action = (1ULL << KEYBINDS_ACTION_SET_ANALOG_DPAD_LSTICK);
                        break;
                     case ANALOG_DPAD_LSTICK:
                        keybind_action = (1ULL << KEYBINDS_ACTION_SET_ANALOG_DPAD_RSTICK);
                        break;
                     case ANALOG_DPAD_RSTICK:
                        break;
                  }

                  if (keybind_action)
                     driver.input->set_keybinds(driver.input_data, g_settings.input.device[controlno], controlno,
0, keybind_action);
               }
               break;
            case SETTING_CONTROLS_DEFAULT_ALL:
               break;
            default:
               {
                  struct platform_bind key_label;
                  strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
                  key_label.joykey = g_settings.input.binds[controlno][current_index].joykey;

                  if (driver.input->set_keybinds)
                     driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));
                  if (driver.input->set_keybinds)
                     driver.input->set_keybinds(driver.input_data, g_settings.input.device[controlno],
                           controlno, current_index, (1ULL << KEYBINDS_ACTION_INCREMENT_BIND));

                  snprintf(button, sizeof(button), "%s #%d: %s",
                        g_settings.input.binds[controlno][current_index].desc, controlno, 
                        key_label.desc);
                  mbstowcs(strw_buffer, button, sizeof(strw_buffer) / sizeof(wchar_t));
                  XuiListSetText(m_controlslist, current_index, strw_buffer);
               }
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_UP:
      case XUI_CONTROL_NAVIGATE_DOWN:
         break;
   }

   set_dpad_emulation_label(controlno, button, sizeof(button));

   mbstowcs(strw_buffer, button, sizeof(strw_buffer) / sizeof(wchar_t));
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DPAD_EMULATION, strw_buffer);
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DEFAULT_ALL, L"Reset all buttons to default");

   return 0;
}

HRESULT CRetroArchControls::OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled )
{
   int current_index, i, controlno;
   char buttons[RARCH_FIRST_META_KEY][128];
   XuiSliderGetValue(m_controlnoslider, &controlno);
   process_input_ret = 0;

   if ( hObjPressed == m_controlslist)
   {
      current_index = XuiListGetCurSel(m_controlslist, NULL);

      switch(current_index)
      {
         case SETTING_CONTROLS_DPAD_EMULATION:
            break;
         case SETTING_CONTROLS_DEFAULT_ALL:
            if (driver.input->set_keybinds)
               driver.input->set_keybinds(driver.input_data,
                     g_settings.input.device[controlno], controlno, 0,
                     (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS));

            for(i = 0; i < RARCH_FIRST_META_KEY; i++)
            {
               struct platform_bind key_label;
               strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
               key_label.joykey = g_settings.input.binds[controlno][i].joykey;

               if (driver.input->set_keybinds)
                  driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));

               snprintf(buttons[i], sizeof(buttons[i]), "%s #%d: %s", 
                     g_settings.input.binds[controlno][i].desc, controlno,  key_label.desc);
               mbstowcs(strw_buffer, buttons[i], sizeof(strw_buffer) / sizeof(wchar_t));
               XuiListSetText(m_controlslist, i, strw_buffer);
            }
            break;
         default:
            {
               if (driver.input->set_keybinds)
                  driver.input->set_keybinds(driver.input_data, g_settings.input.device[controlno],
                        controlno, current_index, (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND));
               
               struct platform_bind key_label;
               strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
               key_label.joykey = g_settings.input.binds[controlno][current_index].joykey;

               if (driver.input->set_keybinds)
                  driver.input->set_keybinds(&key_label, 0, 0, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));

               snprintf(buttons[current_index], sizeof(buttons[current_index]), "%s #%d: %s",
                     g_settings.input.binds[controlno][current_index].desc, controlno, 
                     key_label.desc);
               mbstowcs(strw_buffer, buttons[current_index], sizeof(strw_buffer) / sizeof(wchar_t));
               XuiListSetText(m_controlslist, current_index, strw_buffer);
            }
            break;
      }
   }

   set_dpad_emulation_label(controlno, buttons[current_index], sizeof(buttons[current_index]));

   mbstowcs(strw_buffer, buttons[current_index], sizeof(strw_buffer) / sizeof(wchar_t));
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DPAD_EMULATION, strw_buffer);
   XuiListSetText(m_controlslist, SETTING_CONTROLS_DEFAULT_ALL, L"Reset all buttons to default");

   bHandled = TRUE;
   return 0;
}

HRESULT CRetroArchSettings::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiSettingsList", &m_settingslist);
   GetChildById(L"XuiBackButton", &m_back);

   XuiListSetText(m_settingslist, SETTING_EMU_REWIND_ENABLED, g_settings.rewind_enable ? L"Rewind: ON" : L"Rewind: OFF");
   XuiListSetText(m_settingslist, SETTING_EMU_SHOW_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW)) ? L"Info messages: ON" : L"Info messages: OFF");
   XuiListSetText(m_settingslist, SETTING_EMU_SHOW_DEBUG_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW)) ? L"Debug Info messages: ON" : L"Debug Info messages: OFF");
   XuiListSetText(m_settingslist, SETTING_GAMMA_CORRECTION_ENABLED, g_extern.console.screen.gamma_correction ? L"Gamma correction: ON" : L"Gamma correction: OFF");
   XuiListSetText(m_settingslist, SETTING_HW_TEXTURE_FILTER, g_settings.video.smooth ? L"Hardware filtering shader #1: Linear interpolation" : L"Hardware filtering shader #1: Point filtering");
   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_REWIND_GRANULARITY, sizeof(strw_buffer));
   XuiListSetText(m_settingslist, SETTING_EMU_REWIND_GRANULARITY, strw_buffer);
   XuiListSetText(m_settingslist, SETTING_ENABLE_SRAM_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE)) ? L"SRAM Path Enable: ON" : L"SRAM Path Enable: OFF");
   XuiListSetText(m_settingslist, SETTING_ENABLE_STATE_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE)) ? L"Savestate Path Enable: ON" : L"Savestate Path Enable: OFF");

   return 0;
}

HRESULT CRetroArchSettings::OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled )
{
   int current_index;
   process_input_ret = 0;

   if ( hObjPressed == m_settingslist)
   {
      current_index = XuiListGetCurSel(m_settingslist, NULL);

      switch(current_index)
      {
         case SETTING_EMU_REWIND_ENABLED:
            settings_set(1ULL << S_REWIND);
            XuiListSetText(m_settingslist, SETTING_EMU_REWIND_ENABLED, g_settings.rewind_enable ? L"Rewind: ON" : L"Rewind: OFF");

            if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
               msg_queue_push(g_extern.msg_queue, "INFO - You need to restart RetroArch.", 1, 180);
            break;
	 case SETTING_EMU_REWIND_GRANULARITY:
	    g_settings.rewind_granularity++;

	    menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_REWIND_GRANULARITY, sizeof(strw_buffer));
	    XuiListSetText(m_settingslist, SETTING_EMU_REWIND_GRANULARITY, strw_buffer);
	    break;
     case SETTING_ENABLE_SRAM_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_SRAM_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE)) ? L"SRAM Path Enable: ON" : L"SRAM Path Enable: OFF");
        break;
     case SETTING_ENABLE_STATE_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_STATE_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE)) ? L"Savestate Path Enable: ON" : L"Savestate Path Enable: OFF");
        break;
         case SETTING_EMU_SHOW_INFO_MSG:
            if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
               g_extern.lifecycle_mode_state &= ~(1ULL << MODE_INFO_DRAW);
            else
               g_extern.lifecycle_mode_state |= (1ULL << MODE_INFO_DRAW);
            XuiListSetText(m_settingslist, SETTING_EMU_SHOW_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW)) ? L"Info messages: ON" : L"Info messages: OFF");
            break;
         case SETTING_EMU_SHOW_DEBUG_INFO_MSG:
            if (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW))
               g_extern.lifecycle_mode_state &= ~(1ULL << MODE_FPS_DRAW);
            else
               g_extern.lifecycle_mode_state |= (1ULL << MODE_FPS_DRAW);
            XuiListSetText(m_settingslist, SETTING_EMU_SHOW_DEBUG_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW)) ? L"Debug Info messages: ON" : L"Debug Info messages: OFF");
            break;
         case SETTING_GAMMA_CORRECTION_ENABLED:
            g_extern.console.screen.gamma_correction = g_extern.console.screen.gamma_correction ? 0 : 1;
            driver.video->restart();
            XuiListSetText(m_settingslist, SETTING_GAMMA_CORRECTION_ENABLED, g_extern.console.screen.gamma_correction ? L"Gamma correction: ON" : L"Gamma correction: OFF");
            break;
         case SETTING_HW_TEXTURE_FILTER:
            g_settings.video.smooth = !g_settings.video.smooth;
            XuiListSetText(m_settingslist, SETTING_HW_TEXTURE_FILTER, g_settings.video.smooth ? L"Hardware filtering shader #1: Linear interpolation" : L"Hardware filtering shader #1: Point filtering");
            break;
      }
   }

   bHandled = TRUE;
   return 0;
}

HRESULT CRetroArchSettings::OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled)
{
   int current_index;
   xdk_d3d_video_t *device_ptr = (xdk_d3d_video_t*)driver.video_data;

   current_index = XuiListGetCurSel(m_settingslist, NULL);

   switch(pControlNavigateData->nControlNavigate)
   {
      case XUI_CONTROL_NAVIGATE_LEFT:
         switch(current_index)
         {
            case SETTING_EMU_REWIND_ENABLED:
               settings_set(1ULL << S_REWIND);
               XuiListSetText(m_settingslist, SETTING_EMU_REWIND_ENABLED, g_settings.rewind_enable ? L"Rewind: ON" : L"Rewind: OFF");

               if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
                  msg_queue_push(g_extern.msg_queue, "INFO - You need to restart RetroArch.", 1, 180);
               break;
	    case SETTING_EMU_REWIND_GRANULARITY:
	       if (g_settings.rewind_granularity > 1)
		       g_settings.rewind_granularity--;

	       menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_REWIND_GRANULARITY, sizeof(strw_buffer));
	       XuiListSetText(m_settingslist, SETTING_EMU_REWIND_GRANULARITY, strw_buffer);
	       break;
     case SETTING_ENABLE_SRAM_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_SRAM_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE)) ? L"SRAM Path Enable: ON" : L"SRAM Path Enable: OFF");
        break;
     case SETTING_ENABLE_STATE_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_STATE_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE)) ? L"Savestate Path Enable: ON" : L"Savestate Path Enable: OFF");
        break;
            case SETTING_EMU_SHOW_INFO_MSG:
               if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
                  g_extern.lifecycle_mode_state &= ~(1ULL << MODE_INFO_DRAW);
               else
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_INFO_DRAW);
               XuiListSetText(m_settingslist, SETTING_EMU_SHOW_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW)) ? L"Info messages: ON" : L"Info messages: OFF");
               break;
            case SETTING_EMU_SHOW_DEBUG_INFO_MSG:
               if (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW))
                  g_extern.lifecycle_mode_state &= ~(1ULL << MODE_FPS_DRAW);
               else
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_FPS_DRAW);
               XuiListSetText(m_settingslist, SETTING_EMU_SHOW_DEBUG_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW)) ? L"Debug Info messages: ON" : L"Debug Info messages: OFF");
               break;
            case SETTING_GAMMA_CORRECTION_ENABLED:
               g_extern.console.screen.gamma_correction = g_extern.console.screen.gamma_correction ? 0 : 1;
               driver.video->restart();
               XuiListSetText(m_settingslist, SETTING_GAMMA_CORRECTION_ENABLED, g_extern.console.screen.gamma_correction ? L"Gamma correction: ON" : L"Gamma correction: OFF");
               break;
            case SETTING_HW_TEXTURE_FILTER:
               g_settings.video.smooth = !g_settings.video.smooth;
               XuiListSetText(m_settingslist, SETTING_HW_TEXTURE_FILTER, g_settings.video.smooth ? L"Hardware filtering shader #1: Linear interpolation" : L"Hardware filtering shader #1: Point filtering");
               break;
            default:
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_RIGHT:
         switch(current_index)
         {
            case SETTING_EMU_SHOW_INFO_MSG:
               if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
                  g_extern.lifecycle_mode_state &= ~(1ULL << MODE_INFO_DRAW);
               else
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_INFO_DRAW);
               XuiListSetText(m_settingslist, SETTING_EMU_SHOW_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW)) ? L"Info messages: ON" : L"Info messages: OFF");
               break;
            case SETTING_EMU_SHOW_DEBUG_INFO_MSG:
               if (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW))
                  g_extern.lifecycle_mode_state &= ~(1ULL << MODE_FPS_DRAW);
               else
                  g_extern.lifecycle_mode_state |= (1ULL << MODE_FPS_DRAW);
               XuiListSetText(m_settingslist, SETTING_EMU_SHOW_DEBUG_INFO_MSG, (g_extern.lifecycle_mode_state & (1ULL << MODE_FPS_DRAW)) ? L"Debug Info messages: ON" : L"Debug Info messages: OFF");
               break;
            case SETTING_GAMMA_CORRECTION_ENABLED:
               g_extern.console.screen.gamma_correction = g_extern.console.screen.gamma_correction ? 0 : 1;
               driver.video->restart();
               XuiListSetText(m_settingslist, SETTING_GAMMA_CORRECTION_ENABLED, g_extern.console.screen.gamma_correction ? L"Gamma correction: ON" : L"Gamma correction: OFF");
               break;
            case SETTING_EMU_REWIND_ENABLED:
               settings_set(1ULL << S_REWIND);
               XuiListSetText(m_settingslist, SETTING_EMU_REWIND_ENABLED, g_settings.rewind_enable ? L"Rewind: ON" : L"Rewind: OFF");

               if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
                  msg_queue_push(g_extern.msg_queue, "INFO - You need to restart RetroArch.", 1, 180);
               break;
	    case SETTING_EMU_REWIND_GRANULARITY:
	       g_settings.rewind_granularity++;

	       menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_REWIND_GRANULARITY, sizeof(strw_buffer));
	       XuiListSetText(m_settingslist, SETTING_EMU_REWIND_GRANULARITY, strw_buffer);
	       break;
     case SETTING_ENABLE_SRAM_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_SRAM_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_SRAM_DIR_ENABLE)) ? L"SRAM Path Enable: ON" : L"SRAM Path Enable: OFF");
        break;
     case SETTING_ENABLE_STATE_PATH:
        if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE))
           g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
        else
           g_extern.lifecycle_mode_state |= (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE);
	    XuiListSetText(m_settingslist, SETTING_ENABLE_STATE_PATH, (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME_STATE_DIR_ENABLE)) ? L"Savestate Path Enable: ON" : L"Savestate Path Enable: OFF");
        break;
            case SETTING_HW_TEXTURE_FILTER:
               g_settings.video.smooth = !g_settings.video.smooth;
               XuiListSetText(m_settingslist, SETTING_HW_TEXTURE_FILTER, g_settings.video.smooth ? L"Hardware filtering shader #1: Linear interpolation" : L"Hardware filtering shader #1: Point filtering");
               break;
            default:
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_UP:
      case XUI_CONTROL_NAVIGATE_DOWN:
         break;
   }

   bHandled = TRUE;

   switch(pControlNavigateData->nControlNavigate)
   {
      case XUI_CONTROL_NAVIGATE_LEFT:
      case XUI_CONTROL_NAVIGATE_RIGHT:
      case XUI_CONTROL_NAVIGATE_UP:
      case XUI_CONTROL_NAVIGATE_DOWN:
         pControlNavigateData->hObjDest = pControlNavigateData->hObjSource;
         break;
      default:
         break;
   }

   return 0;
}

HRESULT CRetroArchQuickMenu::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiQuickMenuList", &m_quickmenulist);
   GetChildById(L"XuiBackButton", &m_back);

   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ROTATION, sizeof(strw_buffer));
   XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ORIENTATION, strw_buffer);

   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ASPECT_RATIO, sizeof(strw_buffer));
   XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ASPECT_RATIO, strw_buffer);

   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_LOAD_STATE_SLOT, sizeof(strw_buffer));
   XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_LOAD_STATE, strw_buffer);

   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_SAVE_STATE_SLOT, sizeof(strw_buffer));
   XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_SAVE_STATE, strw_buffer);

   return 0;
}

HRESULT CRetroArchQuickMenu::OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled)
{
   bool aspectratio_changed = false;
   int current_index;

   current_index = XuiListGetCurSel(m_quickmenulist, NULL);

   switch(pControlNavigateData->nControlNavigate)
   {
      case XUI_CONTROL_NAVIGATE_LEFT:
         switch(current_index)
         {
            case MENU_XUI_ITEM_LOAD_STATE:
            case MENU_XUI_ITEM_SAVE_STATE:
               rarch_state_slot_decrease();
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_LOAD_STATE_SLOT, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_LOAD_STATE, strw_buffer);
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_SAVE_STATE_SLOT, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_SAVE_STATE, strw_buffer);
               break;
            case MENU_XUI_ITEM_ASPECT_RATIO:
               settings_set(1ULL << S_ASPECT_RATIO_DECREMENT);
               aspectratio_changed = true;
               break;
            case MENU_XUI_ITEM_ORIENTATION:
               settings_set(1ULL << S_ROTATION_DECREMENT);
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ROTATION, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ORIENTATION, strw_buffer);
               driver.video->set_rotation(driver.video_data, g_extern.console.screen.orientation);
               break;
            default:
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_RIGHT:
         switch(current_index)
         {
            case MENU_XUI_ITEM_LOAD_STATE:
            case MENU_XUI_ITEM_SAVE_STATE:
               rarch_state_slot_increase();
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_LOAD_STATE_SLOT, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_LOAD_STATE, strw_buffer);
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_SAVE_STATE_SLOT, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_SAVE_STATE, strw_buffer);
               break;
            case MENU_XUI_ITEM_ASPECT_RATIO:
               settings_set(1ULL << S_ASPECT_RATIO_INCREMENT);
               aspectratio_changed = true;
               break;
            case MENU_XUI_ITEM_ORIENTATION:
               settings_set(1ULL << S_ROTATION_INCREMENT);
               menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ROTATION, sizeof(strw_buffer));
               XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ORIENTATION, strw_buffer);
               driver.video->set_rotation(driver.video_data, g_extern.console.screen.orientation);
               break;
            default:
               break;
         }
         break;
      case XUI_CONTROL_NAVIGATE_UP:
      case XUI_CONTROL_NAVIGATE_DOWN:
         break;
   }

   if(aspectratio_changed)
   {
      if (driver.video_poke->set_aspect_ratio)
         driver.video_poke->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
      menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ASPECT_RATIO, sizeof(strw_buffer));
      XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ASPECT_RATIO, strw_buffer);
   }

   bHandled = TRUE;

   switch(pControlNavigateData->nControlNavigate)
   {
      case XUI_CONTROL_NAVIGATE_LEFT:
      case XUI_CONTROL_NAVIGATE_RIGHT:
      case XUI_CONTROL_NAVIGATE_UP:
      case XUI_CONTROL_NAVIGATE_DOWN:
         pControlNavigateData->hObjDest = pControlNavigateData->hObjSource;
         break;
      default:
         break;
   }

   return 0;
}

HRESULT CRetroArchQuickMenu::OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled )
{
   xdk_d3d_video_t *device_ptr = (xdk_d3d_video_t*)driver.video_data;
   int current_index = 0;
   process_input_ret = 0;

   if ( hObjPressed == m_quickmenulist)
   {
      current_index = XuiListGetCurSel(m_quickmenulist, NULL);

      switch(current_index)
      {
         case MENU_XUI_ITEM_LOAD_STATE:
            if (g_extern.main_is_init)
            {
               rarch_load_state();
               g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
               process_input_ret = -1;
            }
            break;
         case MENU_XUI_ITEM_SAVE_STATE:
            if (g_extern.main_is_init)
            {
               rarch_save_state();
               g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
               process_input_ret = -1;
            }
            break;
         case MENU_XUI_ITEM_ASPECT_RATIO:
            settings_set(1ULL << S_DEF_ASPECT_RATIO);
            if (driver.video_poke->set_aspect_ratio)
               driver.video_poke->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
            menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ASPECT_RATIO, sizeof(strw_buffer));
            XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ASPECT_RATIO, strw_buffer);
            break;
         case MENU_XUI_ITEM_ORIENTATION:
            settings_set(1ULL << S_DEF_ROTATION);
            menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ROTATION, sizeof(strw_buffer));
            XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ORIENTATION, strw_buffer);
            driver.video->set_rotation(driver.video_data, g_extern.console.screen.orientation);
            break;
         case MENU_XUI_ITEM_RESIZE_MODE:
            input_loop = INPUT_LOOP_RESIZE_MODE;
            g_settings.video.aspect_ratio_idx = ASPECT_RATIO_CUSTOM;
            menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_ASPECT_RATIO, sizeof(strw_buffer));
            XuiListSetText(m_quickmenulist, MENU_XUI_ITEM_ASPECT_RATIO, strw_buffer);

            if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
               msg_queue_push(g_extern.msg_queue, "INFO - Resize the screen by moving around the two analog sticks.\n", 1, 270);
            break;
         case MENU_XUI_ITEM_FRAME_ADVANCE:
            if (g_extern.main_is_init)
            {
               g_extern.lifecycle_state |= (1ULL << RARCH_FRAMEADVANCE);
               settings_set(1ULL << S_FRAME_ADVANCE);
               process_input_ret = -1;
            }
            break;
         case MENU_XUI_ITEM_RESET:
            if (g_extern.main_is_init)
            {
               rarch_game_reset();
               g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
               process_input_ret = -1;
            }
            break;
         case MENU_XUI_ITEM_RETURN_TO_GAME:
            if (g_extern.main_is_init)
            {
               g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
               process_input_ret = -1;
            }
            break;
         case MENU_XUI_ITEM_QUIT_RARCH:
            g_extern.lifecycle_mode_state &= ~(1ULL << MODE_GAME);
            g_extern.lifecycle_mode_state |= (1ULL << MODE_EXIT);
            process_input_ret = -1;
            break;
      }
   }

   bHandled = TRUE;

   return 0;
}

HRESULT CRetroArchShaderBrowser::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiRomList", &m_list);
   GetChildById(L"XuiBackButton1", &m_back);
   GetChildById(L"XuiTxtRomPath", &m_list_path);

   filebrowser_set_root_and_ext(rgui->browser, "cg", "game:\\media\\shaders");
   uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
   filebrowser_fetch_directory_entries(action);

   return 0;
}

HRESULT CRetroArchShaderBrowser::OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled )
{
   char path[PATH_MAX];
   process_input_ret = 0;

   if(hObjPressed == m_list)
   {
      int index = XuiListGetCurSel(m_list, NULL);
      if (path_file_exists(rgui->browser->list->elems[index].data))
         wcstombs(str_buffer, (const wchar_t *)XuiListGetText(m_list, index), sizeof(str_buffer));
      else if (rgui->browser->list->elems[index].attr.b)
      {
         wcstombs(str_buffer, (const wchar_t *)XuiListGetText(m_list, index), sizeof(str_buffer));
         snprintf(path, sizeof(path), "%s\\%s", rgui->browser->current_dir.directory_path, str_buffer);
         filebrowser_set_root_and_ext(rgui->browser, "cg", path);
         uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
         filebrowser_fetch_directory_entries(action);
      }
   }

   bHandled = TRUE;

   return 0;
}

HRESULT CRetroArchCoreBrowser::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiRomList", &m_list);
   GetChildById(L"XuiBackButton1", &m_back);
   GetChildById(L"XuiTxtRomPath", &m_list_path);

   filebrowser_set_root_and_ext(rgui->browser, "xex|XEX", "game:");
   uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
   filebrowser_fetch_directory_entries(action);

   return 0;
}

HRESULT CRetroArchCoreBrowser::OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled )
{
   char path[PATH_MAX];

   process_input_ret = 0;

   if(hObjPressed == m_list)
   {
      int index = XuiListGetCurSel(m_list, NULL);
      wcstombs(str_buffer, (const wchar_t *)XuiListGetText(m_list, index), sizeof(str_buffer));
      if(path_file_exists(rgui->browser->list->elems[index].data))
      {
         snprintf(g_extern.fullpath, sizeof(g_extern.fullpath), "%s\\%s", rgui->browser->current_dir.directory_path, str_buffer);
         g_extern.lifecycle_mode_state |= (1ULL << MODE_EXIT);
         g_extern.lifecycle_mode_state |= (1ULL << MODE_EXITSPAWN);
         process_input_ret = -1;
      }
      else if (rgui->browser->list->elems[index].attr.b)
      {
         snprintf(path, sizeof(path), "%s\\%s", rgui->browser->current_dir.directory_path, str_buffer);
         filebrowser_set_root_and_ext(rgui->browser, "xex|XEX", path);
         uint64_t action = (1ULL << RMENU_DEVICE_NAV_B);
         filebrowser_fetch_directory_entries(action);
      }
   }

   bHandled = TRUE;
   return 0;
}

HRESULT CRetroArchMain::OnInit(XUIMessageInit * pInitData, BOOL& bHandled)
{
   GetChildById(L"XuiLogo", &m_logoimage);
   GetChildById(L"XuiBtnRomBrowser", &m_filebrowser);
   GetChildById(L"XuiBtnSettings", &m_settings);
   GetChildById(L"XuiBtnQuickMenu", &m_quick_menu);
   GetChildById(L"XuiBtnControls", &m_controls);
   GetChildById(L"XuiBtnQuit", &m_quit);
   GetChildById(L"XuiTxtTitle", &m_title);
   GetChildById(L"XuiTxtCoreText", &m_core);
   GetChildById(L"XuiBtnLibretroCore", &m_change_libretro_core);

   mbstowcs(strw_buffer, g_extern.title_buf, sizeof(strw_buffer) / sizeof(wchar_t));
   XuiTextElementSetText(m_core, strw_buffer);
   menu_settings_create_menu_item_label_w(strw_buffer, S_LBL_RARCH_VERSION, sizeof(strw_buffer));
   XuiTextElementSetText(m_title, strw_buffer);

   return 0;
}

HRESULT CRetroArchMain::OnNotifyPress( HXUIOBJ hObjPressed,  int & bHandled )
{
   xdk_d3d_video_t *vid = (xdk_d3d_video_t*)driver.video_data;

   bool hdmenus_allowed = (g_extern.lifecycle_mode_state & (1ULL << MODE_MENU_HD));

   HRESULT hr;

   if ( hObjPressed == m_filebrowser )
   {
      input_loop = INPUT_LOOP_FILEBROWSER;
      hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_filebrowser.xur", NULL, &app.hFileBrowser);

      if (hr < 0)
         RARCH_ERR("Failed to load scene.\n");

      XuiSceneNavigateForward(hCur, false, app.hFileBrowser, XUSER_INDEX_FOCUS);
      hCur = app.hFileBrowser;
   }
   else if ( hObjPressed == m_quick_menu)
   {
      hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_quickmenu.xur", NULL, &app.hQuickMenu);

      if (hr < 0)
         RARCH_ERR("Failed to load scene.\n");

      XuiSceneNavigateForward(hCur, false, app.hQuickMenu, XUSER_INDEX_FOCUS);
      hCur = app.hQuickMenu;
   }
   else if ( hObjPressed == m_controls)
   {
      hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_controls.xur", NULL, &app.hControlsMenu);

      if (hr < 0)
         RARCH_ERR("Failed to load scene.\n");

      if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
         msg_queue_push(g_extern.msg_queue,
               "INFO - Press LEFT/RIGHT to change the controls, and press\n[RetroPad Start] to reset a button to default values.", 1, 180);

      XuiSceneNavigateForward(hCur, false, app.hControlsMenu, XUSER_INDEX_FOCUS);
      hCur = app.hControlsMenu;
   }
   else if ( hObjPressed == m_change_libretro_core )
   {
      hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_libretrocore_browser.xur", NULL, &app.hCoreBrowser);

      if (hr < 0)
         RARCH_ERR("Failed to load scene.\n");

      if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
         msg_queue_push(g_extern.msg_queue,
               "INFO - Select a Libretro core from the menu.", 1, 180);

      XuiSceneNavigateForward(hCur, false, app.hCoreBrowser, XUSER_INDEX_FOCUS);
      hCur = app.hCoreBrowser;
   }
   else if (hObjPressed == m_settings)
   {
      hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_settings.xur", NULL, &app.hRetroArchSettings);

      if (hr < 0)
         RARCH_ERR("Failed to load scene.\n");

      XuiSceneNavigateForward(hCur, false, app.hRetroArchSettings, XUSER_INDEX_FOCUS);
      hCur = app.hRetroArchSettings;
   }
   else if (hObjPressed == m_quit)
   {
      g_extern.lifecycle_mode_state &= ~(1ULL << MODE_GAME);
      g_extern.lifecycle_mode_state |= (1ULL << MODE_EXIT);
      process_input_ret = -1;
   }

   bHandled = TRUE;
   return 0;
}

rgui_handle_t *rgui_init (void)
{
   HRESULT hr;

   rgui_handle_t *rgui = (rgui_handle_t*)calloc(1, sizeof(*rgui));

   xdk_d3d_video_t *device_ptr = (xdk_d3d_video_t*)driver.video_data;

   bool hdmenus_allowed = (g_extern.lifecycle_mode_state & (1ULL << MODE_MENU_HD));

   if (hdmenus_allowed)
      RARCH_LOG("HD menus enabled.\n");

   D3DPRESENT_PARAMETERS d3dpp;
   video_info_t video_info = {0};

   video_info.vsync = g_settings.video.vsync;
   video_info.force_aspect = false;
   video_info.smooth = g_settings.video.smooth;
   video_info.input_scale = 2;
   video_info.fullscreen = true;
   video_info.rgb32 = false;

   xdk_d3d_generate_pp(&d3dpp, &video_info);

   hr = app.InitShared(device_ptr->d3d_render_device, &d3dpp, XuiPNGTextureLoader);

   if (hr != S_OK)
   {
      RARCH_ERR("Failed initializing XUI application.\n");
      return NULL;
   }

   /* Register font */
   TypefaceDescriptor typeface = {0};
   typeface.szTypeface = L"Arial Unicode MS";
   typeface.szLocator = L"file://game:/media/rarch.ttf";
   typeface.szReserved1 = NULL;
   
   hr = XuiRegisterTypeface( &typeface, TRUE );
   if (hr != S_OK)
   {
      RARCH_ERR("Failed to register default typeface.\n");
      return NULL;
   }

   hr = XuiLoadVisualFromBinary( L"file://game:/media/rarch_scene_skin.xur", NULL);
   if (hr != S_OK)
   {
      RARCH_ERR("Failed to load skin.\n");
      return NULL;
   }

   hr = XuiSceneCreate(hdmenus_allowed ? L"file://game:/media/hd/" : L"file://game:/media/sd/", L"rarch_main.xur", NULL, &app.hMainScene);
   if (hr != S_OK)
   {
      RARCH_ERR("Failed to create scene 'rarch_main.xur'.\n");
      return NULL;
   }

   hCur = app.hMainScene;
   hr = XuiSceneNavigateFirst(app.GetRootObj(), app.hMainScene, XUSER_INDEX_FOCUS);
   if (hr != S_OK)
   {
      RARCH_ERR("XuiSceneNavigateFirst failed.\n");
      return NULL;
   }

   if (driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_frame(driver.video_data, NULL,
            true, 0, 0, 1.0f);

   return rgui;
}

void rgui_free (rgui_handle_t *rgui)
{
   app.Uninit();
}

static void ingame_menu_resize (void)
{
   XINPUT_STATE state;

   XInputGetState(0, &state);

   if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || state.Gamepad.sThumbLX < -DEADZONE)
      g_extern.console.screen.viewports.custom_vp.x -= 1;
   else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || state.Gamepad.sThumbLX > DEADZONE)
      g_extern.console.screen.viewports.custom_vp.x += 1;

   if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP || state.Gamepad.sThumbLY > DEADZONE)
      g_extern.console.screen.viewports.custom_vp.y += 1;
   else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN || state.Gamepad.sThumbLY < -DEADZONE) 
      g_extern.console.screen.viewports.custom_vp.y -= 1;

   if (state.Gamepad.sThumbRX < -DEADZONE || state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
      g_extern.console.screen.viewports.custom_vp.width -= 1;
   else if (state.Gamepad.sThumbRX > DEADZONE || state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
      g_extern.console.screen.viewports.custom_vp.width += 1;

   if (state.Gamepad.sThumbRY > DEADZONE || state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
      g_extern.console.screen.viewports.custom_vp.height += 1;
   else if (state.Gamepad.sThumbRY < -DEADZONE || state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
      g_extern.console.screen.viewports.custom_vp.height -= 1;

   if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
   {
      g_extern.console.screen.viewports.custom_vp.x = 0;
      g_extern.console.screen.viewports.custom_vp.y = 0;
      g_extern.console.screen.viewports.custom_vp.width = 1280; //FIXME: hardcoded
      g_extern.console.screen.viewports.custom_vp.height = 720; //FIXME: hardcoded
   }
   if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
      input_loop = INPUT_LOOP_MENU;
}

bool menu_iterate(void)
{
   xdk_d3d_video_t *device_ptr = (xdk_d3d_video_t*)driver.video_data;

   if (g_extern.lifecycle_mode_state & (1ULL << MODE_MENU_PREINIT))
   {
      input_loop = INPUT_LOOP_MENU;
      g_extern.lifecycle_mode_state &= ~(1ULL << MODE_MENU_PREINIT);
   }

   XINPUT_STATE state;
   XInputGetState(0, &state);

   if (g_extern.lifecycle_mode_state & (1ULL << MODE_LOAD_GAME))
   {
      if (g_extern.lifecycle_mode_state & (1ULL << MODE_INFO_DRAW))
      {
         char tmp[PATH_MAX];
         char str[PATH_MAX];

         fill_pathname_base(tmp, g_extern.fullpath, sizeof(tmp));
         snprintf(str, sizeof(str), "INFO - Loading %s...", tmp);
         msg_queue_push(g_extern.msg_queue, str, 1, 1);
      }

      g_extern.lifecycle_mode_state |= (1ULL << MODE_INIT);
      g_extern.lifecycle_mode_state &= ~(1ULL << MODE_LOAD_GAME);
      process_input_ret = -1;
   }

   if (!(g_extern.frame_count < g_extern.delay_timer[0]))
   {
      bool rmenu_enable = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) 
            && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) && (g_extern.main_is_init));

      if (g_extern.lifecycle_mode_state & (1ULL << MODE_MENU))
         if (rmenu_enable)
         {
            g_extern.lifecycle_mode_state |= (1ULL << MODE_GAME);
            process_input_ret = -1;
         }
   }

   switch(input_loop)
   {
      case INPUT_LOOP_FILEBROWSER:
      case INPUT_LOOP_MENU:
         app.RunFrame(); /* Update XUI */
         if((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && hCur != app.hMainScene)
         {
            XuiSceneNavigateBack(hCur, app.hMainScene, XUSER_INDEX_ANY);
            hCur = app.hMainScene;
         }
         break;
      case INPUT_LOOP_RESIZE_MODE:
         ingame_menu_resize();
         break;
      default:
         break;
   }

   if (driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(driver.video_data, true, true);

   rarch_render_cached_frame();

   if (driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(driver.video_data, false, true);

   if(process_input_ret != 0)
      goto deinit;

   return true;

deinit:
   // set a timer delay so that we don't instantly switch back to the menu when
   // press and holding L3 + R3 in the emulation loop (lasts for 30 frame ticks)
   if(!(g_extern.lifecycle_state & (1ULL << RARCH_FRAMEADVANCE)))
      g_extern.delay_timer[0] = g_extern.frame_count + 30;

   g_extern.lifecycle_mode_state &= ~(1ULL << MODE_MENU_INGAME);

   process_input_ret = 0;

   return false;
}

bool menu_iterate_xui(void)
{
   app.Render();
   XuiTimersRun();
   return true;
}
