/*
 * Copyright (C) 2021 Project Lighthouse
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <cstdlib>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

using android::base::GetProperty;
int property_set(const char *key, const char *value) {
    return __system_property_set(key, value);
}

void property_override(char const prop[], char const value[], bool add = true)
{
    prop_info *pi;
    pi = (prop_info *) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

/* From Magisk@native/jni/magiskhide/hide_utils.c */
static const char *cts_prop_key[] =
        { "ro.boot.vbmeta.device_state", "ro.boot.verifiedbootstate", "ro.boot.flash.locked", "ro.boot.selinux",
          "ro.boot.veritymode", "ro.boot.warranty_bit", "ro.warranty_bit",
          "ro.debuggable", "ro.secure", "ro.build.type", "ro.build.tags", "ro.build.keys", "ro.system.build.tags",
          "ro.system.build.type", "ro.system_ext.build.type", "ro.vendor.build.type", "ro.product.build.type", "ro.odm.build.type",
          "ro.vendor.boot.warranty_bit", "ro.vendor.warranty_bit",
          "vendor.boot.vbmeta.device_state", nullptr };

static const char *cts_prop_val[] =
        { "locked", "green", "1", "enforcing",
		  "enforcing", "0", "0",
		  "0", "1", "user", "release-keys", "release-keys", "release-keys",
		  "user", "user", "user", "user", "user",
		  "0", "0",
		  "locked", nullptr };

static const char *cts_late_prop_key[] =
        { "vendor.boot.verifiedbootstate", nullptr };

static const char *cts_late_prop_val[] =
        { "green", nullptr };

static const char *extra_prop_key[] =
        { "ro.build.flavor", nullptr };
		
static const char *extra_prop_val[] =
        { "user", nullptr };
		
static void workaround_cts_properties() {

    // Hide all sensitive props
    for (int i = 0; cts_prop_key[i]; ++i) {
		property_override(cts_prop_key[i], cts_prop_val[i]);
	}
	for (int i = 0; cts_late_prop_key[i]; ++i) {
		property_override(cts_late_prop_key[i], cts_late_prop_val[i]);
    }
	for (int i = 0; extra_prop_key[i]; ++i) {
		property_override(extra_prop_key[i], extra_prop_val[i]);
    }	
}

void vendor_load_properties()
{
    // Workaround SafetyNet
    workaround_cts_properties();
}