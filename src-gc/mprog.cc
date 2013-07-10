#include <sys/types.h>
#include <stdio.h>
#include "stdlib.h"
#include "define.h"
#include "struct.h"


const char* mprog_trigger[ MAX_MPROG_TRIGGER ] = { "entering",
  "leaving", "asking", "none", "death", "kill", "give", "reset", "tell",
  "skin", "timer", "attack", "order" };

const char* mprog_value[ MAX_MPROG_TRIGGER ] = { "direction",
  "direction", "unused", "direction" };


/*
 *   MPROG_DATA CLASS
 */


void extract( mprog_data* mprog, wizard_data* wizard )
{
  for( int i = 0; i < mprog->data; i++ ) {
    wizard->mpdata_edit = mprog->data[i];
    extract( wizard, offset( &wizard->mpdata_edit, wizard ), "mpdata" );
    }

  wizard->mprog_edit = mprog;
  extract( wizard, offset( &wizard->mprog_edit, wizard ), "mprog" );
}


/*
 *   DISPLAY ROUTINE
 */


void mprog_data :: display( char_data* ch )
{
  mprog_data*  mprog;
  int              i  = 0;

  if( species == NULL ) {
    send( ch, "%-10s %-10s %s\n\r", "??", "Mprog", "Null Species??" );
    return;
    }

  for( mprog = species->mprog; mprog != this && mprog != NULL;
    mprog = mprog->next, i++ );

  if( mprog == NULL ) {
    send( ch, "Mob %-11d %-20s %s\n\r", species->vnum, "Fight Code", 
      species->descr->name );
    }
  else {
    send( ch, "Mob %-11d Mprog %-8d %s\n\r", species->vnum, i, 
      species->descr->name );
    }

  return;
}     


/*
 *   EDITING FUNCTIONS
 */


void do_mpedit( char_data* ch, char* argument )
{
  char               arg  [ MAX_STRING_LENGTH ];
  char               buf  [ MAX_STRING_LENGTH ];
  mprog_data*      mprog;
  species_data*  species;
  wizard_data*    wizard;
  int                  i;

  wizard  = (wizard_data*) ch;

  if( ( species = wizard->mob_edit ) == NULL ) {
    send( "You aren't editing any mob.\n\r", ch );
    return;
    }

  if( *argument == '\0' ) {
    if( ( mprog = species->mprog ) == NULL ) {
      send( "This mob has no programs.\n\r", ch );
      return;
      }
    for( i = 0, mprog = species->mprog; mprog != NULL;
      mprog = mprog->next ) {
      sprintf( buf, "[%2d]  %20s %s\n\r", ++i,
        mprog_trigger[mprog->trigger], mprog->string );
      send( buf, ch );
      }
    return;
    }

  argument = one_argument( argument, arg );

  if( is_number( arg ) ) {
    i = atoi( arg );
    for( mprog = species->mprog; mprog != NULL && i != 1;
      mprog = mprog->next, i-- );
    if( mprog == NULL ) {
      send( ch, "No mprog by that number.\n\r" );
      return;
      }
    wizard->mprog_edit  = mprog;
    wizard->mpdata_edit = NULL;
    send( "You now edit that mprog.\n\r", ch );
    return;
    }

  if( !ch->can_edit( species ) )
    return;

  if( !strcasecmp( arg, "new" ) ) {
    mprog               = new mprog_data;
    mprog->next         = NULL;
    mprog->trigger      = 0;
    mprog->value        = -1;
    mprog->string       = empty_string;
    mprog->code         = empty_string;
    wizard->mprog_edit  = mprog;
    append( species->mprog, mprog );
    send( ch, "Mprog added.\n\r" );
    return;
    }

  if( !strcasecmp( arg, "delete" ) ) {
    if( ( mprog = wizard->mprog_edit ) == NULL ) {
      send( "You aren't editing any mprog.\n\r", ch );
      return;
      }
    remove( species->mprog, mprog );
    extract( mprog, wizard );
    delete mprog;
    send( ch, "Mprog deleted.\n\r" );
    return;
    }

  send( ch, "Illegal syntax.\n\r" );
}


void do_mpcode( char_data* ch, char *argument )
{
  char               tmp  [ MAX_STRING_LENGTH ];
  program_data*    mprog;
  species_data*  species;
  wizard_data*    wizard;

  wizard = (wizard_data*) ch;

  if( ( species = wizard->mob_edit ) == NULL ) {
    send( "You aren't editing any mob - use medit <mob>.\n\r", ch );
    return;
    }

  if( *argument != '\0' ) {
    if( !ch->can_edit( species ) )
      return;
    sprintf( tmp, "mpc: %s", argument );
    mob_log( ch, species->vnum, tmp );
    zero_exp( species );
    }

  if( ( mprog = wizard->mprog_edit ) == NULL )
    mprog = species->attack;

  var_ch = ch;
  mprog->code = edit_string( ch, argument, mprog->code, MEM_MPROG );
  compile( mprog );

  if( mprog == species->attack )
    mprog->active = 1;

  return;
}


void do_mpdata( char_data* ch, char* argument )
{
  program_data*    mprog;
  species_data*  species;
  wizard_data*    wizard;

  wizard = (wizard_data*) ch;
  mprog  = wizard->mprog_edit;

  if( ( species = wizard->mob_edit ) == NULL ) {
    send( ch, "You aren't editing any mob.\n\r" );
    return;
    }

  if( mprog == NULL )
    mprog = species->attack;

  if( wizard->mpdata_edit != NULL ) {
    if( !strcasecmp( argument, "exit" ) ) {
      wizard->mpdata_edit = NULL;
      send( ch, "Mpdata now operates on the data list.\n\r" );
      return;
      }
    wizard->mpdata_edit->text = edit_string( ch, 
      argument, wizard->mpdata_edit->text, MEM_EXTRA );
    }
  else {
    edit_extra( mprog->data, wizard,
      offset( &wizard->mpdata_edit, wizard ), argument, "mprog" );
    }

  var_ch = ch;
  compile( mprog ); 

  if( mprog == species->attack )
    mprog->active = 1;

  return;
}


void do_mpstat( char_data* ch, char* )
{
  char              buf  [ 3*MAX_STRING_LENGTH ];
  mprog_data*     mprog;
  wizard_data*   wizard; 

  wizard = (wizard_data*) ch;

  if( ( mprog = wizard->mprog_edit ) == NULL ) {
    send( "You aren't editing any mprog.\n\r", ch );
    return;
    }

  page( ch, "%10s : %s\n\r", "Trigger", mprog_trigger[ mprog->trigger ] );
  page( ch, "%10s : %d\n\r", "Value", mprog->value );
  page( ch, "%10s : %s\n\r\n\r", "String", mprog->string );

  sprintf( buf, "[Code]\n\r%s\n\r", mprog->code );
  page( ch, buf );

  show_extras( ch, mprog->data );

  return;
}


void do_mpset( char_data* ch, char *argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  mprog_data*     mprog;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;

  if( ( mprog = wizard->mprog_edit ) == NULL ) {
    send( ch, "You aren't editing any mprog.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    do_mpstat( ch, argument );
    return;
    }

  if( !ch->can_edit( wizard->mob_edit ) )
    return;

  if( matches( argument, "trigger" ) ) {
    set_type( ch, argument, mprog->trigger, "Trigger",
      "mprog", MAX_MPROG_TRIGGER, mprog_trigger );
    return;
    }

  if( matches( argument, "value" ) ) { 
    mprog->value = atoi( argument );
    sprintf( buf, "Value on mprog set to %d.\n\r", mprog->value );
    send( buf, ch );
    return;
    }

  if( matches( argument, "string" ) ) {
    set_string( ch, argument, mprog->string,
      "string", MEM_MPROG );
    return;
    }

  send( ch, "Syntax: mpset <field> <value>\n\r" );
  return;
}


void do_mpflag( char_data* ch, char* )
{
  send( ch, "Under construction.\n\r" );

  return;
}


/*
 *   ASK ROUTINE
 */


void do_ask( char_data* ch, char* argument )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  char_data*   victim;
  mprog_data*   mprog;

  if( *argument == '\0' ) {
    send( ch, "Ask whom?\n\r" );
    return;
    }

  argument = one_argument( argument, arg );

  if( ( victim = one_character( ch, arg, "ask",
    ch->array ) ) == NULL )
    return;

  if( victim->pcdata != NULL ) {
    send( ch, "They seem to ignore your request.\n\r" );
    return;
    }

  if( !strcasecmp( "about ", argument ) )
    argument += 6;
  else if( !strcasecmp( "for ", argument ) )
    argument += 4;

  skip_spaces( argument ); 

  if( *argument == '\0' ) {
    send( ch, "Ask about what?\n\r" );
    return;
    }

  for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_ASKING 
      && is_name( argument, mprog->string ) ) {
      var_ch  = ch;
      var_mob = victim;
      var_room = ch->in_room;
      execute( mprog );
      return;
      }

  for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_ASKING 
      && is_name( "default", mprog->string ) ) {
      var_ch  = ch;
      var_mob = victim;
      var_room = ch->in_room;
      execute( mprog );
      return;
      }

  process_tell( victim, ch, "I know nothing about that." );

  return;
}
