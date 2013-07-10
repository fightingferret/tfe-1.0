#include <sys/types.h>
#include <stdio.h>
#include "stdlib.h"
#include "define.h"
#include "struct.h"


const char* oprog_trigger[ MAX_OPROG_TRIGGER ] = { "put",
  "get", "timer", "hit", "none", "to_room", "entering", "wear", "consume",
  "sit", "random", "disarm", "use", "unlock", "lock" };


void display_oprog   ( char_data*, oprog_data* );


const char** oprog_msgs [ MAX_OPROG_TRIGGER ] =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  unlock_msg,
  lock_msg
};


/*
 *   DISPLAY ROUTINE
 */


void oprog_data :: display( char_data* ch )
{
  send( ch, "%10s ??    %s\n\r", "Oprog", "Null Object" );
}     


/*
 *   EDITING ROUTINES
 */


void do_opedit( char_data* ch, char *argument )
{
  char                 arg  [ MAX_STRING_LENGTH ];
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  wizard_data*      wizard;
  int                    i;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( "You aren't editing any object - use oedit <obj>.\n\r", ch );
    return;
    }

  if( *argument == '\0' ) {
    display_oprog( ch, obj_clss->oprog );
    return;
    }

  argument = one_argument( argument, arg );

  if( is_number( arg ) ) {
    i = atoi( arg );
    for( oprog = obj_clss->oprog; oprog != NULL && i != 1;
      oprog = oprog->next, i-- );
    if( oprog == NULL ) {
      send( ch, "No oprog by that number.\n\r" );
      return;
      }
    wizard->oprog_edit  = oprog;
    wizard->opdata_edit = NULL;
    send( ch, "You now edit that oprog.\n\r" );
    return;
    }

  if( !ch->can_edit( obj_clss ) )
    return;

  if( !strcasecmp( arg, "new" ) ) {
    oprog               = new oprog_data;
    wizard->oprog_edit  = oprog;
    wizard->opdata_edit = NULL;
    append( obj_clss->oprog, oprog );
    send( ch, "Oprog added.\n\r" );
    return;
    }

  if( !strcasecmp( arg, "delete" ) ) {
    if( ( oprog = wizard->oprog_edit ) == NULL ) {
      send( ch, "You aren't editing any oprog.\n\r" );
      return;
      }
    remove( obj_clss->oprog, oprog );
    extract( wizard, offset( &wizard->oprog_edit, wizard ), "oprog" );
    delete oprog;
    send( ch, "Oprog deleted.\n\r" );
    return;
    }

  send( ch, "Illegal syntax.\n" );
}


void display_oprog( char_data* ch, oprog_data *oprog )
{
  char tmp  [ MAX_INPUT_LENGTH ];
  int    i;

  if( oprog == NULL ) {
    send( ch, "This object has no programs.\n\r" );
    return;
    }

  for( i = 0; oprog != NULL; oprog = oprog->next ) {
    switch( oprog->trigger ) {
     case OPROG_TRIGGER_NONE :
      sprintf( tmp, "%20s %s", oprog->command, oprog->target );
      break;
     default :
      sprintf( tmp, "%20s", oprog_trigger[oprog->trigger] );
      break;
      }
    send( ch, "[%2d]  %s\n\r", ++i, tmp );
    }
}


void do_opcode( char_data* ch, char* argument )
{
  obj_clss_data*    obj_clss;
  oprog_data*          oprog;
  wizard_data*        wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\n\r" );
    return;
    }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\n\r" );
    return;
    }

  if( *argument != '\0' && !ch->can_edit( obj_clss ) )
    return;

  var_ch      = ch;
  oprog->code = edit_string( ch, argument, oprog->code, MEM_OPROG );

  compile( oprog );
}


void do_opdata( char_data* ch, char* argument )
{
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  wizard_data*      wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\n\r" );
    return;
    }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\n\r" );
    return;
    }

  if( *argument != '\0' && !ch->can_edit( obj_clss ) )
    return;
 
  if( wizard->opdata_edit != NULL ) {
    if( !strcasecmp( argument, "exit" ) ) {
      wizard->opdata_edit = NULL;
      send( ch, "Opdata now operates on the data list.\n\r" );
      return;
      }      
    wizard->opdata_edit->text = edit_string( ch, 
      argument, wizard->opdata_edit->text, MEM_EXTRA );
    return;
    }

  if( *argument == '\0' )
    show_defaults( ch, oprog->trigger, oprog_msgs );

  edit_extra( oprog->data, wizard, offset( &wizard->opdata_edit, wizard ),
    argument, "opdata" );

  var_ch = ch;
  compile( oprog );
}


void do_opstat( char_data* ch, char* )
{
  char              buf  [ 3*MAX_STRING_LENGTH ];
  oprog_data*     oprog;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;

  if( wizard->obj_edit == NULL ) {
    send( ch, "You aren't editing any object.\n\r" );
    return;
    }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\n\r" );
    return;
    }

  page( ch, "%10s : %s\n\r", "Trigger",
    oprog_trigger[ oprog->trigger ] );

  page( ch, "%10s : %s\n\r", "Obj_Act",
    oprog->obj_act == NULL ? "none" : oprog->obj_act->Name( ) );

  page( ch, "%10s : %s\n\r", "Command", oprog->command );
  page( ch, "%10s : %s\n\r", "Target", oprog->target );
  page( ch, "%10s : %d\n\r\n\r", "Rand_Value", oprog->value );

  sprintf( buf, "[Code]\n\r%s\n\r", oprog->code );
  page( ch, buf );

  show_extras( ch, oprog->data );
}


void do_opset( char_data* ch, char *argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  obj_data*         obj;
  oprog_data*     oprog;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;  

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    do_opstat( ch, "" );
    return;
    }

#define ot( i )  oprog_trigger[i]

  class type_field type_list[] = {
    { "trigger",   MAX_OPROG_TRIGGER,  &ot(0),  &ot(1),  &oprog->trigger },
    { "",          0,                  NULL,    NULL,    NULL            }
    };

#undef ot

  if( process( type_list, ch, "oprog", argument ) )
    return;

  if( matches( argument, "obj_act" ) ) {
    if( ( obj = one_object( ch, argument, 
      "oprog", &ch->contents ) ) != NULL )
      oprog->obj_act = obj->pIndexData;
    else
      oprog->obj_act = NULL;
    send( ch, "Obj_act set to %s.\n\r",
      oprog->obj_act == NULL ? "none" : oprog->obj_act->Name( ) );
    return;
    }

  if( matches( argument, "command" ) ) {
    free_string( oprog->command, MEM_OPROG );
    oprog->command = alloc_string( argument, MEM_OPROG );
    sprintf( buf, "Command set to %s.\n\r", argument );
    send( buf, ch );
    return;
    }

  if( matches( argument, "target" ) ) {
    free_string( oprog->target, MEM_OPROG );
    oprog->target = alloc_string( argument, MEM_OPROG );
    sprintf( buf, "Target set to %s.\n\r", argument );
    send( buf, ch );
    return;
    }

  send( ch, "Syntax: opset <field> <value>\n\r" );
}


void oset_obj( obj_data *obj, char *argument )
{
  char *word[] = { "value0", "value1", "value2", "value3" };
  int *pInt[] = { &obj->value[0], &obj->value[1],
    &obj->value[2], &obj->value[3] };

  char arg [ MAX_INPUT_LENGTH ];
  int i, j, length;
 
  argument = one_argument( argument, arg );
  length = strlen( arg );

  for( i = 0; i < 4; i++ )
    if( !strncasecmp( word[i], arg, length ) ) {
      j = atoi( argument );
      *pInt[i] = j;
      return;
      }

  bug( "Oset_obj: unknown field." ); 
}
