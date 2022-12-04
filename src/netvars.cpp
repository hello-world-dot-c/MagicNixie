/* 
 * This file is part of the MagicNixie project (https://github.com/hello-world-dot-c/MagicNixie).
 * Copyright (c) 2020 Chris Keydel.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "magicnixie.h"

/**************************************************************************
DEFINITIONS AND SETTINGS
***************************************************************************/
#define MODULE "*NV: "


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/

static const netVars_t netVarsTable[] = {
  { "BLEND"             , TYPE_BOOL   , &gConf.useSoftBlend          , { "blendingOnOffButton"   , "blendingStatus"       } },
  { "RED"               , TYPE_UINT8  , &gConf.ledRed                , { "redSlider"             , "redValue"             } },
  { "GREEN"             , TYPE_UINT8  , &gConf.ledGreen              , { "greenSlider"           , "greenValue"           } },
  { "BLUE"              , TYPE_UINT8  , &gConf.ledBlue               , { "blueSlider"            , "blueValue"            } },
  { "BRIGHTNESS"        , TYPE_UINT8  , &gConf.ledBrightness         , { "ledBrightnessSlider"   , "ledBrightnessValue"   } },
  { "NIXIEBRIGHTNESS"   , TYPE_UINT8  , &gConf.nixieBrightness       , { "nixieBrightnessSlider" , "nixieBrightnessValue" } },
  { "ANTIPOISON"        , TYPE_UINT8  , &gConf.antiPoisoningLevel    , { "antiPoisonSlider"      , "antiPoisonValue"      } },
  { "USE12HDISP"        , TYPE_BOOL   , &gConf.use12hDisplay         , { "" , "" } },
  { "OMITLEAD0"         , TYPE_BOOL   , &gConf.omitLeading0Hour      , { "" , "" } },
  { "SYNCRTC"           , TYPE_BOOL   , &gConf.syncRTC               , { "" , "" } },
  { "QUIETNIGHTS"       , TYPE_BOOL   , &gConf.quietNights           , { "" , "" } },
  { "ALTDISPPERIOD"     , TYPE_UINT16 , &gConf.altDisplayPeriod_s    , { "" , "" } },
  { "ALTDISPDUR"        , TYPE_UINT16 , &gConf.altDisplayDuration_ms , { "" , "" } },
  { "ALTFADESPEED"      , TYPE_UINT16 , &gConf.altFadeSpeed_ms       , { "" , "" } },
  { "ALTFADEDRKPAUSE"   , TYPE_UINT16 , &gConf.altFadeDarkPause_ms   , { "" , "" } },
  { "TEMPTIMEOUT"       , TYPE_UINT16 , &gConf.tempTimeout_s         , { "" , "" } },
#if NUM_TEMP_SENSORS>0
  { "TEMP0"             , TYPE_FLOAT  , &gVars.temp[0]               , { "" , "" } },
#if NUM_TEMP_SENSORS>1
  { "TEMP1"             , TYPE_FLOAT  , &gVars.temp[1]               , { "" , "" } },
#if NUM_TEMP_SENSORS>2
  { "TEMP2"             , TYPE_FLOAT  , &gVars.temp[2]               , { "" , "" } },
#if NUM_TEMP_SENSORS>3
#error Too many temperature sensors, not supported.
#endif
#endif
#endif
#endif
  { ""                  , TYPE_ENDREC , NULL                         , { "" , "" } },
};

static const netVars_t *netVars_p = NULL;

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void nvSetFirstVar () {
  netVars_p = netVarsTable;
  netVars_p--;  // point to one record BEFORE the table (illegal value!)
  return;
}

bool nvGetNextVar(const netVars_t **net_vars_pp) {
  if (NULL == netVars_p) {
    return false;
  } else {
    netVars_p++;  // the first increment sets to the start of the table
    if (TYPE_ENDREC == netVars_p->type) {
      netVars_p = NULL;
      return false;
    } else {
      *net_vars_pp = netVars_p;
      return true;
    }
  }
}

/* not yet used
bool nvSetVar(void *dat) {
  if ( (NULL == netVars_p) || (NULL == dat))
  {
    return false;
  } else {  
    switch(netVars_p->type) {
      case TYPE_BOOL:
        *(bool *)(netVars_p->var) = *(bool *)dat;
        break;
      case TYPE_UINT8:
        *(uint8_t *)(netVars_p->var) = *(uint8_t *)dat;
        break;
      case TYPE_UINT16:
        *(uint16_t *)(netVars_p->var) = *(uint16_t *)dat;
        break;
      case TYPE_UINT32:
        *(uint32_t *)(netVars_p->var) = *(uint32_t *)dat;
        break;
      default:
        break;
    }
    return true;
  }
}

void nvGetVar(void *dat) {
  if ( (NULL == netVars_p) || (NULL == dat))
  {
    return;
  } else {  
    switch(netVars_p->type) {
      case TYPE_BOOL:
        *(bool *)dat = *(bool *)(netVars_p->var);
        break;
      case TYPE_UINT8:
        *(uint8_t *)dat = *(uint8_t *)(netVars_p->var);
        break;
      case TYPE_UINT16:
        *(uint16_t *)dat = *(uint16_t *)(netVars_p->var);
        break;
      case TYPE_UINT32:
        *(uint32_t *)dat = *(uint32_t *)(netVars_p->var);
        break;
      default:
        break;
    }
    return;
  }
}
not yet used */

bool nvSetVarByString(String *Str) {
  bool ret_val = false;
  if (isNumeric(*Str)) {
    long lval;
    float fval;
    switch(netVars_p->type) {
      case TYPE_BOOL:
        lval = Str->toInt();
        if (lval==0) {
          *(bool *)(netVars_p->var) = false;
          ret_val = true;
        } else if (lval==1) {
          *(bool *)(netVars_p->var) = true;
          ret_val = true;
        }
        break;
      case TYPE_UINT8:
        lval = Str->toInt();
        if ((lval>=0) && (lval<=0xFF)) {
          *(uint8_t *)(netVars_p->var) = (uint8_t)lval;
          ret_val = true;
        }
        break;
      case TYPE_UINT16:
        lval = Str->toInt();
        if ((lval>=0) && (lval<=0xFFFF)) {
          *(uint16_t *)(netVars_p->var) = (uint16_t)lval;
          ret_val = true;
        }
        break;
      case TYPE_UINT32:
        lval = Str->toInt();
        if (lval>=0) {
          *(uint32_t *)(netVars_p->var) = (uint32_t)lval;
          ret_val = true;
        }
        break;
      case TYPE_FLOAT:
        fval = Str->toFloat();
        *(float *)(netVars_p->var) = fval;
        ret_val = true;
        break;
      default:
        break;
    }
  }
  return ret_val;
}

bool nvSetThisVarByString(String *varStr, String *valStr) {
  const netVars_t *net_vars_p;
  bool ret_val = false;
  nvSetFirstVar();
  while (nvGetNextVar(&net_vars_p)) {
    if ((*varStr) == netVars_p->name) {
      if (nvSetVarByString(valStr)) {
        // Special treatment of setting of variables
        for (int i=0; i<NUM_TEMP_SENSORS; i++) {
          String compStr="TEMP"+String(i);
          if (*varStr==compStr) {
            gVars.tempTimeout_ms[i] = 1000ul*gConf.tempTimeout_s;
            gVars.tempValid[i] = true;
            break;
          }
        }
        ret_val = true;
      }
      break;
    }
  }
  if (ret_val) {
    _PF(MODULE"Set: %s = %s\n", varStr, valStr);
  } else {
    _PF(MODULE"ERROR setting %s to %s\n", varStr, valStr);
  }
  return ret_val;
}

bool nvGetThisVarAsString(String *varStr, char *val_str, size_t max) {
  const netVars_t *net_vars_p;
  String retStr = "NOT FOUND";
  bool found = false;
  nvSetFirstVar();
  while (nvGetNextVar(&net_vars_p)) {
    if ((*varStr) == netVars_p->name) {
      found = true;
      switch(netVars_p->type) {
        case TYPE_BOOL:
        case TYPE_UINT8:
          retStr = *(uint8_t *)(netVars_p->var);
          break;
        case TYPE_UINT16:
          retStr = *(uint16_t *)(netVars_p->var);
          break;
        case TYPE_UINT32:
          retStr = *(uint32_t *)(netVars_p->var);
          break;
        case TYPE_FLOAT:
          retStr = *(float *)(netVars_p->var);
          break;
        default:
          retStr = "INVALID";
          break;
      }
      break;
    }
  }
  retStr.toCharArray(val_str, max);
  return found;
}

void nvPrintVars() {
  const netVars_t *net_vars_p;
  String retStr = "NOT FOUND";
  nvSetFirstVar();
  while (nvGetNextVar(&net_vars_p)) {
    _PF("%20s: ", netVars_p->name);
    switch(netVars_p->type) {
      case TYPE_BOOL:
      case TYPE_UINT8:
        retStr = *(uint8_t *)(netVars_p->var);
        break;
      case TYPE_UINT16:
        retStr = *(uint16_t *)(netVars_p->var);
        break;
      case TYPE_UINT32:
        retStr = *(uint32_t *)(netVars_p->var);
        break;
      case TYPE_FLOAT:
        retStr = *(float *)(netVars_p->var);
        break;
      default:
        retStr = "INVALID";
        break;
    }
    _PF("%s\n", &retStr);
  }
  return;
}

/* EOF */
