// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "examples/minimal/client_minimal.h"

#include "examples/shared/client_util.h"

#include <X11/Xlib.h>

#define XA_ATOM ((Atom) 4)

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

std::pair<CefPoint, CefSize> GetBrowserWindowBounds(
    CefRefPtr<CefBrowser> browser) {
  auto xDisplay = cef_get_xdisplay();
  auto xWindow = browser->GetHost()->GetWindowHandle();

  Window winDummy;
  int x, y;
  unsigned int w, h, bw, depth;

  XGetGeometry(xDisplay, xWindow, &winDummy, &x, &y, &w, &h, &bw, &depth);
  return {CefPoint(x, y), CefSize(w, h)};
}

void SetXWindowBounds(XDisplay* xdisplay,
                      ::Window xwindow,
                      int x,
                      int y,
                      size_t width,
                      size_t height) {
  CHECK(xdisplay != 0);
  XWindowChanges changes = {0};
  changes.x = x;
  changes.y = y;
  changes.width = static_cast<int>(width);
  changes.height = static_cast<int>(height);
  XConfigureWindow(xdisplay, xwindow, CWX | CWY | CWHeight | CWWidth, &changes);
}

void MakeBrowserWindowAlwaysOnTop(CefRefPtr<CefBrowser> browser)
{
  auto xDisplay = cef_get_xdisplay();
  auto xWindow = browser->GetHost()->GetWindowHandle();

  Atom wm_state = XInternAtom(xDisplay, "_NET_WM_STATE_ABOVE", False);
  XChangeProperty(xDisplay, xWindow, XInternAtom(xDisplay, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_state, 1);
}

static bool keyboard_shortcut_show_developer_tools(CefRefPtr<CefBrowser> browser) {
  CefPoint toplevel_window_location;
  CefSize toplevel_window_size;

  std::tie(toplevel_window_location, toplevel_window_size) = GetBrowserWindowBounds(browser);

  unsigned int width = toplevel_window_size.width;
  unsigned int height = toplevel_window_size.height / 4;
  int x = toplevel_window_location.x;
  int y = toplevel_window_size.height - height;

  CefWindowInfo windowInfo;
  windowInfo.SetAsChild(browser->GetHost()->GetWindowHandle(), CefRect(x, y, width, height));

  auto client = browser->GetHost()->GetClient();
  browser->GetHost()->ShowDevTools(windowInfo, client, CefBrowserSettings(), CefPoint());

  return true;
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

  // Simple keys.
  switch (GetKeyCode(event)) {
    case VKEY_F12:
      handled = keyboard_shortcut_show_developer_tools(browser);
    break;
  }

  *is_keyboard_shortcut = !handled;
  return handled;
}

}  // namespace minimal
