#include "define.h"
#include "struct.h"


/*
 *   APPEARANCE FUNCTIONS
 */


const char* appearance_string = "Your %s has now been set.  When you have\
 completed it, your %s and your description please type request avatar.\
 See help appearance and the identity command for more information.\r\n";

const char* unchangable_msg = "You may not edit your description, keywords,\
 or appearance once approved.\r\nYou may request an avatar to be unapprove\
 you after which you must be\r\nreapproved to gain levels.\r\n";


void do_appearance( char_data* ch, char* argument )
{
  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( !has_permission( ch, PERM_APPROVE ) 
    && is_set( ch->pcdata->pfile->flags, PLR_APPROVED ) ) {
    send( ch, unchangable_msg );
    return;
    }
 
  if( *argument == '\0' ) {
    send( ch, "What do you want your appearance set to?\r\n" );
    return;
    }

  if( !strncasecmp( argument, "a ", 2 )
    || !strncasecmp( argument, "an ", 3 ) ) {
    send( ch, "Your appearance should not start with a or an.\r\n" );
    return;
    }

  if( ispunct( argument[ strlen( argument )-1 ] ) ) {
    send( ch, "Appearances should not end with punctuation.\r\n" );
    return;
    }

  if( strlen( argument ) >= 50 ) {
    send( ch, "Appearances must be less than 50 characters.\r\n" );
    return;
    }

  *argument = tolower( *argument );
  free_string( ch->pcdata->tmp_short, MEM_PLAYER );
  ch->pcdata->tmp_short = alloc_string( argument, MEM_PLAYER );

  fsend( ch, appearance_string, "appearance", "keywords" );
}


void do_keywords( char_data* ch, char* argument )
{
  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( !has_permission( ch, PERM_APPROVE ) 
    && is_set( ch->pcdata->pfile->flags, PLR_APPROVED ) ) {
    send( ch, unchangable_msg );
    return;
    }
 
  if( *argument == '\0' ) {
    send( ch, "What do you want you keywords set to?\r\n" );
    return;
    }

  if( is_name( ch->pcdata->pfile->name, argument ) ) {
    send( ch, "Your keywords should NOT include your name.\r\n" );
    return;
    }

  free_string( ch->pcdata->tmp_keywords, MEM_PLAYER );
  ch->pcdata->tmp_keywords = alloc_string( argument, MEM_PLAYER );

  fsend( ch, appearance_string, "keywords", "appearance" );
}


void do_descript( char_data* ch, char* argument )
{
  char_data*     victim  = ch;
  wizard_data*   imm;

  if( is_confused_pet( ch ) || is_switched( ch )
    || ch->species != NULL ) 
    return;

  if( ( imm = wizard( ch ) ) != NULL && imm->player_edit != NULL )
    victim = imm->player_edit;

  if( !has_permission( ch, PERM_APPROVE ) 
    && is_set( ch->pcdata->pfile->flags, PLR_APPROVED ) ) {
    send( ch, unchangable_msg );
    return;
    }
 
  victim->descr->complete = edit_string( ch, argument,
    victim->descr->complete, MEM_DESCR );
}


/*
 *   REQUEST COMMAND
 */


request_array request_imm;
request_array request_app;


class Request_Data
{
 public: 
  pfile_data*   pfile;
  char*        reason;

  Request_Data( char_data* ch, char* argument  ) {
    record_new( sizeof( request_data ), MEM_REQUEST );
    pfile  = ch->pcdata->pfile;
    reason = alloc_string( argument, MEM_REQUEST );
    }

  ~Request_Data( ) {
    record_delete( sizeof( request_data ), MEM_REQUEST );
    free_string( reason, MEM_REQUEST );
    }
};


void display( char_data* ch, request_array& array, const char* word,
  bool& first )
{
  if( is_empty( array ) ) 
    return;

  if( first ) {
    first = FALSE;
    page_underlined( ch, "%-15s Reason\r\n", "Name" );
    }

  page( ch, "\r\n" );
  page( ch, "-- %s --\r\n", word );
  page( ch, "\r\n" );

  for( int i = 0; i < array; i++ )
    page( ch, "%-15s %s\r\n", array[i]->pfile->name,
      array[i]->reason );
}


bool includes( request_array& array, char_data* ch )
{
  for( int i = 0; i < array; i++ )
    if( array[i]->pfile == ch->pcdata->pfile )
      return TRUE;

  return FALSE;
}


bool remove( request_array& array, char_data* ch )
{
  for( int i = 0; i < array; i++ )
    if( array[i]->pfile == ch->pcdata->pfile ) {
      delete array[i];
      remove( array, i );
      return TRUE;
      }

  return FALSE;
}


void do_request( char_data* ch, char* argument )
{
  char* tmp;

  if( is_mob( ch ) )
    return;

  if( *argument == '\0' ) {
    bool first = TRUE;
    display( ch, request_app, "Avatar", first );
    display( ch, request_imm, "Immortal", first ); 
    if( first )
      send( ch, "Request queues are empty.\r\n" );
    return;
    }

  if( matches( argument, "cancel" ) ) {
    if( remove( request_app, ch )
      | remove( request_imm, ch ) )
      send( ch, "Requests cancelled.\r\n" );
    else
      send( ch, "You have no requests pending.\r\n" ); 
    return;
    } 

  if( matches( argument, "avatar" ) ) {
    if( includes( request_app, ch ) ) {
      send( ch, "You have already requested an avatar.\r\n" );
      return;
      }
    if( *argument == '\0'
      && is_set( ch->pcdata->pfile->flags, PLR_APPROVED ) ) {
      send( ch,
        "Since you are already approved you must specify a reason.\r\n" );
      return;
      }
    request_app += new request_data( ch, argument );
    send( ch, "Request for an avatar registered.\r\n" );
    if( *argument == '\0' ) 
      sprintf( tmp = static_string( ),
        "%s requesting an avatar for approval.",
        ch->Name( ) );
    else
      sprintf( tmp = static_string( ),
        "%s requesting an avatar with reason '%s'.",
        ch->Name( ), argument );
    info( empty_string, LEVEL_AVATAR, tmp,
       IFLAG_REQUESTS, 2, ch );
    return;
    }

  if( matches( argument, "immortal" ) ) {
    if( includes( request_imm, ch ) ) {
      send( ch, "You have already requested immortal attention.\r\n" );
      return;  
      }
    if( *argument == '\0' ) {
      send( ch, "You must include a reason when requesting an\
 immortal.\r\n" );
      return;
      }
    request_imm += new request_data( ch, argument );
    send( ch, "Request for an immortal registered.\r\n" );
    sprintf( tmp = static_string( ),
      "%s requesting an immortal with reason '%s'.",
      ch->Name( ), argument );
    info( empty_string, LEVEL_BUILDER, tmp,
       IFLAG_REQUESTS, 1, ch );
    return;
    }

  send( ch, "Unknown syntax - see help request.\r\n" );
}
 

/*
 *   IMMORTAL COMMANDS
 */


void do_approve( char_data* ch, char* argument )
{
  char             tmp  [ THREE_LINES ];
  player_data*  player;
  int            flags;
  bool           found  = FALSE;
  wizard_data*     imm  = wizard( ch );

  in_character = FALSE;

  if( !get_flags( ch, argument, &flags, "slpfr", "Approve" ) )
    return;

  if( is_set( &flags, 1 ) ) {
    for( int i = 0; i < player_list; i++ ) {
      player = player_list[i];
      if( !player->In_Game( )
        || ( player->pcdata->tmp_short == empty_string
        && player->pcdata->tmp_keywords == empty_string )
        || !can_see_who( ch, player ) )
        continue;

      if( !found ) {
        found = TRUE;
        sprintf( tmp, "%-15s   %s\r\n", "Name", "Appearance" );
        page_underlined( ch, tmp );
        }
      page( ch, "%-15s   %s\r\n",
        player->descr->name, player->descr->singular );          
      }
    if( !found ) 
      send( ch, "There are no players with unapproved appearances.\r\n" );
    return;
    }     

  if( *argument == '\0' ) {
    if( ( player = imm->player_edit ) == NULL ) {
      send( ch, "Which player?\r\n" );
      return;
      }
    }
  else {
    if( ( player = one_player( ch, argument, "approve",
      (thing_array*) &player_list ) ) == NULL )
      return;
    }

  if( is_set( &flags, 3 ) ) {
    if( is_set( player->pcdata->pfile->flags, PLR_APPROVED ) ) {
      send( ch, "%s is already marked as approved.\r\n",
        player->descr->name );
      }
    else {
      set_bit( player->pcdata->pfile->flags, PLR_APPROVED );
      send( ch, "You mark %s as approved.\r\n",
        player->descr->name );
      remove( request_app, player );
      }
    return;
    }

  if( is_set( &flags, 4 ) ) {
    if( !is_set( player->pcdata->pfile->flags, PLR_APPROVED ) ) {
      send( ch, "%s isn't approved.\r\n", player->descr->name );
      }
    else {
      remove_bit( player->pcdata->pfile->flags, PLR_APPROVED );
      send( ch, "You unapprove %s.\r\n", player->descr->name );
      sprintf( tmp, "Unapproved by %s.", ch->descr->name );
      player_log( player, tmp );
      }
    return;
    }

  if( is_set( &flags, 2 ) ) {
    if( player == ch ) {
      send( ch, "You are now editting yourself.\r\n" );
      imm->player_edit = NULL;
      return;
      }
    if( player->pcdata->trust >= LEVEL_AVATAR
      || ( player->shdata->level > 10
      && ch->pcdata->trust == LEVEL_AVATAR ) ) {
      send( ch, "You don't have permission to edit their description.\r\n" );
      return;
      }  
    send( ch, "You are now editting %s.\r\n", player->descr->name );
    send( player, "%s is now editting your appearance.\r\n",
      ch->descr->name );
    imm->player_edit = player;
    return;
    }

  if( !is_set( &flags, 0 ) ) {
    page( ch, "   Name: %s\r\n", player->descr->name );
    page( ch, "   Race: %s\r\n", race_table[ player->shdata->race ].name );
    page( ch, "  Class: %s\r\n", clss_table[ player->pcdata->clss ].name );
    page( ch, "    Sex: %s\r\n", sex_name[ player->sex ] );

    page( ch, "\r\n[Current]\r\n" );
    page( ch, "  Appearance: %s\r\n", player->descr->singular );
    page( ch, "    Keywords: %s\r\n", player->descr->keywords );
    page( ch, "\r\n[New]\r\n" );
    page( ch, "  Appearance: %s\r\n", player->pcdata->tmp_short );
    page( ch, "    Keywords: %s\r\n\r\n", player->pcdata->tmp_keywords );
    page( ch, "[Description]\r\n" );
    page( ch, player->descr->complete );
    return;
    }

  if( player->pcdata->tmp_short != empty_string ) {
    free_string( player->descr->singular, MEM_DESCR );
    player->descr->singular    = player->pcdata->tmp_short;
    player->pcdata->tmp_short  = empty_string;
    }
  
  if( player->pcdata->tmp_keywords != empty_string ) {
    free_string( player->descr->keywords, MEM_DESCR );
    player->descr->keywords      = player->pcdata->tmp_keywords;
    player->pcdata->tmp_keywords = empty_string;
    }

  send( ch, "You have approved %s.\r\n", player );
  send( player, "%s has approved you.\r\n", ch );

  sprintf( tmp, "Approved by %s.", ch->real_name( ) );
  player_log( player, tmp );

  set_bit( player->pcdata->pfile->flags, PLR_APPROVED );
}


void do_pbug( char_data* ch, char* argument )
{
  char_data*      victim  = ch;
  wizard_data*       imm;
  account_data*  account;

  if( ( imm = wizard( ch ) ) == NULL ) {
    bug( "Pbug: Non-wizard accessing it!?" );
    return;
    }

  if( imm->player_edit != NULL )
    victim = imm->player_edit;

  if( victim->pcdata == NULL ) {
    send( ch, "This command my only be used on players.\r\n" );
    return;
    }

  if( ( account = victim->pcdata->pfile->account ) == NULL ) {
    send( ch, "%s doesn't have an account.\r\n", victim );
    return;
    }
 
  account->notes = edit_string( ch, argument, account->notes, MEM_DESCR );
}
