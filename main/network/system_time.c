// SPDX-FileCopyrightText: 2026 airplay-esp32 contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "system_time.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"

static const char *TAG = "system_time";

static bool s_initialized;
static volatile bool s_synced;

static void on_time_sync(struct timeval *tv) {
  (void)tv;
  s_synced = true;
  ESP_LOGI(TAG, "System time synchronized");
}

static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) {
  (void)arg;
  (void)event_data;

  if (event_base != IP_EVENT ||
      (event_id != IP_EVENT_STA_GOT_IP && event_id != IP_EVENT_ETH_GOT_IP)) {
    return;
  }

  esp_err_t err = esp_netif_sntp_start();
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "Could not start DHCP-provided SNTP: %s",
             esp_err_to_name(err));
  }
}

esp_err_t system_time_init(void) {
  if (s_initialized) {
    return ESP_OK;
  }

  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(0, {});
  config.server_from_dhcp = true;
  config.start = false;
  config.sync_cb = on_time_sync;

  esp_err_t err = esp_netif_sntp_init(&config);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, on_ip_event,
                                   NULL);
  if (err != ESP_OK) {
    esp_netif_sntp_deinit();
    return err;
  }

  s_initialized = true;
  return ESP_OK;
}

bool system_time_is_synced(void) { return s_synced; }