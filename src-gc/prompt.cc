#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "syslog.h"
#include "define.h"
#include "struct.h"


bool sprintf_prompt( char*, char_data*, const char*, const char* ); 


/*
 *   PROMPT LIST
 */


const char* prompt_default = 
  "?p'-- MORE -- '<%f?f|%hhp %ee ?m'[%mmv]'!m'%vmv' ?b'%c'!b'%d'> ";

const char* prompt_immortal =
  "?p'-MORE- '?f'%f '%T %d> ";

const char* prompt_warrior =
  "?p'-- MORE -- '<%f?f|%hhp ?m'[%mmv]'!m'%vmv'> ";

const char* prompt_cleric = 
  "?p'-- MORE -- '<%f?f|%hhp %ee ?m'[%mmv]'!m'%vmv'?l' %C'> ";

const char* prompt_simple = 
  "?p'-- MORE -- '<%f?f|%hhp %ee ?m'[%mmv]'!m'%vmv'> ";

const char* prompt_complex = 
  "?p' -- MORE -- '<%f?f|%hhp %ee ?m'[%mmv]'!m'%vmv' %gwm %xxp?l' %C'\
 %d?b' %c'> ";

const char* prompt_color = 
  "?p' -- MORE -- '<%f?f|@R%hhp@n @G%ee@n @B?m'[%mmv]'!m'%vmv'@n %gwm\
 %xxp?l' %C' %d?b' %c'> ";


/*
 *   DO_PROMPT COMMAND   
 */


void do_prompt( char_data* ch, char* argument )
{
  const char* prompt_list [] = {
    "default",   empty_string,
    "immortal",  prompt_immortal,
    "simple",    prompt_simple, 
    "warrior",   prompt_warrior,
    "cleric",    prompt_cleric,
    "complex",   prompt_complex,
    "color",     prompt_color,
    "" };

  int  i;
  int  j;

  if( not_player( ch ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Prompt:\n\r%s\n\r\n\r", ch->pcdata->prompt == empty_string
      ? prompt_default : ch->pcdata->prompt );
    send( ch, "Set your prompt to what?\n\r" );
    return;
    }

  if( strlen( argument ) > 120 ) {
    send( ch, "You can't set a prompt longer than 120 characters.\n\r" );
    return;
    }

  for( i = 0, j = 0; argument[i] != '\0'; i++ )
    if( argument[i] == '%' ) {
      j++;
      i++;
      }

  if( j > 20 ) {
    send( ch, "A prompt may contain at most 20 substitutions.\n\r" );
    return;
    }

  free_string( ch->pcdata->prompt, MEM_PLAYER );

  for( i = 0; *prompt_list[2*i] != '\0'; i++ )
    if( !strcasecmp( argument, prompt_list[2*i] ) ) {
      ch->pcdata->prompt = alloc_string( prompt_list[2*i+1], MEM_PLAYER );  
      return;
      }

  ch->pcdata->prompt = alloc_string( argument, MEM_PLAYER );

  return; 
}


/*
 *   SUPPORT FUNCTIONS
 */


void prompt_flags( char* flags, char_data* ch )
{
  bool request = ( !is_empty( request_app ) 
    && has_permission( ch, PERM_APPROVE )
    && ch->shdata->level < LEVEL_APPRENTICE )
    || ( !is_empty( request_imm ) && is_builder( ch ) );

  sprintf( flags, "%s%s%s%s%s%s%s%s%s%s%s%s",
    request                                           ? "R" : "",
    in_sanctuary( ch, FALSE )                         ? "S" : "", 
    can_pkill( ch, NULL, FALSE )                      ? "P" : "", 
    is_set( ch->pcdata->pfile->flags, PLR_WIZINVIS )  ? "w" : "",
    is_set( ch->affected_by, AFF_INVISIBLE )          ? "i" : "",
    is_set( ch->pcdata->pfile->flags, PLR_SNEAK )     ? "s" : "",
    is_set( &ch->status, STAT_HIDING )                ? "h" : "",
    is_set( &ch->status, STAT_CAMOUFLAGED )           ? "c" : "",
    is_set( &ch->status, STAT_BERSERK )               ? "b" : "",
    is_set( ch->pcdata->pfile->flags, PLR_TRACK )     ? "t" : "",
    is_set( ch->pcdata->pfile->flags, PLR_SEARCHING ) ? "x" : "",
    is_set( ch->pcdata->pfile->flags, PLR_PARRY )     ? "p" : "" );
}


/*
 *  STATUS BAR PROMPT ROUTINE
 */


void prompt_ansi( link_data* link )
{
  char          tmp  [ FIVE_LINES ];
  char        flags  [ 10 ];
  char        exits  [ TWO_LINES ];
  char         time  [ 10 ];
  char_data*     ch  = link->character;
  int             i;

  move_cursor( ch, ch->pcdata->lines-1, 1 ); 
  
  prompt_flags( flags, ch );

  i = exits_prompt( exits, ch );
  add_spaces( exits, 6-i );

  if( ch->shdata->level >= LEVEL_APPRENTICE ) {
    sprintf_time( time, current_time, 0 );
    sprintf( tmp,
      "-- Time: %s   Room: %-5d   Hp: %-4d En: %-4d Mv: %-4d Ex: %s %8s --",
      time, ch->in_room->vnum, ch->hit, ch->mana, ch->move,
      exits, flags );
    }
  else
    sprintf( tmp,
      "-- Hp: %s%4d/%-4d%s En: %s%4d/%-4d%s Mv: %s%4d/%-4d%s\
 Xp: %-8d Ex: %s %8s --",
      bold_red_v( ch ), ch->hit, ch->max_hit, normal( ch ),
      blue( ch ), ch->mana, ch->max_mana, normal( ch ),
      green( ch ), ch->move, ch->max_move, normal( ch ),
      ch->species != NULL ? 0 : ( exp_for_level( ch )-ch->exp ),
      exits, flags );

  send( ch, tmp );

  return;
}


/* 
 *   NORMAL PROMPT ROUTINE
 */


void prompt_nml( link_data* link )
{
  char          tmp  [ EIGHT_LINES ];
  char        flags  [ 10 ];
  char_data*     ch  = link->character;

  prompt_flags( flags, ch );

  send( link, "\n\r%s", normal( ch ) );
  sprintf_prompt( tmp, ch, ch->pcdata->prompt == empty_string
    ? prompt_default : ch->pcdata->prompt, flags );

  send( link, tmp );
}


/*
 *   SETTABLE PROMPT ROUTINE
 */


bool sprintf_prompt( char* output, char_data* ch, const char* input,
  const char* flags ) 
{
  char              tmp  [ TWO_LINES ]; 
  int             i,j,k;
  bool              opt;
  int            length  = strlen( input );
  player_data*   player  = ch->link->player;

  for( i = 0, j = 0; i < length; i++ ) {
    if( input[i] == '@' ) {
      for( i++, k = 0; ; k++ ) {
        if( color_key[k] == '\0' ) {
          output[j++] = input[i];
          break;
	  } 
        if( color_key[k] == input[i] ) {
          if( ch->pcdata->terminal != TERM_DUMB ) {
            strcpy( &output[j],
              term_table[ ch->pcdata->terminal ].codes(
              ch->pcdata->color[ k == 0 ? COLOR_DEFAULT : COLOR_MILD+k-1 ] ) );
            j += strlen( &output[j] );
	    }
          break;
          }
        }
      continue;
      }

    if( input[i] == '%' ) {
      switch( input[++i] ) {
       case 'h'  : sprintf( &output[j], "%d", ch->hit );               break;
       case 'H'  : sprintf( &output[j], "%d", ch->max_hit );           break;
       case 'e'  : sprintf( &output[j], "%d", ch->mana );              break;
       case 'E'  : sprintf( &output[j], "%d", ch->max_mana );          break;
       case 'v'  : sprintf( &output[j], "%d", ch->move );              break;
       case 'V'  : sprintf( &output[j], "%d", ch->max_move );          break;
       case 'g'  : sprintf( &output[j], "%d", min_group_move( ch ) );  break;
       case 'G'  : sprintf( &output[j], "%d", player->gossip_pts );    break;
       case 'f'  : strcpy( &output[j], flags );                        break;
       case '\\' : strcpy( &output[j], "\n\r" );                       break;
       case 'd'  : exits_prompt( &output[j], ch );                     break;

       case 'c' : 
         strcpy( &output[j], condition_short( ch, ch->fighting ) );
         break;

       case 'C' : 
         strcpy( &output[j], condition_short( ch, ch->leader ) );
         break;

       case 's' : 
         strcpy( &output[j], condition_short( ch ) );
         break;

       case 't' :
         sprintf( &output[j], "%d:%02d", weather.hour, weather.minute );
         break;

       case 'T' :
         sprintf_time( &output[j], current_time, player->timezone );
         break;

       case 'x' :
        sprintf( &output[j], "%d",
          ch->species != NULL ? 0 : exp_for_level( ch )-ch->exp );
        break;

       case 'm' :
        if( ch->mount != NULL )
          sprintf( &output[j], "%d", ch->mount->move );
        else
          sprintf( &output[j], "-" );
        break;

       case 'M' :
        if( ch->mount != NULL )
          sprintf( &output[j], "%d", ch->mount->max_move );
        else
          sprintf( &output[j], "-" );
        break;

       default:
        output[j]   = input[i];
        output[j+1] = '\0';
        }
      j += strlen( &output[j] );
      continue;
      }

    if( input[i] == '?' || input[i] == '!' ) {      
      opt = ( ( input[++i] == 'p' && ch->link->paged != NULL )
	|| ( input[i] == 'f' && *flags != '\0' )
        || ( input[i] == 'm' && ch->mount != NULL )
        || ( input[i] == 'b' && ch->fighting != NULL )
        || ( input[i] == 'l' && ch->leader != NULL ) );
      if( input[i-1] == '!' )
        opt = !opt;
      if( input[++i] != '\'' ) {
        if( opt )
          output[j++] = input[i];
        continue;
        }

      for( k = 0, i++; input[i] != '\0' && input[i] != '\''; i++ )
        tmp[k++] = input[i];
      tmp[k] = '\0';

      if( opt ) {
        sprintf_prompt( &output[j], ch, tmp, flags );
        j += strlen( &output[j] ); 
        }
      continue;
      }

    output[j++] = input[i];
    }

  output[j] = '\0';

  return TRUE;
}    









