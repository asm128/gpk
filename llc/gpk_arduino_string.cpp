#include "gpk_arduino_string.h"

#ifdef GPK_ARDUINO
stxp gpk::vcst_c    trim_blanks = " \n\r\t";

gpk::err_t  gpk::rtrim          (String & trimmed, const String & input) {
    trimmed = input;
    int             trimCount           = 0;
    while(trimmed.length()) {   // Loop to trim trailing whitespace characters from 'trimmed'.
        sc_c                lastChar        = trimmed[trimmed.length() - 1];
        if(-1 == trim_blanks.find(lastChar))  // Check if 'lastChar' is in 'trim_blanks'.
            break;

        trimmed     = trimmed.substring(0, trimmed.length() - 1);// Resize 'trimmed' to remove the last character.
        ++trimCount;
    }
    return trimCount;
}
#endif // GPK_ARDUINO
