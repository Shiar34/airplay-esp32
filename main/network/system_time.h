// SPDX-FileCopyrightText: 2026 airplay-esp32 contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdbool.h>

#include "esp_err.h"

esp_err_t system_time_init(void);
bool system_time_is_synced(void);