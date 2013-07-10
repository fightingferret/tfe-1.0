#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


/*
 *   CONST TABLES
 */


const char* affect_location[] = { "None", "Strength", "Dexterity",
  "Intelligence", "Wisdom", "Constitution", "Magic", "Fire", "Cold",
  "Electricity", "Mind", "Age", "Mana", "Hit Points", "Move", "Unused",
  "Unused",  "Armor", "Hitroll", "Damroll", "Mana_regen", "Hit_regen",
  "Move_regen", "Acid", "Poison" };


/*
 *   AFFECT CLASS
 */


Affect_Data :: Affect_Data( )
{
  record_new( sizeof( affect_data ), MEM_AFFECT );

  leech  = NULL;
};


Affect_Data :: ~Affect_Data( )
{
  record_delete( sizeof( affect_data ), MEM_AFFECT );
};


/*
 *   DISK ROUTINES
 */


void read_affects( FILE* fp, char_data* ch )
{
  affect_data*  affect;
  int           i, j;

  fread( &i, sizeof( int ), 1, fp );

  for( ; i > 0; i-- ) {
    affect = new affect_data;
    fread( &affect->type,     sizeof( int ), 1, fp );
    fread( &affect->duration, sizeof( int ), 1, fp );
    fread( &affect->level,    sizeof( int ), 1, fp );
    if( affect->type == AFF_NONE ) {  
      fread( &affect->modifier, sizeof( int ), 1, fp );
      fread( &affect->location, sizeof( int ), 1, fp ); 
      } 
    fread( &j, sizeof( int ), 1, fp );
    if( j != -1 ) {
      affect->leech_regen = j;
      affect->leech       = ch;
      ch->leech_list     += affect;
      fread( &affect->leech_max, sizeof( int ), 1, fp );
      }
    affect->victim = ch;
    modify_affect( ch, affect, TRUE );
    ch->affected += affect;
    }
}


void write_affects( FILE* fp, char_data* ch )
{
  affect_data*  affect;
  int             i, j;

  if( is_empty( ch->affected ) )
    return;

  fwrite( "Afft", 4, 1, fp );
  fwrite( &ch->affected.size, sizeof( int ), 1, fp );

  for( i = 0; i < ch->affected; i++ ) {
    affect = ch->affected[i];
    fwrite( &affect->type,     sizeof( int ), 1, fp );
    fwrite( &affect->duration, sizeof( int ), 1, fp );
    fwrite( &affect->level,    sizeof( int ), 1, fp );
    if( affect->type == AFF_NONE ) {  
      fwrite( &affect->modifier, sizeof( int ), 1, fp );
      fwrite( &affect->location, sizeof( int ), 1, fp );
      }
    if( affect->leech == ch ) {
      fwrite( &affect->leech_regen, sizeof( int ), 1, fp );
      fwrite( &affect->leech_max,   sizeof( int ), 1, fp );        
      }
    else {
      j = -1;
      fwrite( &j, sizeof( int ), 1, fp );
      }
    } 
}


void read_affects( FILE* fp, obj_clss_data* obj_clss )
{
  affect_data*  affect;
  char          letter;

  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != 'A' )
      break;

    affect            = new affect_data;    
    affect->type      = AFF_NONE;
    affect->duration  = -1;
    affect->location  = fread_number( fp );
    affect->modifier  = fread_number( fp );

    obj_clss->affected += affect;
    }

  ungetc( letter, fp );
};


void write_affects( FILE* fp, obj_clss_data* obj_clss )
{
  affect_data* affect;

  for( int i = 0; i < obj_clss->affected; i++ ) {
    affect = obj_clss->affected[i]; 
    fprintf( fp, "A %d %d\n", affect->location, affect->modifier );
    }

  return;
};


/*
 *   CHARACTER SUBROUTINES
 */


int prep_max( char_data* ch )
{
  cast_data* cast;
  int           i;

  for( i = 0, cast = ch->prepare; cast != NULL; cast = cast->next )
    i += cast->mana*cast->times;
  
  return i;
}


int leech_max( char_data* ch )
{
  int  i, j;

  for( i = j = 0; i < ch->leech_list; i++ )
    j += ch->leech_list[i]->leech_max;

  return j;
}


int leech_regen( char_data* ch )
{
  int  i, j;

  for( i = j = 0; i < ch->leech_list; i++ )
    j += ch->leech_list[i]->leech_regen;

  return j;
}


/*
 *   DO_AFFECT ROUTINE
 */


const char* source( char_data* ch, int sn )
{
  obj_data*        obj;
  affect_data*  affect;

  if( ch->shdata->race < MAX_PLYR_RACE 
    && is_set( plyr_race_table[ch->shdata->race].affect, sn ) )
    return "innate";  

  for( int i = 0; i < ch->wearing; i++ )
    if( ( obj = object( ch->wearing[i] ) ) != NULL 
      && is_set( obj->pIndexData->affect_flags, sn ) )
      return obj->Seen_Name( ch, 1, TRUE );

  for( int i = 0; i < ch->affected; i++ ) {
    affect = ch->affected[i];
    if( sn == affect->type ) {
      if( affect->leech == NULL ) 
        return "temporary";
      char* tmp = static_string( );
      sprintf( tmp, "leech: %s",
        affect->leech == ch ? "self"
        : affect->leech->Seen_Name( ch ) );
      return tmp;
      }
    }

  return "unknown";
}


void do_affects( char_data* ch, char* argument )
{
  affect_data*   affect;
  bool            found  = FALSE;
  int             flags;

  if( is_confused_pet( ch ) )
    return;

  if( get_trust( ch ) >= LEVEL_IMMORTAL ) {
    if( !get_flags( ch, argument, &flags, "l", "Affect" ) )
      return;
    if( is_set( &flags, 0 ) ) {
      if( ch->affected == NULL ) {
        send( ch, "You have no non-default affects.\n\r" );
        return;
        }
      page_underlined( ch, "Dur.   Level   Affect\n\r" );
      for( int i = 0; i < ch->affected; i++ ) {
        affect = ch->affected[i];
        page( ch, "%3d%8d    %s\n\r", affect->duration, affect->level,
          affect->type == AFF_NONE ? "Unknown?"
          : aff_char_table[ affect->type ].name );
        }
      return;
      }
    }   

  for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) 
    if( is_set( ch->affected_by, i )
      && *aff_char_table[i].score_name != '\0' ) {
      if( !found ) {
        found = TRUE;
        page_underlined( ch, "%-50s%s\n\r", "Affects", "Source" );
        }
      page( ch, "%-50s%s\n\r",
        aff_char_table[i].score_name,
        truncate( source( ch, i ), 30 ) );
      }

  if( !found ) 
    send( ch, "You have not noticed any affects to yourself.\n\r" ); 
}


/*
 *   AFFECT ROUTINES
 */


void add_affect( char_data* ch, affect_data* paf )
{
  affect_data* paf_new  = NULL;

  if( paf->type != AFF_NONE ) 
    for( int i = 0; i < ch->affected; i++ ) 
      if( paf->type == ch->affected[i]->type ) {
        paf_new = ch->affected[i];
        paf_new->duration = max( paf_new->duration, paf->duration );
        break;
        }
 
  if( paf_new == NULL ) {
    paf_new = new affect_data;

    memcpy( paf_new, paf, sizeof( *paf_new ) );
    modify_affect( ch, paf_new, TRUE );

    ch->affected   += paf_new;
    paf_new->leech  = NULL;
    paf_new->victim = ch;
    }

  if( paf_new->leech == NULL && paf->leech != NULL ) {
    paf->leech->leech_list   += paf_new;
    paf->leech->max_mana     -= paf->leech_max;
    paf_new->leech            = paf->leech;
    paf_new->leech_regen      = paf->leech_regen;
    paf_new->leech_max        = paf->leech_max;
    }  
}


void remove_affect( char_data* ch )
{
  for( int i = ch->affected.size-1; i >= 0; i-- )
    remove_affect( ch, ch->affected[i] );
}


void remove_affect( char_data* ch, affect_data* affect )
{
  ch->affected -= affect;

  modify_affect( ch, affect, FALSE );
  remove_leech( affect );

  if( affect->type == AFF_SLEEP && ch->pcdata == NULL
    && !is_set( &ch->status, STAT_PET ) && ch->reset != NULL
    && ch->reset->value != RSPOS_SLEEPING )
    do_stand( ch, "" );

  delete affect;
}


void strip_affect( char_data* ch, int sn )
{
  for( int i = ch->affected.size-1; i >= 0; i-- ) 
    if( ch->affected[i]->type == sn )
      remove_affect( ch, ch->affected[i] );
}


bool has_affect( char_data* ch, int sn )
{
  obj_data* obj;

  if( sn == AFF_NONE ) 
    return FALSE;

  if( ch->shdata->race < MAX_PLYR_RACE 
    && is_set( plyr_race_table[ch->shdata->race].affect, sn ) )
    return TRUE;  

  for( int i = 0; i < ch->affected; i++ )
    if( ch->affected[i]->type == sn )
      return TRUE;

  for( int i = 0; i < ch->wearing; i++ )
    if( ( obj = object( ch->wearing[i] ) ) != NULL 
      && is_set( obj->pIndexData->affect_flags, sn ) )
      return TRUE;

  return FALSE;
}


void modify_affect( char_data* ch, affect_data* paf, bool fAdd )
{
  char *modify_amount[] = { "slightly", "somewhat", "quite a bit",
    "much", "**MUCH**" };

  char* affect_name [ 2*MAX_AFF_LOCATION ] = {
    "stronger", "weaker",
    "more agile", "less agile",
    "more intelligent", "less intelligent",
    "wiser", "less wise",
    "tougher", "less tough",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "older", "younger",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "",
    "", "" };

  int      mod;
  int        i;
  bool   error;

  if( paf->type != AFF_NONE ) {
    if( fAdd ? is_set( ch->affected_by, paf->type )
      : has_affect( ch, paf->type ) )
      return;

    paf->location = aff_char_table[ paf->type ].location;
    paf->modifier = evaluate(
      aff_char_table[ paf->type ].modifier,
      error, 0, 0 );

    if( error ) {
      bug( "Modify_Affect: Failed evaluate." );
      bug( "-- Aff = %s", aff_char_table[ paf->type ].name ); 
      }

    if( fAdd ) {
      if( ch->array != NULL ) {
        send( ch, "%s\n\r", aff_char_table[ paf->type ].msg_on );
        act_notchar( aff_char_table[ paf->type ].msg_on_room, ch );
        }
      set_bit( ch->affected_by, paf->type );
      }
    else {
      remove_bit( ch->affected_by, paf->type );
      if( ch->array != NULL ) {
        send( ch, "%s\n\r", aff_char_table[ paf->type ].msg_off );
        act_notchar( aff_char_table[ paf->type ].msg_off_room, ch );
        }
      }
    }

  mod = paf->modifier*( fAdd ? 1 : -1 );

  if( mod == 0 )
    return;

  switch( paf->location ) {
    default:
      bug( "Modify_affect: unknown location %d.", paf->location );
      return;
 
    case APPLY_AGE:
      if( ch->species == NULL && ch->pcdata != NULL )
        ch->pcdata->mod_age += mod;
      break;

    case APPLY_NONE:                                                break;
    case APPLY_INT:         ch->mod_int                   += mod;   break;
    case APPLY_STR:         ch->mod_str                   += mod;   break;
    case APPLY_DEX:         ch->mod_dex                   += mod;   break;
    case APPLY_WIS:         ch->mod_wis                   += mod;   break;
    case APPLY_CON:         ch->mod_con                   += mod;   break;
    case APPLY_MOVE:        ch->mod_move                  += mod;   break;
    case APPLY_MAGIC:       ch->mod_resist[RES_MAGIC]     += mod;   break;
    case APPLY_FIRE:        ch->mod_resist[RES_FIRE]      += mod;   break;
    case APPLY_COLD:        ch->mod_resist[RES_COLD]      += mod;   break;
    case APPLY_ELECTRIC:    ch->mod_resist[RES_SHOCK]     += mod;   break;
    case APPLY_MIND:        ch->mod_resist[RES_MIND]      += mod;   break;
    case APPLY_ACID:        ch->mod_resist[RES_ACID]      += mod;   break;
    case APPLY_POISON:      ch->mod_resist[RES_POISON]    += mod;   break;
    case APPLY_MANA:        ch->mod_mana                  += mod;   break;
    case APPLY_HIT:         ch->mod_hit                   += mod;   break;
    case APPLY_AC:          ch->mod_armor                 += mod;   break;
    case APPLY_HITROLL:     ch->hitroll                   += mod;   break;
    case APPLY_DAMROLL:     ch->damroll                   += mod;   break;
    case APPLY_MANA_REGEN:  ch->mana_regen                += mod;   break;
    case APPLY_HIT_REGEN:   ch->hit_regen                 += mod;   break;
    case APPLY_MOVE_REGEN:  ch->move_regen                += mod;   break;
    }
 
  i = 2*( paf->location-1 )+( mod < 0 );

  if( ch->link != NULL && ch->link->connected == CON_PLAYING
    && affect_name[i][0] != '\0' ) 
    send( ch, "[You feel %s %s.]\n\r", modify_amount[
      min( max( mod, -mod ), 5 )-1], affect_name[i] );  

  if( ch->array != NULL )
    update_maxes( ch );
}


/*
 *   OBJECT AFFECT ROUTINES
 */


void update_affect( event_data* event )
{
  obj_data*       obj  = (obj_data*)    event->owner;
  affect_data* affect  = (affect_data*) event->pointer;

  if( affect->type == AFF_BURNING ) {
    if( ( obj->condition -= 10 ) > 0 ) 
      add_queue( event, 100 );
    else {
      remove_affect( obj, affect );
      obj->Extract( );
      }
    return;
    }

  if( --affect->duration > 0 ) {
    add_queue( event, 100 );
    return;
    }

  obj->events -= event;
  remove_affect( obj, affect );
 
  delete event;
}


void write_affects( FILE* fp, obj_data* obj )
{
  affect_data*  affect;

  if( is_empty( obj->affected ) )
    return;

  fwrite( "Afft", 4, 1, fp );
  fwrite( &obj->affected.size, sizeof( int ), 1, fp );

  for( int i = 0; i < obj->affected; i++ ) {
    affect = obj->affected[i];
    fwrite( &affect->type,     sizeof( int ), 1, fp );
    fwrite( &affect->duration, sizeof( int ), 1, fp );
    fwrite( &affect->level,    sizeof( int ), 1, fp );
    } 
}


void read_affects( FILE* fp, obj_data* obj )
{
  affect_data*  affect;
  event_data*    event;
  int                i;

  fread( &i, sizeof( int ), 1, fp );

  for( ; i > 0; i-- ) {
    affect = new affect_data;
    fread( &affect->type,     sizeof( int ), 1, fp );
    fread( &affect->duration, sizeof( int ), 1, fp );
    fread( &affect->level,    sizeof( int ), 1, fp );
    obj->affected += affect;

    event          = new event_data( update_affect, obj );
    event->pointer = (void*) affect;
    add_queue( event, 100 );
    }
}


void add_affect( obj_data* obj, affect_data* paf )
{
  affect_data*   paf_new;
  char_data*          ch;
  event_data*      event;

  for( int i = 0; i < obj->affected; i++ )
    if( paf->type == obj->affected[i]->type ) {
      obj->affected[i]->duration
        = max( obj->affected[i]->duration, paf->duration );
      return;
      }

  if( ( ch = carried_by( obj ) ) != NULL ) 
    act( ch, aff_obj_table[ paf->type ].msg_on, ch, obj );

  paf_new = new affect_data;

  memcpy( paf_new, paf, sizeof( *paf_new ) );

  obj->affected  += paf_new;
  paf_new->leech  = NULL;
  paf_new->victim = NULL;

  event          = new event_data( update_affect, obj );
  event->pointer = (thing_data*) paf_new;
  add_queue( event, 100 );

  set_bit( obj->extra_flags, aff_obj_table[ paf->type ].location );
}


bool is_affected( obj_data* obj, int type )
{
  for( int i = 0; i < obj->affected; i++ )
    if( obj->affected[i]->type == type )
      return TRUE;

  return FALSE;
}


void remove_affect( obj_data* obj, affect_data* affect, bool msg )
{
  char_data*             ch; 
  room_data*          where;
  int                  flag;
  event_data*         event;

  flag           = aff_obj_table[ affect->type ].location;
  obj->selected  = 1;
  obj->affected -= affect;

  if( !is_set( obj->pIndexData->extra_flags, flag )
    && !is_affected( obj, affect->type ) )
    remove_bit( obj->extra_flags, flag );

  if( msg ) {
    if( ( where = Room( obj->array->where ) ) != NULL ) 
      act_room( where, aff_obj_table[ affect->type ].msg_off, obj );
    else if( ( ch = character( obj->array->where ) ) != NULL ) 
      act( ch, aff_obj_table[ affect->type ].msg_off, NULL, obj );
    }

  for( int i = 0; i < obj->events; i++ ) {
    event = obj->events[i];
    if( event->pointer == (thing_data*) affect ) {
      extract( event );
      break;
      }
    }

  delete affect;
}


bool strip_affect( obj_data* obj, int sn )
{
  int             flag  = aff_obj_table[ sn ].location;
  affect_data*  affect;

  if( !is_set( obj->extra_flags, flag )
    || is_set( obj->pIndexData->extra_flags, flag ) )
    return FALSE;

  for( int i = 0; i < obj->affected; i++ ) {
    affect = obj->affected[i];
    if( affect->type == sn ) {
      remove_affect( obj, affect, FALSE );
      return TRUE;
      }
    }

  remove_bit( obj->extra_flags, flag );

  roach( "Strip_Affect: Affect on object not found." );
  roach( "-- Obj = %s", obj );

  return TRUE;
}


/*
 *
 */


int affect_level( char_data* ch, int bit ) 
{
  if( is_set( ch->affected_by, bit ) ) 
    for( int i = 0; i < ch->affected; i++ )
      if( ch->affected[i]->type == bit ) 
        return ch->affected[i]->level;

  return 0;
}


int affect_duration( char_data* ch, int bit ) 
{
  if( is_set( ch->affected_by, bit ) ) 
    for( int i = 0; i < ch->affected; i++ )
      if( ch->affected[i]->type == bit ) 
        return ch->affected[i]->duration;

  return 0;
}


     











