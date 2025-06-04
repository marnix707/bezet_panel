#include "config_state.h"
#include "storage.h"  // your NVS helpers

static String SSID;
static String SSID_password;
static String serverIP;
static String roomID;

void initConfig() {
  serverIP = loadServerIP();
  roomID = loadRoomID();
  SSID = loadSSID();
  SSID_password = loadPassword();
}

String getServerIP() {
  return serverIP;
}

String getRoomID() {
  return roomID;
}

String getSSID() {
  return SSID;
}

String getPassword() {
  return SSID_password;
}