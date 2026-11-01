#include "llc_arduino_string.h"

#ifdef LLC_ARDUINO
stxp llc::vcst_c    trim_blanks = " \n\r\t";

llc::error_t    llc::rtrim      (String & trimmed, const String & input) {
    trimmed = input;
    int         trimCount   = 0;
    while(trimmed.length()) {   // Loop to trim trailing whitespace characters from 'trimmed'.
        const char  lastChar    = trimmed[trimmed.length() - 1];
        if(-1 == trim_blanks.find(lastChar))  // Check if 'lastChar' is in 'trim_blanks'.
            break;

        trimmed = trimmed.substring(0, trimmed.length() - 1);// Resize 'trimmed' to remove the last character.
        ++trimCount;
    }
    return trimCount;
}
#endif // LLC_ARDUINO
