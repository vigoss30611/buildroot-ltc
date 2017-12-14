/*
 * Copyright (C) 2009 The Android Open Source Project
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

#include <linux/input.h>
#include <stdio.h>

#include "common.h"
#include "device.h"
#include "screen_ui.h"

static const char* HEADERS[] = { "UP/DOWN arrows to move highlight up/down",
                                 "ENTER to select",
                                 NULL };

static const char* ITEMS[] =  {"reboot system now",
                               "apply update from 'Android' partition",
                               NULL };

class DefaultUI : public ScreenRecoveryUI {
    bool mToggled = false;
  public:
    virtual KeyAction CheckKey(int key) {
        // first key pressed will toggle, one time only
        if (!mToggled) {
            mToggled = true;
            return TOGGLE;
        }

        return ENQUEUE;
    }
};

class DefaultDevice : public Device {
  public:
    DefaultDevice() :
        ui(new DefaultUI) {
    }

    RecoveryUI* GetUI() { return ui; }

    int HandleMenuKey(int key, int visible) {
        if (visible) {
            switch (key) {
              case KEY_ENTER:
                return kInvokeItem;
              case KEY_UP:
                return kHighlightUp;
              case KEY_DOWN:
                return kHighlightDown;
            }
        }

        return kNoAction;
    }

    BuiltinAction InvokeMenuItem(int menu_position) {
        switch (menu_position) {
          case 0: return REBOOT;
          case 1: return APPLY_EXT;
          default: return NO_ACTION;
        }
    }

    const char* const* GetMenuHeaders() { return HEADERS; }
    const char* const* GetMenuItems() { return ITEMS; }

  private:
    RecoveryUI* ui;
};

Device* make_device() {
    return new DefaultDevice();
}
