/*
 *  Define terminal capabilities macros and constants for game interface.
 */

#ifndef __TERMCODES_H__
#define __TERMCODES_H__

#define	BOLD		1
#define	UNDERLINE	2
#define	REVERSE		3

#define TERM_RESET(ch)    	(wtb(ch->link,term_table[ch->pcdata->terminal].rs))

#define CLEAR_LINE(ch)    	(wtb(ch->link,term_table[ch->pcdata->terminal].cl))

#define TERM_SETUP(ch,buf) 	{sprintf(buf,term_table[ch->pcdata->terminal].su, ch->pcdata->lines-2); wtb(ch->link,buf); }

#define TERM_INPUT(ch,buf) 	{ sprintf(buf,term_table[ch->pcdata->terminal].ti, ch->pcdata->lines, ch->pcdata->lines, ch->pcdata->lines); wtb(ch->link,buf); }

#define TERM_STATUS(ch,buf)	{ sprintf(buf,term_table[ch->pcdata->terminal].ts, ch->pcdata->lines-1, ch->pcdata->lines-1, ch->pcdata->lines-1 );  wtb(ch->link,buf); }

#define TERM_WINDOW(ch,buf)	{ sprintf(buf,term_table[ch->pcdata->terminal].tw, ch->pcdata->lines-2, ch->pcdata->lines-2 ); wtb(ch->link,buf); }

#define TERM_BOLD(ch)		(wtb(ch->link,term_table[ch->pcdata->terminal].format[FORMAT_BOLD]))

#endif /* __TERMCODES_H__ */



