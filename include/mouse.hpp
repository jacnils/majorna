#pragma once


enum class ClickType : int {
    ClickWindow = 0,
    ClickPrompt = 1,
    ClickInput = 2,
    ClickLeftArrow = 3,
    ClickRightArrow = 4,
    ClickItem = 5,
    ClickMatchCounter = 6,
    ClickCapsLockIndicator = 7,
    ClickModeIndicator = 8,
#if IMAGE
    ClickImage = 9,
#endif
    ClickNone = 10,
};
