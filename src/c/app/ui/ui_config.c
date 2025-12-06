#include "ui_config.h"

// 0 = follow system preferred_content_size(), 1..3 = override
static int8_t s_text_size_override = 0;

void ui_set_text_size_override(int8_t override)
{
    if (override < 0 || override > 3)
        override = 0;
    s_text_size_override = override;
}

int8_t ui_get_text_size_override(void)
{
    return s_text_size_override;
}

PreferredContentSize ui_effective_preferred_content_size(void)
{
    if (s_text_size_override == 0)
    {
        return preferred_content_size();
    }

#ifdef PBL_PLATFORM_EMERY
    switch (s_text_size_override)
    {
    case 1:
        return PreferredContentSizeMedium;
    case 2:
        return PreferredContentSizeLarge;
    case 3:
        return PreferredContentSizeExtraLarge;
    default:
        return preferred_content_size();
    }
#else
    switch (s_text_size_override)
    {
    case 1:
        return PreferredContentSizeSmall;
    case 2:
        return PreferredContentSizeMedium;
    case 3:
        return PreferredContentSizeLarge;
    default:
        return preferred_content_size();
    }
#endif
}
