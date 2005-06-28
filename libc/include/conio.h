#ifndef _CONIO_H
#define _CONIO_H

#ifdef __cplusplus
extern "C" {
#endif


extern int getch(void);
extern int getche(void);
extern int kbhit(void);
extern int ungetch(unsigned char c);


#ifdef __cplusplus
}
#endif

#endif /* !defined _CONIO_H */
