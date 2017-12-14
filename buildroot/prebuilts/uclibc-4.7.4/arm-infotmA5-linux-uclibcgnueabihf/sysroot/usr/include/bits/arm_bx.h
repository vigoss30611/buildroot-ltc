/* Copyright (C) 2013 Yann E. MORIN <yann.morin.1998@free.fr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _ARM_BX_H
#define _ARM_BX_H

/* We need features.h first */
#if !defined _FEATURES_H
#error Please include features.h first
#endif /* features.h not yet included */

# define BX(reg)	mov pc, reg
# define BXC(cond, reg)	mov##cond pc, reg

#endif /* _ARM_BX_H */
