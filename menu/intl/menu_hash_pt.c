/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 *
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
#include <string.h>

#include "../menu_hash.h"

const char *menu_hash_to_str_pt(uint32_t hash)
{
   switch (hash)
   {
      case MENU_LABEL_VALUE_VIDEO_FONT_ENABLE:
         return "Mostrar Mensagem de Tela";
      case MENU_LABEL_VALUE_VIDEO_FONT_PATH:
         return "Fonte da Mensagem de Tela";
      case MENU_LABEL_VALUE_VIDEO_FONT_SIZE:
         return "Tamanho da Mensagem de Tela";
      case MENU_LABEL_VIDEO_MESSAGE_POS_X:
         return "video_message_pos_x";
      case MENU_LABEL_VALUE_VIDEO_MESSAGE_POS_X:
         return "Posi��o X da Mensagem de Tela";
      case MENU_LABEL_VIDEO_MESSAGE_POS_Y:
         return "video_message_pos_y";
      case MENU_LABEL_VALUE_VIDEO_MESSAGE_POS_Y:
         return "Posi��o Y da Mensagem de Tela";
      case MENU_LABEL_VALUE_VIDEO_SOFT_FILTER:
         return "Ativar Filtro de Suaviza��o";
      case MENU_LABEL_VALUE_VIDEO_FILTER_FLICKER:
         return "Filtro de Cintila��o";
      case MENU_VALUE_DIRECTORY_CONTENT:
         return "<Dir. de Conte�do>";
      case MENU_VALUE_UNKNOWN:
         return "Desconhecido";
      case MENU_VALUE_DIRECTORY_DEFAULT:
         return "<Padr�o>";
      case MENU_VALUE_DIRECTORY_NONE:
         return "<Nenhum>";
      case MENU_VALUE_NOT_AVAILABLE:
         return "N/A";
      case MENU_LABEL_VALUE_INPUT_REMAPPING_DIRECTORY:
         return "Dir. de Remapeamento de Controladores";
      case MENU_LABEL_JOYPAD_AUTOCONFIG_DIR:
         return "joypad_autoconfig_dir";
      case MENU_LABEL_VALUE_JOYPAD_AUTOCONFIG_DIR:
         return "Dir. de Config. Autom�tica de Dispositivos de Entrada";
      case MENU_LABEL_VALUE_RECORDING_CONFIG_DIRECTORY:
         return "Dir. de Config. de Grava��o";
      case MENU_LABEL_VALUE_RECORDING_OUTPUT_DIRECTORY:
         return "Dir. de Grava��es";
      case MENU_LABEL_VALUE_SCREENSHOT_DIRECTORY:
         return "Dir. de Capturas de Telas";
      case MENU_LABEL_PLAYLIST_DIRECTORY:
         return "playlist_directory";
      case MENU_LABEL_VALUE_PLAYLIST_DIRECTORY:
         return "Dir. de Listas de Jogos";
      case MENU_LABEL_VALUE_SAVEFILE_DIRECTORY:
         return "Dir. de Saves";
      case MENU_LABEL_VALUE_SAVESTATE_DIRECTORY:
         return "Dir. de Savestates";
      case MENU_LABEL_VALUE_STDIN_CMD_ENABLE:
         return "Comandos stdin";
      case MENU_LABEL_VALUE_VIDEO_DRIVER:
         return "Driver de V�deo";
      case MENU_LABEL_VALUE_RECORD_ENABLE:
         return "Ativar Grava��o";
      case MENU_LABEL_VALUE_VIDEO_GPU_RECORD:
         return "Ativar Grava��o na GPU";
      case MENU_LABEL_VALUE_RECORD_PATH:
         return "Caminho da Grava��o";
      case MENU_LABEL_RECORD_USE_OUTPUT_DIRECTORY:
         return "record_use_output_directory";
      case MENU_LABEL_VALUE_RECORD_USE_OUTPUT_DIRECTORY:
         return "Usar Dir. de Grava��es";
      case MENU_LABEL_VALUE_RECORD_CONFIG:
         return "Configurar Grava��o";
      case MENU_LABEL_VIDEO_POST_FILTER_RECORD:
         return "video_post_filter_record";
      case MENU_LABEL_VALUE_VIDEO_POST_FILTER_RECORD:
         return "Ativar Filtro P�s-Grava��o";
      case MENU_LABEL_VALUE_CORE_ASSETS_DIRECTORY:
         return "Dir. de Recursos (Assets) dos Cores";
      case MENU_LABEL_ASSETS_DIRECTORY:
         return "assets_directory";
      case MENU_LABEL_VALUE_ASSETS_DIRECTORY:
         return "Dir. de Recursos (Assets)";
      case MENU_LABEL_VALUE_DYNAMIC_WALLPAPERS_DIRECTORY:
         return "Dir. de Pap�is de Parede Din�micos";
      case MENU_LABEL_VALUE_BOXARTS_DIRECTORY:
         return "Dir. de Boxarts";
      case MENU_LABEL_RGUI_BROWSER_DIRECTORY:
         return "rgui_browser_directory";
      case MENU_LABEL_VALUE_RGUI_BROWSER_DIRECTORY:
         return "Dir. de Navega��o";
      case MENU_LABEL_RGUI_CONFIG_DIRECTORY:
         return "rgui_config_directory";
      case MENU_LABEL_VALUE_RGUI_CONFIG_DIRECTORY:
         return "Dir. de Configura��es";
      case MENU_LABEL_LIBRETRO_INFO_PATH:
         return "libretro_info_path";
      case MENU_LABEL_VALUE_LIBRETRO_INFO_PATH:
         return "Dir. de Informa��es de Cores";
      case MENU_LABEL_LIBRETRO_DIR_PATH:
         return "libretro_dir_path";
      case MENU_LABEL_VALUE_LIBRETRO_DIR_PATH:
         return "Dir. de Cores";
      case MENU_LABEL_CURSOR_DIRECTORY:
         return "cursor_directory";
      case MENU_LABEL_VALUE_CURSOR_DIRECTORY:
         return "Dir. de Cursores";
      case MENU_LABEL_CONTENT_DATABASE_DIRECTORY:
         return "content_database_path";
      case MENU_LABEL_VALUE_CONTENT_DATABASE_DIRECTORY:
         return "Dir. de Base de Dados de Conte�dos";
      case MENU_LABEL_SYSTEM_DIRECTORY:
         return "system_directory";
      case MENU_LABEL_VALUE_SYSTEM_DIRECTORY:
         return "Diret�rio System";
      case MENU_LABEL_EXTRACTION_DIRECTORY:
         return "extraction_directory";
      case MENU_LABEL_CHEAT_DATABASE_PATH:
         return "cheat_database_path";
      case MENU_LABEL_VALUE_CHEAT_DATABASE_PATH:
         return "Dir. de Arquivos Cheat";
      case MENU_LABEL_VALUE_EXTRACTION_DIRECTORY:
         return "Dir. de Descompacta��o";
      case MENU_LABEL_AUDIO_FILTER_DIR:
         return "audio_filter_dir";
      case MENU_LABEL_VALUE_AUDIO_FILTER_DIR:
         return "Dir. de Filtros de �udio";
      case MENU_LABEL_VIDEO_FILTER_DIR:
         return "video_filter_dir";
      case MENU_LABEL_VIDEO_SHADER_DIR:
         return "video_shader_dir";
      case MENU_LABEL_VALUE_VIDEO_SHADER_DIR:
         return "Dir. de Shaders de V�deo";
      case MENU_LABEL_VALUE_VIDEO_FILTER_DIR:
         return "Dir. de Filtros de V�deo";
      case MENU_LABEL_OVERLAY_DIRECTORY:
         return "overlay_directory";
      case MENU_LABEL_VALUE_OVERLAY_DIRECTORY:
         return "Dir. de Overlays";
      case MENU_LABEL_OSK_OVERLAY_DIRECTORY:
         return "osk_overlay_directory";
      case MENU_LABEL_VALUE_OSK_OVERLAY_DIRECTORY:
         return "Dir. de Overlay de Teclado";
      case MENU_LABEL_NETPLAY_CLIENT_SWAP_INPUT:
         return "netplay_client_swap_input";
      case MENU_LABEL_VALUE_NETPLAY_CLIENT_SWAP_INPUT:
         return "Trocar Controlador de Netplay";
      case MENU_LABEL_NETPLAY_SPECTATOR_MODE_ENABLE:
         return "netplay_spectator_mode_enable";
      case MENU_LABEL_VALUE_NETPLAY_SPECTATOR_MODE_ENABLE:
         return "Ativar Espectador de Netplay";
      case MENU_LABEL_NETPLAY_IP_ADDRESS:
         return "netplay_ip_address";
      case MENU_LABEL_VALUE_NETPLAY_IP_ADDRESS:
         return "Endere�o IP";
      case MENU_LABEL_NETPLAY_TCP_UDP_PORT:
         return "netplay_tcp_udp_port";
      case MENU_LABEL_VALUE_NETPLAY_TCP_UDP_PORT:
         return "Portas TCP/UDP de Netplay";
      case MENU_LABEL_NETPLAY_ENABLE:
         return "netplay_enable";
      case MENU_LABEL_VALUE_NETPLAY_ENABLE:
         return "Ativar Netplay";
      case MENU_LABEL_NETPLAY_DELAY_FRAMES:
         return "netplay_delay_frames";
      case MENU_LABEL_VALUE_NETPLAY_DELAY_FRAMES:
         return "Quadros de Retardo de Netplay";
      case MENU_LABEL_NETPLAY_MODE:
         return "netplay_mode";
      case MENU_LABEL_VALUE_NETPLAY_MODE:
         return "Ativar Cliente de Netplay";
      case MENU_LABEL_RGUI_SHOW_START_SCREEN:
         return "rgui_show_start_screen";
      case MENU_LABEL_VALUE_RGUI_SHOW_START_SCREEN:
         return "Mostrar Tela de In�cio";
      case MENU_LABEL_TITLE_COLOR:
         return "menu_title_color";
      case MENU_LABEL_VALUE_TITLE_COLOR:
         return "Cor do Menu T�tulo";
      case MENU_LABEL_ENTRY_HOVER_COLOR:
         return "menu_entry_hover_color";
      case MENU_LABEL_VALUE_ENTRY_HOVER_COLOR:
         return "Cor de Realce do Menu Inicial";
      case MENU_LABEL_TIMEDATE_ENABLE:
         return "menu_timedate_enable";
      case MENU_LABEL_VALUE_TIMEDATE_ENABLE:
         return "Mostrar Hora / Data";
      case MENU_LABEL_THREADED_DATA_RUNLOOP_ENABLE:
         return "threaded_data_runloop_enable";
      case MENU_LABEL_VALUE_THREADED_DATA_RUNLOOP_ENABLE:
         return "Ativar Runloop de Threads de Dados";
      case MENU_LABEL_ENTRY_NORMAL_COLOR:
         return "menu_entry_normal_color";
      case MENU_LABEL_VALUE_ENTRY_NORMAL_COLOR:
         return "Cor do Menu Inicial";
      case MENU_LABEL_SHOW_ADVANCED_SETTINGS:
         return "menu_show_advanced_settings";
      case MENU_LABEL_VALUE_SHOW_ADVANCED_SETTINGS:
         return "Mostrar Configura��es Avan�adas";
      case MENU_LABEL_COLLAPSE_SUBGROUPS_ENABLE:
         return "menu_collapse_subgroups_enable";
      case MENU_LABEL_VALUE_COLLAPSE_SUBGROUPS_ENABLE:
         return "Juntar Subgrupos";
      case MENU_LABEL_MOUSE_ENABLE:
         return "menu_mouse_enable";
      case MENU_LABEL_VALUE_MOUSE_ENABLE:
         return "Suporte a Mouse";
      case MENU_LABEL_POINTER_ENABLE:
         return "menu_pointer_enable";
      case MENU_LABEL_VALUE_POINTER_ENABLE:
         return "Suporte a Touch";
      case MENU_LABEL_CORE_ENABLE:
         return "menu_core_enable";
      case MENU_LABEL_VALUE_CORE_ENABLE:
         return "Mostrar Nome dos Cores";
      case MENU_LABEL_DPI_OVERRIDE_ENABLE:
         return "dpi_override_enable";
      case MENU_LABEL_VALUE_DPI_OVERRIDE_ENABLE:
         return "Ativar Sobreposi��o de DPI";
      case MENU_LABEL_DPI_OVERRIDE_VALUE:
         return "dpi_override_value";
      case MENU_LABEL_VALUE_DPI_OVERRIDE_VALUE:
         return "Sobreposi��o de DPI";
      case MENU_LABEL_SUSPEND_SCREENSAVER_ENABLE:
         return "suspend_screensaver_enable";
      case MENU_LABEL_VALUE_SUSPEND_SCREENSAVER_ENABLE:
         return "Suspender Prote��o de Tela";
      case MENU_LABEL_VIDEO_DISABLE_COMPOSITION:
         return "video_disable_composition";
      case MENU_LABEL_VALUE_VIDEO_DISABLE_COMPOSITION:
         return "Desativar Desktop Composition";
      case MENU_LABEL_PAUSE_NONACTIVE:
         return "pause_nonactive";
      case MENU_LABEL_VALUE_PAUSE_NONACTIVE:
         return "N�o Rodar em Background";
      case MENU_LABEL_UI_COMPANION_START_ON_BOOT:
         return "ui_companion_start_on_boot";
      case MENU_LABEL_VALUE_UI_COMPANION_START_ON_BOOT:
         return "Ativar UI Companion ao Iniciar";
      case MENU_LABEL_UI_MENUBAR_ENABLE:
         return "ui_menubar_enable";
      case MENU_LABEL_VALUE_UI_MENUBAR_ENABLE:
         return "Barra de Menu (Dica)";
      case MENU_LABEL_ARCHIVE_MODE:
         return "archive_mode";
      case MENU_LABEL_VALUE_ARCHIVE_MODE:
         return "Modo Archive";
      case MENU_LABEL_NETWORK_CMD_ENABLE:
         return "network_cmd_enable";
      case MENU_LABEL_VALUE_NETWORK_CMD_ENABLE:
         return "Comandos de Rede";
      case MENU_LABEL_NETWORK_CMD_PORT:
         return "network_cmd_port";
      case MENU_LABEL_VALUE_NETWORK_CMD_PORT:
         return "Porta para A��es de Rede";
      case MENU_LABEL_HISTORY_LIST_ENABLE:
         return "history_list_enable";
      case MENU_LABEL_VALUE_HISTORY_LIST_ENABLE:
         return "Ativar Lista de Hist�rico";
      case MENU_LABEL_CONTENT_HISTORY_SIZE:
         return "Content History Size";
      case MENU_LABEL_VALUE_CONTENT_HISTORY_SIZE:
         return "Tamanho da Lista de Hist�rico";
      case MENU_LABEL_VIDEO_REFRESH_RATE_AUTO:
         return "video_refresh_rate_auto";
      case MENU_LABEL_VALUE_VIDEO_REFRESH_RATE_AUTO:
         return "Taxa de Atualiza��o de Quadros Estimada";
      case MENU_LABEL_DUMMY_ON_CORE_SHUTDOWN:
         return "dummy_on_core_shutdown";
      case MENU_LABEL_VALUE_DUMMY_ON_CORE_SHUTDOWN:
         return "Desligar Core Dummy On";
      case MENU_LABEL_CORE_SET_SUPPORTS_NO_CONTENT_ENABLE:
         return "core_set_supports_no_content_enable";
      case MENU_LABEL_VALUE_CORE_SET_SUPPORTS_NO_CONTENT_ENABLE:
         return "N�o Iniciar Cores Automaticamente";
      case MENU_LABEL_FRAME_THROTTLE_SETTINGS:
         return "fastforward_ratio_throttle_enable";
      case MENU_LABEL_VALUE_FRAME_THROTTLE_SETTINGS:
         return "Limitar Velocidade M�xima de Execu��o";
      case MENU_LABEL_FASTFORWARD_RATIO:
         return "fastforward_ratio";
      case MENU_LABEL_VALUE_FASTFORWARD_RATIO:
         return "Velocidade M�xima de Execu��o";
      case MENU_LABEL_AUTO_REMAPS_ENABLE:
         return "auto_remaps_enable";
      case MENU_LABEL_VALUE_AUTO_REMAPS_ENABLE:
         return "Carregar Automaticamente Arquivos Remapeados";
      case MENU_LABEL_SLOWMOTION_RATIO:
         return "slowmotion_ratio";
      case MENU_LABEL_VALUE_SLOWMOTION_RATIO:
         return "Taxa de C�mera Lenta";
      case MENU_LABEL_CORE_SPECIFIC_CONFIG:
         return "core_specific_config";
      case MENU_LABEL_VALUE_CORE_SPECIFIC_CONFIG:
         return "Configura��o por Core";
      case MENU_LABEL_AUTO_OVERRIDES_ENABLE:
         return "auto_overrides_enable";
      case MENU_LABEL_VALUE_AUTO_OVERRIDES_ENABLE:
         return "Carregar Automaticamente Arquivos de Sobreposi��o";
      case MENU_LABEL_CONFIG_SAVE_ON_EXIT:
         return "config_save_on_exit";
      case MENU_LABEL_VALUE_CONFIG_SAVE_ON_EXIT:
         return "Salvar Configura��o ao Sair";
      case MENU_LABEL_VIDEO_SMOOTH:
         return "video_smooth";
      case MENU_LABEL_VALUE_VIDEO_SMOOTH:
         return "Filtragem Bilinear por Hardware";
      case MENU_LABEL_VIDEO_GAMMA:
         return "video_gamma";
      case MENU_LABEL_VALUE_VIDEO_GAMMA:
         return "Gamma de V�deo";
      case MENU_LABEL_VIDEO_ALLOW_ROTATE:
         return "video_allow_rotate";
      case MENU_LABEL_VALUE_VIDEO_ALLOW_ROTATE:
         return "Permitir Rota��o";
      case MENU_LABEL_VIDEO_HARD_SYNC:
         return "video_hard_sync";
      case MENU_LABEL_VALUE_VIDEO_HARD_SYNC:
         return "Sincronizar GPU com CPU";
      case MENU_LABEL_VIDEO_SWAP_INTERVAL:
         return "video_swap_interval";
      case MENU_LABEL_VALUE_VIDEO_SWAP_INTERVAL:
         return "Intervalo de Permuta do Sincronismo Vertical";
      case MENU_LABEL_VIDEO_VSYNC:
         return "video_vsync";
      case MENU_LABEL_VALUE_VIDEO_VSYNC:
         return "Sincronismo Vertical";
      case MENU_LABEL_VIDEO_THREADED:
         return "video_threaded";
      case MENU_LABEL_VALUE_VIDEO_THREADED:
         return "V�deo em Threads";
      case MENU_LABEL_VIDEO_ROTATION:
         return "video_rotation";
      case MENU_LABEL_VALUE_VIDEO_ROTATION:
         return "Rota��o";
      case MENU_LABEL_VIDEO_GPU_SCREENSHOT:
         return "video_gpu_screenshot";
      case MENU_LABEL_VALUE_VIDEO_GPU_SCREENSHOT:
         return "Ativar Captura de Tela via GPU";
      case MENU_LABEL_VIDEO_CROP_OVERSCAN:
         return "video_crop_overscan";
      case MENU_LABEL_VALUE_VIDEO_CROP_OVERSCAN:
         return "Descartar Overscan (Recarregue)";
      case MENU_LABEL_VIDEO_ASPECT_RATIO_INDEX:
         return "aspect_ratio_index";
      case MENU_LABEL_VALUE_VIDEO_ASPECT_RATIO_INDEX:
         return "�ndice de Propor��es de Tela";
      case MENU_LABEL_VIDEO_ASPECT_RATIO_AUTO:
         return "video_aspect_ratio_auto";
      case MENU_LABEL_VALUE_VIDEO_ASPECT_RATIO_AUTO:
         return "Propor��o de Tela Autom�tica";
      case MENU_LABEL_VIDEO_FORCE_ASPECT:
         return "video_force_aspect";
      case MENU_LABEL_VALUE_VIDEO_FORCE_ASPECT:
         return "For�ar Propor��o de Tela";
      case MENU_LABEL_VIDEO_REFRESH_RATE:
         return "video_refresh_rate";
      case MENU_LABEL_VALUE_VIDEO_REFRESH_RATE:
         return "Taxa de Atualiza��o de Tela";
      case MENU_LABEL_VIDEO_FORCE_SRGB_DISABLE:
         return "video_force_srgb_disable";
      case MENU_LABEL_VALUE_VIDEO_FORCE_SRGB_DISABLE:
         return "Forcar Desativa��o de sRGB FBO";
      case MENU_LABEL_VIDEO_WINDOWED_FULLSCREEN:
         return "video_windowed_fullscreen";
      case MENU_LABEL_VALUE_VIDEO_WINDOWED_FULLSCREEN:
         return "Modo Tela Cheia em Janela";
      case MENU_LABEL_PAL60_ENABLE:
         return "pal60_enable";
      case MENU_LABEL_VALUE_PAL60_ENABLE:
         return "Usar Modo PAL60";
      case MENU_LABEL_VIDEO_VFILTER:
         return "video_vfilter";
      case MENU_LABEL_VALUE_VIDEO_VFILTER:
         return "Eliminar Cintila��o";
      case MENU_LABEL_VIDEO_VI_WIDTH:
         return "video_vi_width";
      case MENU_LABEL_VALUE_VIDEO_VI_WIDTH:
         return "Fixar Largura de Tela VI";
      case MENU_LABEL_VALUE_VIDEO_BLACK_FRAME_INSERTION:
         return "Inser��o de Quadro Negro";
      case MENU_LABEL_VIDEO_BLACK_FRAME_INSERTION:
         return "video_black_frame_insertion";
      case MENU_LABEL_VIDEO_HARD_SYNC_FRAMES:
         return "video_hard_sync_frames";
      case MENU_LABEL_VALUE_VIDEO_HARD_SYNC_FRAMES:
         return "Quadros de Sincroniza��o entre GPU e CPU";
      case MENU_LABEL_SORT_SAVEFILES_ENABLE:
         return "sort_savefiles_enable";
      case MENU_LABEL_VALUE_SORT_SAVEFILES_ENABLE:
         return "Ordenar Saves em Pastas";
      case MENU_LABEL_SORT_SAVESTATES_ENABLE:
         return "sort_savestates_enable";
      case MENU_LABEL_VALUE_SORT_SAVESTATES_ENABLE:
         return "Ordenar Savestates em Pastas";
      case MENU_LABEL_VIDEO_FULLSCREEN:
         return "video_fullscreen";
      case MENU_LABEL_VALUE_VIDEO_FULLSCREEN:
         return "Usar Modo Tela Cheia";
      case MENU_LABEL_PERFCNT_ENABLE:
         return "perfcnt_enable";
      case MENU_LABEL_VIDEO_SCALE:
         return "video_scale";
      case MENU_LABEL_VALUE_VIDEO_SCALE:
         return "Interpola��o em Janela";
      case MENU_LABEL_VIDEO_SCALE_INTEGER:
         return "video_scale_integer";
      case MENU_LABEL_VALUE_VIDEO_SCALE_INTEGER:
         return "Interpola��o em M�ltiplos Inteiros";
      case MENU_LABEL_VALUE_PERFCNT_ENABLE:
         return "Contadores de Desempenho";
      case MENU_LABEL_LIBRETRO_LOG_LEVEL:
         return "libretro_log_level";
      case MENU_LABEL_VALUE_LIBRETRO_LOG_LEVEL:
         return "N�vel de Registro de Core";
      case MENU_LABEL_LOG_VERBOSITY:
         return "log_verbosity";
      case MENU_LABEL_VALUE_LOG_VERBOSITY:
         return "Detalhamento de Registro";
      case MENU_LABEL_SAVESTATE_AUTO_SAVE:
         return "savestate_auto_save";
      case MENU_LABEL_SAVESTATE_AUTO_LOAD:
         return "savestate_auto_load";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_LOAD:
         return "Carregar Savestate Automaticamente";
      case MENU_LABEL_SAVESTATE_AUTO_INDEX:
         return "savestate_auto_index";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_INDEX:
         return "�ndice Autom�tico de Savestates";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_SAVE:
         return "Savestate Autom�tico";
      case MENU_LABEL_AUTOSAVE_INTERVAL:
         return "autosave_interval";
      case MENU_LABEL_VALUE_AUTOSAVE_INTERVAL:
         return "Intervalo de Grava��o Autom�tica de SaveRAM";
      case MENU_LABEL_BLOCK_SRAM_OVERWRITE:
         return "block_sram_overwrite";
      case MENU_LABEL_VALUE_BLOCK_SRAM_OVERWRITE:
         return "N�o Sobrescrever SaveRAM ao Carregar Savestate";
      case MENU_LABEL_VIDEO_SHARED_CONTEXT:
         return "video_shared_context";
      case MENU_LABEL_VALUE_VIDEO_SHARED_CONTEXT:
         return "Ativar Contexto Compartilhado de Hardware";
      case MENU_LABEL_RESTART_RETROARCH:
         return "restart_retroarch";
      case MENU_LABEL_VALUE_RESTART_RETROARCH:
         return "Reiniciar RetroArch";
      case MENU_LABEL_NETPLAY_NICKNAME:
         return "netplay_nickname";
      case MENU_LABEL_VALUE_NETPLAY_NICKNAME:
         return "Nome de Usu�rio";
      case MENU_LABEL_USER_LANGUAGE:
         return "user_language";
      case MENU_LABEL_VALUE_USER_LANGUAGE:
         return "Idioma";
      case MENU_LABEL_CAMERA_ALLOW:
         return "camera_allow";
      case MENU_LABEL_VALUE_CAMERA_ALLOW:
         return "Permitir C�mera";
      case MENU_LABEL_LOCATION_ALLOW:
         return "location_allow";
      case MENU_LABEL_VALUE_LOCATION_ALLOW:
         return "Permitir Localiza��o";
      case MENU_LABEL_PAUSE_LIBRETRO:
         return "menu_pause_libretro";
      case MENU_LABEL_VALUE_PAUSE_LIBRETRO:
         return "Pausar Quando o Menu for Ativado";
      case MENU_LABEL_INPUT_OSK_OVERLAY_ENABLE:
         return "input_osk_overlay_enable";
      case MENU_LABEL_VALUE_INPUT_OSK_OVERLAY_ENABLE:
         return "Mostrar Overlay de Teclado";
      case MENU_LABEL_INPUT_OVERLAY_ENABLE:
         return "input_overlay_enable";
      case MENU_LABEL_VALUE_INPUT_OVERLAY_ENABLE:
         return "Mostrar Overlay";
      case MENU_LABEL_VIDEO_MONITOR_INDEX:
         return "video_monitor_index";
      case MENU_LABEL_VALUE_VIDEO_MONITOR_INDEX:
         return "�ndice de Monitores";
      case MENU_LABEL_VIDEO_FRAME_DELAY:
         return "video_frame_delay";
      case MENU_LABEL_VALUE_VIDEO_FRAME_DELAY:
         return "Retardo de Quadro";
      case MENU_LABEL_VALUE_INPUT_DUTY_CYCLE:
         return "Ciclo de Trabalho";
      case MENU_LABEL_INPUT_DUTY_CYCLE:
         return "input_duty_cycle";
      case MENU_LABEL_INPUT_TURBO_PERIOD:
         return "input_turbo_period";
      case MENU_LABEL_VALUE_INPUT_TURBO_PERIOD:
         return "Per�odo de Turbo";
      case MENU_LABEL_VALUE_INPUT_AXIS_THRESHOLD:
         return "Limiar de Eixo do Controlador";
      case MENU_LABEL_INPUT_AXIS_THRESHOLD:
         return "input_axis_threshold";
      case MENU_LABEL_INPUT_REMAP_BINDS_ENABLE:
         return "input_remap_binds_enable";
      case MENU_LABEL_VALUE_INPUT_REMAP_BINDS_ENABLE:
         return "Ativar Remapeamentos";
      case MENU_LABEL_INPUT_MAX_USERS:
         return "input_max_users";
      case MENU_LABEL_VALUE_INPUT_MAX_USERS:
         return "Usu�rios M�ximos";
      case MENU_LABEL_VALUE_INPUT_AUTODETECT_ENABLE:
         return "Ativar Autoconfigura��o";
      case MENU_LABEL_INPUT_AUTODETECT_ENABLE:
         return "input_autodetect_enable";
      case MENU_LABEL_AUDIO_OUTPUT_RATE:
         return "audio_output_rate";
      case MENU_LABEL_VALUE_AUDIO_OUTPUT_RATE:
         return "Taxa de Amostragem de �udio (KHz)";
      case MENU_LABEL_AUDIO_MAX_TIMING_SKEW:
         return "audio_max_timing_skew";
      case MENU_LABEL_VALUE_AUDIO_MAX_TIMING_SKEW:
         return "Distor��o M�xima de Sincroniza��o de �udio";
      case MENU_LABEL_VALUE_CHEAT_NUM_PASSES:
         return "C�digos de Cheat";
      case MENU_LABEL_CHEAT_APPLY_CHANGES:
         return "cheat_apply_changes";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_CORE:
         return "Salvar Remapeamento de Core";
      case MENU_LABEL_REMAP_FILE_SAVE_CORE:
         return "remap_file_save_core";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_GAME:
         return "Salvar Remapeamento de Jogo";
      case MENU_LABEL_REMAP_FILE_SAVE_GAME:
         return "remap_file_save_game";
      case MENU_LABEL_VALUE_CHEAT_APPLY_CHANGES:
         return "Aplicar Altera��es de Cheats";
      case MENU_LABEL_CHEAT_NUM_PASSES:
         return "cheat_num_passes";
      case MENU_LABEL_VALUE_SHADER_APPLY_CHANGES:
         return "Aplicar Altera��es de Shaders";
      case MENU_LABEL_SHADER_APPLY_CHANGES:
         return "shader_apply_changes";
      case MENU_LABEL_COLLECTION:
         return "collection";
      case MENU_LABEL_REWIND_ENABLE:
         return "rewind_enable";
      case MENU_LABEL_VALUE_REWIND_ENABLE:
         return "Ativar Retrocesso";
      case MENU_LABEL_CONTENT_COLLECTION_LIST:
         return "content_collection_list";
      case MENU_LABEL_VALUE_CONTENT_COLLECTION_LIST:
         return "Carregar Conte�do (Cole��o)"; /* FIXME */
      case MENU_LABEL_DETECT_CORE_LIST:
         return "detect_core_list";
      case MENU_LABEL_VALUE_DETECT_CORE_LIST:
         return "Carregar Conte�do (Detectar Core)"; /* FIXME */
      case MENU_LABEL_LOAD_CONTENT_HISTORY:
         return "history_list";
      case MENU_LABEL_VALUE_LOAD_CONTENT_HISTORY:
         return "Carregar Conte�do (Hist�rico)";
      case MENU_LABEL_AUDIO_ENABLE:
         return "audio_enable";
      case MENU_LABEL_VALUE_AUDIO_ENABLE:
         return "Ativar �udio";
      case MENU_LABEL_FPS_SHOW:
         return "fps_show";
      case MENU_LABEL_VALUE_FPS_SHOW:
         return "Mostrar Taxa de Quadros";
      case MENU_LABEL_AUDIO_MUTE:
         return "audio_mute_enable";
      case MENU_LABEL_VALUE_AUDIO_MUTE:
         return "Desligar �udio";
      case MENU_LABEL_VIDEO_SHADER_PASS:
         return "video_shader_pass";
      case MENU_LABEL_AUDIO_VOLUME:
         return "audio_volume";
      case MENU_LABEL_VALUE_AUDIO_VOLUME:
         return "Volume de �udio (dB)";
      case MENU_LABEL_AUDIO_SYNC:
         return "audio_sync";
      case MENU_LABEL_VALUE_AUDIO_SYNC:
         return "Ativar Sincronismo de �udio";
      case MENU_LABEL_AUDIO_RATE_CONTROL_DELTA:
         return "audio_rate_control_delta";
      case MENU_LABEL_VALUE_AUDIO_RATE_CONTROL_DELTA:
         return "Varia��o M�xima de Taxa de �udio";
      case MENU_LABEL_VIDEO_SHADER_FILTER_PASS:
         return "video_shader_filter_pass";
      case MENU_LABEL_VIDEO_SHADER_SCALE_PASS:
         return "video_shader_scale_pass";
      case MENU_LABEL_VALUE_VIDEO_SHADER_NUM_PASSES:
         return "N�mero de Shaders";
      case MENU_LABEL_VIDEO_SHADER_NUM_PASSES:
         return "video_shader_num_passes";
      case MENU_LABEL_RDB_ENTRY_DESCRIPTION:
         return "rdb_entry_description";
      case MENU_LABEL_RDB_ENTRY_ORIGIN:
         return "rdb_entry_origin";
      case MENU_LABEL_RDB_ENTRY_PUBLISHER:
         return "rdb_entry_publisher";
      case MENU_LABEL_RDB_ENTRY_DEVELOPER:
         return "rdb_entry_developer";
      case MENU_LABEL_RDB_ENTRY_FRANCHISE:
         return "rdb_entry_franchise";
      case MENU_LABEL_RDB_ENTRY_MAX_USERS:
         return "rdb_entry_max_users";
      case MENU_LABEL_RDB_ENTRY_NAME:
         return "rdb_entry_name";
      case MENU_LABEL_RDB_ENTRY_EDGE_MAGAZINE_RATING:
         return "rdb_entry_edge_magazine_rating";
      case MENU_LABEL_RDB_ENTRY_FAMITSU_MAGAZINE_RATING:
         return "rdb_entry_famitsu_magazine_rating";
      case MENU_LABEL_RDB_ENTRY_EDGE_MAGAZINE_ISSUE:
         return "rdb_entry_edge_magazine_issue";
      case MENU_LABEL_RDB_ENTRY_RELEASE_MONTH:
         return "rdb_entry_releasemonth";
      case MENU_LABEL_RDB_ENTRY_RELEASE_YEAR:
         return "rdb_entry_releaseyear";
      case MENU_LABEL_RDB_ENTRY_ENHANCEMENT_HW:
         return "rdb_entry_enhancement_hw";
      case MENU_LABEL_RDB_ENTRY_SHA1:
         return "rdb_entry_sha1";
      case MENU_LABEL_RDB_ENTRY_CRC32:
         return "rdb_entry_crc32";
      case MENU_LABEL_RDB_ENTRY_MD5:
         return "rdb_entry_md5";
      case MENU_LABEL_RDB_ENTRY_BBFC_RATING:
         return "rdb_entry_bbfc_rating";
      case MENU_LABEL_RDB_ENTRY_ESRB_RATING:
         return "rdb_entry_esrb_rating";
      case MENU_LABEL_RDB_ENTRY_ELSPA_RATING:
         return "rdb_entry_elspa_rating";
      case MENU_LABEL_RDB_ENTRY_PEGI_RATING:
         return "rdb_entry_pegi_rating";
      case MENU_LABEL_RDB_ENTRY_CERO_RATING:
         return "rdb_entry_cero_rating";
      case MENU_LABEL_RDB_ENTRY_ANALOG:
         return "rdb_entry_analog";
      case MENU_LABEL_CONFIGURATIONS:
         return "configurations";
      case MENU_LABEL_VALUE_CONFIGURATIONS: /* FIXME */
         return "Arquivos de Configura��o";
      case MENU_LABEL_LOAD_OPEN_ZIP:
         return "load_open_zip";
      case MENU_LABEL_REWIND_GRANULARITY:
         return "rewind_granularity";
      case MENU_LABEL_VALUE_REWIND_GRANULARITY:
         return "Granularidade do Retrocesso";
      case MENU_LABEL_REMAP_FILE_LOAD:
         return "remap_file_load";
      case MENU_LABEL_VALUE_REMAP_FILE_LOAD:
         return "Carregar Remapeamento";
      case MENU_LABEL_REMAP_FILE_SAVE_AS:
         return "remap_file_save_as";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_AS:
         return "Salvar Remapeamento Como";
      case MENU_LABEL_CUSTOM_RATIO:
         return "custom_ratio";
      case MENU_LABEL_VALUE_CUSTOM_RATIO:
         return "Propor��o de Tela Personalizada";
      case MENU_LABEL_VALUE_USE_THIS_DIRECTORY:
         return "<Usar este diret�rio>";
      case MENU_LABEL_USE_THIS_DIRECTORY:
         return "use_this_directory";
      case MENU_LABEL_VALUE_RDB_ENTRY_START_CONTENT:
         return "Iniciar Conte�do";
      case MENU_LABEL_RDB_ENTRY_START_CONTENT:
         return "rdb_entry_start_content";
      case MENU_LABEL_CUSTOM_BIND:
         return "custom_bind";
      case MENU_LABEL_CUSTOM_BIND_ALL:
         return "custom_bind_all";
      case MENU_LABEL_DISK_OPTIONS:
         return "core_disk_options";
      case MENU_LABEL_VALUE_DISK_OPTIONS:
         return "Op��es de Discos do Core";
      case MENU_LABEL_VALUE_CORE_OPTIONS:
         return "Op��es de Core";
      case MENU_LABEL_VALUE_CORE_CHEAT_OPTIONS:
         return "Op��es de Cheat do Core";
      case MENU_LABEL_CORE_CHEAT_OPTIONS:
         return "core_cheat_options";
      case MENU_LABEL_CORE_OPTIONS:
         return "core_options";
      case MENU_LABEL_DATABASE_MANAGER_LIST:
         return "database_manager_list";
      case MENU_LABEL_DEFERRED_DATABASE_MANAGER_LIST:
         return "deferred_database_manager_list";
      case MENU_LABEL_CURSOR_MANAGER_LIST:
         return "cursor_manager_list";
      case MENU_LABEL_DEFERRED_CURSOR_MANAGER_LIST:
         return "deferred_cursor_manager_list";
      case MENU_LABEL_VALUE_CHEAT_FILE_LOAD:
         return "Carregar Cheat";
      case MENU_LABEL_CHEAT_FILE_LOAD:
         return "cheat_file_load";
      case MENU_LABEL_CHEAT_FILE_SAVE_AS:
         return "cheat_file_save_as";
      case MENU_LABEL_VALUE_CHEAT_FILE_SAVE_AS:
         return "Salvar Cheat Como";
      case MENU_LABEL_DEFERRED_RDB_ENTRY_DETAIL:
         return "deferred_rdb_entry_detail";
      case MENU_LABEL_FRONTEND_COUNTERS:
         return "frontend_counters";
      case MENU_LABEL_CORE_COUNTERS:
         return "core_counters";
      case MENU_LABEL_VALUE_CORE_COUNTERS:
         return "Contadores de Cores";
      case MENU_LABEL_DISK_CYCLE_TRAY_STATUS:
         return "disk_cycle_tray_status";
      case MENU_LABEL_DISK_IMAGE_APPEND:
         return "disk_image_append";
      case MENU_LABEL_DEFERRED_CORE_LIST:
         return "deferred_core_list";
      case MENU_LABEL_DEFERRED_CORE_LIST_SET:
         return "deferred_core_list_set";
      case MENU_LABEL_VALUE_TAKE_SCREENSHOT:
         return "Capturar Tela";
      case MENU_LABEL_INFO_SCREEN:
         return "info_screen";
      case MENU_LABEL_VALUE_RESUME:
         return "Retomar";
      case MENU_LABEL_VALUE_DISK_INDEX:
         return "�ndice de Discos";
      case MENU_LABEL_VALUE_FRONTEND_COUNTERS:
         return "Contadores de Frontend";
      case MENU_LABEL_VALUE_DISK_IMAGE_APPEND:
         return "Anexar Imagem de Disco";
      case MENU_LABEL_VALUE_DISK_CYCLE_TRAY_STATUS:
         return "Estado do Drive de Disco";
      case MENU_LABEL_VALUE_NO_PLAYLIST_ENTRIES_AVAILABLE:
         return "Lista de Jogos Vazia.";
      case MENU_LABEL_VALUE_NO_CORE_INFORMATION_AVAILABLE:
         return "Nenhuma Informa��o de Core Dispon�vel.";
      case MENU_LABEL_VALUE_NO_CORE_OPTIONS_AVAILABLE:
         return "Nenhuma Op��o de Core Dispon�vel.";
      case MENU_LABEL_VALUE_NO_CORES_AVAILABLE:
         return "Nenhum Core Dispon�vel.";
      case MENU_VALUE_NO_CORE:
         return "Nenhum Core";
      case MENU_LABEL_VALUE_DATABASE_MANAGER:
         return "Gerenciador de Base de Dados";
      case MENU_LABEL_VALUE_CURSOR_MANAGER:
         return "Gerenciador de Cursores";
      case MENU_VALUE_RECORDING_SETTINGS:
         return "Configura��es de Grava��o";
      case MENU_VALUE_MAIN_MENU:
         return "Main Menu";
      case MENU_LABEL_SETTINGS:
         return "frontend_settings";
      case MENU_LABEL_VALUE_SETTINGS:
         return "Configura��es";
      case MENU_LABEL_QUIT_RETROARCH:
         return "quit_retroarch";
      case MENU_LABEL_VALUE_QUIT_RETROARCH:
         return "Sair do RetroArch";
      case MENU_LABEL_VALUE_HELP:
         return "Ajuda";
      case MENU_LABEL_HELP:
         return "help";
      case MENU_LABEL_SAVE_NEW_CONFIG:
         return "save_new_config";
      case MENU_LABEL_VALUE_SAVE_NEW_CONFIG:
         return "Salvar Nova Configura��o";
      case MENU_LABEL_RESTART_CONTENT:
         return "restart_content";
      case MENU_LABEL_VALUE_RESTART_CONTENT:
         return "Reiniciar Conte�do";
      case MENU_LABEL_TAKE_SCREENSHOT:
         return "take_screenshot";
      case MENU_LABEL_CORE_UPDATER_LIST:
         return "core_updater_list";
      case MENU_LABEL_VALUE_CORE_UPDATER_LIST:
         return "Atualiza��o de Cores";
      case MENU_LABEL_CORE_UPDATER_BUILDBOT_URL:
         return "core_updater_buildbot_url";
      case MENU_LABEL_VALUE_CORE_UPDATER_BUILDBOT_URL:
         return "URL de Cores - Buildbot";
      case MENU_LABEL_BUILDBOT_ASSETS_URL:
         return "buildbot_assets_url";
      case MENU_LABEL_VALUE_BUILDBOT_ASSETS_URL:
         return "URL de Recursos (Assets) - Buildbot";
      case MENU_LABEL_NAVIGATION_WRAPAROUND_VERTICAL:
         return "menu_navigation_wraparound_vertical_enable";
      case MENU_LABEL_NAVIGATION_WRAPAROUND_HORIZONTAL:
         return "menu_navigation_wraparound_horizontal_enable";
      case MENU_LABEL_VALUE_NAVIGATION_WRAPAROUND_HORIZONTAL:
         return "Navega��o Horizontal Circular";
      case MENU_LABEL_VALUE_NAVIGATION_WRAPAROUND_VERTICAL:
         return "Navega��o Vertical Circular";
      case MENU_LABEL_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE:
         return "menu_navigation_browser_filter_supported_extensions_enable";
      case MENU_LABEL_VALUE_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE:
         return "Navegador - Filtrar por Extens�es Suportadas";
      case MENU_LABEL_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE:
         return "core_updater_auto_extract_archive";
      case MENU_LABEL_VALUE_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE:
         return "Extrair Arquivos Baixados Automaticamente";
      case MENU_LABEL_SYSTEM_INFORMATION:
         return "system_information";
      case MENU_LABEL_VALUE_SYSTEM_INFORMATION:
         return "Informa��o de Sistema";
      case MENU_LABEL_OPTIONS:
         return "options";
      case MENU_LABEL_VALUE_OPTIONS:
         return "Op��es"; /* FIXME */
      case MENU_LABEL_CORE_INFORMATION:
         return "core_information";
      case MENU_LABEL_VALUE_CORE_INFORMATION:
         return "Informa��o de Core";
      case MENU_LABEL_VALUE_DIRECTORY_NOT_FOUND:
         return "Diret�rio N�o Encontrado.";
      case MENU_LABEL_VALUE_NO_ITEMS:
         return "Nenhum Item.";
      case MENU_LABEL_VALUE_CORE_LIST:
         return "Carregar Core";
      case MENU_LABEL_CORE_LIST:
         return "core_list";
      case MENU_LABEL_LOAD_CONTENT:
         return "load_content_default";
      case MENU_LABEL_VALUE_LOAD_CONTENT:
         return "Carregar Conte�do";  /* FIXME */
      case MENU_LABEL_CLOSE_CONTENT:
         return "unload_core";
      case MENU_LABEL_VALUE_CLOSE_CONTENT:
         return "Descarregar Core"; /* FIXME */
      case MENU_LABEL_MANAGEMENT:
         return "database_settings";
      case MENU_LABEL_VALUE_MANAGEMENT:
         return "Gerenciamento Avan�ado";
      case MENU_LABEL_SAVE_STATE:
         return "savestate";
      case MENU_LABEL_VALUE_SAVE_STATE:
         return "Salvar Savestate";
      case MENU_LABEL_LOAD_STATE:
         return "loadstate";
      case MENU_LABEL_VALUE_LOAD_STATE:
         return "Carregar Savestate";
      case MENU_LABEL_VALUE_RESUME_CONTENT:
         return "Retomar Conte�do";
      case MENU_LABEL_RESUME_CONTENT:
         return "resume_content";
      case MENU_LABEL_VALUE_DRIVER_SETTINGS:
         return "Configura��es de Drivers";
      case MENU_LABEL_INPUT_DRIVER:
         return "input_driver";
      case MENU_LABEL_VALUE_INPUT_DRIVER:
         return "Driver de Controladores";
      case MENU_LABEL_AUDIO_DRIVER:
         return "audio_driver";
      case MENU_LABEL_VALUE_AUDIO_DRIVER:
         return "Driver de �udio";
      case MENU_LABEL_JOYPAD_DRIVER:
         return "input_joypad_driver";
      case MENU_LABEL_VALUE_JOYPAD_DRIVER:
         return "Driver de Joypad";
      case MENU_LABEL_AUDIO_RESAMPLER_DRIVER:
         return "audio_resampler_driver";
      case MENU_LABEL_VALUE_AUDIO_RESAMPLER_DRIVER:
         return "Driver de Amostragem de �udio";
      case MENU_LABEL_VALUE_RECORD_DRIVER:
         return "Driver de Grava��o";
      case MENU_LABEL_VALUE_MENU_DRIVER:
         return "Driver de Menu";
      case MENU_LABEL_VALUE_CAMERA_DRIVER:
         return "Driver de C�mera";
      case MENU_LABEL_VALUE_LOCATION_DRIVER:
         return "Driver de Localiza��o";
      case MENU_LABEL_VALUE_UNABLE_TO_READ_COMPRESSED_FILE:
         return "Incapaz de Ler Arquivo Comprimido.";
      case MENU_LABEL_OVERLAY_SCALE:
         return "input_overlay_scale";
      case MENU_LABEL_VALUE_OVERLAY_SCALE:
         return "Interpola��o de Overlay";
      case MENU_LABEL_VALUE_OVERLAY_PRESET:
         return "Predefini��es de Overlay";
      case MENU_LABEL_VALUE_AUDIO_LATENCY:
         return "Lat�ncia de �udio (ms)";
      case MENU_LABEL_VALUE_AUDIO_DEVICE:
         return "Dispositivo de �udio";
      case MENU_LABEL_VALUE_KEYBOARD_OVERLAY_PRESET:
         return "Predefini��es de Overlay de Teclado";
      case MENU_LABEL_OVERLAY_OPACITY:
         return "input_overlay_opacity";
      case MENU_LABEL_VALUE_OVERLAY_OPACITY:
         return "Opacidade de Overlay";
      case MENU_LABEL_MENU_WALLPAPER:
         return "menu_wallpaper";
      case MENU_LABEL_VALUE_MENU_WALLPAPER:
         return "Papel de Parede do Menu";
      case MENU_LABEL_VALUE_DYNAMIC_WALLPAPER:
         return "Papel de Parede Din�mico";
      case MENU_LABEL_VALUE_BOXART:
         return "Mostrar Boxart";
      case MENU_LABEL_VALUE_CORE_INPUT_REMAPPING_OPTIONS:
         return "Op��es de Remapeamento de Controlador de Core";
      case MENU_LABEL_VALUE_SHADER_OPTIONS:
         return "Op��es de Shaders";
      case MENU_LABEL_VALUE_VIDEO_SHADER_PRESET_SAVE_AS:
         return "Salvar Predefini��es de Shader Como";
      case MENU_LABEL_VALUE_NO_SHADER_PARAMETERS:
         return "Nenhum Par�metro de Shader Dispon�vel.";
      case MENU_LABEL_VALUE_VIDEO_SHADER_PRESET:
         return "Carregar Predefini��es de Shader";
      case MENU_LABEL_VALUE_VIDEO_FILTER:
         return "Filtros de V�deo";
      case MENU_LABEL_DEFERRED_VIDEO_FILTER:
         return "deferred_video_filter";
      case MENU_LABEL_DEFERRED_CORE_UPDATER_LIST:
         return "deferred_core_updater_list";
      case MENU_LABEL_AUDIO_DSP_PLUGIN:
         return "audio_dsp_plugin";
      case MENU_LABEL_VALUE_AUDIO_DSP_PLUGIN:
         return "Plugin de DSP de �udio";
      case MENU_LABEL_VALUE_STARTING_DOWNLOAD:
         return "Iniciando Download: ";
      case MENU_VALUE_SECONDS:
         return "Segundos";
      case MENU_VALUE_OFF:
         return "OFF";
      case MENU_VALUE_ON:
         return "ON";
      default:
         break;
   }
 
   return "null";
}
