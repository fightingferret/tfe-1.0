#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


int max_players = 0;


/*
 *   LOCAL FUNCTIONS
 */


void display_last   ( pfile_data* pfile, char_data* ch );


/*
 *   LAST ROUTINES 
 */


void display_last( pfile_data* pfile, char_data* ch )
{
  page( ch, "%-15s %s  %s\n\r", pfile->name,
    ltime( pfile->last_on ),
    is_demigod( ch ) ? pfile->last_host : "" );
}


void do_last( char_data *ch, char *argument ) 
{
  int          found = FALSE;
  int            pos;
  int         length;

  if( *argument == '\0' ) {
    send( "Specify first few letters of name to search for.\n\r", ch );
    return;
    }

  if( !strcasecmp( argument, "avatar" ) ) {
    for( pos = 0; pos < max_pfile; pos++ ) 
      if( pfile_list[pos]->level < LEVEL_AVATAR
        && pfile_list[pos]->trust >= LEVEL_AVATAR ) {
        found = TRUE;
        display_last( pfile_list[pos], ch );
        }  
    }

  else if( !strcasecmp( argument, "immortal" ) ) {
    for( pos = 0; pos < max_pfile; pos++ ) 
      if( pfile_list[pos]->level >= LEVEL_APPRENTICE ) {
        found = TRUE;
        display_last( pfile_list[pos], ch );
        }  
    }
 
  else {
    if( ( pos = pntr_search( pfile_list, max_pfile, argument ) ) < 0 )
      pos = -pos-1;
    length = strlen( argument );
    for( ; pos < max_pfile; pos++ ) {
      if( strncasecmp( pfile_list[pos]->name, argument, length ) )
        break;
      found = TRUE;
      display_last( pfile_list[pos], ch );
      }
    }
  
  if( !found ) 
    send( ch, "No matches found.\n\r" );
}


void do_whois( char_data* ch, char* argument )
{
  pfile_data*  pfile;
  bool         email;
  wizard_data*   imm  = wizard( ch );

  if( not_player( ch ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Specify full name of character.\n\rTo search for a name use\
 last <first letter of name>.\n\r" );
    return;
    }

  if( ( pfile = find_pfile_exact( argument ) ) == NULL || pfile->level == 0 ) {
    send( ch, "No character by that name exists.\n\r" );
    return;
    }

  bool see_private = ( is_demigod( ch ) || ch->pcdata->pfile == pfile );
  bool see_account = ( imm != NULL && imm->See_Account( pfile ) );

  send( ch, scroll_line[1] );
  send( ch, "\n\r" );
 
  send( ch, "        Name: %s\n\r", pfile->name );
  send( ch, "        Race: %s\n\r", race_table[ pfile->race ].name );

  if( !is_incognito( pfile, ch ) ) {
    send( ch, "       Class: %s\n\r", clss_table[ pfile->clss ].name );
    send( ch, "         Sex: %s\n\r", sex_name[ pfile->sex] );

    if( pfile->level < LEVEL_APPRENTICE )
      send( ch, "       Level: %d  [ Rank %d%s ]\n\r",
        pfile->level, pfile->rank+1, number_suffix( pfile->rank+1 ) );
    else
      send( ch, "       Level: %s\n\r",
        imm_title[ pfile->level-LEVEL_AVATAR ] );
    }

  if( is_demigod( ch ) && pfile->trust > pfile->level )
    send( ch, "       Trust: %d\n\r", pfile->trust );

  if( pfile->level < LEVEL_APPRENTICE )
    send( ch, "      Bounty: %d\n\r", pfile->bounty );

  send( ch, "        Clan: %s\n\r\n\r",
    ( pfile->clan == NULL || !knows_members( ch, pfile->clan ) ) 
    ? "none" : pfile->clan->name );

  send( ch, scroll_line[1] );
  send( ch, "\n\r" );

  send( ch, "  Last Login: %s\n\r", ltime( pfile->last_on ) );
  send( ch, "     Created: %s\n\r", ltime( pfile->created ) );

  if( is_god( ch ) ) 
    send( ch, "    Password: %s\n\r", pfile->pwd );

  if( see_account || see_private ) {
    send( ch, "        Site: %s\n\r", pfile->last_host );
    send( ch, "     Account: %s\n\r", pfile->account == NULL ?
      "none" : pfile->account->name );
    if( is_god( ch ) && pfile->account != NULL ) 
      send( ch, "  Acnt. Pswd: %s\n\r", pfile->account->pwd );
    }

  if( pfile->account != NULL &&
    ( ( email = is_set( pfile->flags, PLR_EMAIL_PUBLIC ) ) || see_private ) ) 
    send( ch, "       Email: %s%s\n\r", pfile->account->email,
      email ? "" : "  (Hidden)" );

  send( ch, "    Homepage: %s\n\r",
    pfile->homepage == empty_string ? "none" : pfile->homepage );

  if( see_private ) {
    send( ch, "\n\r" );
    send( ch, scroll_line[1] );
    send( ch, "\n\r" );
    send( ch, "     Balance: $%.2f\n\r",
      pfile->account == NULL ? 0. : (float)pfile->account->balance/100 );
    }
 
  send( ch, "\n\r" );
  send( scroll_line[1], ch );
}


/*
 *   WHO ROUTINES
 */


void do_qwho( char_data* ch, char* argument )
{
  char          tmp  [ ONE_LINE ];
  char_data*    wch;
  link_data*   link;
  int             i  = 0;
  int         count  = 0;
  int         flags;

  if( not_player( ch ) )
    return;

  if( !get_flags( ch, argument, &flags, "ib", "Qwho" ) )
    return;

  page_centered( ch, "-- PLAYERS --" );
  page( ch, "\n\r" );

  for( link = link_list; link != NULL; link = link->next ) {
    if( link->connected != CON_PLAYING )
      continue;

    count++;
    wch = link->player;

    if( !can_see_who( ch, wch )
      || ( flags == 1 && !ch->Recognizes( wch ) )
      || ( flags == 2 && !ch->Befriended( wch ) )
      || !fmatches( argument, ch->descr->name, -1 ) ) 
      continue;      
 
    if( ch->pcdata->terminal != TERM_ANSI ) {
      page( ch, "%17s%s", wch->descr->name, ++i%4 ? "" : "\n\r" );
      }
    else { 
      sprintf( tmp, "%s%17s%s%s",
        same_clan( ch, wch ) ? red( ch )
        : ( ch->Befriended( wch ) ? green( ch )
        : ( ch->Recognizes( wch ) ? yellow( ch ) : "" ) ),
        wch->descr->name, normal( ch ),
        ++i%4 ? "" : "\n\r" );
      page( ch, tmp );
      }
    }
 
  if( i%4 != 0 )
    page( ch, "\n\r" );

  if( count > max_players )
    max_players = count; 

  page( ch, "\n\r" );
  sprintf( tmp, "%d players | %d high", count, max_players );
  page_centered( ch, tmp );
}


void do_who( char_data* ch, char* )
{
  char               buf  [ THREE_LINES ];
  char               tmp  [ ONE_LINE ];
  char_data*         wch;
  link_data*        link;
  wizard_data*    imm;
  int               type;
  int                  i  = 0;
  int              count  = 0;
  int             length;
  bool             found;
  char*           abbrev;
  const char*  lvl_title;
  clan_data*        clan;
  title_data*      title;

  if( not_player( ch ) )
    return;

  page( ch, scroll_line[0] );

  for( type = 0; type < 4; type++ ) {
    found = FALSE;
    *buf = '\0';

    for( link = link_list; link != NULL; link = link->next ) {
      if( link->connected != CON_PLAYING )
        continue;

      wch = link->player;
        
      if( type == 0 ) {
        if( wch->shdata->level < LEVEL_APPRENTICE )
          continue;
        }
      else if( wch->shdata->level >= LEVEL_APPRENTICE )
        continue;
      else if( ch->Recognizes( wch ) ) {
        if( type == 3 || ch->Befriended( wch ) != ( type == 1 ) )
          continue; 
        }
      else
        if( type != 3 )
          continue;

      count++;

      if( !can_see_who( ch, wch ) )
        continue;

      if( !found ) {
        if( i++ != 0 )
          page( ch, "\n\r" );
        page_title( ch, type > 1 ? 
          ( type == 2 ? "Known" : "Unknown" ) :
          ( type == 0 ? "Immortals" : "Befriended" ) );
        found = TRUE;
        }  

      clan   = wch->pcdata->pfile->clan;
      imm    = wizard( wch );
      abbrev = "  ";

      if( wch->shdata->level >= LEVEL_APPRENTICE ) {
        lvl_title = ( imm != NULL
          && imm->level_title != empty_string ) 
          ? imm->level_title
          : imm_title[ wch->shdata->level-LEVEL_AVATAR ];
        length = strlen( lvl_title );
        strcpy( buf, "[               ]" );
        memcpy( buf+8-length/2, lvl_title, length );
        }
      else {
        if( clan != NULL
          && ( title = get_title( wch->pcdata->pfile ) ) != NULL
          && knows_members( ch, clan ) ) {
          abbrev = "CL";
	  }
        else if( wch->pcdata->trust >= LEVEL_AVATAR ) {
          if( has_permission( wch, PERM_APPROVE ) )
            abbrev = "AV";
          else if( ch->pcdata->trust >= LEVEL_AVATAR )
            abbrev = "IP";
	  }

        if( is_incognito( wch, ch ) ) {
          sprintf( buf, "[   ??   %s %s ]",
            race_table[wch->shdata->race].abbrev, abbrev );
          }
        else {
          sprintf( buf, "[ %2d %s %s %s ]", wch->shdata->level,
            clss_table[wch->pcdata->clss].abbrev,
            race_table[wch->shdata->race].abbrev, abbrev );
          }
        }

      if( type > 0 ) {
        sprintf( tmp, " %%s%%s%%s%%-%ds  %%s\n\r", 
          53-strlen( wch->descr->name ) );

        sprintf( buf+17, tmp,
          bold_cyan_v( ch ), wch->descr->name, normal( ch ),
          wch->pcdata->title, clan != NULL && knows_members( ch, clan )
          ? clan->abbrev : " -- " );
        }
      else {
        if( imm->wizinvis > 0 
          && is_set( wch->pcdata->pfile->flags, PLR_WIZINVIS ) )
          sprintf( tmp, " %%s%%s%%s%%-%ds   %d\n\r", 
            53-strlen( wch->descr->name ), imm->wizinvis );
        else  
          sprintf( tmp, " %%s%%s%%s%%-%ds\n\r", 
            53-strlen( wch->descr->name ) );

        sprintf( buf+17, tmp,
          bold_cyan_v( ch ), wch->descr->name, normal( ch ),
          wch->pcdata->title );
        }

      page( ch, buf );
      }
    }
  
  if( count > max_players )
    max_players = count; 

  page( ch, "%28s[ %d players | %d high ]\n\r", "", count, max_players );
  page( ch,scroll_line[0] );
}


void w3_who( )
{
  player_data*  pc;
  FILE*         fp;

  if( ( fp = open_file( W3_DIR, "who.html", "w" ) ) == NULL )
    return;

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body bgcolor=\"#ffffff\">\n" );
  fprintf( fp, "<center>\n" );
  fprintf( fp, "<font size=+4> Players </font>\n" );
  fprintf( fp, "</center>\n" );
  fprintf( fp, "<hr><br>\n" );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i]; 
    if( pc->In_Game( )  
      && !is_set( pc->pcdata->pfile->flags, PLR_WIZINVIS ) )
      fprintf( fp, "%s %s <br>\n",
        race_table[ pc->shdata->race ].abbrev,
        pc->descr->name );
    }

  fprintf( fp, "<hr><br>\n" );
  fprintf( fp, "<a href=\"./index.html\"> Return to Main </a></p>\n" );

  fclose( fp );
}  


/*
 *   USERS ROUTINE
 */


bool Wizard_Data :: See_Account( pfile_data* pfile )
{
  if( is_demigod( this ) || pcdata->pfile == pfile )
    return TRUE;

  if( pfile->level >= LEVEL_APPRENTICE )
    return FALSE;

  return has_permission( this, PERM_SITE_NAMES ); 
}


void do_users( char_data* ch, char* argument )
{
  char            tmp  [ THREE_LINES ];
  player_data* victim;
  link_data*     link;
  int           count  = 0;
  int           flags;
  int          length;

  if( !get_flags( ch, argument, &flags, "aiw", "Users" ) )
    return;

  length = strlen( argument );

  if( is_set( &flags, 0 ) ) {
    sprintf( tmp, "%-15s   %s\n\r",
      "Name", "Appearance" );
    }
  else if( is_set( &flags, 1 ) ) {
    sprintf( tmp, "%-18s%3s %3s %3s %3s   %3s %3s   %4s %4s %4s   %-7s\n\r",
      "Name", "Cls", "Rce", "Ali", "Lvl", "Trs", "Idl",
      "Hits", "Enrg", "Move", "Bank" );
    }
  else if( is_set( &flags, 2 ) ) {
    sprintf( tmp, "%-15s  %4s  %s\n\r", "Name", "Idle", "What?" );
    }
  else {
    sprintf( tmp, "%-15s   %-30s   %s\n\r",
      "Name", "Site", "Location" );
    }

  page_underlined( ch, tmp );

  for( link = link_list; link != NULL; link = link->next ) {
    victim = link->player;
    if( ( is_god( ch ) || ( victim != NULL && can_see_who( ch, victim ) ) )
      && !strncasecmp( argument, victim == NULL ? ""
      : victim->descr->name, length ) ) {
      if( flags == 0 ) {
        sprintf( tmp, "%-15s   %-30s   %s",
          victim == NULL ? "(Logging In)" : victim->descr->name,
          is_demigod( ch ) || victim == ch
          || ( victim != NULL && victim->shdata->level < LEVEL_APPRENTICE ) 
          ? &link->host[ max( 0, strlen( link->host )-30 ) ] : "(protected)",
          ( victim == NULL || victim->array == NULL )
          ? "(nowhere)" : victim->array->where->Location( ) );
        truncate( tmp, 78 );
        strcat( tmp, "\n\r" );
        }
      else {
        if( link->connected != CON_PLAYING ) {
          sprintf( tmp, "-- Logging In --\n\r" );
	  }
        else if( is_set( &flags, 0 ) ) {
          sprintf( tmp, "%-15s   %s\n\r",
            victim->descr->name, victim->descr->singular );          
          }
        else if( is_set( &flags, 2 ) ) {
          sprintf( tmp, "%-15s  %4d  %s\n\r",
            victim->descr->name, (int) current_time-victim->timer,
            "??" );          
          }
        else {
          sprintf( tmp,
            "%-18s%3s %3s  %2s %3d   %3d %3d   %4d %4d %4d   %-7d\n\r",
            victim->descr->name,
            clss_table[victim->pcdata->clss].abbrev,
            race_table[victim->shdata->race].abbrev,
            alignment_table[ victim->shdata->alignment ].abbrev,
            victim->shdata->level,
            victim->pcdata->trust, (int) current_time-victim->timer,
            victim->max_hit, victim->max_mana, victim->max_move,
            victim->bank );
          }
        }
      page( ch, tmp );
      count++;
      }
    }
}


/*
 *   HOMEPAGE
 */


void do_homepage( char_data* ch, char* argument )
{
  if( is_mob( ch ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "What is your homepage address?\n\r" );
    return;
    } 

  if( strlen( argument ) > 60 ) {
    send( ch, "You homepage address must be less than 60 characters.\n\r" );
    return;
    } 

  free_string( ch->pcdata->pfile->homepage, MEM_PFILE );
  ch->pcdata->pfile->homepage = alloc_string( argument, MEM_PFILE );

  send( ch, "Your homepage is set to %s.\n\r", argument );
}  





