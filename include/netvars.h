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

#ifndef _NETVARS_H_
#define _NETVARS_H_

/**************************************************************************
DEFINITIONS AND SETTINGS
***************************************************************************/
typedef enum {
  TYPE_BOOL,
  TYPE_UINT8,
  TYPE_UINT16,
  TYPE_UINT32,
  TYPE_FLOAT,
  TYPE_ENDREC
} dType_t;

typedef struct {
  char    name[20];
  dType_t type;
  void    *var;
  char    jsname[2][30];
} netVars_t;


// Function prototypes
void nvSetFirstVar();
bool nvGetNextVar(const netVars_t **net_vars_pp);
bool nvSetVar(void *dat);
bool nvSetVarByString(String *Str);
void nvGetVar(void *dat);
bool nvSetThisVarByString(String *varStr, String *valStr);
bool nvGetThisVarAsString(String *varStr, char *val_str, size_t max);
void nvPrintVars();

#endif // _NETVARS_H_
/* EOF */
