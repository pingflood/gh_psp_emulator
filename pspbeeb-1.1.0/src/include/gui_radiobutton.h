/* START OF radiobutton.h ------------------------------------------------------
 *
 *	A simple Radio Button (wraps the TickBox widget really).
 *
 *	---
 *	Written by David Eggleston (2006) <deggleston@users.sourceforge.net>
 *	for the 'BeebEm' Acorn BBC Model B, Integra-B, Model B Plus and
 *	Master 128 emulator.
 *
 *	This file is part of 'Economy GUI' and may be copied only under the
 *	terms of either the GNU General Public License (GPL) or Dr. David
 *	Alan Gilbert's BeebEm license.
 *
 *	For more details please visit:
 *
 *	http://www.gnu.org/copyleft/gpl.html
 *	---
 */


#ifndef _DW_RADIOBUTTON_H_
#define _DW_RADIOBUTTON_H_


#if HAVE_CONFIG_H
#       include <config.h>
#endif

#include "include/types.h"
#include "include/sdl.h"

#include "include/gui_window.h"
#include "include/gui_widget.h"

#include "include/gui_tickbox.h"

#include <SDL.h>


/* Widget type:
 */
#define EG_Widget_Type_RadioButton 	EG_Widget_Type_TickBox


/* Public functions:
 */

EG_Widget* EG_RadioButton_Create(const char *name_ptr, SDL_Color color, const char *caption_ptr, SDL_Rect dimension);

#define EG_RadioButton_SetCaption		EG_TickBox_SetCaption
#define EG_RadioButton_GetCaption		EG_TickBox_GetCaption

#define EG_RadioButton_IsSelected		EG_TickBox_IsTicked

#define EG_RadioButton_Destroy			EG_Widget_CallDestroy

#define EG_RadioButton_SetMyCallback_OnEvent	EG_Widget_SetUserCallback_OnEvent
#define EG_RadioButton_ClearMyCallback_OnEvent	EG_Widget_ClearUserCallback_OnEvent

#define EG_RadioButton_SetMyCallback_OnClick	EG_Widget_SetUserCallback_OnClick
#define EG_RadioButton_ClearMyCallback_OnClick	EG_Widget_ClearUserCallback_OnClick

#define EG_RadioButton_SetBackgroundColor	EG_Widget_SetBackgroundColor
#define EG_RadioButton_GetBackgroundColor	EG_Widget_GetBackgroundColor

#define EG_RadioButton_Repaint			EG_Widget_Repaint
#define EG_RadioButton_RepaintLot		EG_Widget_RepaintLot

#define EG_RadioButton_Hide			EG_Window_HideWidget
#define EG_RadioButton_Show			EG_Window_ShowWidget

#define EG_RadioButton_Enable			EG_Window_EnableWidget
#define EG_RadioButton_Disable			EG_Window_DisableWidget

#define EG_RadioButton_IsVisible		EG_Widget_IsVisible
#define EG_RadioButton_IsHidden			EG_Widget_IsHidden
#define EG_RadioButton_IsEnabled		EG_Widget_IsEnabled

#define EG_RadioButton_GetID			EG_Widget_GetID
#define EG_RadioButton_GetName			EG_Widget_GetName

/* END OF radiobutton.h --------------------------------------------------------
 */
#endif



