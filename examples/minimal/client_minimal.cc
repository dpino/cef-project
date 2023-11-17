// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "examples/minimal/client_minimal.h"

#include "examples/shared/client_util.h"

namespace minimal {

Client::Client() {}

void Client::OnTitleChange(CefRefPtr<CefBrowser> browser,
                           const CefString& title) {
  // Call the default shared implementation.
  shared::OnTitleChange(browser, title);
}

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  // Call the default shared implementation.
  shared::OnAfterCreated(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser) {
  // Call the default shared implementation.
  return shared::DoClose(browser);
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  // Call the default shared implementation.
  return shared::OnBeforeClose(browser);
}

#ifdef linux
enum VKEY {
  VKEY_ARROW_LEFT = 0x71,
  VKEY_ARROW_RIGHT = 0x72,
  VKEY_F1 = 0x43,
  VKEY_F5 = 0x47,
  VKEY_F12 = 0x60,
  VKEY_H = 0x2b,
  VKEY_P = 0x21,
  VKEY_Q = 0x18,
  VKEY_R = 0x1b
};
#else
enum VKEY {
  VKEY_ARROW_LEFT = 0x25,
  VKEY_ARROW_RIGHT = 0x27,
  VKEY_F1 = 0x70,
  VKEY_F5 = 0x74,
  VKEY_F12 = 0x7B,
  VKEY_H = 0x48,
  VKEY_P = 0x50,
  VKEY_Q = 0x51,
  VKEY_R = 0x52
};
#endif

static bool keyboard_shortcut_print_page(CefRefPtr<CefBrowser> browser) {
  browser->GetHost()->Print();
  return true;
}

static int GetKeyCode(const CefKeyEvent& event) {
  #ifdef linux
    return event.native_key_code;
  #else
    return event.windows_key_code;
  #endif
}

bool Client::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                           const CefKeyEvent& event,
                           CefEventHandle os_event,
                           bool* is_keyboard_shortcut) {
  bool handled = false;

  if (event.type == KEYEVENT_RAWKEYDOWN) {
    // Ctrl+Key shortcuts.
    if (event.modifiers & EVENTFLAG_CONTROL_DOWN) {
      switch (GetKeyCode(event)) {
        case VKEY_H:
          // handled = keyboard_shortcut_go_homepage(browser, this->homepageUrl);
        break;
        case VKEY_P:
          handled = keyboard_shortcut_print_page(browser);
        case VKEY_Q:
          // handled = keyboard_shortcut_close_browser(browser);
        break;
        case VKEY_R:
          // handled = keyboard_shortcut_reload(browser);
        break;
      }
    }
  }
  *is_keyboard_shortcut = !handled;
  return handled;
}

}  // namespace minimal
