/*
   k_os (Konnex Operating-System based on the OSEK/VDX-Standard).

   (C) 2007-2010 by Christoph Schueler <chris@konnex-tools.de,
                                       cpu12.gems@googlemail.com>

   All Rights Reserved

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 

   s. FLOSS-EXCEPTION.txt
*/
#if !defined(__COM_EXT_H)
#define __COM_EXT_H

#include "Com_If.h"
#include "S12_Can.h" /* todo: 'CanIf.h'!!! */

void ComExt_Init(void);
void ComExt_RxHandler(void);
void ComExt_TxHandler(void);
void ComExt_TimeoutHandler(void);

#endif /* __COM_EXT_H */

