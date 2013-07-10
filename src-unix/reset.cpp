#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


/*
 *   CONSTANTS
 */


const char* rsflags_mob [] = { "Leader", "Follower", "Sentinel",
  "Night", "Day", "Aggressive", "" };
const char* rsflags_obj [] = { "Container", "Inside", "Unknown",
  "Unused1", "Unused2", "" };


const char* reset_pos_name [] = { "sleeping", "meditating", "resting",
  "standing" }; 
const char* reset_pos_abbrev [] = { "Slee", "Medi", "Rest", "Stan" };
const int rspos_index [] = { POS_SLEEPING, POS_MEDITATING, POS_RESTING,
  POS_STANDING };


/*
 *   LOCAL FUNCTIONS
 */


void    extract         ( reset_data* );

char*   set_rsflags     ( char_data*, reset_data*, char* );
void    display         ( char_data*, reset_data*, int ); 
void    modify_reset    ( char_data*, reset_data*, char* );
void    rowhere_key     ( char_data*, obj_clss_data* );

void    register_reset  ( char_data*, room_data* );

obj_data*  create       ( obj_clss_data*, reset_data*, mob_data* );
void       mob_setup    ( mob_data*, room_data* );
void       place        ( obj_data*, reset_data*, room_data*,
                             mob_data*, obj_data*& );

void    reset_mob       ( reset_data*, room_data*, mob_data*&, char_data*& );
void    reset_obj       ( reset_data*, room_data*, mob_data*, obj_data*& );
void    reset_table     ( reset_data*, int, room_data*, mob_data*,
                          obj_data*& );


int rtable_calls;


/*
 *   RESET_DATA CLASS
 */


Reset_Data :: Reset_Data( )
{
  record_new( sizeof( reset_data ), MEM_RESET );

  next     = NULL;
  chances  = ( 100 + ( 90 << 24 ) );
  vnum     = 0;
  flags    = ( 1 << RSFLAG_REROLL );
  count    = 0;
  liquid   = 0;

  return;
}


Reset_Data :: ~Reset_Data( )
{
  record_delete( sizeof( reset_data ), MEM_RESET );

  return;
}


/*
 *   SUPPORT ROUTINES
 */


void extract( reset_data* reset )
{
  for( int i = 0; i < mob_list; i++ )
    if( mob_list[i]->reset == reset )
      mob_list[i]->reset = NULL;

  delete reset;
}


char* name( reset_data* reset )
{
  if( reset == NULL )
    return "none";

  return "somewhere";
}


/*
 *   ONLINE EDITING COMMAND
 */


void do_reset( char_data* ch, char* argument )
{
  room_data*      room;
  wizard_data*  wizard;

  wizard = (wizard_data*) ch;

  if( !strcasecmp( argument, "room" ) ) {
    reset_room( ch->in_room );
    send( ch, "Room reset.\n\r" );
    return;
    }

  if( !strcasecmp( argument, "area" ) ) {
    for( room = ch->in_room->area->room_first; room != NULL;
      room = room->next )
      reset_room( room );
    send( ch, "Area reset.\n\r" );
    return;
    }

  edit_reset( ch, argument, ch->in_room->reset, RST_ROOM );
}


void do_mreset( char_data* ch, char* argument )
{
  species_data*  species;
  wizard_data*    wizard;

  wizard = (wizard_data*) ch;

  if( ( species = wizard->mob_edit ) == NULL ) {
    send( ch, "You aren't editing any mob.\n\r" );
    return;
    }

  edit_reset( ch, argument, species->reset, RST_MOB );
}


void edit_reset( char_data* ch, char* argument, reset_data*& list,
  int type, const char* name )
{
  reset_data*        reset;
  reset_data*         prev;
  int                 i, j;

  if( *argument == '\0' ) {
    if( name != empty_string ) {
      page_centered( ch, "-=- %s -=-", name );
      page( ch, "\n\r" );
      }
    display( ch, list, type );
    return;
    }

  if( type == RST_ROOM )
    ch->in_room->area->modified = TRUE;

  if( number_arg( argument, i ) ) {
    if( *argument == '\0' ) {
      send( ch, "What flag or option do you want to set?\n\r" );
      return;
      }
    if( ( reset = locate( list, i ) ) == NULL ) {
      send( ch, "No reset exists with that number.\n\r" );
      return;
      }
    if( isdigit( *argument ) && number_arg( argument, j ) ) {
      if( j == i ) {
        send( ch, "Moving a reset to where it already is does nothing\
 interesting.\n\r" ); 
        return;
        }
      if( j  == 1 ) {
        remove( list, reset );
        reset->next = list;
        list        = reset;
        }
      else {
        if( j < 1 || j > count( list ) ) {
          send( ch, "You can only move a reset to a sensible position.\n\r" );
          return;
	  }
        remove( list, reset );
        prev        = locate( list, j-1 );
        reset->next = prev->next;
        prev->next  = reset;
        }
      send( ch, "Reset %d moved to position %d.\n\r", i, j );
      return;
      }
    argument = set_rsflags( ch, reset, argument );
    modify_reset( ch, reset, argument );
    return;
    }

  if( matches( argument, "delete" ) ) {
    if( list == NULL ) {
      send( ch, "The list of resets is empty.\n\r" );
      return;
      }
    if( !strcasecmp( argument, "all" ) ) {
      for( reset = list; reset != NULL; reset = list ) {
        list = list->next;
        extract( reset );
        }
      send( ch, "All resets deleted.\n\r" );
      return;
      }
    if( ( reset = locate( list, atoi( argument ) ) ) == NULL ) {
      send( ch, "No reset with that number found to delete.\n\r" );
      return;
      }
    send( ch, "Reset deleted.\n\r" );
    remove( list, reset );
    extract( reset );
    return;
    }

  if( matches( argument, "table" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What rtable do you wish to reset?\n\r" );
      return;
      }
    if( number_arg( argument, i ) ) {
      if( --i < 0 || i >= num_rtable ) { 
        send( ch, "No rtable exists with that index.\n\r" );
        return;
        }
      }
    else {
      for( i = 0; !matches( argument, rtable_list[i]->name ); ) {
        if( ++i >= num_rtable ) {
          send( ch, "No such rtable exists.\n\r" );
          return;
	  } 
        }
      }
 
    send( ch, "Reset for rtable %d '%s' added.\n\r",
      i+1, rtable_list[i]->name );

    reset           = new reset_data;
    reset->vnum     = i;
    reset->flags    = 0;
    reset->value    = 0;

    append( list, reset );
    return;
    }

  thing_data*  thing;
  mob_data*      npc;
  obj_data*      obj;

  if( ( thing = one_thing( ch, argument, 
    "reset", ch->array, &ch->contents ) ) == NULL )
    return;

  if( ( npc = mob( thing ) ) != NULL ) {
    if( type != RST_ROOM ) {
      send( ch, "You can't reset a mob on a mob or in a table.\n\r" );
      return;
      }

    reset           = new reset_data;
    reset->vnum     = npc->species->vnum;
    reset->flags    = ( 1 << RSFLAG_MOB ) | ( 1 << RSFLAG_REROLL );
    reset->value    = RSPOS_STANDING;

    append( list, reset );
    send( ch, "Reset for %s added.\n\r", npc );

    return;
    }

  if( ( obj = object( thing ) ) != NULL ) {
    reset           = new reset_data;
    reset->vnum     = obj->pIndexData->vnum;
    reset->flags    = ( 1 << RSFLAG_OBJECT ) | ( 1 << RSFLAG_REROLL );
    reset->value    = -2;

    append( list, reset );
    send( ch, "Reset for %s added.\n\r", obj );
    return;
    }

  send( ch, "You can't reset %s.\n\r", thing );
}


void display( char_data* ch, reset_data* reset, int type ) 
{
  char                  buf  [ TWO_LINES ];
  char                flags  [ 32 ];
  char               liquid  [ 32 ];
  char                 rust  [ 5 ];
  int*              chances;
  int               i, j, k;
  obj_clss_data*   obj_clss;
  species_data*     species; 

  if( reset == NULL ) {
    send( ch, "No resets found.\n\r" );
    return;
    }  

  page_underlined( ch,
    "Nmbr Rrl  1_Ch 2_Ch 3_Ch  Rust %sLiq Flags Vnum Descr\n\r",
    type == RST_TABLE ? "" : "Posi " );

  for( i = 1; reset != NULL; i++, reset = reset->next ) {
    chances = unpack_int( reset->chances );

    if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
      obj_clss = get_obj_index( reset->vnum );
      for( k = 0, j = RSFLAG_LEADER; j < MAX_RSFLAG; j++ ) {
        if( is_set( &reset->flags, j ) )
          flags[k++] = rsflags_obj[ j-RSFLAG_LEADER ][0];
        }
      flags[k] = '\0';
      sprintf( liquid, ( obj_clss != NULL
        && ( obj_clss->item_type == ITEM_DRINK_CON
        || obj_clss->item_type == ITEM_FOUNTAIN ) ) 
        ? liquid_table[reset->liquid].name : "   " );
      liquid[3] = '\0';
      if( obj_clss->metal( ) 
        && !is_set( obj_clss->extra_flags, OFLAG_RUST_PROOF ) )
        sprintf( rust, "%3d%%", chances[3] );
      else
        sprintf( rust, "    " );
      if( type == RST_TABLE ) 
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%  %s %s %-5s %4d %s\n\r", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2], rust,
          liquid, flags, reset->vnum,
          truncate( name_brief( obj_clss ), 29 ) );
      else
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%  %s %s %s %-5s %4d %s\n\r", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2], rust,
          wear_abbrev[ reset->value+2 ], liquid, flags,
          reset->vnum, truncate( name_brief( obj_clss ), 29 ) );
      }
    else if( is_set( &reset->flags, RSFLAG_MOB ) ) {
      species = get_species( reset->vnum );
      for( k = 0, j = RSFLAG_LEADER; j < MAX_RSFLAG; j++ ) {
        if( is_set( &reset->flags, j ) )
          flags[k++] = rsflags_mob[ j-RSFLAG_LEADER ][0];
        }
      flags[k] = '\0';
      sprintf( buf,
        "[%2d] %c%c%c  %3d%% %3d%% %3d%%       %s     %-5s %4d %s\n\r", 
        i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
        is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
        is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
        chances[0], chances[1], chances[2],
        reset_pos_abbrev[ reset->value ], flags, reset->vnum,
        species == NULL ? "## Null Pointer?? ##" : species->Name( ) );
      }
    else {
      *flags = '\0';
      if( type == RST_TABLE ) 
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%           %-5s  TBL %s\n\r", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          flags, truncate( rtable_list[reset->vnum]->name, 29 ) );
      else
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%       %s     %-5s  TBL %s\n\r", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          wear_abbrev[reset->value+2], flags, 
          truncate( rtable_list[reset->vnum]->name, 29 ) );
      }
    page( ch, buf );
    }
}


char* set_rsflags( char_data* ch, reset_data* reset, char* argument )
{
  bool             set;
  int                i;
  const char**   flags;

  flags = ( is_set( &reset->flags, RSFLAG_MOB )
    ? rsflags_mob : rsflags_obj );

  for( ; ; ) {
    if( *argument != '+' && *argument != '-' )
      return argument;
 
    set = ( *argument++ == '+' );

    for( ; *argument != '\0' && *argument != ' '; argument++ ) {
      for( i = 0; flags[i][0] != '\0'; i++ ) 
        if( toupper( *argument ) == flags[i][0] ) {
          assign_bit( &reset->flags, RSFLAG_LEADER+i, set );
          send( ch, "%s flag set %s.\n\r", flags[i],
            true_false( &reset->flags, RSFLAG_LEADER+i ) );
          break;
          }
      if( *argument >= '1' && *argument <= '3' ) {
        i = *argument-'1';
        assign_bit( &reset->flags, RSFLAG_REROLL+i, set );
        send( ch, "Reroll bit %d set %s.\n\r", i+1,
          true_false( &reset->flags, RSFLAG_REROLL+i ) );
        }
      else if( flags[i][0] == '\0' ) { 
        send( ch, "Unknown flag -- %c\n\r", *argument );
        }
      }  

    for( ; *argument == ' '; argument++ );
    }
}


void modify_reset( char_data* ch, reset_data* reset, char* argument )
{
  int*   chances  = unpack_int( reset->chances ); 
  bool      flag;

  if( *argument == '\0' )
    return;

  argument = set_rsflags( ch, reset, argument );

  if( *argument == '\0' )
    return; 

#define rpn( i )   reset_pos_name[i]
#define rwn( i )   reset_wear_name[i]
#define ltn( i )   liquid_table[i].name

  if( is_set( &reset->flags, RSFLAG_MOB ) ) {
    class type_field type_list[] = {
      { "Position",  MAX_RESET_POS,  &rpn(0),  &rpn(1), &reset->value },
      { "",          0,              NULL,     NULL,    NULL          }
      };
    if( process( type_list, ch, "reset", argument ) )
      return;
    }
  else {
    reset->value += 2;
    class type_field type_list[] = {
      { "position",  MAX_ITEM_WEAR-1,   &rwn(0),  &rwn(1),  &reset->value   },
      { "liquid",    MAX_ENTRY_LIQUID,  &ltn(0),  &ltn(1),  &reset->liquid  },
      { "",          0,                 NULL,     NULL,     NULL            }
      };
    flag = process( type_list, ch, "reset", argument );
    reset->value -= 2;
    if( flag ) 
      return;
    }

#undef rpn
#undef rwn
#undef ltn

  class int_field int_list[] = {
    { "Vnum",              0, 9999,  &reset->vnum    },
    { "1_Chance",          0,  100,  &chances[0]     },
    { "2_Chance",          0,  100,  &chances[1]     },
    { "3_Chance",          0,  100,  &chances[2]     },
    { "Rust",              0,  100,  &chances[3]     },
    { "",                  0,    0,  NULL            }
    };

  if( process( int_list, ch, "reset", argument ) ) {
    reset->chances = pack_int( chances ); 
    return;
    }

  send( ch, "Unknown Field - See help reset.\n\r" );
 
  return;
}


/*
 *   MAIN ROUTINE TO RESET AN AREA
 */


bool passes( reset_data* reset, int* roll )
{
  int*  chances;

  chances = unpack_int( reset->chances );

  for( int i = 0; i < 3; i++ )
    if( is_set( &reset->flags, RSFLAG_REROLL+i ) ) 
      roll[i] = -102;

  for( int i = 0; i < 3; i++ ) {
    if( chances[i] > 0 ) {
      if( roll[i] == -102 )
        roll[i] = number_range( 0, 99 );
      else if( roll[i] < 0 ) {
        roll[i] = -101;
        return FALSE;
        }
      if( ( roll[i] -= chances[i] ) >= 0 )
        return FALSE;
      }
    else { 
      if( roll[i] == -101 || roll[i] >= 0 )
        return FALSE;
      }
    }

  return TRUE;
}


void reset_room( room_data* room )
{
  char_data*       leader  = NULL;
  mob_data*           mob  = NULL;
  int                roll  [ 3 ];
  int               count  = -1; 
  exit_data*         exit;
  exit_data*         back;
  obj_data*     container  = NULL;
  obj_data*           obj;
  reset_data*       reset;
  int*                 w1;
  int*                 w2; 

  if( is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) )
    return;

  for( int i = room->contents-1; i >= 0; i-- ) 
    if( ( obj = object( room->contents[i] ) ) != NULL 
      && obj->pIndexData->vnum != OBJ_CORPSE_PC )
      obj->Extract( );

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = room->reset; reset != NULL; reset = reset->next ) {
    if( count < 0 || ( is_set( &reset->flags, RSFLAG_REROLL )
      && ( is_set( &reset->flags, RSFLAG_MOB ) || 
      reset->value == -2 ) ) )
      count = reset->count;
    if( count <= 0 && passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_MOB ) ) {
        reset_mob( reset, room, mob, leader );
        }
      else if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        reset_obj( reset, room, mob, container );
        }
      else {
        rtable_calls = 0;
        reset_table( reset, reset->vnum, room, mob, container );
        }
      }
    }

  /* OPEN, CLOSE DOORS */

  int reset_flags [] = { EX_RESET_CLOSED, EX_RESET_LOCKED, EX_RESET_OPEN };
  int status_flags [] = { EX_CLOSED, EX_LOCKED, EX_CLOSED };
  int value [] = { 1,1,0 }; 

  for( int i = 0; i < room->exits; i++ ) {
    exit = room->exits[i];
    if( is_set( &exit->exit_info, EX_ISDOOR ) ) {
      w1 = &exit->exit_info;
      if( ( back = reverse( exit ) ) != NULL ) {
        if( player_in_room( exit->to_room ) )
          continue;
        w2 = &back->exit_info;
        }
      else {
        w2 = w1;
        } 
      for( int j = 0; j < 3; j++ )
        if( is_set( w1, reset_flags[j] ) ) {
          assign_bit( w1, status_flags[j], value[j] );
          assign_bit( w2, status_flags[j], value[j] );
	  }
      }
    }
  
  set_bit( &room->room_flags, RFLAG_RESET0 );
  set_bit( &room->room_flags, RFLAG_RESET1 );
  set_bit( &room->room_flags, RFLAG_RESET2 );

  return;
}    


/*
 *   OBJECT RESET FUNCTION
 */


obj_data* create( obj_clss_data* obj_clss, reset_data* reset, mob_data* mob )
{
  obj_data*    obj;
  int*     chances;

  obj     = create( obj_clss );
  chances = unpack_int( reset->chances );

  enchant_object( obj ); 
  set_alloy( obj, 10 );
  rust_object( obj, chances[3] );

  if( is_set( &obj_clss->size_flags, SFLAG_RANDOM ) ) 
    set_bit( &obj->size_flags, mob == NULL
      ? number_range( SFLAG_TINY, SFLAG_GIANT )
      : wear_size( mob ) ); 

  if( obj_clss->item_type == ITEM_DRINK_CON
    || obj_clss->item_type == ITEM_FOUNTAIN )
    obj->value[2] = reset->liquid;

  return obj;
}    


void place( obj_data* obj, reset_data* reset, room_data* room,
  mob_data* mob, obj_data*& container )
{
  if( is_set( &reset->flags, RSFLAG_INSIDE ) ) {
    if( container == NULL ) {
      bug( "Reset_Obj: Inside flag with no container - %s %d",
        room != NULL ? "Room" : "Species",
        room != NULL ? room->vnum : mob->species->vnum );
      obj->Extract( );
      return;
      }
    obj->To( container );
    }

  else if( reset->value == -2 ) {
    if( room == NULL ) {
      bug( "Reset_Obj: Ground Object with NULL room - Species %d",
        mob->species->vnum );
      obj->Extract( );
      return;
      }
    obj->To( room );
    stop_events( obj, execute_decay );
    }

  else {
    if( mob == NULL ) {
      bug( "Reset_Obj: Wear loc with null mob - Room %d", room->vnum );
      obj->Extract( );
      return;
      }
    if( reset->value >= 0 ) {
      obj->position = reset->value; 
      obj->To( &mob->wearing );
      }
    else 
      obj->To( mob );
    }

  if( is_set( &reset->flags, RSFLAG_CONTAINER ) )
    container = obj;
}


void reset_obj( reset_data* reset, room_data* room, mob_data* mob,
  obj_data*& container )
{
  obj_clss_data*  obj_clss;

  if( ( obj_clss = get_obj_index( reset->vnum ) ) == NULL 
    || ( room == NULL && is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) ) )
    return;

  if( mob != NULL && mob->pShop != NULL && reset->value == -1 )
    return;

  place( create( obj_clss, reset, mob ), reset, room, mob, container );

  return;
}


/*
 *   RESET TABLE FUNCTION
 */


void reset_table( reset_data* base_reset, int n, room_data* room,
  mob_data* mob, obj_data*& container )
{
  obj_clss_data*   obj_clss;
  obj_data*             obj;
  reset_data*         reset;
  int                  roll  [ 3 ];
  int                     i;

  if( n < 0 || n >= num_rtable ) {
    bug( "Reset_Table: Invalid table." );
    bug( "-- Index = %d", n );
    return;
    }

  rtable_calls++;

  if( rtable_calls >= 100 ) {
    if( rtable_calls == 100 )
      bug( "Reset_Table: Infinite lookup chain." );
    return;
    }

  for( i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = rtable_list[n]->reset; reset != NULL; reset = reset->next ) 
    if( passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        if( ( obj_clss = get_obj_index( reset->vnum ) ) == NULL ) 
          return;
        obj = create( obj_clss, reset, mob );
        place( obj, base_reset, room, mob, container );
        }
      else {
        reset_table( base_reset, reset->vnum, room, mob, container );
        }
      }

  return;
}


/*
 *   MOB RESET FUNCTIONS
 */


void reset_mob( reset_data* reset, room_data* room,
   mob_data*& mob, char_data*& leader )
{
  species_data* species;

  if( ( is_set( &reset->flags, RSFLAG_NIGHT ) && is_day( ) )
    || ( is_set( &reset->flags, RSFLAG_DAY ) && !is_day( ) ) )
    return;

  if( ( species = get_species( reset->vnum ) ) == NULL )
    return;

  mob = create_mobile( species );

  if(  is_set( &reset->flags, RSFLAG_SENTINEL ) 
    || is_set( &mob->species->act_flags, ACT_SENTINEL ) ) { 
    set_bit( &mob->status, STAT_SENTINEL );    
    }
  else {
    delay_wander( new event_data( execute_wander, mob ) );
    }

  if( is_set( &reset->flags, RSFLAG_AGGRESSIVE ) )
    set_bit( &mob->status, STAT_AGGR_ALL );    

  mob->To( room );
  
  mob->position = rspos_index[ reset->value ];
  mob->reset    = reset;

  mob_setup( mob, room );
  register_reset( mob, room );
  mreset_mob( mob );

  if( leader != NULL && is_set( &reset->flags, RSFLAG_FOLLOWER ) )
    add_follower( mob, leader );

  if( is_set( &reset->flags, RSFLAG_LEADER ) )
    leader = mob;
}


void register_reset( char_data* mob, room_data* room )
{
  reset_data*   mark = room->reset;
  reset_data*  reset;

  for( reset = room->reset; ; reset = reset->next ) {
    if( is_set( &reset->flags, RSFLAG_REROLL ) )  
      mark = reset;
    if( reset == mob->reset )
      break;
    }

  mark->count++;
  mob->reset = mark;
}


void unregister_reset( char_data* mob )
{
  if( mob->reset != NULL ) 
    mob->reset->count--;
} 


void mob_setup( mob_data* mob, room_data* room )
{
  shop_data*        shop;

  if( room->is_dark( ) )
    set_bit( mob->affected_by, AFF_INFRARED );

  for( shop = shop_list; shop != NULL; shop = shop->next ) 
    if( shop->room == room && shop->keeper == mob->species->vnum )
      mob->pShop = shop;

  set_trainer( mob, room );
}


/*
 *   MRESET FUNCTIONS
 */


void mreset_mob( mob_data* mob )
{
  reset_data*      reset;
  obj_data*    container  = NULL;
  int               roll  [ 3 ];
  int                  i;

  for( i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = mob->species->reset; reset != NULL; reset = reset->next ) 
    if( passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        reset_obj( reset, NULL, mob, container );
        }
      else {
        rtable_calls = 0;
        reset_table( reset, reset->vnum, NULL, mob, container );
        }
      }

  return;
}


thing_array* get_skin_list( species_data* species )
{
  obj_data*            obj;
  obj_clss_data*  obj_clss  = NULL;
  thing_array*        list  = NULL;
  reset_data*        reset;
  int                 roll  [ 3 ];

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = species->reset; reset != NULL; reset = reset->next )
    if( ( obj_clss = get_obj_index( reset->vnum ) ) != NULL 
      && is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) )
      break;

  if( reset == NULL )
    return (thing_array*) -1; 

  for( reset = species->reset; reset != NULL; reset = reset->next ) {
    if( !passes( reset, roll )
      || ( obj_clss = get_obj_index( reset->vnum ) ) == NULL 
      || !is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) )
      continue;
    if( list == NULL )
      list = new thing_array;
    obj = create( obj_clss );
    *list += obj;
    }

  return list;
}


/*
 *   SHOP RESET FUNCTION
 */


void reset_shop( mob_data* ch )
{
  reset_data*  reset;
  obj_data*      obj;
  int           roll  [ 3 ];

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = ch->reset->next; reset != NULL; reset = reset->next ) {
    if( !is_set( &reset->flags, RSFLAG_OBJECT )
      || reset->value == -2 )
      break;
  
    if( passes( reset, roll ) && reset->value == -1 ) {
      obj = create( get_obj_index( reset->vnum ) );
      set_bit( obj->extra_flags, OFLAG_IDENTIFIED );    
      set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
      if( obj->pIndexData->item_type == ITEM_DRINK_CON )
        obj->value[2] = reset->liquid; 
      obj->To( ch );
      consolidate( obj );
      }
    }
}


/*
 *   DISK ROUTINES
 */


void load( FILE* fp, reset_data*& list )
{
  reset_data*   reset;
  int               i;

  for( ; ; ) {
    if( ( i = fread_number( fp ) ) == -1 )
      break;

    reset = new reset_data;
    
    reset->vnum     = i;
    reset->flags    = fread_number( fp );
    reset->chances  = fread_number( fp );
    reset->value    = fread_number( fp );
    reset->liquid   = fread_number( fp );

    append( list, reset );
    }

  return;
}


void write( FILE* fp, reset_data* reset )
{
  for( ; reset != NULL; reset = reset->next ) 
    fprintf( fp, "%d %d %d %d %d\n", 
      reset->vnum, reset->flags, reset->chances,
      reset->value, reset->liquid );

  fprintf( fp, "-1\n" );
}


/*
 *   COMMANDS TO LOCATE RESETS 
 */


bool has_reset( obj_clss_data* obj_clss )
{
  area_data*        area;
  room_data*        room;
  reset_data*      reset;
  species_data*  species;
  int                  i;

  for( area = area_list; area != NULL; area = area->next )
    for( room = area->room_first; room != NULL; room = room->next )
      for( reset = room->reset; reset != NULL; reset = reset->next )
        if( is_set( &reset->flags, RSFLAG_OBJECT )
          && reset->vnum == obj_clss->vnum )
          return TRUE;

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;
    for( reset = species->reset; reset != NULL; reset = reset->next )
      if( is_set( &reset->flags, RSFLAG_OBJECT )
        && reset->vnum == obj_clss->vnum )
        return TRUE;
    }

  return FALSE;
}


/* 
 *   ROWHERE ROUTINES
 */


void do_rowhere( char_data* ch, char* argument )
{
  char                  tmp  [ TWO_LINES ];
  action_data*       action;
  area_data*           area;
  custom_data*       custom;
  mprog_data*         mprog;
  obj_clss_data*   obj_clss;
  reset_data*         reset;
  room_data*           room;
  shop_data*           shop;
  species_data*     species;
  const char*          name;
  bool                first  = TRUE;
  bool                found;
  int           vnum, index;
  int                  i, j;
  int                 flags;

  if( !get_flags( ch, argument, &flags, "p", "Rowhere" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rowhere <object>\n\r" );
    return;
    }

  vnum = atoi( argument );

  for( index = 0; index < MAX_OBJ_INDEX; index++ ) {
    if( ( obj_clss = obj_index_list[index] ) == NULL )
      continue; 
   
    name = obj_clss->Name( );

    if( !is_name( argument, name )
      && obj_clss->vnum != vnum )
      continue;

    page( ch, "\n\r" );
    sprintf( tmp, "--- %s (%d) ---", name, index );
    tmp[4] = toupper( tmp[4] );
    page_centered( ch, tmp ); 
    page( ch, "\n\r" );

    first = FALSE;
    found = FALSE;

  /* SEARCH MRESETS */

    for( i = 0; i < MAX_SPECIES; i++ ) {
      if( ( species = species_list[i] ) == NULL ) 
        continue;
      for( reset = species->reset; reset != NULL; reset = reset->next ) {
        if( obj_clss->vnum == reset->vnum 
	  && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          page( ch, "  On %s\n\r", species->Name( ) );
          found = TRUE;
	  }
        }

      if( is_set( &flags, 0 ) ) {
        if( search_oload( species->attack->binary, index ) ) {
          page( ch, "  Loads in attack prog on mob #%d.\n\r", i );
          found = TRUE;
          }
        for( j = 1, mprog = species->mprog; mprog != NULL; 
          j++, mprog = mprog->next )
          if( search_oload( mprog->binary, index ) ) {
            page( ch, "  Loads in mprog #%d on mob #%d.\n\r", j, i );
            found = TRUE;
            }
        }
      }

  /* SEARCH ROOM RESETS */

    for( area = area_list; area != NULL; area = area->next ) {
      for( room = area->room_first; room != NULL; room = room->next ) {
        species =  NULL;
        for( reset = room->reset; reset != NULL; reset = reset->next ) {
          if( is_set( &reset->flags, RSFLAG_MOB ) ) {
            species = get_species( reset->vnum );
            continue;
	    }
          if( obj_clss->vnum != reset->vnum 
            || !is_set( &reset->flags, RSFLAG_OBJECT ) )
            continue;
          found = TRUE;
          if( reset->value == -2 ) {
            page( ch, "  At %s [%d]\n\r", room->name, room->vnum );
	    } 
          else if( species == NULL ) {
            page( ch, "  [BUG] Illegal reset structure [%d]\n\r",
              room->vnum );
            }
          else {
            page( ch, "  On %s at %s [%d]\n\r",
              species->Name( ), room->name, room->vnum );
	    }
          }
        }
      }

    if( is_set( &flags, 0 ) ) {
      for( area = area_list; area != NULL; area = area->next ) 
        for( room = area->room_first; room != NULL; room = room->next )
          for( j = 1, action = room->action; action != NULL;
            j++, action = action->next )
            if( action->binary != NULL
              && search_oload( action->binary, index ) ) {
              page( ch, "  Loads in action #%d in room #%d.\n\r",
                j, room->vnum );
              found = TRUE;
	      }
      }

  /* SEARCH TABLES */

    for( i = 0; i < num_rtable; i++ ) 
      for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next )
        if( reset->vnum == index 
          && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          page( ch, "  In rtable %d, %s.\n\r", 
            i+1, rtable_list[i]->name );
          found = TRUE;
	  }

  /* SEARCH SHOPS */

    for( shop = shop_list; shop != NULL; shop = shop->next ) 
      for( custom = shop->custom; custom != NULL; custom = custom->next ) {
        if( custom->item == obj_clss ) {
          page( ch, "  Custom in room %d\n\r", 
            shop->room->vnum );
          }  
        for( i = 0; i < MAX_INGRED; i++ )
          if( custom->ingred[i] == obj_clss ) {
            page( ch, 
              "  Ingredient for making %s in room %d\n\r",  
              custom->item->Name( ), shop->room->vnum );
            found = TRUE;
            }
        }

  /* SEARCH SPELLS */

    for( i = 0; i < MAX_SPELL; i++ )
      for( j = 0; j < MAX_SPELL_WAIT; j++ )
        if( abs( spell_table[i].reagent[j] ) == index ) {
          page( ch, "  Reagent for spell %s\n\r",
            spell_table[i].name );
          found = TRUE;
          break;
          }

   /* CORPSE? */

    for( i = 0; i < MAX_SPECIES; i++ ) {
      if( ( species = species_list[i] ) == NULL ) 
        continue;
      if( species->corpse == index ) {
        page( ch, "  Corpse of %s\n\r", species );
        found = TRUE;
        }
      }

    rowhere_key( ch, obj_clss );

    if( !found )
      page_centered( ch, "  None found" ); 
    }
  
  if( first )
    send( ch, "Nothing like that in hell, earth, or heaven.\n\r" );

  return;
}


void rowhere_key( char_data* ch, obj_clss_data* key )
{
  area_data*            area;
  obj_clss_data*   container;
  room_data*            room;
  int                  index;

  if( key->item_type != ITEM_KEY )
    return;
 
  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next )
      for( int i = 0; i < room->exits; i++ )
        if( room->exits[i]->key == key->vnum ) 
          page( ch, "  -- Unlocks %s door of room %d --\n\r",
            dir_table[ room->exits[i]->direction ].name,
            room->vnum );

  for( index = 0; index < MAX_OBJ_INDEX; index++ )
    if( ( container = obj_index_list[index] ) != NULL 
      && container->item_type == ITEM_CONTAINER 
      && container->value[2] == key->vnum ) 
      page( ch, "  -- Unlocks object %d, %s --\n\r",
        index, container->Name( ) );

  return;
}


void do_rmwhere( char_data* ch, char* argument )
{
  action_data*    action;
  area_data*        area;
  reset_data*      reset;
  room_data*        room;
  species_data*  species;
  species_data*   undead;
  bool             first  = TRUE;
  bool             found  = TRUE;
  int               vnum;
  int              flags;

  if( !get_flags( ch, argument, &flags, "p", "Rowhere" ) )
    return;;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rmwhere <mob>\n\r" );
    return;
    }

  vnum = atoi( argument );

  for( int i = 1; i < MAX_SPECIES; i++ ) { 
    if( ( species = species_list[i] ) == NULL || ( vnum != i 
      && !is_name( argument, species->Name( ) ) ) )
      continue;

    page( ch, "%s%s:\n\r", ( first ? "" : "\n\r" ),
      species->Name( ) );

    first = FALSE;
    found = FALSE;

    for( area = area_list; area != NULL; area = area->next ) 
      for( room = area->room_first; room != NULL; room = room->next ) 
        for( reset = room->reset; reset != NULL; reset = reset->next ) 
          if( reset->vnum == i && is_set( &reset->flags, RSFLAG_MOB ) ) {
            page( ch, "  at %s [%5d]\n\r", room->name, room->vnum );
            found = TRUE;
	    }

    for( int j = 1; j < MAX_SPECIES; j++ ) { 
      if( ( undead = species_list[j] ) != NULL ) {
        if( undead->zombie == i ) {
          page( ch, "  -- Zombie form of %s, mob #%d --\n\r",
            undead->Name( ), j );
          found = TRUE;
	  }
        if( undead->skeleton == i ) {
          page( ch, "  -- Skeletal form of %s, mob #%d --\n\r",
            undead->Name( ), j );
          found = TRUE;
	  }
        }
      }

    if( is_set( &flags, 0 ) ) {
      for( area = area_list; area != NULL; area = area->next ) {
        for( room = area->room_first; room != NULL; room = room->next ) {
          action = room->action;
          for( int j = 1; action != NULL; j++, action = action->next ) 
            if( search_mload( action->binary, i ) ) {
              page( ch, "  --> Loads in action #%d in room #%d <--\n\r",
                j, room->vnum );
              found = TRUE;
              }
	  }
        }
      }

    if( !found )
      page( ch, "  no resets found\n\r" );
    }

  if( first )
    send( ch, "Nothing like that in hell, earth, or heaven.\n\r" );

  return;
}



