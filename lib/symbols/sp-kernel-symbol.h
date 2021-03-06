/* sp-kernel-symbol.h
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SP_KERNEL_SYMBOL_H
#define SP_KERNEL_SYMBOL_H

#include "capture/sp-capture-types.h"

G_BEGIN_DECLS

typedef struct
{
  SpCaptureAddress  address;
  const gchar      *name;
} SpKernelSymbol;

const SpKernelSymbol *sp_kernel_symbol_from_address (SpCaptureAddress address);

G_END_DECLS

#endif /* SP_KERNEL_SYMBOL_H */
