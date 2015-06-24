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

 /* IMPORTANT:
  * For non-english characters to work without proper unicode support,
  * we need this file to be encoded in ISO 8859-1 (Latin1), not UTF-8.
  * If you save this file as UTF-8, you'll break non-english characters
  * (e.g. German "Umlauts" and Portugese diacritics).
 */
extern const char encoding_test[sizeof("�")==2 ? 1 : -1];

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
      case MENU_LABEL_VALUE_VIDEO_MESSAGE_POS_X:
         return "Posi��o X da Mensagem de Tela";
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
      case MENU_LABEL_VALUE_JOYPAD_AUTOCONFIG_DIR:
         return "Dir. de Config. Autom�tica de Dispositivos de Entrada";
      case MENU_LABEL_VALUE_RECORDING_CONFIG_DIRECTORY:
         return "Dir. de Config. de Grava��o";
      case MENU_LABEL_VALUE_RECORDING_OUTPUT_DIRECTORY:
         return "Dir. de Grava��es";
      case MENU_LABEL_VALUE_SCREENSHOT_DIRECTORY:
         return "Dir. de Capturas de Telas";
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
      case MENU_LABEL_VALUE_RECORD_USE_OUTPUT_DIRECTORY:
         return "Usar Dir. de Grava��es";
      case MENU_LABEL_VALUE_RECORD_CONFIG:
         return "Configurar Grava��o";
      case MENU_LABEL_VALUE_VIDEO_POST_FILTER_RECORD:
         return "Ativar Filtro P�s-Grava��o";
      case MENU_LABEL_VALUE_CORE_ASSETS_DIRECTORY:
         return "Dir. de Recursos (Assets) dos Cores";
      case MENU_LABEL_VALUE_ASSETS_DIRECTORY:
         return "Dir. de Recursos (Assets)";
      case MENU_LABEL_VALUE_DYNAMIC_WALLPAPERS_DIRECTORY:
         return "Dir. de Pap�is de Parede Din�micos";
      case MENU_LABEL_VALUE_BOXARTS_DIRECTORY:
         return "Dir. de Boxarts";
      case MENU_LABEL_VALUE_RGUI_BROWSER_DIRECTORY:
         return "Dir. de Navega��o";
      case MENU_LABEL_VALUE_RGUI_CONFIG_DIRECTORY:
         return "Dir. de Configura��es";
      case MENU_LABEL_VALUE_LIBRETRO_INFO_PATH:
         return "Dir. de Informa��es de Cores";
      case MENU_LABEL_VALUE_LIBRETRO_DIR_PATH:
         return "Dir. de Cores";
      case MENU_LABEL_VALUE_CURSOR_DIRECTORY:
         return "Dir. de Cursores";
      case MENU_LABEL_VALUE_CONTENT_DATABASE_DIRECTORY:
         return "Dir. de Base de Dados de Conte�dos";
      case MENU_LABEL_VALUE_SYSTEM_DIRECTORY:
         return "Diret�rio System";
      case MENU_LABEL_VALUE_CHEAT_DATABASE_PATH:
         return "Dir. de Arquivos Cheat";
      case MENU_LABEL_VALUE_EXTRACTION_DIRECTORY:
         return "Dir. de Descompacta��o";
      case MENU_LABEL_VALUE_AUDIO_FILTER_DIR:
         return "Dir. de Filtros de �udio";
      case MENU_LABEL_VALUE_VIDEO_SHADER_DIR:
         return "Dir. de Shaders de V�deo";
      case MENU_LABEL_VALUE_VIDEO_FILTER_DIR:
         return "Dir. de Filtros de V�deo";
      case MENU_LABEL_VALUE_OVERLAY_DIRECTORY:
         return "Dir. de Overlays";
      case MENU_LABEL_VALUE_OSK_OVERLAY_DIRECTORY:
         return "Dir. de Overlay de Teclado";
      case MENU_LABEL_VALUE_NETPLAY_CLIENT_SWAP_INPUT:
         return "Trocar Controlador de Netplay";
      case MENU_LABEL_VALUE_NETPLAY_SPECTATOR_MODE_ENABLE:
         return "Ativar Espectador de Netplay";
      case MENU_LABEL_VALUE_NETPLAY_IP_ADDRESS:
         return "Endere�o IP";
      case MENU_LABEL_VALUE_NETPLAY_TCP_UDP_PORT:
         return "Portas TCP/UDP de Netplay";
      case MENU_LABEL_VALUE_NETPLAY_ENABLE:
         return "Ativar Netplay";
      case MENU_LABEL_VALUE_NETPLAY_DELAY_FRAMES:
         return "Quadros de Retardo de Netplay";
      case MENU_LABEL_VALUE_NETPLAY_MODE:
         return "Ativar Cliente de Netplay";
      case MENU_LABEL_VALUE_RGUI_SHOW_START_SCREEN:
         return "Mostrar Tela de In�cio";
      case MENU_LABEL_VALUE_TITLE_COLOR:
         return "Cor do Menu T�tulo";
      case MENU_LABEL_VALUE_ENTRY_HOVER_COLOR:
         return "Cor de Realce do Menu Inicial";
      case MENU_LABEL_VALUE_TIMEDATE_ENABLE:
         return "Mostrar Hora / Data";
      case MENU_LABEL_VALUE_THREADED_DATA_RUNLOOP_ENABLE:
         return "Ativar Runloop de Threads de Dados";
      case MENU_LABEL_VALUE_ENTRY_NORMAL_COLOR:
         return "Cor do Menu Inicial";
      case MENU_LABEL_VALUE_SHOW_ADVANCED_SETTINGS:
         return "Mostrar Configura��es Avan�adas";
      case MENU_LABEL_VALUE_COLLAPSE_SUBGROUPS_ENABLE:
         return "Juntar Subgrupos";
      case MENU_LABEL_VALUE_MOUSE_ENABLE:
         return "Suporte a Mouse";
      case MENU_LABEL_VALUE_POINTER_ENABLE:
         return "Suporte a Touch";
      case MENU_LABEL_VALUE_CORE_ENABLE:
         return "Mostrar Nome dos Cores";
      case MENU_LABEL_VALUE_DPI_OVERRIDE_ENABLE:
         return "Ativar Sobreposi��o de DPI";
      case MENU_LABEL_VALUE_DPI_OVERRIDE_VALUE:
         return "Sobreposi��o de DPI";
      case MENU_LABEL_VALUE_SUSPEND_SCREENSAVER_ENABLE:
         return "Suspender Prote��o de Tela";
      case MENU_LABEL_VALUE_VIDEO_DISABLE_COMPOSITION:
         return "Desativar Desktop Composition";
      case MENU_LABEL_VALUE_PAUSE_NONACTIVE:
         return "N�o Rodar em Background";
      case MENU_LABEL_VALUE_UI_COMPANION_START_ON_BOOT:
         return "Ativar UI Companion ao Iniciar";
      case MENU_LABEL_VALUE_UI_MENUBAR_ENABLE:
         return "Barra de Menu (Dica)";
      case MENU_LABEL_VALUE_ARCHIVE_MODE:
         return "Modo Archive";
      case MENU_LABEL_VALUE_NETWORK_CMD_ENABLE:
         return "Comandos de Rede";
      case MENU_LABEL_VALUE_NETWORK_CMD_PORT:
         return "Porta para A��es de Rede";
      case MENU_LABEL_VALUE_HISTORY_LIST_ENABLE:
         return "Ativar Lista de Hist�rico";
      case MENU_LABEL_CONTENT_HISTORY_SIZE:
         return "Content History Size";
      case MENU_LABEL_VALUE_CONTENT_HISTORY_SIZE:
         return "Tamanho da Lista de Hist�rico";
      case MENU_LABEL_VALUE_VIDEO_REFRESH_RATE_AUTO:
         return "Taxa de Atualiza��o de Quadros Estimada";
      case MENU_LABEL_VALUE_DUMMY_ON_CORE_SHUTDOWN:
         return "Desligar Core Dummy On";
      case MENU_LABEL_VALUE_CORE_SET_SUPPORTS_NO_CONTENT_ENABLE:
         return "N�o Iniciar Cores Automaticamente";
      case MENU_LABEL_VALUE_FRAME_THROTTLE_SETTINGS:
         return "Limitar Velocidade M�xima de Execu��o";
      case MENU_LABEL_VALUE_FASTFORWARD_RATIO:
         return "Velocidade M�xima de Execu��o";
      case MENU_LABEL_VALUE_AUTO_REMAPS_ENABLE:
         return "Carregar Automaticamente Arquivos Remapeados";
      case MENU_LABEL_VALUE_SLOWMOTION_RATIO:
         return "Taxa de C�mera Lenta";
      case MENU_LABEL_VALUE_CORE_SPECIFIC_CONFIG:
         return "Configura��o por Core";
      case MENU_LABEL_VALUE_AUTO_OVERRIDES_ENABLE:
         return "Carregar Automaticamente Arquivos de Sobreposi��o";
      case MENU_LABEL_VALUE_CONFIG_SAVE_ON_EXIT:
         return "Salvar Configura��o ao Sair";
      case MENU_LABEL_VALUE_VIDEO_SMOOTH:
         return "Filtragem Bilinear por Hardware";
      case MENU_LABEL_VALUE_VIDEO_GAMMA:
         return "Gamma de V�deo";
      case MENU_LABEL_VALUE_VIDEO_ALLOW_ROTATE:
         return "Permitir Rota��o";
      case MENU_LABEL_VALUE_VIDEO_HARD_SYNC:
         return "Sincronizar GPU com CPU";
      case MENU_LABEL_VALUE_VIDEO_SWAP_INTERVAL:
         return "Intervalo de Permuta do Sincronismo Vertical";
      case MENU_LABEL_VALUE_VIDEO_VSYNC:
         return "Sincronismo Vertical";
      case MENU_LABEL_VALUE_VIDEO_THREADED:
         return "V�deo em Threads";
      case MENU_LABEL_VALUE_VIDEO_ROTATION:
         return "Rota��o";
      case MENU_LABEL_VALUE_VIDEO_GPU_SCREENSHOT:
         return "Ativar Captura de Tela via GPU";
      case MENU_LABEL_VALUE_VIDEO_CROP_OVERSCAN:
         return "Descartar Overscan (Recarregue)";
      case MENU_LABEL_VALUE_VIDEO_ASPECT_RATIO_INDEX:
         return "�ndice de Propor��es de Tela";
      case MENU_LABEL_VALUE_VIDEO_ASPECT_RATIO_AUTO:
         return "Propor��o de Tela Autom�tica";
      case MENU_LABEL_VALUE_VIDEO_FORCE_ASPECT:
         return "For�ar Propor��o de Tela";
      case MENU_LABEL_VALUE_VIDEO_REFRESH_RATE:
         return "Taxa de Atualiza��o de Tela";
      case MENU_LABEL_VALUE_VIDEO_FORCE_SRGB_DISABLE:
         return "Forcar Desativa��o de sRGB FBO";
      case MENU_LABEL_VALUE_VIDEO_WINDOWED_FULLSCREEN:
         return "Modo Tela Cheia em Janela";
      case MENU_LABEL_VALUE_PAL60_ENABLE:
         return "Usar Modo PAL60";
      case MENU_LABEL_VALUE_VIDEO_VFILTER:
         return "Eliminar Cintila��o";
      case MENU_LABEL_VALUE_VIDEO_VI_WIDTH:
         return "Fixar Largura de Tela VI";
      case MENU_LABEL_VALUE_VIDEO_BLACK_FRAME_INSERTION:
         return "Inser��o de Quadro Negro";
      case MENU_LABEL_VALUE_VIDEO_HARD_SYNC_FRAMES:
         return "Quadros de Sincroniza��o entre GPU e CPU";
      case MENU_LABEL_VALUE_SORT_SAVEFILES_ENABLE:
         return "Ordenar Saves em Pastas";
      case MENU_LABEL_VALUE_SORT_SAVESTATES_ENABLE:
         return "Ordenar Savestates em Pastas";
      case MENU_LABEL_VALUE_VIDEO_FULLSCREEN:
         return "Usar Modo Tela Cheia";
      case MENU_LABEL_VALUE_VIDEO_SCALE:
         return "Interpola��o em Janela";
      case MENU_LABEL_VALUE_VIDEO_SCALE_INTEGER:
         return "Interpola��o em M�ltiplos Inteiros";
      case MENU_LABEL_VALUE_PERFCNT_ENABLE:
         return "Contadores de Desempenho";
      case MENU_LABEL_VALUE_LIBRETRO_LOG_LEVEL:
         return "N�vel de Registro de Core";
      case MENU_LABEL_VALUE_LOG_VERBOSITY:
         return "Detalhamento de Registro";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_LOAD:
         return "Carregar Savestate Automaticamente";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_INDEX:
         return "�ndice Autom�tico de Savestates";
      case MENU_LABEL_VALUE_SAVESTATE_AUTO_SAVE:
         return "Savestate Autom�tico";
      case MENU_LABEL_VALUE_AUTOSAVE_INTERVAL:
         return "Intervalo de Grava��o Autom�tica de SaveRAM";
      case MENU_LABEL_VALUE_BLOCK_SRAM_OVERWRITE:
         return "N�o Sobrescrever SaveRAM ao Carregar Savestate";
      case MENU_LABEL_VALUE_VIDEO_SHARED_CONTEXT:
         return "Ativar Contexto Compartilhado de Hardware";
      case MENU_LABEL_VALUE_RESTART_RETROARCH:
         return "Reiniciar RetroArch";
      case MENU_LABEL_VALUE_NETPLAY_NICKNAME:
         return "Nome de Usu�rio";
      case MENU_LABEL_VALUE_USER_LANGUAGE:
         return "Idioma";
      case MENU_LABEL_VALUE_CAMERA_ALLOW:
         return "Permitir C�mera";
      case MENU_LABEL_VALUE_LOCATION_ALLOW:
         return "Permitir Localiza��o";
      case MENU_LABEL_VALUE_PAUSE_LIBRETRO:
         return "Pausar Quando o Menu for Ativado";
      case MENU_LABEL_VALUE_INPUT_OSK_OVERLAY_ENABLE:
         return "Mostrar Overlay de Teclado";
      case MENU_LABEL_VALUE_INPUT_OVERLAY_ENABLE:
         return "Mostrar Overlay";
      case MENU_LABEL_VALUE_VIDEO_MONITOR_INDEX:
         return "�ndice de Monitores";
      case MENU_LABEL_VALUE_VIDEO_FRAME_DELAY:
         return "Retardo de Quadro";
      case MENU_LABEL_VALUE_INPUT_DUTY_CYCLE:
         return "Ciclo de Trabalho";
      case MENU_LABEL_VALUE_INPUT_TURBO_PERIOD:
         return "Per�odo de Turbo";
      case MENU_LABEL_VALUE_INPUT_AXIS_THRESHOLD:
         return "Limiar de Eixo do Controlador";
      case MENU_LABEL_VALUE_INPUT_REMAP_BINDS_ENABLE:
         return "Ativar Remapeamentos";
      case MENU_LABEL_VALUE_INPUT_MAX_USERS:
         return "Usu�rios M�ximos";
      case MENU_LABEL_VALUE_INPUT_AUTODETECT_ENABLE:
         return "Ativar Autoconfigura��o";
      case MENU_LABEL_VALUE_AUDIO_OUTPUT_RATE:
         return "Taxa de Amostragem de �udio (KHz)";
      case MENU_LABEL_VALUE_AUDIO_MAX_TIMING_SKEW:
         return "Distor��o M�xima de Sincroniza��o de �udio";
      case MENU_LABEL_VALUE_CHEAT_NUM_PASSES:
         return "C�digos de Cheat";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_CORE:
         return "Salvar Remapeamento de Core";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_GAME:
         return "Salvar Remapeamento de Jogo";
      case MENU_LABEL_VALUE_CHEAT_APPLY_CHANGES:
         return "Aplicar Altera��es de Cheats";
      case MENU_LABEL_VALUE_SHADER_APPLY_CHANGES:
         return "Aplicar Altera��es de Shaders";
      case MENU_LABEL_VALUE_REWIND_ENABLE:
         return "Ativar Retrocesso";
      case MENU_LABEL_VALUE_CONTENT_COLLECTION_LIST:
         return "Carregar Conte�do (Cole��o)"; /* FIXME */
      case MENU_LABEL_VALUE_DETECT_CORE_LIST:
         return "Carregar Conte�do (Detectar Core)"; /* FIXME */
      case MENU_LABEL_VALUE_LOAD_CONTENT_HISTORY:
         return "Carregar Conte�do (Hist�rico)";
      case MENU_LABEL_VALUE_AUDIO_ENABLE:
         return "Ativar �udio";
      case MENU_LABEL_VALUE_FPS_SHOW:
         return "Mostrar Taxa de Quadros";
      case MENU_LABEL_VALUE_AUDIO_MUTE:
         return "Desligar �udio";
      case MENU_LABEL_VALUE_AUDIO_VOLUME:
         return "Volume de �udio (dB)";
      case MENU_LABEL_VALUE_AUDIO_SYNC:
         return "Ativar Sincronismo de �udio";
      case MENU_LABEL_VALUE_AUDIO_RATE_CONTROL_DELTA:
         return "Varia��o M�xima de Taxa de �udio";
      case MENU_LABEL_VALUE_VIDEO_SHADER_NUM_PASSES:
         return "N�mero de Shaders";
      case MENU_LABEL_VALUE_CONFIGURATIONS: /* FIXME */
         return "Arquivos de Configura��o";
      case MENU_LABEL_VALUE_REWIND_GRANULARITY:
         return "Granularidade do Retrocesso";
      case MENU_LABEL_VALUE_REMAP_FILE_LOAD:
         return "Carregar Remapeamento";
      case MENU_LABEL_VALUE_REMAP_FILE_SAVE_AS:
         return "Salvar Remapeamento Como";
      case MENU_LABEL_VALUE_CUSTOM_RATIO:
         return "Propor��o de Tela Personalizada";
      case MENU_LABEL_VALUE_USE_THIS_DIRECTORY:
         return "<Usar este diret�rio>";
      case MENU_LABEL_VALUE_RDB_ENTRY_START_CONTENT:
         return "Iniciar Conte�do";
      case MENU_LABEL_VALUE_DISK_OPTIONS:
         return "Op��es de Discos do Core";
      case MENU_LABEL_VALUE_CORE_OPTIONS:
         return "Op��es de Core";
      case MENU_LABEL_VALUE_CORE_CHEAT_OPTIONS:
         return "Op��es de Cheat do Core";
      case MENU_LABEL_VALUE_CHEAT_FILE_LOAD:
         return "Carregar Cheat";
      case MENU_LABEL_VALUE_CHEAT_FILE_SAVE_AS:
         return "Salvar Cheat Como";
      case MENU_LABEL_VALUE_CORE_COUNTERS:
         return "Contadores de Cores";
      case MENU_LABEL_VALUE_TAKE_SCREENSHOT:
         return "Capturar Tela";
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
      case MENU_LABEL_VALUE_SETTINGS:
         return "Configura��es";
      case MENU_LABEL_VALUE_QUIT_RETROARCH:
         return "Sair do RetroArch";
      case MENU_LABEL_VALUE_HELP:
         return "Ajuda";
      case MENU_LABEL_VALUE_SAVE_NEW_CONFIG:
         return "Salvar Nova Configura��o";
      case MENU_LABEL_VALUE_RESTART_CONTENT:
         return "Reiniciar Conte�do";
      case MENU_LABEL_VALUE_CORE_UPDATER_LIST:
         return "Atualiza��o de Cores";
      case MENU_LABEL_VALUE_CORE_UPDATER_BUILDBOT_URL:
         return "URL de Cores - Buildbot";
      case MENU_LABEL_VALUE_BUILDBOT_ASSETS_URL:
         return "URL de Recursos (Assets) - Buildbot";
      case MENU_LABEL_VALUE_NAVIGATION_WRAPAROUND_HORIZONTAL:
         return "Navega��o Horizontal Circular";
      case MENU_LABEL_VALUE_NAVIGATION_WRAPAROUND_VERTICAL:
         return "Navega��o Vertical Circular";
      case MENU_LABEL_VALUE_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE:
         return "Navegador - Filtrar por Extens�es Suportadas";
      case MENU_LABEL_VALUE_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE:
         return "Extrair Arquivos Baixados Automaticamente";
      case MENU_LABEL_VALUE_SYSTEM_INFORMATION:
         return "Informa��o de Sistema";
      case MENU_LABEL_VALUE_CORE_INFORMATION:
         return "Informa��o de Core";
      case MENU_LABEL_VALUE_DIRECTORY_NOT_FOUND:
         return "Diret�rio N�o Encontrado.";
      case MENU_LABEL_VALUE_NO_ITEMS:
         return "Nenhum Item.";
      case MENU_LABEL_VALUE_CORE_LIST:
         return "Carregar Core";
      case MENU_LABEL_VALUE_LOAD_CONTENT:
         return "Carregar Conte�do";  /* FIXME */
      case MENU_LABEL_VALUE_CLOSE_CONTENT:
         return "Descarregar Core"; /* FIXME */
      case MENU_LABEL_VALUE_MANAGEMENT:
         return "Gerenciamento Avan�ado";
      case MENU_LABEL_VALUE_SAVE_STATE:
         return "Salvar Savestate";
      case MENU_LABEL_VALUE_LOAD_STATE:
         return "Carregar Savestate";
      case MENU_LABEL_VALUE_RESUME_CONTENT:
         return "Retomar Conte�do";
      case MENU_LABEL_VALUE_DRIVER_SETTINGS:
         return "Configura��es de Drivers";
      case MENU_LABEL_VALUE_INPUT_DRIVER:
         return "Driver de Controladores";
      case MENU_LABEL_VALUE_AUDIO_DRIVER:
         return "Driver de �udio";
      case MENU_LABEL_VALUE_JOYPAD_DRIVER:
         return "Driver de Joypad";
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
      case MENU_LABEL_VALUE_OVERLAY_OPACITY:
         return "Opacidade de Overlay";
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
