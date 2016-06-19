/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2016 - Daniel De Matteis
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

#include <retro_assert.h>
#include <compat/strl.h>

#include "../msg_hash.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Winvalid-source-encoding"
#endif

 /* IMPORTANT:
  * For non-english characters to work without proper unicode support,
  * we need this file to be encoded in ISO 8859-1 (Latin1), not UTF-8.
  * If you save this file as UTF-8, you'll break non-english characters
  * (e.g. German "Umlauts" and Portugese diacritics).
 */
/* DO NOT REMOVE THIS. If it causes build failure, it's because you saved the file as UTF-8. Read the above comment. */
extern const char force_iso_8859_1[sizeof("�������")==7+1 ? 1 : -1];

const char *msg_hash_to_str_es(enum msg_hash_enums msg)
{
   switch (msg)
   {
      case MSG_PROGRAM:
         return "RetroArch";
      case MSG_MOVIE_RECORD_STOPPED:
         return "Deteniendo grabaci�n de v�deo.";
      case MSG_MOVIE_PLAYBACK_ENDED:
         return "Reproducci�n detenida.";
      case MSG_AUTOSAVE_FAILED:
         return "No se ha podido iniciar el autoguardado.";
      case MSG_NETPLAY_FAILED_MOVIE_PLAYBACK_HAS_STARTED:
         return "Se ha iniciado una reproducci�n. No se puede ejecutar el juego en red.";
      case MSG_NETPLAY_FAILED:
         return "Error al iniciar el juego en red.";
      case MSG_LIBRETRO_ABI_BREAK:
         return "se ha compilado con una versi�n distinta a esta implementaci�n de libretro.";
      case MSG_REWIND_INIT_FAILED_NO_SAVESTATES:
         return "Esta implementaci�n no admite guardados r�pidos. No se puede utilizar el rebobinado.";
      case MSG_REWIND_INIT_FAILED_THREADED_AUDIO:
         return "La implementaci�n utiliza sonido multin�cleo. No se puede utilizar el rebobinado.";
      case MSG_REWIND_INIT_FAILED:
         return "Error al iniciar el b�fer de rebobinado. Rebobinado desactivado.";
      case MSG_REWIND_INIT:
         return "Iniciando b�fer de rebobinado, tama�o:";
      case MSG_CUSTOM_TIMING_GIVEN:
         return "Se ha indicado un ritmo personalizado";
      case MSG_VIEWPORT_SIZE_CALCULATION_FAILED:
         return "�Error al calcular el tama�o de ventana! Se utilizar�n datos en bruto. Probablemente esto no acabe bien...";
      case MSG_HW_RENDERED_MUST_USE_POSTSHADED_RECORDING:
         return "El n�cleo Libretro se renderiza por hardware. Es necesario utilizar la grabaci�n post-shaders.";
      case MSG_RECORDING_TO:
         return "Grabando a";
      case MSG_DETECTED_VIEWPORT_OF:
         return "Ventana detectada:";
      case MSG_TAKING_SCREENSHOT:
         return "Capturando pantalla.";
      case MSG_FAILED_TO_TAKE_SCREENSHOT:
         return "Error al capturar pantalla.";
      case MSG_FAILED_TO_START_RECORDING:
         return "Error al comenzar a grabar.";
      case MSG_RECORDING_TERMINATED_DUE_TO_RESIZE:
         return "Grabaci�n terminada por cambio de tama�o.";
      case MSG_USING_LIBRETRO_DUMMY_CORE_RECORDING_SKIPPED:
         return "Se est� utilizando el n�cleo dummy de libretro. Anulando grabaci�n.";
      case MSG_UNKNOWN:
         return "Desconocido";
      case MSG_LOADING_CONTENT_FILE:
         return "Cargando archivo de contenido";
      case MSG_RECEIVED:
         return "recibido";
      case MSG_UNRECOGNIZED_COMMAND:
         return "Comando no reconocido";
      case MSG_SENDING_COMMAND:
         return "Enviando comando";
      case MSG_GOT_INVALID_DISK_INDEX:
         return "Se ha obtenido un �ndice de disco no v�lido.";
      case MSG_FAILED_TO_REMOVE_DISK_FROM_TRAY:
         return "Error al extraer el disco de la bandeja.";
      case MSG_REMOVED_DISK_FROM_TRAY:
         return "Se ha retirado el disco de la bandeja.";
      case MSG_VIRTUAL_DISK_TRAY:
         return "bandeja de disco virtual.";
      case MSG_FAILED_TO:
         return "Error:";
      case MSG_TO:
         return "a";
      case MSG_SAVING_RAM_TYPE:
         return "Guardando tipo de RAM";
      case MSG_SAVING_STATE:
         return "Guardando r�pidamente";
      case MSG_LOADING_STATE:
         return "Cargando r�pidamente";
      case MSG_FAILED_TO_LOAD_MOVIE_FILE:
         return "Error al cargar el archivo de pel�cula";
      case MSG_FAILED_TO_LOAD_CONTENT:
         return "Error al cargar el contenido";
      case MSG_COULD_NOT_READ_CONTENT_FILE:
         return "No se ha podido leer el archivo de contenido";
      case MSG_GRAB_MOUSE_STATE:
         return "Capturar estado de rat�n";
      case MSG_PAUSED:
         return "En pausa.";
      case MSG_UNPAUSED:
         return "Sin pausa.";
      case MSG_FAILED_TO_LOAD_OVERLAY:
         return "Error al cargar sobreimposici�n.";
      case MSG_FAILED_TO_UNMUTE_AUDIO:
         return "Error al recuperar el sonido.";
      case MSG_AUDIO_MUTED:
         return "Sonido silenciado.";
      case MSG_AUDIO_UNMUTED:
         return "Sonido recuperado.";
      case MSG_RESET:
         return "Reiniciar";
      case MSG_FAILED_TO_LOAD_STATE:
         return "Error al cargar r�pidamente desde";
      case MSG_FAILED_TO_SAVE_STATE_TO:
         return "Error al guardar r�pidamente a";
      case MSG_FAILED_TO_SAVE_SRAM:
         return "Error al guardar la SRAM";
      case MSG_STATE_SIZE:
         return "Tama�o de guardado r�pido";
      case MSG_FOUND_SHADER:
         return "Shader encontrado";
      case MSG_SRAM_WILL_NOT_BE_SAVED:
         return "No se guardar� la SRAM.";
      case MSG_BLOCKING_SRAM_OVERWRITE:
         return "Bloqueando sobrescritura de SRAM";
      case MSG_CORE_DOES_NOT_SUPPORT_SAVESTATES:
         return "El n�cleo no es compatible con los guardados r�pidos.";
      case MSG_SAVED_STATE_TO_SLOT:
         return "Guardando r�pidamente a la ranura";
      case MSG_SAVED_SUCCESSFULLY_TO:
         return "Partida guardada en";
      case MSG_BYTES:
         return "bytes";
      case MSG_CONFIG_DIRECTORY_NOT_SET:
         return "No se ha asignado la carpeta de configuraci�n. No se puede guardar la nueva configuraci�n.";
      case MSG_SKIPPING_SRAM_LOAD:
         return "Omitiendo carga de SRAM.";
      case MSG_APPENDED_DISK:
         return "Disco incorporado";
      case MSG_STARTING_MOVIE_PLAYBACK:
         return "Iniciando reproducci�n.";
      case MSG_FAILED_TO_REMOVE_TEMPORARY_FILE:
         return "Error al borrar el archivo temporal";
      case MSG_REMOVING_TEMPORARY_CONTENT_FILE:
         return "Borrando archivo temporal de contenido";
      case MSG_LOADED_STATE_FROM_SLOT:
         return "Carga r�pida desde la ranura";
      case MSG_COULD_NOT_PROCESS_ZIP_FILE:
         return "No se ha podido procesar el archivo ZIP.";
      case MSG_SCANNING_OF_DIRECTORY_FINISHED:
         return "Escaneado de carpetas terminado";
      case MSG_SCANNING:
         return "Escaneando";
      case MSG_REDIRECTING_CHEATFILE_TO:
         return "Redirigiendo archivo de trucos a";
      case MSG_REDIRECTING_SAVEFILE_TO:
         return "Redirigiendo partida guardada a";
      case MSG_REDIRECTING_SAVESTATE_TO:
         return "Redirigiendo guardado r�pido a";
      case MSG_SHADER:
         return "Shader";
      case MSG_APPLYING_SHADER:
         return "Aplicando shader";
      case MSG_FAILED_TO_APPLY_SHADER:
         return "Error al aplicar shader.";
      case MSG_STARTING_MOVIE_RECORD_TO:
         return "Iniciando grabaci�n en";
      case MSG_FAILED_TO_START_MOVIE_RECORD:
         return "Error al iniciar la grabaci�n.";
      case MSG_STATE_SLOT:
         return "Ranura de guardado r�pido";
      case MSG_RESTARTING_RECORDING_DUE_TO_DRIVER_REINIT:
         return "Reiniciando grabaci�n debido al reinicio de un controlador.";
      case MSG_SLOW_MOTION:
         return "C�mara lenta.";
      case MSG_SLOW_MOTION_REWIND:
         return "Rebobinar c�mara lenta.";
      case MSG_REWINDING:
         return "Rebobinando.";
      case MSG_REWIND_REACHED_END:
         return "Se ha llegado al final del b�fer de rebobinado.";
      case MSG_CHEEVOS_HARDCORE_MODE_ENABLE:
         return "Modo Extremo Activado: guardar estado y reboninar se han desactivado.";
      case MSG_TASK_FAILED:
        return "Error";
      case MSG_DOWNLOADING:
        return "Descargando";
      case MSG_EXTRACTING:
        return "Extrayendo";
      case MENU_ENUM_LABEL_VALUE_MENU_LINEAR_FILTER:
         return "Filtro lineal del men�";
      case MENU_ENUM_LABEL_MENU_THROTTLE_FRAMERATE:
         return "Acelerar velocidad del men�";
      case MENU_ENUM_LABEL_VALUE_CHEEVOS_TEST_UNOFFICIAL:
         return "Probar versi�n no oficial";
      case MENU_ENUM_LABEL_VALUE_CHEEVOS_SETTINGS:
         return "Retrologros";
      case MENU_ENUM_LABEL_VALUE_INPUT_ICADE_ENABLE:
         return "Activar asignar mando al teclado";
      case MENU_ENUM_LABEL_VALUE_INPUT_KEYBOARD_GAMEPAD_MAPPING_TYPE:
         return "Tipo de asignaci�n de mando para teclado";
      case MENU_ENUM_LABEL_VALUE_INPUT_SMALL_KEYBOARD_ENABLE:
         return "Activar miniteclado";
      case MENU_ENUM_LABEL_VALUE_SAVE_CURRENT_CONFIG:
         return "Guardar configuraci�n actual";
      case MENU_ENUM_LABEL_VALUE_STATE_SLOT:
         return "Ranura de guardado";
      case MENU_ENUM_LABEL_VALUE_CHEEVOS_HARDCORE_MODE_ENABLE:
         return "Modo Extremo";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_CHEEVOS_SETTINGS:
         return "Cuenta Cheevos";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_CHEEVOS_USERNAME:
         return "Usuario";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_CHEEVOS_PASSWORD:
         return "Contrase�a";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_RETRO_ACHIEVEMENTS:
         return "Retrologros";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_LIST:
         return "Cuentas";
      case MENU_ENUM_LABEL_VALUE_ACCOUNTS_LIST_END:
         return "Enlace a lista de cuentas";
      case MENU_ENUM_LABEL_VALUE_DEBUG_PANEL_ENABLE:
         return "Activar panel de depuraci�n";
      case MENU_ENUM_LABEL_VALUE_HELP_SCANNING_CONTENT:
         return "Buscar contenido";
      case MENU_ENUM_LABEL_VALUE_CHEEVOS_DESCRIPTION:
         return "Descripci�n";
      case MENU_ENUM_LABEL_VALUE_HELP_AUDIO_VIDEO_TROUBLESHOOTING:
         return "Solucionar problemas de v�deo/sonido";
      case MENU_ENUM_LABEL_VALUE_HELP_CHANGE_VIRTUAL_GAMEPAD:
         return "Cambiar el mando virtual superpuesto";
      case MENU_ENUM_LABEL_VALUE_HELP_WHAT_IS_A_CORE:
         return "�Qu� es un n�cleo?";
      case MENU_ENUM_LABEL_VALUE_HELP_LOADING_CONTENT:
         return "Cargando contenidos";
      case MENU_ENUM_LABEL_VALUE_HELP_LIST:
         return "Ayuda";
      case MENU_ENUM_LABEL_VALUE_HELP_CONTROLS:
         return "Controles b�sicos del men�";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS:
         return "Controles b�sicos del men�";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_SCROLL_UP:
         return "Desplazar hacia arriba";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_SCROLL_DOWN:
         return "Desplazar hacia abajo";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_CONFIRM:
         return "Confirmar/Aceptar";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_BACK:
         return "Retroceder";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_START:
         return "Valores predeterminados";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_INFO:
         return "Informaci�n";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_TOGGLE_MENU:
         return "Alternar men�";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_QUIT:
         return "Abandonar";
      case MENU_ENUM_LABEL_VALUE_BASIC_MENU_CONTROLS_TOGGLE_KEYBOARD:
         return "Alternar teclado";
      case MENU_ENUM_LABEL_VALUE_OPEN_ARCHIVE:
         return "Abrir archivo como una carpeta";
      case MENU_ENUM_LABEL_VALUE_LOAD_ARCHIVE:
         return "Cargar archivo con un n�cleo";
      case MENU_ENUM_LABEL_VALUE_INPUT_BACK_AS_MENU_ENUM_TOGGLE_ENABLE:
         return "Permitir alternar Back como men�";
      case MENU_ENUM_LABEL_INPUT_MENU_ENUM_TOGGLE_GAMEPAD_COMBO:
         return "Combo para alternar mando con men�";
      case MENU_ENUM_LABEL_VALUE_INPUT_OVERLAY_HIDE_IN_MENU:
         return "Ocultar superposici�n en el men�";
      case MENU_ENUM_LABEL_VALUE_LANG_POLISH:
         return "Polaco";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_AUTOLOAD_PREFERRED:
         return "Cargar superposici�n preferida autom�ticamente";
      case MENU_ENUM_LABEL_VALUE_UPDATE_CORE_INFO_FILES:
         return "Actualizar archivos de informaci�n de n�cleos";
      case MENU_ENUM_LABEL_VALUE_DOWNLOAD_CORE_CONTENT:
         return "Descargar contenido";
      case MENU_ENUM_LABEL_VALUE_SCAN_THIS_DIRECTORY:
         return "(Escanear esta carpeta)";
      case MENU_ENUM_LABEL_VALUE_SCAN_FILE:
         return "Escanear archivo";
      case MENU_ENUM_LABEL_VALUE_SCAN_DIRECTORY:
         return "Escanear carpeta";
      case MENU_ENUM_LABEL_VALUE_ADD_CONTENT_LIST:
         return "A�adir contenido";
      case MENU_ENUM_LABEL_VALUE_INFORMATION_LIST:
         return "Informaci�n";
      case MENU_ENUM_LABEL_VALUE_USE_BUILTIN_PLAYER:
         return "Usar reproductor de medios integrado";
      case MENU_ENUM_LABEL_VALUE_CONTENT_SETTINGS:
         return "Men� r�pido";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_CRC32:
         return "CRC32";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_MD5:
         return "MD5";
      case MENU_ENUM_LABEL_VALUE_LOAD_CONTENT_LIST:
         return "Cargar contenido";
      case MENU_ENUM_LABEL_VALUE_ASK_ARCHIVE:
         return "Preguntar";
      case MENU_ENUM_LABEL_VALUE_PRIVACY_SETTINGS:
         return "Privacidad";
#if 0
      case MENU_ENUM_LABEL_VALUE_HORIZONTAL_MENU: /* Don't change. Breaks everything. (Would be: "Men� horizontal") */
         return "Horizontal Menu";
         /* Don't change these yet. Breaks everything. */
      case MENU_ENUM_LABEL_VALUE_SETTINGS_TAB:
         return "Pesta�a de ajustes";
      case MENU_ENUM_LABEL_VALUE_HISTORY_TAB:
         return "Pesta�a de historial";
      case MENU_ENUM_LABEL_VALUE_ADD_TAB:
         return "A�adir pesta�a";
      case MENU_ENUM_LABEL_VALUE_PLAYLISTS_TAB:
         return "Pesta�a de listas de reproducci�n";
#endif
      case MENU_ENUM_LABEL_VALUE_NO_SETTINGS_FOUND:
         return "No se ha encontrado una configuraci�n.";
      case MENU_ENUM_LABEL_VALUE_NO_PERFORMANCE_COUNTERS:
         return "No hay contadores de rendimiento.";
      case MENU_ENUM_LABEL_VALUE_DRIVER_SETTINGS:
         return "Controlador";
      case MENU_ENUM_LABEL_VALUE_CONFIGURATION_SETTINGS:
         return "Configuraci�n";
      case MENU_ENUM_LABEL_VALUE_CORE_SETTINGS:
         return "N�cleo";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SETTINGS:
         return "V�deo";
      case MENU_ENUM_LABEL_VALUE_LOGGING_SETTINGS:
         return "Registros";
      case MENU_ENUM_LABEL_VALUE_SAVING_SETTINGS:
         return "Guardado";
      case MENU_ENUM_LABEL_VALUE_REWIND_SETTINGS:
         return "Rebobinado";
      case MENU_ENUM_LABEL_VALUE_SHADER:
         return "Shader";
      case MENU_ENUM_LABEL_VALUE_CHEAT:
         return "Truco";
      case MENU_ENUM_LABEL_VALUE_USER:
         return "Usuario";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_BGM_ENABLE:
         return "Activar m�sica del sistema";
      case MENU_ENUM_LABEL_VALUE_RETROPAD:
         return "RetroPad";
      case MENU_ENUM_LABEL_VALUE_RETROKEYBOARD:
         return "RetroKeyboard";
      case MENU_ENUM_LABEL_VALUE_AUDIO_BLOCK_FRAMES:
         return "Bloquear fotogramas";
      case MENU_ENUM_LABEL_VALUE_INPUT_BIND_MODE:
         return "Modo de asignaci�n";
      case MENU_ENUM_LABEL_VALUE_INPUT_DESCRIPTOR_LABEL_SHOW:
         return "Mostrar etiquetas de descripci�n de la entrada";
      case MENU_ENUM_LABEL_VALUE_INPUT_DESCRIPTOR_HIDE_UNBOUND:
         return "Ocultar descripciones sin asignar de la entrada del n�cleo";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FONT_ENABLE:
         return "Mostrar mensajes en pantalla";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FONT_PATH:
         return "Fuente de mensajes en pantalla";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FONT_SIZE:
         return "Tama�o de mensajes en pantalla";
      case MENU_ENUM_LABEL_VALUE_VIDEO_MESSAGE_POS_X:
         return "Posici�n X de mensajes en pantalla";
      case MENU_ENUM_LABEL_VALUE_VIDEO_MESSAGE_POS_Y:
         return "Posici�n Y de mensajes en pantalla";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SOFT_FILTER:
         return "Activar filtros por software";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FILTER_FLICKER:
         return "Filtro de parpadeo";
      case MENU_ENUM_LABEL_VALUE_DIRECTORY_CONTENT:
         return "(Carpeta de contenido)";
      case MENU_ENUM_LABEL_VALUE_UNKNOWN:
         return "Desconocido";
      case MENU_ENUM_LABEL_VALUE_DONT_CARE:
         return "No importa";
      case MENU_ENUM_LABEL_VALUE_LINEAR:
         return "Lineal";
      case MENU_ENUM_LABEL_VALUE_NEAREST:
         return "M�s cercano";
      case MENU_ENUM_LABEL_VALUE_DIRECTORY_DEFAULT:
         return "(Predeterminada)";
      case MENU_ENUM_LABEL_VALUE_DIRECTORY_NONE:
         return "(Ninguna)";
      case MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE:
         return "No disponible";
      case MENU_ENUM_LABEL_VALUE_INPUT_REMAPPING_DIRECTORY:
         return "Carpeta de reasignaci�n de entrada";
      case MENU_ENUM_LABEL_VALUE_JOYPAD_AUTOCONFIG_DIR:
         return "Carpeta de autoconfiguraci�n de dispositivo de entrada";
      case MENU_ENUM_LABEL_VALUE_RECORDING_CONFIG_DIRECTORY:
         return "Carpeta de configuraci�n de grabaci�n";
      case MENU_ENUM_LABEL_VALUE_RECORDING_OUTPUT_DIRECTORY:
         return "Carpeta de salida de grabaci�n";
      case MENU_ENUM_LABEL_VALUE_SCREENSHOT_DIRECTORY:
         return "Carpeta de capturas de pantalla";
      case MENU_ENUM_LABEL_VALUE_PLAYLIST_DIRECTORY:
         return "Carpeta de listas de reproducci�n";
      case MENU_ENUM_LABEL_VALUE_SAVEFILE_DIRECTORY:
         return "Carpeta de partidas guardadas";
      case MENU_ENUM_LABEL_VALUE_SAVESTATE_DIRECTORY:
         return "Carpeta de guardados r�pidos";
      case MENU_ENUM_LABEL_VALUE_STDIN_CMD_ENABLE:
         return "Comandos stdin";
      case MENU_ENUM_LABEL_VALUE_NETWORK_REMOTE_ENABLE:
         return "Mando en red";
      case MENU_ENUM_LABEL_VALUE_VIDEO_DRIVER:
         return "Controlador de v�deo";
      case MENU_ENUM_LABEL_VALUE_RECORD_ENABLE:
         return "Activar grabaci�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_GPU_RECORD:
         return "Activar grabaci�n de GPU";
      case MENU_ENUM_LABEL_VALUE_RECORD_PATH:
         return "Carpeta de salida";
      case MENU_ENUM_LABEL_VALUE_RECORD_USE_OUTPUT_DIRECTORY:
         return "Usar carpeta de salida";
      case MENU_ENUM_LABEL_VALUE_RECORD_CONFIG:
         return "Configuraci�n de grabaci�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_POST_FILTER_RECORD:
         return "Activar grabaci�n con filtros";
      case MENU_ENUM_LABEL_VALUE_CORE_ASSETS_DIRECTORY:
         return "Carpeta de descargas";
      case MENU_ENUM_LABEL_VALUE_ASSETS_DIRECTORY:
         return "Carpeta de recursos";
      case MENU_ENUM_LABEL_VALUE_DYNAMIC_WALLPAPERS_DIRECTORY:
         return "Carpeta de fondos de pantalla din�micos";
      case MENU_ENUM_LABEL_VALUE_RGUI_BROWSER_DIRECTORY:
         return "Carpeta del navegador de archivos";
      case MENU_ENUM_LABEL_VALUE_RGUI_CONFIG_DIRECTORY:
         return "Carpeta de configuraci�n";
      case MENU_ENUM_LABEL_VALUE_LIBRETRO_INFO_PATH:
         return "Carpeta de informaci�n del n�cleo";
      case MENU_ENUM_LABEL_VALUE_LIBRETRO_DIR_PATH:
         return "Carpeta de n�cleos";
      case MENU_ENUM_LABEL_VALUE_CURSOR_DIRECTORY:
         return "Carpeta de cursores";
      case MENU_ENUM_LABEL_VALUE_CONTENT_DATABASE_DIRECTORY:
         return "Carpeta de bases de datos de contenidos";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_DIRECTORY:
         return "Carpeta de sistema/BIOS";
      case MENU_ENUM_LABEL_VALUE_CHEAT_DATABASE_PATH:
         return "Carpeta de archivos de trucos";
      case MENU_ENUM_LABEL_VALUE_CACHE_DIRECTORY:
         return "Carpeta de cach�";
      case MENU_ENUM_LABEL_VALUE_AUDIO_FILTER_DIR:
         return "Carpeta de filtros de sonido";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_DIR:
         return "Carpeta de shaders de v�deo";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FILTER_DIR:
         return "Carpeta de filtros de v�deo";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_DIRECTORY:
         return "Carpeta de superposiciones";
      case MENU_ENUM_LABEL_VALUE_OSK_OVERLAY_DIRECTORY:
         return "Carpeta de teclados superpuestos";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_CLIENT_SWAP_INPUT:
         return "Intercambiar entrada en red";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_SPECTATOR_MODE_ENABLE:
         return "Permitir espectadores en red";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_IP_ADDRESS:
         return "Direcci�n IP";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_TCP_UDP_PORT:
         return "Puerto TCP/UDP para juego en red";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_ENABLE:
         return "Activar juego en red";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_DELAY_FRAMES:
         return "Retraso de fotogramas en red";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_MODE:
         return "Activar cliente en red";
      case MENU_ENUM_LABEL_VALUE_RGUI_SHOW_START_SCREEN:
         return "Mostrar pantalla de inicio";
      case MENU_ENUM_LABEL_VALUE_TITLE_COLOR:
         return "Color de t�tulos del men�";
      case MENU_ENUM_LABEL_VALUE_ENTRY_HOVER_COLOR:
         return "Color de entrada resaltada del men�";
      case MENU_ENUM_LABEL_VALUE_TIMEDATE_ENABLE:
         return "Mostrar fecha y hora";
      case MENU_ENUM_LABEL_VALUE_THREADED_DATA_RUNLOOP_ENABLE:
         return "Buclar datos hilados";
      case MENU_ENUM_LABEL_VALUE_ENTRY_NORMAL_COLOR:
         return "Color de entrada normal del men�";
      case MENU_ENUM_LABEL_VALUE_SHOW_ADVANCED_SETTINGS:
         return "Mostrar ajustes avanzados";
      case MENU_ENUM_LABEL_VALUE_MOUSE_ENABLE:
         return "Soporte para teclado";
      case MENU_ENUM_LABEL_VALUE_POINTER_ENABLE:
         return "Soporte t�ctil";
      case MENU_ENUM_LABEL_VALUE_CORE_ENABLE:
         return "Mostrar nombre del n�cleo";
      case MENU_ENUM_LABEL_VALUE_DPI_OVERRIDE_ENABLE:
         return "Activar anulaci�n de PPP";
      case MENU_ENUM_LABEL_VALUE_DPI_OVERRIDE_VALUE:
         return "Anular PPP";
      case MENU_ENUM_LABEL_VALUE_XMB_SCALE_FACTOR:
         return "Escala del XMB";
      case MENU_ENUM_LABEL_VALUE_XMB_ALPHA_FACTOR:
         return "Transparencia del XMB";
      case MENU_ENUM_LABEL_VALUE_XMB_FONT:
         return "Fuente del XMB";
      case MENU_ENUM_LABEL_VALUE_SUSPEND_SCREENSAVER_ENABLE:
         return "Suspender salvapantallas";
      case MENU_ENUM_LABEL_VALUE_VIDEO_DISABLE_COMPOSITION:
         return "Desactivar composici�n de escritorio";
      case MENU_ENUM_LABEL_VALUE_PAUSE_NONACTIVE:
         return "Detenerse en segundo plano";
      case MENU_ENUM_LABEL_VALUE_UI_COMPANION_START_ON_BOOT:
         return "Ejecutar al inicio la IU ayudante";
      case MENU_ENUM_LABEL_VALUE_UI_COMPANION_ENABLE:
         return "Activar IU ayudante";
      case MENU_ENUM_LABEL_VALUE_UI_MENUBAR_ENABLE:
         return "Barra de men�s";
      case MENU_ENUM_LABEL_VALUE_ARCHIVE_MODE:
         return "Acci�n para asociar tipos de archivo";
      case MENU_ENUM_LABEL_VALUE_NETWORK_CMD_ENABLE:
         return "Comandos de red";
      case MENU_ENUM_LABEL_VALUE_NETWORK_CMD_PORT:
         return "Puerto de comandos de red";
      case MENU_ENUM_LABEL_VALUE_HISTORY_LIST_ENABLE:
         return "Activar historial";
      case MENU_ENUM_LABEL_VALUE_CONTENT_HISTORY_SIZE:
         return "Tama�o del historial";
      case MENU_ENUM_LABEL_VALUE_VIDEO_REFRESH_RATE_AUTO:
         return "Frecuencia estimada del monitor";
      case MENU_ENUM_LABEL_VALUE_DUMMY_ON_CORE_SHUTDOWN:
         return "Anular al cerrar n�cleo";
      case MENU_ENUM_LABEL_VALUE_CORE_SET_SUPPORTS_NO_CONTENT_ENABLE: /* TODO/FIXME */
         return "No ejecutar autom�ticamente";
      case MENU_ENUM_LABEL_VALUE_FRAME_THROTTLE_ENABLE:
         return "Limitar velocidad m�xima de ejecuci�n";
      case MENU_ENUM_LABEL_VALUE_FASTFORWARD_RATIO:
         return "Velocidad m�xima de ejecuci�n";
      case MENU_ENUM_LABEL_VALUE_AUTO_REMAPS_ENABLE:
         return "Cargar autom. archivos de reasignaci�n";
      case MENU_ENUM_LABEL_VALUE_SLOWMOTION_RATIO:
         return "Cantidad de velocidad reducida";
      case MENU_ENUM_LABEL_VALUE_CORE_SPECIFIC_CONFIG:
         return "Configuraci�n por n�cleo";
      case MENU_ENUM_LABEL_VALUE_GAME_SPECIFIC_OPTIONS:
         return "Usar opciones de n�cleo para cada juego si existen";
      case MENU_ENUM_LABEL_VALUE_GAME_SPECIFIC_OPTIONS_CREATE:
         return "Crear archivo de opciones del juego";
      case MENU_ENUM_LABEL_VALUE_GAME_SPECIFIC_OPTIONS_IN_USE:
         return "Archivo de opciones del juego";
      case MENU_ENUM_LABEL_VALUE_AUTO_OVERRIDES_ENABLE:
         return "Cargar autom. archivos de anulaci�n";
      case MENU_ENUM_LABEL_VALUE_CONFIG_SAVE_ON_EXIT:
         return "Guardar configuraci�n al salir";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SMOOTH:
         return "Filtrado bilineal por hardware";
      case MENU_ENUM_LABEL_VALUE_VIDEO_GAMMA:
         return "Gamma de v�deo";
      case MENU_ENUM_LABEL_VALUE_VIDEO_ALLOW_ROTATE:
         return "Permitir rotaci�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_HARD_SYNC:
         return "Sincron�a estricta de GPU";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SWAP_INTERVAL:
         return "Intervalo de alternado de VSync";
      case MENU_ENUM_LABEL_VALUE_VIDEO_VSYNC:
         return "VSync/Sincron�a vertical";
      case MENU_ENUM_LABEL_VALUE_VIDEO_THREADED:
         return "V�deo por hilos";
      case MENU_ENUM_LABEL_VALUE_VIDEO_ROTATION:
         return "Rotaci�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_GPU_SCREENSHOT:
         return "Permitir capturas de pantalla de GPU";
      case MENU_ENUM_LABEL_VALUE_VIDEO_CROP_OVERSCAN:
         return "Recortar Overscan (Reinicio)";
      case MENU_ENUM_LABEL_VALUE_VIDEO_ASPECT_RATIO_INDEX:
         return "�ndice de proporci�n de aspecto";
      case MENU_ENUM_LABEL_VALUE_VIDEO_ASPECT_RATIO_AUTO:
         return "Proporci�n de aspecto autom�tica";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FORCE_ASPECT:
         return "Forzar proporci�n de aspecto";
      case MENU_ENUM_LABEL_VALUE_VIDEO_REFRESH_RATE:
         return "Frecuencia de actualizaci�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FORCE_SRGB_DISABLE:
         return "Forzar anulaci�n del FBO sRGB";
      case MENU_ENUM_LABEL_VALUE_VIDEO_WINDOWED_FULLSCREEN:
         return "Pantalla completa en ventana";
      case MENU_ENUM_LABEL_VALUE_PAL60_ENABLE:
         return "Usar modo PAL60";
      case MENU_ENUM_LABEL_VALUE_VIDEO_VFILTER:
         return "Filtro contra parpadeos";
      case MENU_ENUM_LABEL_VALUE_VIDEO_VI_WIDTH:
         return "Asignar ancho de interfaz visual";
      case MENU_ENUM_LABEL_VALUE_VIDEO_BLACK_FRAME_INSERTION:
         return "Insertar fotogramas negros";
      case MENU_ENUM_LABEL_VALUE_VIDEO_HARD_SYNC_FRAMES:
         return "Fotogramas a sincronizar estrictamente";
      case MENU_ENUM_LABEL_VALUE_SORT_SAVEFILES_ENABLE:
         return "Ordenar partidas guardadas por carpetas";
      case MENU_ENUM_LABEL_VALUE_SORT_SAVESTATES_ENABLE:
         return "Ordenar guardados r�pidos por carpetas";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FULLSCREEN:
         return "Pantalla completa";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SCALE:
         return "Escala de ventana";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SCALE_INTEGER:
         return "Escala integral";
      case MENU_ENUM_LABEL_VALUE_PERFCNT_ENABLE:
         return "Contadores de rendimiento";
      case MENU_ENUM_LABEL_VALUE_LIBRETRO_LOG_LEVEL:
         return "Nivel de registro del n�cleo";
      case MENU_ENUM_LABEL_VALUE_LOG_VERBOSITY:
         return "Verbosidad del registro";
      case MENU_ENUM_LABEL_VALUE_SAVESTATE_AUTO_LOAD:
         return "Cargar guardado r�pido autom�ticamente";
      case MENU_ENUM_LABEL_VALUE_SAVESTATE_AUTO_INDEX:
         return "Indizar autom�ticamente guardados r�pidos";
      case MENU_ENUM_LABEL_VALUE_SAVESTATE_AUTO_SAVE:
         return "Guardado r�pido autom�tico";
      case MENU_ENUM_LABEL_VALUE_AUTOSAVE_INTERVAL:
         return "Intervalo de autoguardados de SaveRAM";
      case MENU_ENUM_LABEL_VALUE_BLOCK_SRAM_OVERWRITE:
         return "No sobrescribir SaveRAM al cargar un guardado r�pido";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHARED_CONTEXT:
         return "Activar contexto compartido por HW";
      case MENU_ENUM_LABEL_VALUE_RESTART_RETROARCH:
         return "Reiniciar RetroArch";
      case MENU_ENUM_LABEL_VALUE_NETPLAY_NICKNAME:
         return "Nombre de usuario";
      case MENU_ENUM_LABEL_VALUE_USER_LANGUAGE:
         return "Idioma";
      case MENU_ENUM_LABEL_VALUE_CAMERA_ALLOW:
         return "Permitir c�mara";
      case MENU_ENUM_LABEL_VALUE_LOCATION_ALLOW:
         return "Permitir ubicaci�n";
      case MENU_ENUM_LABEL_VALUE_PAUSE_LIBRETRO:
         return "Pausar al activar el men�";
      case MENU_ENUM_LABEL_VALUE_INPUT_OSK_OVERLAY_ENABLE:
         return "Mostrar teclado superpuesto";
      case MENU_ENUM_LABEL_VALUE_INPUT_OVERLAY_ENABLE:
         return "Mostrar superposici�n";
      case MENU_ENUM_LABEL_VALUE_VIDEO_MONITOR_INDEX:
         return "�ndice del monitor";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FRAME_DELAY:
         return "Retraso de fotogramas";
      case MENU_ENUM_LABEL_VALUE_INPUT_DUTY_CYCLE:
         return "Ciclo de trabajo";
      case MENU_ENUM_LABEL_VALUE_INPUT_TURBO_PERIOD:
         return "Per�odo de turbo";
      case MENU_ENUM_LABEL_VALUE_INPUT_AXIS_THRESHOLD:
         return "Margen de ejes de entrada";
      case MENU_ENUM_LABEL_VALUE_INPUT_REMAP_BINDS_ENABLE:
         return "Permitir reasignar controles";
      case MENU_ENUM_LABEL_VALUE_INPUT_MAX_USERS:
         return "N�mero m�ximo de usuarios";
      case MENU_ENUM_LABEL_VALUE_INPUT_AUTODETECT_ENABLE:
         return "Activar autoconfiguraci�n";
      case MENU_ENUM_LABEL_VALUE_AUDIO_OUTPUT_RATE:
         return "Frecuencia de sonido (KHz)";
      case MENU_ENUM_LABEL_VALUE_AUDIO_MAX_TIMING_SKEW:
         return "Variaci�n m�xima de sincron�a de sonido";
      case MENU_ENUM_LABEL_VALUE_CHEAT_NUM_PASSES:
         return "Pasadas de trucos";
      case MENU_ENUM_LABEL_VALUE_REMAP_FILE_SAVE_CORE:
         return "Guardar archivo de reasignaci�n del n�cleo";
      case MENU_ENUM_LABEL_VALUE_REMAP_FILE_SAVE_GAME:
         return "Guardar archivo de reasignaci�n del juego";
      case MENU_ENUM_LABEL_VALUE_CHEAT_APPLY_CHANGES:
         return "Aplicar cambios en trucos";
      case MENU_ENUM_LABEL_VALUE_SHADER_APPLY_CHANGES:
         return "Aplicar cambios en shaders";
      case MENU_ENUM_LABEL_VALUE_REWIND_ENABLE:
         return "Activar rebobinado";
      case MENU_ENUM_LABEL_VALUE_CONTENT_COLLECTION_LIST:
         return "Seleccionar de una colecci�n";
      case MENU_ENUM_LABEL_VALUE_DETECT_CORE_LIST:
         return "Seleccionar archivo y detectar n�cleo";
      case MENU_ENUM_LABEL_VALUE_DOWNLOADED_FILE_DETECT_CORE_LIST:
         return "Seleccionar archivo descargado y detectar n�cleo";
      case MENU_ENUM_LABEL_VALUE_LOAD_CONTENT_HISTORY:
         return "Cargar archivos recientes";
      case MENU_ENUM_LABEL_VALUE_AUDIO_ENABLE:
         return "Activar sonido";
      case MENU_ENUM_LABEL_VALUE_FPS_SHOW:
         return "Mostrar velocidad de fotogramas";
      case MENU_ENUM_LABEL_VALUE_AUDIO_MUTE:
         return "Silenciar sonido";
      case MENU_ENUM_LABEL_VALUE_AUDIO_VOLUME:
         return "Volumen de sonido (dB)";
      case MENU_ENUM_LABEL_VALUE_AUDIO_SYNC:
         return "Activar sincron�a de sonido";
      case MENU_ENUM_LABEL_VALUE_AUDIO_RATE_CONTROL_DELTA:
         return "Delta de control de frecuencia de sonido";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_NUM_PASSES:
         return "Pasadas del shader";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_SHA1:
         return "SHA1";
      case MENU_ENUM_LABEL_VALUE_CONFIGURATIONS:
         return "Cargar configuraci�n";
      case MENU_ENUM_LABEL_VALUE_REWIND_GRANULARITY:
         return "Nivel de detalle del rebobinado";
      case MENU_ENUM_LABEL_VALUE_REMAP_FILE_LOAD:
         return "Cargar archivo de reasignaci�n";
      case MENU_ENUM_LABEL_VALUE_CUSTOM_RATIO:
         return "Proporci�n personalizada";
      case MENU_ENUM_LABEL_VALUE_USE_THIS_DIRECTORY:
         return "(Usar esta carpeta)";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_START_CONTENT:
         return "Ejecutar contenido";
      case MENU_ENUM_LABEL_VALUE_DISK_OPTIONS: /* UPDATE/FIXME */
         return "Opciones del disco del n�cleo";
      case MENU_ENUM_LABEL_VALUE_CORE_OPTIONS:
         return "Opciones";
      case MENU_ENUM_LABEL_VALUE_CORE_CHEAT_OPTIONS: /* UPDATE/FIXME */
         return "Opciones de trucos del n�cleo";
      case MENU_ENUM_LABEL_VALUE_CHEAT_FILE_LOAD:
         return "Cargar archivo de trucos";
      case MENU_ENUM_LABEL_VALUE_CHEAT_FILE_SAVE_AS:
         return "Guardar archivo de trucos como...";
      case MENU_ENUM_LABEL_VALUE_CORE_COUNTERS:
         return "Contadores del n�cleo";
      case MENU_ENUM_LABEL_VALUE_TAKE_SCREENSHOT:
         return "Capturar pantalla";
      case MENU_ENUM_LABEL_VALUE_RESUME:
         return "Reanudar";
      case MENU_ENUM_LABEL_VALUE_DISK_INDEX:
         return "�ndice del disco";
      case MENU_ENUM_LABEL_VALUE_FRONTEND_COUNTERS:
         return "Contadores del frontend";
      case MENU_ENUM_LABEL_VALUE_DISK_IMAGE_APPEND:
         return "Asignar imagen de disco";
      case MENU_ENUM_LABEL_VALUE_DISK_CYCLE_TRAY_STATUS:
         return "Estado de la bandeja del disco";
      case MENU_ENUM_LABEL_VALUE_NO_PLAYLIST_ENTRIES_AVAILABLE:
         return "No hay listas de reproducci�n.";
      case MENU_ENUM_LABEL_VALUE_NO_CORE_INFORMATION_AVAILABLE:
         return "No hay informaci�n del n�cleo.";
      case MENU_ENUM_LABEL_VALUE_NO_CORE_OPTIONS_AVAILABLE:
         return "No hay opciones del n�cleo.";
      case MENU_ENUM_LABEL_VALUE_NO_CORES_AVAILABLE:
         return "No hay n�cleos.";
      case MENU_ENUM_LABEL_VALUE_NO_CORE:
         return "Sin n�cleo";
      case MENU_ENUM_LABEL_VALUE_DATABASE_MANAGER:
         return "Gestor de bases de datos";
      case MENU_ENUM_LABEL_VALUE_CURSOR_MANAGER:
         return "Gestor de cursores";
      case MENU_ENUM_LABEL_VALUE_MAIN_MENU:
         return "Men� principal"; 
      case MENU_ENUM_LABEL_VALUE_SETTINGS:
         return "Ajustes";
      case MENU_ENUM_LABEL_VALUE_QUIT_RETROARCH:
         return "Abandonar RetroArch";
      case MENU_ENUM_LABEL_VALUE_SHUTDOWN:
         return "Apagar";
      case MENU_ENUM_LABEL_VALUE_REBOOT:
         return "Reiniciar";
      case MENU_ENUM_LABEL_VALUE_HELP:
         return "Ayuda";
      case MENU_ENUM_LABEL_VALUE_SAVE_NEW_CONFIG:
         return "Guardar configuraci�n nueva";
      case MENU_ENUM_LABEL_VALUE_RESTART_CONTENT:
         return "Reiniciar";
      case MENU_ENUM_LABEL_VALUE_CORE_UPDATER_LIST:
         return "Actualizador de n�cleos";
      case MENU_ENUM_LABEL_VALUE_CORE_UPDATER_BUILDBOT_URL:
         return "URL de n�cleos de Buildbot";
      case MENU_ENUM_LABEL_VALUE_BUILDBOT_ASSETS_URL:
         return "URL de recursos de Buildbot";
      case MENU_ENUM_LABEL_VALUE_NAVIGATION_WRAPAROUND:
         return "Seguir navegaci�n";
      case MENU_ENUM_LABEL_VALUE_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE:
         return "Filtrar por extensiones compatibles";
      case MENU_ENUM_LABEL_VALUE_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE:
         return "Extraer autom�ticamente el archivo descargado";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFORMATION:
         return "Informaci�n del sistema";
      case MENU_ENUM_LABEL_VALUE_DEBUG_INFORMATION:
         return "Informaci�n de depuraci�n";
      case MENU_ENUM_LABEL_VALUE_ACHIEVEMENT_LIST:
         return "Lista de logros";
      case MENU_ENUM_LABEL_VALUE_ONLINE_UPDATER:
         return "Actualizador en l�nea";
      case MENU_ENUM_LABEL_VALUE_CORE_INFORMATION:
         return "Informaci�n del n�cleo";
      case MENU_ENUM_LABEL_VALUE_DIRECTORY_NOT_FOUND:
         return "No se ha encontrado la carpeta.";
      case MENU_ENUM_LABEL_VALUE_NO_ITEMS:
         return "No hay elementos.";
      case MENU_ENUM_LABEL_VALUE_CORE_LIST:
         return "Cargar n�cleo";
      case MENU_ENUM_LABEL_VALUE_LOAD_CONTENT:
         return "Seleccionar archivo";
      case MENU_ENUM_LABEL_VALUE_CLOSE_CONTENT:
         return "Cerrar";
      case MENU_ENUM_LABEL_VALUE_MANAGEMENT:
         return "Ajustes de bases de datos";
      case MENU_ENUM_LABEL_VALUE_SAVE_STATE:
         return "Guardado r�pido";
      case MENU_ENUM_LABEL_VALUE_LOAD_STATE:
         return "Carga r�pida";
      case MENU_ENUM_LABEL_VALUE_RESUME_CONTENT:
         return "Reanudar";
      case MENU_ENUM_LABEL_VALUE_INPUT_DRIVER:
         return "Controlador de entrada";
      case MENU_ENUM_LABEL_VALUE_AUDIO_DRIVER:
         return "Controlador de sonido";
      case MENU_ENUM_LABEL_VALUE_JOYPAD_DRIVER:
         return "Controlador de joypad";
      case MENU_ENUM_LABEL_VALUE_AUDIO_RESAMPLER_DRIVER:
         return "Controlador de muestreo de sonido";
      case MENU_ENUM_LABEL_VALUE_RECORD_DRIVER:
         return "Controlador de grabaci�n";
      case MENU_ENUM_LABEL_VALUE_MENU_DRIVER:
         return "Controlador de men�";
      case MENU_ENUM_LABEL_VALUE_CAMERA_DRIVER:
         return "Controlador de c�mara";
      case MENU_ENUM_LABEL_VALUE_LOCATION_DRIVER:
         return "Controlador de ubicaci�n";
      case MENU_ENUM_LABEL_VALUE_UNABLE_TO_READ_COMPRESSED_FILE:
         return "No se ha podido leer el archivo comprimido.";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_SCALE:
         return "Escala de superposici�n";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_PRESET:
         return "Preajuste de superposici�n";
      case MENU_ENUM_LABEL_VALUE_AUDIO_LATENCY:
         return "Retraso de sonido (ms)";
      case MENU_ENUM_LABEL_VALUE_AUDIO_DEVICE:
         return "Controlador de sonido";
      case MENU_ENUM_LABEL_VALUE_KEYBOARD_OVERLAY_PRESET:
         return "Preajuste de teclado superpuesto";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_OPACITY:
         return "Opacidad de la superposici�n";
      case MENU_ENUM_LABEL_VALUE_MENU_WALLPAPER:
         return "Fondo del men�";
      case MENU_ENUM_LABEL_VALUE_DYNAMIC_WALLPAPER:
         return "Fondo de pantalla din�mico";
      case MENU_ENUM_LABEL_VALUE_CORE_INPUT_REMAPPING_OPTIONS:
         return "Opciones de entrada del n�cleo";
      case MENU_ENUM_LABEL_VALUE_SHADER_OPTIONS:
         return "Opciones de shaders";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_PARAMETERS:
         return "Previsualizar par�metros de shaders";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_PRESET_PARAMETERS:
         return "Par�metros de shaders del men�";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_PRESET_SAVE_AS:
         return "Guardar preajuste de shaders como...";
      case MENU_ENUM_LABEL_VALUE_NO_SHADER_PARAMETERS:
         return "No hay par�metros de shaders.";
      case MENU_ENUM_LABEL_VALUE_VIDEO_SHADER_PRESET:
         return "Cargar preajuste de shaders";
      case MENU_ENUM_LABEL_VALUE_VIDEO_FILTER:
         return "Filtro de v�deo";
      case MENU_ENUM_LABEL_VALUE_AUDIO_DSP_PLUGIN:
         return "Plugin DSP de sonido";
      case MENU_ENUM_LABEL_VALUE_STARTING_DOWNLOAD:
         return "Iniciando descarga: ";
      case MENU_ENUM_LABEL_VALUE_SECONDS:
         return "segundos";
      case MENU_ENUM_LABEL_VALUE_OFF: /* Not changed. Would be "S�" */
         return "OFF";
      case MENU_ENUM_LABEL_VALUE_ON: /* Not changed. Would be "NO" */
         return "ON";
      case MENU_ENUM_LABEL_VALUE_UPDATE_ASSETS:
         return "Actualizar recursos";
      case MENU_ENUM_LABEL_VALUE_UPDATE_LAKKA:
         return "Actualizar Lakka";
      case MENU_ENUM_LABEL_VALUE_UPDATE_CHEATS:
         return "Actualizar trucos";
      case MENU_ENUM_LABEL_VALUE_UPDATE_AUTOCONFIG_PROFILES:
         return "Actualizar perfiles de autoconfiguraci�n";
      case MENU_ENUM_LABEL_VALUE_UPDATE_AUTOCONFIG_PROFILES_HID:
         return "Actualizar perfiles de autoconfiguraci�n (HID)";
      case MENU_ENUM_LABEL_VALUE_UPDATE_DATABASES:
         return "Actualizar bases de datos";
      case MENU_ENUM_LABEL_VALUE_UPDATE_OVERLAYS:
         return "Actualizar superposiciones";
      case MENU_ENUM_LABEL_VALUE_UPDATE_CG_SHADERS:
         return "Actualizar shaders Cg";
      case MENU_ENUM_LABEL_VALUE_UPDATE_GLSL_SHADERS:
         return "Actualizar shaders GLSL";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_CORE_NAME:
         return "Nombre del n�cleo";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_CORE_LABEL:
         return "Etiqueta del n�cleo";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_SYSTEM_NAME:
         return "Nombre del sistema";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_SYSTEM_MANUFACTURER:
         return "Fabricante del sistema";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_CATEGORIES:
         return "Categor�as";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_AUTHORS:
         return "Autores";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_PERMISSIONS:
         return "Permisos";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_LICENSES:
         return "Licencia(s)";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_SUPPORTED_EXTENSIONS:
         return "Extensiones compatibles";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_FIRMWARE:
         return "Firmware";
      case MENU_ENUM_LABEL_VALUE_CORE_INFO_CORE_NOTES:
         return "Notas del n�cleo";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_BUILD_DATE:
         return "Fecha de compilaci�n";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_GIT_VERSION:
         return "Versi�n de Git";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_CPU_FEATURES:
         return "Caracter�sticas de CPU";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FRONTEND_IDENTIFIER:
         return "Identificador del frontend";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FRONTEND_NAME:
         return "Nombre del frontend";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FRONTEND_OS:
         return "S.O. del frontend";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_RETRORATING_LEVEL:
         return "Nivel de RetroRating";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_POWER_SOURCE:
         return "Fuente de alimentaci�n";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_POWER_SOURCE_NO_SOURCE:
         return "No hay una fuente";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_POWER_SOURCE_CHARGING:
         return "Cargando";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_POWER_SOURCE_CHARGED:
         return "Cargada";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_POWER_SOURCE_DISCHARGING:
         return "Descargando";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_VIDEO_CONTEXT_DRIVER:
         return "Controlador de contexto de v�deo";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DISPLAY_METRIC_MM_WIDTH:
         return "Mostrar ancho m�trico (mm)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DISPLAY_METRIC_MM_HEIGHT:
         return "Mostrar alto m�trico (mm)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DISPLAY_METRIC_DPI:
         return "Mostrar PPP m�tricos";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_LIBRETRODB_SUPPORT:
         return "Soporte de LibretroDB";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OVERLAY_SUPPORT:
         return "Soporte de superposiciones";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_COMMAND_IFACE_SUPPORT:
         return "Soporte de interfaz de comandos";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_NETWORK_REMOTE_SUPPORT:
         return "Soporte de mando en red";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_NETWORK_COMMAND_IFACE_SUPPORT:
         return "Soporte de interfaz de comandos en red";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_COCOA_SUPPORT:
         return "Soporte de Cocoa";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_RPNG_SUPPORT:
         return "Soporte de PNG (RPNG)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_SDL_SUPPORT:
         return "Soporte de SDL1.2";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_SDL2_SUPPORT:
         return "Soporte de SDL2";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_VULKAN_SUPPORT:
         return "Soporte Vulkan";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OPENGL_SUPPORT:
         return "Soporte de OpenGL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OPENGLES_SUPPORT:
         return "Soporte de OpenGL ES";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_THREADING_SUPPORT:
         return "Soporte de hilos";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_KMS_SUPPORT:
         return "Soporte de KMS/EGL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_UDEV_SUPPORT:
         return "Soporte de Udev";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OPENVG_SUPPORT:
         return "Soporte de OpenVG";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_EGL_SUPPORT:
         return "Soporte de EGL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_X11_SUPPORT:
         return "Soporte de X11";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_WAYLAND_SUPPORT:
         return "Soporte de Wayland";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_XVIDEO_SUPPORT:
         return "Soporte de XVideo";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_ALSA_SUPPORT:
         return "Soporte de ALSA";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OSS_SUPPORT:
         return "Soporte de OSS";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OPENAL_SUPPORT:
         return "Soporte de OpenAL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_OPENSL_SUPPORT:
         return "Soporte de OpenSL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_RSOUND_SUPPORT:
         return "Soporte de RSound";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_ROARAUDIO_SUPPORT:
         return "Soporte de RoarAudio";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_JACK_SUPPORT:
         return "Soporte de JACK";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_PULSEAUDIO_SUPPORT:
         return "Soporte de PulseAudio";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DSOUND_SUPPORT:
         return "Soporte de DirectSound";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_XAUDIO2_SUPPORT:
         return "Soporte de XAudio2";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_ZLIB_SUPPORT:
         return "Soporte de Zlib";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_7ZIP_SUPPORT:
         return "Soporte de 7zip";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DYLIB_SUPPORT:
         return "Soporte de librer�as din�micas";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_CG_SUPPORT:
         return "Soporte de Cg";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_GLSL_SUPPORT:
         return "Soporte de GLSL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_HLSL_SUPPORT:
         return "Soporte de HLSL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_LIBXML2_SUPPORT:
         return "Soporte de parseo XML libxml2";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_SDL_IMAGE_SUPPORT:
         return "Soporte de im�genes SDL";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FBO_SUPPORT:
         return "Soporte de render-to-texture OpenGL/Direct3D (shaders multipasos)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_DYNAMIC_SUPPORT:
         return "Carga din�mica en tiempo real de librer�a libretro";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FFMPEG_SUPPORT:
         return "Soporte de FFmpeg";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_CORETEXT_SUPPORT:
         return "Soporte de CoreText";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_FREETYPE_SUPPORT:
         return "Soporte de FreeType";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_NETPLAY_SUPPORT:
         return "Soporte de juego en red (peer-to-peer)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_PYTHON_SUPPORT:
         return "Soporte de Python (soporte de scripts para shaders)";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_V4L2_SUPPORT:
         return "Soporte de Video4Linux2";
      case MENU_ENUM_LABEL_VALUE_SYSTEM_INFO_LIBUSB_SUPPORT:
         return "Soporte de Libusb";
      case MENU_ENUM_LABEL_VALUE_YES:
         return "S�";
      case MENU_ENUM_LABEL_VALUE_NO:
         return "No";
      case MENU_ENUM_LABEL_VALUE_BACK:
         return "BACK";
      case MENU_ENUM_LABEL_VALUE_SCREEN_RESOLUTION:
         return "Resoluci�n de pantalla";
      case MENU_ENUM_LABEL_VALUE_DISABLED:
         return "Desactivado";
      case MENU_ENUM_LABEL_VALUE_PORT:
         return "Puerto";
      case MENU_ENUM_LABEL_VALUE_NONE:
         return "Ninguno";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_DEVELOPER:
         return "Desarrollador";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_PUBLISHER:
         return "Distribuidora";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_DESCRIPTION:
         return "Descripci�n";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_GENRE:
         return "G�nero";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_NAME:
         return "Nombre";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_ORIGIN:
         return "Origen";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_FRANCHISE:
         return "Franquicia";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_RELEASE_MONTH:
         return "Mes de lanzamiento";
      case MENU_ENUM_LABEL_VALUE_RDB_ENTRY_RELEASE_YEAR:
         return "A�o de lanzamiento";
      case MENU_ENUM_LABEL_VALUE_TRUE:
         return "Activado";
      case MENU_ENUM_LABEL_VALUE_FALSE:
         return "Desactivado";
      case MENU_ENUM_LABEL_VALUE_MISSING:
         return "Desaparecido";
      case MENU_ENUM_LABEL_VALUE_PRESENT:
         return "Presente";
      case MENU_ENUM_LABEL_VALUE_OPTIONAL:
         return "Opcional";
      case MENU_ENUM_LABEL_VALUE_REQUIRED:
         return "Necesario";
      case MENU_ENUM_LABEL_VALUE_STATUS:
         return "Estado";
      case MENU_ENUM_LABEL_VALUE_AUDIO_SETTINGS:
         return "Sonido";
      case MENU_ENUM_LABEL_VALUE_INPUT_SETTINGS:
         return "Entrada";
      case MENU_ENUM_LABEL_VALUE_ONSCREEN_DISPLAY_SETTINGS:
         return "Textos en pantalla (OSD)";
      case MENU_ENUM_LABEL_VALUE_OVERLAY_SETTINGS:
         return "Superposici�n";
      case MENU_ENUM_LABEL_VALUE_MENU_SETTINGS:
         return "Men�";
      case MENU_ENUM_LABEL_VALUE_MULTIMEDIA_SETTINGS:
         return "Multimedia";
      case MENU_ENUM_LABEL_VALUE_UI_SETTINGS:
         return "Interfaz de usuario";
      case MENU_ENUM_LABEL_VALUE_MENU_FILE_BROWSER_SETTINGS:
         return "Navegador de archivos del men�";
      case MENU_ENUM_LABEL_VALUE_CORE_UPDATER_SETTINGS:
         return "Actualizador";
      case MENU_ENUM_LABEL_VALUE_NETWORK_SETTINGS:
         return "Red";
      case MENU_ENUM_LABEL_VALUE_PLAYLIST_SETTINGS:
         return "Lista de reproducci�n";
      case MENU_ENUM_LABEL_VALUE_USER_SETTINGS:
         return "Usuario";
      case MENU_ENUM_LABEL_VALUE_DIRECTORY_SETTINGS:
         return "Carpeta";
      case MENU_ENUM_LABEL_VALUE_RECORDING_SETTINGS:
         return "Grabaci�n";
      case MENU_ENUM_LABEL_VALUE_NO_INFORMATION_AVAILABLE:
         return "No hay informaci�n disponible.";
      case MENU_ENUM_LABEL_VALUE_INPUT_USER_BINDS:
         return "Asignaciones de entrada del usuario %u";
      case MENU_ENUM_LABEL_VALUE_LANG_ENGLISH:
         return "Ingl�s";
      case MENU_ENUM_LABEL_VALUE_LANG_JAPANESE:
         return "Japon�s";
      case MENU_ENUM_LABEL_VALUE_LANG_FRENCH:
         return "Franc�s";
      case MENU_ENUM_LABEL_VALUE_LANG_SPANISH:
         return "Espa�ol";
      case MENU_ENUM_LABEL_VALUE_LANG_GERMAN:
         return "Alem�n";
      case MENU_ENUM_LABEL_VALUE_LANG_ITALIAN:
         return "Italiano";
      case MENU_ENUM_LABEL_VALUE_LANG_DUTCH:
         return "Holand�s";
      case MENU_ENUM_LABEL_VALUE_LANG_PORTUGUESE:
         return "Portugu�s";
      case MENU_ENUM_LABEL_VALUE_LANG_RUSSIAN:
         return "Ruso";
      case MENU_ENUM_LABEL_VALUE_LANG_KOREAN:
         return "Coreano";
      case MENU_ENUM_LABEL_VALUE_LANG_CHINESE_TRADITIONAL:
         return "Chino (Tradicional)";
      case MENU_ENUM_LABEL_VALUE_LANG_CHINESE_SIMPLIFIED:
         return "Chino (Simplificado)";
      case MENU_ENUM_LABEL_VALUE_LANG_ESPERANTO:
         return "Esperanto";
      case MENU_ENUM_LABEL_VALUE_LEFT_ANALOG:
         return "Anal�gico izquierdo";
      case MENU_ENUM_LABEL_VALUE_RIGHT_ANALOG:
         return "Anal�gico derecho";
      case MENU_ENUM_LABEL_VALUE_INPUT_HOTKEY_BINDS:
         return "Asignaciones de teclas r�pidas";
      case MENU_ENUM_LABEL_VALUE_FRAME_THROTTLE_SETTINGS:
         return "Aumento de fotogramas";
      case MENU_ENUM_LABEL_VALUE_SEARCH:
         return "Buscar:";
      case MENU_ENUM_LABEL_VALUE_USE_BUILTIN_IMAGE_VIEWER:
         return "Usar visualizador de im�genes integrado";
      case MENU_ENUM_LABEL_VALUE_ENABLE:
         return "Activar";
      case MENU_ENUM_LABEL_VALUE_START_CORE:
         return "Iniciar n�cleo";
      case MENU_ENUM_LABEL_VALUE_INPUT_POLL_TYPE_BEHAVIOR:
         return "Tipo de retardo";
      default:
         break;
   }

   return "null";
}
