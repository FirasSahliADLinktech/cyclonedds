/*
 * Copyright(c) 2006 to 2018 ADLINK Technology Limited and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#ifndef _DDS_TKMAP_H_
#define _DDS_TKMAP_H_

#include "dds__types.h"
#include "os/os_atomics.h"

#if defined (__cplusplus)
extern "C" {
#endif

struct ddsi_tkmap;
struct ddsi_serdata;
struct dds_topic;

struct ddsi_tkmap_instance
{
  struct ddsi_serdata * m_sample;
  uint64_t m_iid;
  os_atomic_uint32_t m_refc;
};


struct ddsi_tkmap * ddsi_tkmap_new (void);
void ddsi_tkmap_free (_Inout_ _Post_invalid_ struct ddsi_tkmap *tkmap);
void ddsi_tkmap_instance_ref (_In_ struct ddsi_tkmap_instance *tk);
uint64_t ddsi_tkmap_lookup (_In_ struct ddsi_tkmap *tkmap, _In_ const struct ddsi_serdata *serdata);
_Check_return_ struct ddsi_tkmap_instance * ddsi_tkmap_find(
        _In_ struct ddsi_serdata * sd,
        _In_ const bool rd,
        _In_ const bool create);
_Check_return_ struct ddsi_tkmap_instance * ddsi_tkmap_find_by_id (_In_ struct ddsi_tkmap * map, _In_ uint64_t iid);

DDS_EXPORT _Check_return_ struct ddsi_tkmap_instance * ddsi_tkmap_lookup_instance_ref (_In_ struct ddsi_serdata * sd);
DDS_EXPORT void ddsi_tkmap_instance_unref (_In_ struct ddsi_tkmap_instance * tk);

#if defined (__cplusplus)
}
#endif
#endif
