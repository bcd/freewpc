
#ifndef _TEXT_H
#define _TEXT_H

#if (MACHINE_ALPHANUMERIC == 1)
#define STR_RECENT "REC. "
#define STR_PERCENT "% "
#define STR_LEFT "L. "
#define STR_CENTER "CTR. "
#define STR_RIGHT "R. "
#define STR_CREDIT "CRED. "
#define STR_LEVEL ""
#else
#define STR_RECENT "RECENT "
#define STR_PERCENT "PERCENT "
#define STR_LEFT "LEFT "
#define STR_CENTER "CENTER "
#define STR_RIGHT "RIGHT "
#define STR_CREDIT "CREDIT "
#define STR_LEVEL "LEVEL"
#endif

#endif /* _TEXT_H */
