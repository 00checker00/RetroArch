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

#include "../menu_hash.h"
#include "../../configuration.h"
#include "../../msg_hash.h"

/* IMPORTANT:
 * For non-english characters to work without proper unicode support,
 * we need this file to be encoded in ISO 8859-1 (Latin1), not UTF-8.
 * If you save this file as UTF-8, you'll break non-english characters
 * (e.g. German "Umlauts" and Portugese diacritics).
 */
/* DO NOT REMOVE THIS. If it causes build failure, it's because you saved the file as UTF-8. Read the above comment. */
extern const char force_iso_8859_1[sizeof("��������")==9+1 ? 1 : -1];

int menu_hash_get_help_es(uint32_t hash, char *s, size_t len)
{
   uint32_t driver_hash = 0;
   settings_t      *settings = config_get_ptr();

   /* If this one throws errors, stop sledgehammering square pegs into round holes and */
   /* READ THE COMMENTS at the top of the file. */
   (void)sizeof(force_iso_8859_1);

   switch (hash)
   {
      case MENU_LABEL_VALUE_HELP_AUDIO_VIDEO_TROUBLESHOOTING_DESC:
         {
            /* Work around C89 limitations */
            char u[501];
            char t[501];

            strlcpy(t,
                  "RetroArch utiliza un formato �nico para \n"
                  "sincronizar v�deo y sonido que necesita \n"
                  "calibrarse con la frecuencia de \n"
                  "actualizaci�n de tu monitor para obtener \n"
                  "el mejor rendimiento. \n"
                  " \n"
                  "Si notas cortes de sonido o en la imagen,\n"
                  "lo normal es que necesites calibrar estos\n"
                  "ajustes. Aqu� van algunas opciones:\n"
                  " \n", sizeof(t));
            snprintf(u, sizeof(u),
                  "a) Ve a '%s' -> '%s' y activa\n"
                  "'V�deo por hilos'. En este modo la tasa\n"
                  "de refresco es irrelevante, habr� m�s fps,\n"
                  "'V�deo multin�cleo'. En este modo la \n"
                  "frecuencia es irrelevante, habr� m�s fps,\n"
                  "pero la imagen podr�a ser menos fluida.\n"
                  "b) Ve a '%s' -> '%s' y busca\n"
                  "'%s'. Deja que se ejecute durante\n"
                  "2048 fotogramas y selecciona Aceptar.",
                  msg_hash_to_str(MENU_ENUM_LABEL_VALUE_SETTINGS),
                  msg_hash_to_str(MENU_ENUM_LABEL_VALUE_VIDEO_SETTINGS),
                  msg_hash_to_str(MENU_ENUM_LABEL_VALUE_SETTINGS),
                  msg_hash_to_str(MENU_ENUM_LABEL_VALUE_VIDEO_SETTINGS),
                  msg_hash_to_str(MENU_ENUM_LABEL_VALUE_VIDEO_REFRESH_RATE_AUTO)
                     );
            strlcat(s, t, len);
            strlcat(s, u, len);
         }
         break;
      case MENU_LABEL_VALUE_HELP_SCANNING_CONTENT_DESC:
         snprintf(s, len,
               "Para escanear contenidos ve a '%s' \n"
               "y selecciona '%s' o \n"
               "'%s'.\n"
               " \n"
               "Esto comparar� los archivos con las entradas en \n"
               "la base de datos. Si hay una coincidencia, \n"
               "a�adir� una entrada en una colecci�n.\n"
               " \n"
               "Entonces podr�s acceder f�cilmente al contenido\n"
               "si vas a '%s' ->\n"
               "'%s'\n"
               "en vez de tener que pasar por el navegador \n"
               "de archivos constantemente.\n"
               " \n"
               "NOTA: El contenido de algunos n�cleos podr�a\n"
               "no ser localizable. Entre los ejemplos est�n\n"
               "PlayStation, MAME, FBA, y puede que otros."
               ,
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_ADD_CONTENT_LIST),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_SCAN_DIRECTORY),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_SCAN_FILE),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_LOAD_CONTENT_LIST),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CONTENT_COLLECTION_LIST)
               );
         break;
      case MENU_LABEL_VALUE_MENU_CONTROLS_PROLOG:
         snprintf(s, len,
               "Puedes usar estos controles en tu mando\n"
               "o teclado para controlar el men�: \n"
               " \n"
               );
         break;
      case MENU_LABEL_VALUE_EXTRACTING_PLEASE_WAIT:
         strlcpy(s, "Extrayendo, espera, por favor...\n", len);
         break;
      case MENU_LABEL_WELCOME_TO_RETROARCH:
         snprintf(s, len,
               "Bienvenido a RetroArch\n"
               "\n"
               "Para m�s informaci�n ve al men� \n"
			   "de Ayuda.\n"
               );
         break;
      case MENU_LABEL_INPUT_DRIVER:
         driver_hash = menu_hash_calculate(settings->input.driver);

         switch (driver_hash)
         {
            case MENU_LABEL_INPUT_DRIVER_UDEV:
               {
                  /* Work around C89 limitations */
                  char u[501];
                  char t[501];

                  strlcpy(t,
                        "Controlador de entrada udev. \n"
                        " \n"
                        "Este controlador puede funcionar sin X. \n"
                        " \n"
                        "Utiliza la API m�s reciente para joypads \n"
                        "evdec para dar compatibilidad con joysticks. \n"
                        "Permite conexi�n en caliente y force \n"
                        "feedback (si lo admite el dispositivo). \n",
                        sizeof(t));
                  strlcpy(u,
                        " \n"
                        "El controlador lee los eventos evdev para \n"
                        "dar compatibilidad con teclados. Tambi�n \n"
                        "es compatible con retrollamadas de teclado, \n"
                        "ratones y pantallas t�ctiles. \n"
                        " \n"
                        "La mayor�a de las distros tienen los nodos \n"
                        "/dev/input en modo root-only (modo 600). \n"
                        "Puedes configurar una regla udev que los haga \n"
                        "accesibles fuera de la ra�z.", sizeof(u)
                        );

                  strlcat(s, t, len);
                  strlcat(s, u, len);
               }
               break;
            case MENU_LABEL_INPUT_DRIVER_LINUXRAW:
               snprintf(s, len,
                     "Controlador de entrada linuxraw. \n"
                     " \n"
                     "Este controlador necesita de un TTY activo. \n"
                     "Los eventos de teclado se leen directamente \n"
                     "desde el TTY, lo que es m�s simple pero no tan \n"
                     "flexible como udev. \n"
                     "No es compatible con ratones, etc. \n"
                     " \n"
                     "Este controlador utiliza la antigua API de \n"
                     "joysticks (/dev/input/js*).");
               break;
            default:
               snprintf(s, len,
                     "Controlador de entrada.\n"
                     " \n"
                     "El controlador de v�deo podr�a forzar \n"
                     "el uso de un controlador de entrada \n"
                     "distinto.");
               break;
         }
         break;
      case MENU_LABEL_LOAD_CONTENT:
         snprintf(s, len,
               "Cargar contenido. \n"
               "Buscar contenido. \n"
               " \n"
               "Para cargar contenidos necesitas \n"
               "un 'n�cleo' y un archivo de contenido.\n"
               " \n"
               "Para controlar el lugar donde el men� \n"
               "empieza a buscar contenidos, cambia \n"
               "la opci�n 'Carpeta del navegador de \n"
               "archivos'. En caso de que no est� \n"
               "configurada, empezar� desde la ra�z.\n"
               " \n"
               "El navegador filtrar� las extensiones \n"
               "del �ltimo n�cleo seleccionado en \n"
               "'Cargar n�cleo' y lo utilizar� al \n"
               "cargar un contenido."
               );
         break;
      case MENU_LABEL_CORE_LIST:
         snprintf(s, len,
               "Cargar n�cleo. \n"
               " \n"
               "Busca una implementaci�n de n�cleo \n"
               "para libretro. El navegador empezar� \n"
               "desde la ruta de tu carpeta de n�cleos.\n"
               "Si est� en blanco, empezar� desde \n"
               "la ra�z.\n"
               " \n"
               "Si la carpeta de n�cleos es una carpeta,\n"
               "el men� la utilizar� como carpeta \n"
               "base. Si la carpeta de n�cleos es una \n"
               "ruta completa, empezar� en la carpeta \n"
               "donde se encuentre el archivo.");
         break;
      case MENU_LABEL_LOAD_CONTENT_HISTORY:
         snprintf(s, len,
               "Cargar contenido del historial. \n"
               " \n"
               "Cuando se cargan contenidos, estos y \n"
               "las combinaciones de n�cleos de libretro \n"
               "se guardan en el historial. \n"
               " \n"
               "El historial se guarda en un archivo en la \n"
               "misma carpeta que el archivo de configura- \n"
               "ci�n de RetroArch. Si no se ha cargado un \n"
               "archivo de configuraci�n al iniciar, no se \n"
               "guardar� ni cargar� el historial, y la \n"
               "opci�n no existir� en el men� principal."
               );
         break;
      case MENU_LABEL_VIDEO_DRIVER:
         driver_hash = menu_hash_calculate(settings->video.driver);

         switch (driver_hash)
         {
            case MENU_LABEL_VIDEO_DRIVER_GL:
               snprintf(s, len,
                     "Controlador de v�deo OpenGL. \n"
                     " \n"
                     "Este controlador permite que los n�cleos \n"
                     "libretro GL se utilicen, adem�s de las \n"
                     "implementaciones renderizadas por\n"
                     "software del n�cleo.\n"
                     " \n"
                     "El rendimiento de las implementaciones \n"
                     "por software y libretro GL dependen \n"
                     "del controlador GL que tenga tu \n"
                     "tarjeta gr�fica.");
               break;
            case MENU_LABEL_VIDEO_DRIVER_SDL2:
               snprintf(s, len,
                     "Controlador de v�deo SDL 2.\n"
                     " \n"
                     "Este es un controlador de v�deo por \n"
                     "software SDL 2.\n"
                     " \n"
                     "El rendimiento para las implementaciones \n"
                     "libretro por software depende de la \n"
                     "implementaci�n SDL de tu plataforma.");
               break;
            case MENU_LABEL_VIDEO_DRIVER_SDL1:
               snprintf(s, len,
                     "Controlador de v�deo SDL.\n"
                     " \n"
                     "Este es un controlador de v�deo por \n"
                     "software SDL 1.2.\n"
                     " \n"
                     "Su rendimiento es considerado inferior \n"
                     "a lo �ptimo. Util�zalo �nicamente como \n"
                     "�ltimo recurso.");
               break;
            case MENU_LABEL_VIDEO_DRIVER_D3D:
               snprintf(s, len,
                     "Controlador de v�deo Direct3D. \n"
                     " \n"
                     "El rendimiento de los n�cleos que \n"
                     "rendericen por software depender� \n"
                     "del controlador D3D de tu tarjeta \n"
                     "gr�fica.");
               break;
            case MENU_LABEL_VIDEO_DRIVER_EXYNOS:
               snprintf(s, len,
                     "Controlador de v�deo Exynos-G2D. \n"
                     " \n"
                     "Este es un controlador de v�deo Exynos \n"
                     "de bajo nivel. Utiliza el bloque G2D \n"
                     "del SoC Exynos de Samsung para las \n"
                     "operaciones de blit. \n"
                     " \n"
                     "El rendimiento de los n�cleos \n"
                     "renderizados por software deber�a \n"
                     "ser �ptimo.");
               break;
            case MENU_LABEL_VIDEO_DRIVER_SUNXI:
               snprintf(s, len,
                     "Controlador de v�deo Sunxi-G2D. \n"
                     " \n"
                     "Este es un controlador de v�deo Sunxi \n"
                     "de bajo nivel. Utiliza el bloque G2D \n"
                     "de todos los SoC Allwinner.");
               break;
            default:
               snprintf(s, len,
                     "Controlador de v�deo actual.");
               break;
         }
         break;
      case MENU_LABEL_AUDIO_DSP_PLUGIN:
         snprintf(s, len,
               "Plugin de sonido DSP.\n"
               "Procesa el sonido antes de enviarlo \n"
               "al controlador."
               );
         break;
      case MENU_LABEL_AUDIO_RESAMPLER_DRIVER:
         driver_hash = menu_hash_calculate(settings->audio.resampler);

         switch (driver_hash)
         {
            case MENU_LABEL_AUDIO_RESAMPLER_DRIVER_SINC:
               snprintf(s, len,
                     "Implementaci�n SINC en ventana.");
               break;
            case MENU_LABEL_AUDIO_RESAMPLER_DRIVER_CC:
               snprintf(s, len,
                     "Implementaci�n de cosenos complejos.");
               break;
         }
         break;
      case MENU_LABEL_VIDEO_SHADER_PRESET:
         snprintf(s, len,
               "Cargar preajustes de shaders. \n"
               " \n"
               " Carga directamente un preajuste "
#ifdef HAVE_CG
               "Cg"
#endif
#ifdef HAVE_GLSL
#ifdef HAVE_CG
               "/"
#endif
               "GLSL"
#endif
#ifdef HAVE_HLSL
#if defined(HAVE_CG) || defined(HAVE_HLSL)
               "/"
#endif
               "HLSL"
#endif
               ". \n"
               "El men� de shaders se actualizar�. \n"
               " \n"
               "Si el CGP utiliza m�todos de escalado \n"
               "complejos (por ejemplo, escalado de \n"
               "origen, el mismo factor para X/Y), podr�a \n"
               "no mostrar un factor de escalado correcto \n"
               "en el men�."
               );
         break;
      case MENU_LABEL_VIDEO_SHADER_SCALE_PASS:
         snprintf(s, len,
               "La escala de esta pasada. \n"
               " \n"
               "El factor de escala es acumulativo, \n"
               "por ejemplo: 2x para el primer pase y \n"
               "2x para el segundo dar� una escala \n"
               "total de 4x. \n"
               " \n"
               "Si el �ltimo pase tiene un factor de \n"
               "escala, el resultado se estirar� por \n"
               "toda la pantalla con el filtro espe- \n"
               "cificado en 'Filtro predeterminado'. \n"
               " \n"
               "Si has seleccionado 'No importa', se \n"
               "utilizar� o bien la escala 1x o se \n"
               "estirar� a pantalla completa en funci�n \n"
               "de si es el �ltimo pase o no."
               );
         break;
      case MENU_LABEL_VIDEO_SHADER_NUM_PASSES:
         snprintf(s, len,
               "Pases de shaders. \n"
               " \n"
               "RetroArch permite mezclar diversos shaders \n"
               "con pasadas arbitrarias, filtros persona- \n"
               "lizados de hardware y factores de escala. \n"
               " \n"
               "Esta opci�n especifica la cantidad de pasadas \n"
               "de shaders a utilizar. Si seleccionas 0 y \n"
               "luego 'Aplicar cambios en shaders', \n"
               "utilizar�s un shader 'en blanco'. \n"
               " \n"
               "La opci�n filtro predeterminado afectar� \n"
               "al filtro de estiramiento.");
         break;
      case MENU_LABEL_VIDEO_SHADER_PARAMETERS:
         snprintf(s, len,
               "Par�metros de shaders. \n"
               " \n"
               "Modifica directamente el shader actual. \n"
               "No se guardar� en el preajuste CGP/GLSLP.");
         break;
      case MENU_LABEL_VIDEO_SHADER_PRESET_PARAMETERS:
         snprintf(s, len,
               "Par�metros del preajuste de shaders. \n"
               " \n"
               "Modifica el preajuste de shaders que \n"
               "se encuentra actualmente en el men�."
               );
         break;
      case MENU_LABEL_VIDEO_SHADER_PASS:
         snprintf(s, len,
               "Ruta al shader. \n"
               " \n"
               "Todos los shaders deben ser del \n"
               "mismo tipo (por ejemplo: CG, GLSL \n"
               "o HLSL). \n"
               " \n"
               "Modifica la carpeta de shaders para \n"
               "indicar la ruta en la que el navegador \n"
               "comenzar� a buscar shaders."
               );
         break;
      case MENU_LABEL_CONFIG_SAVE_ON_EXIT:
         snprintf(s, len,
               "Guarda la configuraci�n al salir.\n"
               "Es �til para el men�, ya que los \n"
               "ajustes pueden ser modificados. \n"
               "Sobrescribir� la configuraci�n. \n"
               " \n"
               "No se preservar�n los #include \n"
               "y los comentarios. \n"
               " \n"
               "El archivo de configuraci�n se \n"
               "considera inmutable por dise�o \n"
               "ya que es probable que el usuario \n"
               "lo controle, y no deber�a ser \n"
               "sobrescrito sin que el usuario \n"
               "no se entere."
#if defined(RARCH_CONSOLE) || defined(RARCH_MOBILE)
               "\nSin embargo, no suele ser el caso \n"
               "en consolas, donde no es posible \n"
               "examinar el archivo de configuraci�n \n"
               "de forma manual."
#endif
               );
         break;
      case MENU_LABEL_VIDEO_SHADER_FILTER_PASS:
         snprintf(s, len,
               "Filtro de hardware para esta pasada. \n"
               " \n"
               "Si se ha seleccionado 'No importa', \n"
               "se utilizar� el filtro predeterminado."
               );
         break;
      case MENU_LABEL_AUTOSAVE_INTERVAL:
         snprintf(s, len,
               "Guarda autom�ticamente la SRAM \n"
               "no vol�til en intervalos regulares.\n"
               " \n"
               "Esta opci�n est� desactivada por \n"
               "defecto a menos que se haya configurado \n"
               "lo contrario. El intervalo se mide \n"
               "en segundos. \n"
               " \n"
               "Si utlizas 0, desactivar�s el \n"
               "guardado autom�tico.");
         break;
      case MENU_LABEL_INPUT_BIND_DEVICE_TYPE:
         snprintf(s, len,
               "Tipo de dispositivo de entrada. \n"
               " \n"
               "Selecciona el tipo de dispositivo a \n"
               "utilizar. Es relevante para el n�cleo \n"
               "de libretro."
               );
         break;
      case MENU_LABEL_LIBRETRO_LOG_LEVEL:
         snprintf(s, len,
               "Ajusta el nivel de registro de los \n"
               "n�cleos de libretro (GET_LOG_INTERFACE). \n"
               " \n"
               " Si un nivel de registro indicado por \n"
               " un n�cleo de libretro est� por debajo \n"
               " del nivel indicado en libretro_log, \n"
               " ser� ignorado.\n"
               " \n"
               " Los registros DEBUG siempre son ignorados \n"
               " a menos que est� activado el modo \n"
               " de verbosidad (--verbose).\n"
               " \n"
               " DEBUG = 0\n"
               " INFO  = 1\n"
               " WARN  = 2\n"
               " ERROR = 3"
               );
         break;
      case MENU_LABEL_STATE_SLOT_INCREASE:
      case MENU_LABEL_STATE_SLOT_DECREASE:
         snprintf(s, len,
               "Ranuras de guardados r�pidos.\n"
               " \n"
               " Si se selecciona la ranura 0, el nombre \n"
               " del guardado r�pido es *.state \n"
               " (o lo que est� definido en la l�nea \n"
               "de comandos).\n"
               "Si la ranura es != 0, la ruta ser� (path)(d), \n"
               "siendo (d) el n�mero de ranura.");
         break;
      case MENU_LABEL_SHADER_APPLY_CHANGES:
         snprintf(s, len,
               "Aplicar cambios en shaders. \n"
               " \n"
               "Despu�s de cambiar los ajustes de shaders, \n"
               "utiliza esta opci�n para aplicar los \n"
               "cambios. \n"
               " \n"
               "Los cambios en los shaders es una \n"
               "operaci�n que ocupa bastante memoria, \n"
               "as� que han de hacerse de forma \n"
               "expl�cita. \n"
               " \n"
               "Al aplicar shaders, se guarda su configuraci�n \n"
               "a un archivo temporal (menu.cgp ormenu.glslp) \n"
               "y se cargan. El archivo persistir� al \n"
               "abandonar RetroArch. El archivo se guarda en \n"
               "la carpeta Shader."
               );
         break;
      case MENU_LABEL_INPUT_BIND_DEVICE_ID:
         snprintf(s, len,
               "Dispositivo de entrada. \n"
               " \n"
               "Selecciona el mando a utilizar por \n"
               "el usuario N. \n"
               "El nombre del mando est� disponible."
               );
         break;
      case MENU_LABEL_MENU_TOGGLE:
         snprintf(s, len,
               "Muestra u oculta el men�.");
         break;
      case MENU_LABEL_GRAB_MOUSE_TOGGLE:
         snprintf(s, len,
               "Permite o no capturar el rat�n.\n"
               " \n"
               "Al capturar el rat�n, RetroArch lo oculta \n"
               "y mantiene el puntero del rat�n dentro de \n"
               "la ventana para que la entrada relativa \n"
               "del rat�n funcione mejor.");
         break;
      case MENU_LABEL_DISK_NEXT:
         snprintf(s, len,
               "Rota entre las im�genes de disco. \n"
               "Utiliza esta opci�n despu�s de \n"
               "expulsar un disco. \n"
               " \n"
               " Termina la operaci�n volviendo \n"
               " a conmutar el bot�n de expulsi�n.");
         break;
      case MENU_LABEL_VIDEO_FILTER:
#ifdef HAVE_FILTERS_BUILTIN
         snprintf(s, len,
               "Filtro de v�deo basado en la CPU.");
#else
         snprintf(s, len,
               "Filtro de v�deo basado en la CPU.\n"
               " \n"
               "Ruta a una librer�a din�mica.");
#endif
         break;
      case MENU_LABEL_AUDIO_DEVICE:
         snprintf(s, len,
               "Anula el dispositivo de sonido \n"
               "predeterminado que utiliza el \n"
               "controlador de sonido.\n"
               "Esta opci�n depende del contro- \n"
               "lador. Por ejemplo:\n"
#ifdef HAVE_ALSA
               " \n"
               "ALSA necesita un dispositivo PCM."
#endif
#ifdef HAVE_OSS
               " \n"
               "OSS necesita una ruta (por ejemplo: \n"
               "/dev/dsp)."
#endif
#ifdef HAVE_JACK
               " \n"
               "JACK necesita nombres de puertos \n"
               "(por ejemplo: system:playback1\n"
               ",system:playback_2)."
#endif
#ifdef HAVE_RSOUND
               " \n"
               "RSound necesita una direcci�n IP \n"
               "de un servidor RSound."
#endif
               );
         break;
      case MENU_LABEL_DISK_EJECT_TOGGLE:
         snprintf(s, len,
               "Alterna el bot�n de expulsi�n \n"
               "de discos.\n"
               " \n"
               "Se utiliza para contenidos \n"
               "que utilicen varios discos.");
         break;
      case MENU_LABEL_ENABLE_HOTKEY:
         snprintf(s, len,
               "Activa otras teclas r�pidas.\n"
               " \n"
               "Si esta tecla r�pida est� asignada a un \n"
               "teclado, un bot�n o un eje de un joystick, \n"
               "el resto de teclas r�pidas se desactivar�n \n"
               "a menos que esta tecla se mantenga pulsada \n"
               "al mismo tiempo. \n"
               " \n"
               "Esto sirve para implementaciones centradas \n"
               "en RETRO_KEYBOARD, que ocupan una gran \n"
               "porci�n del teclado y no es una buena idea \n"
               "que las teclas r�pidas interfieran con \n"
               "el funcionamiento normal.");
         break;
      case MENU_LABEL_REWIND_ENABLE:
         snprintf(s, len,
               "Activa el rebobinado.\n"
               " \n"
               "Esto consume rendimiento, as� que \n"
               "est� desactivado por defecto.");
         break;
      case MENU_LABEL_LIBRETRO_DIR_PATH:
         snprintf(s, len,
               "Carpeta de n�cleos. \n"
               " \n"
               "Una carpeta donde se buscar�n las \n"
               "implementaciones de n�cleos libretro.");
         break;
      case MENU_LABEL_VIDEO_REFRESH_RATE_AUTO:
         {
            /* Work around C89 limitations */
            char u[501];
            char t[501];

            strlcpy(t,
                  "Frecuencia de actualizaci�n autom�tica.\n"
                  " \n"
                  "La frecuencia de actualizaci�n precisa del \n"
                  "monitor (en Hz). Se utiliza para calcular \n"
                  "la frecuencia de entrada de sonido con esta \n"
                  "f�rmula: \n"
                  " \n"
                  "audio_input_rate = veloc. de entrada de juego \n"
                  "* frecuencia de actualizaci�n de pantalla / \n"
                  "frecuencia de actualizaci�n de juego \n"
                  " \n", sizeof(t));
            strlcpy(u,
                  "Si la implementaci�n no indica un valor, se \n"
                  "asumir� de forma predeterminada el sistema \n"
                  "NTSC por compatibilidad.\n"
                  " \n"
                  "Este valor deber�a ser lo m�s similar a 60Hz \n"
                  "para evitar cambios en el tono. Si tu \n"
                  "monitor no funciona a 60Hz o similar, \n"
                  "desactiva la sincron�a vertical y deja \n"
                  "esta opci�n en su valor predeterminado.", sizeof(u));
            strlcat(s, t, len);
            strlcat(s, u, len);
         }
         break;
      case MENU_LABEL_VIDEO_ROTATION:
         snprintf(s, len,
               "Fuerza una rotaci�n concreta \n"
               "de la pantalla.\n"
               " \n"
               "La rotaci�n se a�ade a las rotaciones \n"
               "que indique el n�cleo libretro (ver \n"
               "Permitir rotaci�n de v�deo).");
         break;
      case MENU_LABEL_VIDEO_SCALE:
         snprintf(s, len,
               "Resoluci�n en pantalla completa.\n"
               " \n"
               "Si se indica 0, se utilizar� la \n"
               "resoluci�n del entorno.\n");
         break;
      case MENU_LABEL_FASTFORWARD_RATIO:
         snprintf(s, len,
               "Proporci�n de aceleraci�n."
               " \n"
               "Indica la velocidad m�xima a la que \n"
               "se ejecutar� el contenido mientras \n"
               "es acelerado.\n"
               " \n"
               " (Por ejemplo, 5.0 para un contenido \n"
               "a 60 FPS => M�ximo de 300 FPS).\n"
               " \n"
               "RetroArch pasar� a segundo plano \n"
               "para asegurarse de que no se supere \n"
               "la velocidad m�xima. Este l�mite \n"
               "no es completamente preciso.");
         break;
      case MENU_LABEL_VIDEO_MONITOR_INDEX:
         snprintf(s, len,
               "Monitor preferido.\n"
               " \n"
               "0 (predeterminado) significa que no hay \n"
               "preferencia por un monitor en concreto, \n"
               "y de 1 en adelante (siendo 1 el primer \n"
               "monitor) sugiere a RetroArch que \n"
               "utilice ese monitor espec�fico.");
         break;
      case MENU_LABEL_VIDEO_CROP_OVERSCAN:
         snprintf(s, len,
               "Obliga a recortar los fotogramas \n"
               "con overscan.\n"
               " \n"
               "El comportamiento exacto de esta \n"
               "opci�n depende de la implementaci�n \n"
               "del n�cleo.");
         break;
      case MENU_LABEL_VIDEO_SCALE_INTEGER:
         snprintf(s, len,
               "Solo escala el v�deo con n�meros \n"
               "enteros.\n"
               " \n"
               "El tama�o base depende de la geometr�a \n"
               "que indique el sistema y de la \n"
               "proporci�n de aspecto.\n"
               " \n"
               "Si no se ha configurado la opci�n \n"
               "Forzar proporci�n, los valores X/Y \n"
               "se escalar�n de forma independiente \n"
               "seg�n n�meros enteros.");
         break;
      case MENU_LABEL_AUDIO_VOLUME:
         snprintf(s, len,
               "Volumen de sonido expresado en dB.\n"
               " \n"
               "0 dB es el volumen normal, sin ganancia \n"
               "aplicada. La ganancia se puede controlar \n"
               "en tiempo real con los botones de \n"
               "Subir volumen / Bajar volumen.");
         break;
      case MENU_LABEL_AUDIO_RATE_CONTROL_DELTA:
         snprintf(s, len,
               "Control de la frecuencia de sonido.\n"
               " \n"
               "Si seleccionas 0, desactivar�s el control \n"
               "de la frecuencia. Cualquier otro valor \n"
               "cambiar� el delta de control de la \n"
               "frecuencia de sonido.\n"
               " \n"
               "Define cu�nta frecuencia de entrada puede \n"
               "ajustarse de forma din�mica.\n"
               " \n"
               " La frecuencia de entrada se define como: \n"
               " frecuencia de entrada * (1.0 +/- \n"
               "(delta de control de frecuencia))");
         break;
      case MENU_LABEL_AUDIO_MAX_TIMING_SKEW:
         snprintf(s, len,
               "Variaci�n m�xima en la sincron�a de \n"
               "sonido.\n"
               " \n"
               "Define la variaci�n m�xima de la \n"
               "frecuencia de entrada. Podr�as aumentar \n"
               "el valor para cambiar la sincron�a, por \n"
               "ejemplo, si ejecutas n�cleos PAL en \n"
               "monitores NTSC, a cambio de tener un \n"
               "tono de sonido impreciso.\n"
               " \n"
               " La frecuencia de entrada se define como: \n"
               " frecuencia de entrada * (1.0 +/- \n"
               "(variaci�n m�xima de sincron�a))");
         break;
      case MENU_LABEL_OVERLAY_NEXT:
         snprintf(s, len,
               "Cambia a la siguiente superposici�n.\n"
               " \n"
               "Se expande alrededor.");
         break;
      case MENU_LABEL_LOG_VERBOSITY:
         snprintf(s, len,
               "Activa o desactiva el nivel de \n"
               "verbosidad del frontend.");
         break;
      case MENU_LABEL_VOLUME_UP:
         snprintf(s, len,
               "Aumenta el volumen del sonido.");
         break;
      case MENU_LABEL_VOLUME_DOWN:
         snprintf(s, len,
               "Disminuye el volumen del sonido.");
         break;
      case MENU_LABEL_VIDEO_DISABLE_COMPOSITION:
         snprintf(s, len,
               "Desactiva por la fuerza la composici�n.\n"
               "Actualmente solo funciona en \n"
               "Windows Vista/7.");
         break;
      case MENU_LABEL_PERFCNT_ENABLE:
         snprintf(s, len,
               "Activa o desactiva los contadores \n"
               "de rendimiento del frontend.");
         break;
      case MENU_LABEL_SYSTEM_DIRECTORY:
         snprintf(s, len,
               "Carpeta de sistemas. \n"
               " \n"
               "Asigna la carpeta 'system'.\n"
               "Los n�cleos pueden buscar dentro \n"
               "de ella para cargar BIOS, \n"
               "configuraciones espec�ficas para \n"
               "un sistema, etc.");
         break;
      case MENU_LABEL_SAVESTATE_AUTO_SAVE:
         snprintf(s, len,
               "Guarda autom�ticamente un guardado r�pido \n"
               "al terminar la ejecuci�n de RetroArch.\n"
               " \n"
               "RetroArch cargar� autom�ticamente cualquier \n"
               "guardado r�pido que se encuentre en esa ruta \n"
               "al arrancar si la opci�n 'Cargar guardado \n"
               "r�pido autom�ticamente' est� activada.");
         break;
      case MENU_LABEL_VIDEO_THREADED:
         snprintf(s, len,
               "Utilizar un controlador de v�deo \n"
               "por hilos.\n"
               " \n"
               "Esta opci�n podr�a mejorar el rendimiento \n"
               "a costa de ganar retraso y perder fluidez \n"
               "de v�deo.");
         break;
      case MENU_LABEL_VIDEO_VSYNC:
         snprintf(s, len,
               "Sincron�a vertical para v�deo.\n");
         break;
      case MENU_LABEL_VIDEO_HARD_SYNC:
         snprintf(s, len,
               "Intenta sincronizar la CPU y la GPU \n"
               "de una forma m�s estricta.\n"
               " \n"
               "Puede reducir el retraso a costa \n"
               "de perder rendimiento.");
         break;
      case MENU_LABEL_REWIND_GRANULARITY:
         snprintf(s, len,
               "Nivel de detalle del rebobinado.\n"
               " \n"
               "Cuando se rebobina un n�mero determinado \n"
               "de fotogramas, puedes rebobinar varios \n"
               "fotogramas a la vez, aumentando la \n"
               "velocidad de rebobinado.");
         break;
      case MENU_LABEL_SCREENSHOT:
         snprintf(s, len,
               "Capturar la pantalla.");
         break;
      case MENU_LABEL_VIDEO_FRAME_DELAY:
         snprintf(s, len,
               "Ajusta la cantidad de retraso en \n"
               "milisegundos despu�s de la sincron�a \n"
               "vertical antes de ejecutar el n�cleo.\n"
               "\n"
               "Puede reducir el retraso a costa de \n"
               "aumentar la probabilidad de perder \n"
               "fluidez.\n"
               " \n"
               "El valor m�ximo es 15.");
         break;
      case MENU_LABEL_VIDEO_HARD_SYNC_FRAMES:
         snprintf(s, len,
               "Ajusta la cantidad de fotogramas \n"
               "que puede adelantar la CPU a la GPU \n"
               "al utilizar 'Sincron�a estricta \n"
               "de CPU'.\n"
               " \n"
               "El valor m�ximo es 3.\n"
               " \n"
               " 0: Sincroniza inmediatamente a GPU.\n"
               " 1: Sincroniza con el fotograma anterior.\n"
               " 2: Etc...");
         break;
      case MENU_LABEL_VIDEO_BLACK_FRAME_INSERTION:
         snprintf(s, len,
               "Introduce un fotograma negro \n"
               "entre fotogramas.\n"
               " \n"
               "Es �til para los monitores que \n"
               "funcionan a 120 Hz y quieren jugar \n"
               "a material a 60 Hz eliminando el \n"
               "ghosting.\n"
               " \n"
               "La frecuencia de actualizaci�n de v�deo \n"
               "a�n debe configurarse como si utilizaras \n"
               "un monitor de 60 Hz (divide la frecuencia \n"
               "de actualizaci�n entre 2).");
         break;
      case MENU_LABEL_RGUI_SHOW_START_SCREEN:
         snprintf(s, len,
               "Muestra la pantalla de inicio en el men�.\n"
               "Se desactiva autom�ticamente cuando se \n"
               "ha visto por primera vez.\n"
               " \n"
               "Esta opci�n solo se actualiza dentro \n"
               "de la configuraci�n si 'Guardar \n"
               "configuraci�n al salir' est� activado.\n");
         break;
      case MENU_LABEL_CORE_SPECIFIC_CONFIG:
         snprintf(s, len,
               "Carga un archivo de configuraci�n \n"
               "concreto en funci�n del n�cleo \n"
               "que se vaya a utilizar.\n");
         break;
      case MENU_LABEL_VIDEO_FULLSCREEN:
         snprintf(s, len, 
                "Activa o desactiva el modo \n"
                "en pantalla completa.");
         break;
      case MENU_LABEL_BLOCK_SRAM_OVERWRITE:
         snprintf(s, len,
               "Impide que la SRAM sea sobrescrita \n"
               "mientras se carga un guardado r�pido.\n"
               " \n"
               "Podr�a provocar defectos en los \n"
               "juegos.");
         break;
      case MENU_LABEL_PAUSE_NONACTIVE:
         snprintf(s, len,
               "Pausa el juego cuando la ventana \n"
               "pasa a estar en segundo plano.");
         break;
      case MENU_LABEL_VIDEO_GPU_SCREENSHOT:
         snprintf(s, len,
               "Si es posible, las capturas de \n"
               "pantalla se realizan del material \n"
               "pos-shaders de GPU.");
         break;
      case MENU_LABEL_SCREENSHOT_DIRECTORY:
         snprintf(s, len,
               "Carpeta de capturas de pantalla. \n"
               " \n"
               "En esta carpeta se guardar�n \n"
               "las capturas de pantalla."
               );
         break;
      case MENU_LABEL_VIDEO_SWAP_INTERVAL:
         snprintf(s, len,
               "Intervalo de cambio de la \n"
               "sincron�a vertical.\n"
               " \n"
               "Utiliza un intervalo personalizado para \n"
               "la sincron�a vertical. Utiliza este \n"
               "par�metro para reducir a la mitad \n"
               "la frecuencia de actualizaci�n \n"
               "del monitor.");
         break;
      case MENU_LABEL_SAVEFILE_DIRECTORY:
         snprintf(s, len,
               "Carpeta de partidas guardadas. \n"
               " \n"
               "Almacena todas las partidas guardadas \n"
               "(*.srm) en esta carpeta. Incluye los \n"
               "archivos relacionados, como .bsv, .rt, \n"
               ".psrm, etc�tera.\n"
               " \n"
               "Esta carpeta ser� ignorada si se \n"
               "utilizan expl�citamente las opciones por \n"
               "l�nea de comandos.");
         break;
      case MENU_LABEL_SAVESTATE_DIRECTORY:
         snprintf(s, len,
               "Carpeta de guardados r�pidos. \n"
               " \n"
               "Almacena todos los guardados r�pidos \n"
               "(*.state) en esta carpeta.\n"
               " \n"
               "Esta carpeta ser� ignorada si se \n"
               "utilizan expl�citamente las opciones por \n"
               "l�nea de comandos.");
         break;
      case MENU_LABEL_ASSETS_DIRECTORY:
         snprintf(s, len,
               "Carpeta de recursos. \n"
               " \n"
               "Esta ubicaci�n es donde las interfaces \n"
               "de men�s buscan de forma predeterminada \n"
               "los recursos a cargar, etc.");
         break;
      case MENU_LABEL_DYNAMIC_WALLPAPERS_DIRECTORY:
         snprintf(s, len,
               "Carpeta de fondos din�micos de pantalla. \n"
               " \n"
               "Aqu� se almacenan los fondos de pantalla \n"
               "que el men� cargar� de forma din�mica \n"
               "seg�n el contexto.");
         break;
      case MENU_LABEL_SLOWMOTION_RATIO:
         snprintf(s, len,
               "Cantidad de velocidad reducida."
               " \n"
               "Al reducir la velocidad, el contenido \n"
               "se ralentizar� seg�n este factor.");
         break;
      case MENU_LABEL_INPUT_AXIS_THRESHOLD:
         snprintf(s, len,
               "Define el margen de los ejes.\n"
               " \n"
               "Indica la distancia m�nima que debe \n"
               "recorrer un eje para que provoque \n"
               "una pulsaci�n del bot�n.\n"
               "Los valores posibles son [0.0, 1.0].");
         break;
      case MENU_LABEL_INPUT_TURBO_PERIOD:
         snprintf(s, len, 
               "Per�odo de turbo.\n"
               " \n"
               "Describe la velocidad con la que se \n"
               "pulsan los botones en el modo turbo."
               );
         break;
      case MENU_LABEL_INPUT_AUTODETECT_ENABLE:
         snprintf(s, len,
               "Activa la autoconfiguraci�n de \n"
               "entrada. \n"
               " \n"
               "Intentar� configurar de forma \n"
               "autom�tica los mandos que se \n"
               "conecten en cualquier momento, \n"
               "como el sistema Plug-and-play.");
         break;
      case MENU_LABEL_CAMERA_ALLOW:
         snprintf(s, len,
               "Permite o no el acceso a la c�mara \n"
               "por parte de los n�cleos.");
         break;
      case MENU_LABEL_LOCATION_ALLOW:
         snprintf(s, len,
               "Permite o no los servicios de \n"
               "localizaci�n por parte de los n�cleos.");
         break;
      case MENU_LABEL_TURBO:
         snprintf(s, len,
               "Permite usar el turbo.\n"
               " \n"
               "Si mantienes pulsado el bot�n de turbo \n"
               "mientras pulsas otro bot�n, este �ltimo \n"
               "entrar� en un modo turbo en el que el \n"
               "estado del bot�n se modula mediante una \n"
               "se�al peri�dica. \n"
               " \n"
               "La modulaci�n se detiene cuando se deja \n"
               "de pulsar el bot�n original (no el bot�n \n"
               "de turbo).");
         break;
      case MENU_LABEL_OSK_ENABLE:
         snprintf(s, len,
               "Activa o desactiva el teclado \n"
               "en pantalla.");
         break;
      case MENU_LABEL_AUDIO_MUTE:
         snprintf(s, len,
               "Silencia o no el sonido.");
         break;
      case MENU_LABEL_REWIND:
         snprintf(s, len,
               "Mant�n pulsado este bot�n para rebobinar.\n"
               " \n"
               "Para que este bot�n funcione, debes tener \n"
               "activada la opci�n de rebobinar.");
         break;
      case MENU_LABEL_EXIT_EMULATOR:
         snprintf(s, len,
               "Asigna una tecla para abandonar \n"
               "RetroArch limpiamente."
#if !defined(RARCH_MOBILE) && !defined(RARCH_CONSOLE)
               "\nSi cierras el programa con \n"
               "cualquier forma brusca (SIGKILL, \n"
               "etc.) no se guardar� la RAM, etc. \n"
               "En sistemas Unix, SIGINT/SIGTERM \n"
               "permite un cierre limpio."
#endif
               );
         break;
      case MENU_LABEL_LOAD_STATE:
         snprintf(s, len,
               "Carga el guardado r�pido.");
         break;
      case MENU_LABEL_SAVE_STATE:
         snprintf(s, len,
               "Guarda r�pidamente la partida.");
         break;
      case MENU_LABEL_NETPLAY_FLIP_PLAYERS:
         snprintf(s, len,
               "Cambia los usuarios en red.");
         break;
      case MENU_LABEL_CHEAT_INDEX_PLUS:
         snprintf(s, len,
               "Aumenta el �ndice de trucos.\n");
         break;
      case MENU_LABEL_CHEAT_INDEX_MINUS:
         snprintf(s, len,
               "Disminuye el �ndice de trucos.\n");
         break;
      case MENU_LABEL_SHADER_PREV:
         snprintf(s, len,
               "Aplica el shader anterior de la carpeta.");
         break;
      case MENU_LABEL_SHADER_NEXT:
         snprintf(s, len,
               "Aplica el siguiente shader de la carpeta.");
         break;
      case MENU_LABEL_RESET:
         snprintf(s, len,
               "Reinicia el contenido.\n");
         break;
      case MENU_LABEL_PAUSE_TOGGLE:
         snprintf(s, len,
               "Alterna el estado en pausa.");
         break;
      case MENU_LABEL_CHEAT_TOGGLE:
         snprintf(s, len,
               "Alterna el �ndice de trucos.\n");
         break;
      case MENU_LABEL_HOLD_FAST_FORWARD:
         snprintf(s, len,
               "Mant�n pulsado este bot�n para avanzar \n"
               "r�pidamente. Su�ltalo para desactivar \n"
               "esta funci�n.");
         break;
      case MENU_LABEL_SLOWMOTION:
         snprintf(s, len,
               "Mant�n pulsado este bot�n para \n"
               "ir a c�mara lenta.");
         break;
      case MENU_LABEL_FRAME_ADVANCE:
         snprintf(s, len,
               "Avanza un fotograma mientras \n"
               "el contenido est� en pausa.");
         break;
      case MENU_LABEL_MOVIE_RECORD_TOGGLE:
         snprintf(s, len,
               "Activa o desactiva la grabaci�n.");
         break;
      case MENU_LABEL_L_X_PLUS:
      case MENU_LABEL_L_X_MINUS:
      case MENU_LABEL_L_Y_PLUS:
      case MENU_LABEL_L_Y_MINUS:
      case MENU_LABEL_R_X_PLUS:
      case MENU_LABEL_R_X_MINUS:
      case MENU_LABEL_R_Y_PLUS:
      case MENU_LABEL_R_Y_MINUS:
         snprintf(s, len,
               "El eje de un joystick anal�gico \n"
               "(estilo DualShock).\n"
               " \n"
               "Se asigna como siempre, sin embargo, si se \n"
               "asigna un eje anal�gico real, puede leerse \n"
               "de forma anal�gica.\n"
               " \n"
               "El eje X positivo es hacia la derecha. \n"
               "El eje Y positivo es hacia abajo.");
         break;
      case MENU_LABEL_VALUE_WHAT_IS_A_CORE_DESC:
         snprintf(s, len,
               "RetroArch, por si solo, no hace nada. \n"
               " \n"
               "Para que haga algo necesitas cargar \n"
               "un programa en �l. \n"
               "\n"
               "Llamamos a estos programas 'n�cleos \n"
               "libretro', o 'n�cleos' para abreviar. \n"
               " \n"
               "Para cargar un n�cleo, selecciona uno \n"
               "en 'Cargar n�cleo'. \n"
               " \n"
#ifdef HAVE_NETWORKING
               "Puedes conseguir n�cleos de varias formas: \n"
               "* Desc�rgalos en:\n"
               "'%s' -> '%s'.\n"
               "* C�pialos manualmente a:\n"
               "'%s'.",
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_ONLINE_UPDATER),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CORE_UPDATER_LIST),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_LIBRETRO_DIR_PATH)
#else
               "Puedes conseguir n�cleos si los\n"
               "trasladas a mano en la carpeta\n"
               "'%s'.",
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_LIBRETRO_DIR_PATH)
#endif
               );
         break;
      case MENU_LABEL_VALUE_HELP_CHANGE_VIRTUAL_GAMEPAD_DESC:
         snprintf(s, len,
               "Puedes cambiar la superposici�n del \n"
               "mando virtual si vas a '%s' \n"
               "-> '%s'."
               " \n"
               "Desde ah� puedes cambiar la superposici�n, \n"
               "el tama�o y opacidad de sus botones, etc.\n"
               " \n"
               "NOTA: Las superposiciones de mandos \n"
               "virtuales est�n ocultas de forma \n"
               "predeterminada si est�s dentro del men�. \n"
               "Si quieres cambiar este comportamiento, \n"
               "cambia '%s' a Desactivado/false.",
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_SETTINGS),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_OVERLAY_SETTINGS),
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_INPUT_OVERLAY_HIDE_IN_MENU)
               );
         break;
      default:
         if (s[0] == '\0')
            strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NO_INFORMATION_AVAILABLE), len);
         return -1;
   }

   return 0;
}
