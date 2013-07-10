#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


bool        can_die         ( char_data* );
void        death_cry       ( char_data* );
obj_data*   make_corpse     ( char_data*, content_array* );
void        raw_kill        ( char_data* );
void        loot_corpse     ( obj_data*, char_data*, char_data* );
void        register_death  ( char_data*, char_data*, char* );


/*
 *   HAS LIVE-SAVING?
 */ 


bool can_die( char_data* victim )
{
  obj_data*    obj;
  room_data*  room;

  if( victim->species != NULL )
    return TRUE;

  if( ( room = Room( victim->array->where ) ) != NULL
    && is_set( &room->room_flags, RFLAG_ARENA ) ) {
    send( *victim->array, "In a flash of light %s disappears.\n\r", victim );
    victim->hit = 1;
    update_pos( victim );
    victim->From( );
    victim->To( get_temple( victim ) );
    dismount( victim );
    victim->position = POS_RESTING;
    return FALSE;
    }  

  if( victim->shdata->level >= LEVEL_BUILDER ) {
    fsend_seen( victim, "The body of %s slowly fades out of existence.",
      victim );
    fsend( *victim->array,
      "A swirling mist appears and %s slowly reforms.", victim );
    send( victim, "You find yourself alive again.\n\r" );
    victim->hit = victim->max_hit;
    update_max_move( victim );
    update_pos( victim );
    return FALSE;
    }   

  for( int i = 0 ; ; i++ ) {
    if( i >= victim->affected ) {
      for( int j = 0; ; j++ ) {
        if( j >= victim->wearing )
          return TRUE;
        obj = (obj_data*) victim->wearing[j];
        if( is_set( obj->pIndexData->affect_flags, AFF_LIFE_SAVING ) )
          break;
        }
      fsend( *victim->array,
        "%s that %s was wearing starts to glow a deep purple!",
        obj, victim );
      fsend_seen( victim, "The body of %s slowly disappears!", victim );
      obj->Extract( 1 ); 
      break;
      } 
    if( victim->affected[i]->type == AFF_LIFE_SAVING
      && number_range( 0, 100 ) > 60-2*victim->affected[i]->level ) {
      send_seen( victim,
        "%s disappears in an explosion of light and energy.\n\r", victim );
      break;
      }
    }  

  victim->From( );

  remove_affect( victim );
  remove_leech( victim );

  rejuvenate( victim );

  victim->To( get_temple( victim ) );
  dismount( victim );
  victim->position = POS_RESTING;

  send_seen( victim, "%s slowly materializes.\n\rFrom %s appearance\
 %s obviously came close to death.\n\r",
    victim, victim->His_Her( ), victim->He_She( ) );
  send( victim, "You wake up confused and dazed, but seem alive despite\
 your memories.\n\r" );

  return FALSE;
}


bool die_forever( char_data* ch )
{
  if( ch->Type( ) == MOB_DATA ) 
    return TRUE;

  return FALSE;
}


/*
 *   DEATH HANDLER
 */


void death( char_data* victim, char_data* ch, char* dt )
{
  char               tmp  [ TWO_LINES ];
  obj_data*       corpse;
  content_array*   where  = victim->array;
  char_data*         rch;
  bool           survive;
 
  remove_bit( &victim->status, STAT_BERSERK );

  if( ch == NULL ) 
    for( int i = 0; i < *where; i++ ) 
      if( ( rch = character( where->list[i] ) ) != NULL 
        && includes( rch->aggressive, victim ) ) {
        ch = rch;
        break;
        }

  stop_fight( victim );
  clear_queue( victim );

  if( !can_die( victim ) )
    return;
 
  disburse_exp( victim );
  register_death( victim, ch, dt );

  clear_queue( victim );
  death_cry( victim );

  if( survive = !die_forever( victim ) )
    raw_kill( victim );

  corpse = make_corpse( victim, where );
  loot_corpse( corpse, ch, victim );

  if( survive ) 
    return;

  if( mob( victim ) != NULL ) {
    victim->Extract( );
    return;
    }

  sprintf( tmp, "%s's soul is taken by death.", victim->Name( ) );
  info( tmp, LEVEL_BUILDER, tmp, IFLAG_DEATHS, 1, victim );

  clear_screen( victim );
  reset_screen( victim );

  send( victim, "Death is surprisingly peaceful.\n\r" );
  send( victim, "Good night.\n\r" );

  purge( player( victim ) );
}


void raw_kill( char_data* victim )
{
  affect_data    affect;
 
  victim->From( );

  remove_affect( victim );
  remove_leech( victim );

  victim->position  = POS_RESTING;
  victim->hit       = max( 1, victim->hit  );
  victim->mana      = max( 1, victim->mana );
  victim->move      = max( 1, victim->move );

  affect.type      = AFF_DEATH;
  affect.duration  = 20;
  affect.level     = 10;
  affect.leech     = NULL;

  add_affect( victim, &affect );
  delete_list( victim->prepare );
  update_maxes( victim );

  victim->To( get_room_index( ROOM_DEATH, FALSE ) );

  dismount( victim );
  clear_enemies( victim );

  write( player( victim ) );
}


void register_death( char_data* victim, char_data* ch, char* dt )
{
  char          tmp1  [ TWO_LINES ];
  char          tmp2  [ TWO_LINES ];
  mprog_data*  mprog;
  int            exp;

  if( victim->species != NULL ) {
    if( is_set( &victim->status, STAT_PET ) && victim->leader != NULL
      && victim->leader->pcdata != NULL ) {
      sprintf( tmp1, "%s killed by %s at %s. (Pet)",
        victim->Name( ), ch == NULL ? dt : ch->Name( ),
        victim->array->where->Location( ) );
      player_log( victim->leader, tmp1 );
      }
    for( mprog = victim->species->mprog; mprog != NULL;
      mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_DEATH ) {
        var_mob  = victim;
        var_ch   = ch;
        var_room = Room( victim->array->where );
        execute( mprog );
        }
    return;
    }

  sprintf( tmp1, "%s killed by %s.", victim->Name( ),
    ch == NULL ? dt : ch->Name( ) );
  sprintf( tmp2, "%s killed by %s at %s.", victim->Name( ),
    ch == NULL ? dt : ch->Name( ), victim->array->where->Location( ) );
  info( tmp1, LEVEL_APPRENTICE, tmp2, IFLAG_DEATHS, 1, victim );

  exp = death_exp( victim, ch );
  add_exp( victim, -exp, "You lose %d exp for dying.\n\r" );

  sprintf( tmp1, "Killed by %s at %s.",
    ch == NULL ? dt : ch->real_name( ), victim->array->where->Location( ) );
  player_log( victim, tmp1 );
      
  if( ch != NULL && ch->pcdata != NULL ) {
    sprintf( tmp1, "Pkilled %s at %s.",
      victim->real_name( ), victim->array->where->Location( ) );
    player_log( ch, tmp1 );
    }
}


/* 
 *   DEATH CRY
 */


void death_message( char_data* victim )
{
  char          tmp1  [ ONE_LINE ];
  char          tmp2  [ ONE_LINE ];
  char_data*     rch;
  const char*   name;

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( rch = character( victim->array->list[i] ) ) == NULL
      || rch->link == NULL || rch == victim || !victim->Seen( rch ) ) 
      continue;
    name = victim->Name( rch );
    send( rch, bold_red_v( rch ) );
    if( victim->species != NULL
      && is_set( &victim->species->act_flags, ACT_MELT ) ) {
      sprintf( tmp1, "  +-- %%%ds --+\n\r", strlen( name )+11 );
      sprintf( tmp2, tmp1, "" );
      sprintf( tmp1, "      %s SHATTERS!!\n\r", name );
      }
    else {
      sprintf( tmp1, "  +-- %%%ds --+\n\r", strlen( name )+10 );
      sprintf( tmp2, tmp1, "" );
      sprintf( tmp1, "      %s is DEAD!!\n\r", name );
      }
    tmp1[6] = toupper( tmp1[6] );
    send( rch, tmp2 );
    send( rch, tmp1 );
    send( rch, tmp2 );        
    send( rch, normal( rch ) );
    }

  send( victim, "You have been KILLED!!\n\r" );
}


void death_cry( char_data* ch )
{
  room_data*  room;
  char*        msg;
 
  if( ( room = Room( ch->array->where ) ) == NULL )
    return;

  msg = ch->species ? "You hear something's death cry.\n\r"
    : "You hear someone's death cry.\n\r";

  for( int i = 0; i < room->exits; i++ ) 
    send( room->exits[i]->to_room->contents, msg );

  if( is_set( &ch->status, STAT_FAMILIAR ) && ch->leader != NULL )  
    send( ch->leader, "You sense your familiars death!\n\r" );
}


/*
 *   CORPSE ROUTINES
 */


const char* fragments_msg =
  "The fragments from %s quickly melt, leaving %s %s.";   


obj_data* make_corpse( char_data* ch, content_array* where )
{
  obj_data*          corpse;
  obj_data*             obj;
  obj_clss_data*   obj_clss;
  thing_data*         thing;

  /* GHOSTS */

  if( ch->species != NULL && is_set( &ch->species->act_flags, ACT_GHOST ) ) {
    for( int i = ch->wearing-1; i >= 0; i-- ) {
      thing = ch->wearing[i]->From( ch->wearing[i]->number );
      thing->To( &ch->contents ); 
      }
    send_publ( ch, &ch->contents, "fades out of existence", "dropping" );
    for( int i = ch->contents.size-1; i >= 0; i-- ) {
      thing = ch->contents[i]->From( ch->contents[i]->number );
      thing->To( where );
      }
    return NULL;
    }

  /*
  if( ch->species != NULL && is_set( &ch->species->act_flags, ACT_MELT ) ) {
    if( ( obj = ch->contents ) == NULL ) {
      fsend_room( room, fragments_msg, ch, "nothing", "behind" );
      }
    else if( obj->next_content == NULL ) {
      fsend_room( room, fragments_msg, ch, obj,
        "lying on the ground" );
      }
    else {
      fsend_room( room, fragments_msg, ch, "several items",
        "on the ground" );
      }
    for( ; obj != NULL; obj = ch->contents ) {
      obj = remove( obj, obj->number );
      put_obj( obj, room );
      }
    return NULL;
    }
  */

  /* CREATE CORPSE */

  if( ch->species != NULL ) {
    if( ( obj_clss = get_obj_index( ch->species->corpse ) ) == NULL ) 
      return NULL;
    corpse = create( obj_clss );
    if( obj_clss->item_type == ITEM_CORPSE )
      corpse->value[1] = ch->species->vnum;
    }
  else {
    corpse           = create( get_obj_index( OBJ_CORPSE_PC ) );
    corpse->value[1] = ch->pcdata->pfile->ident;
    }

  /* WEIGHT */

  if( corpse->pIndexData->item_type == ITEM_CORPSE
    && corpse->pIndexData->weight == 0 )
    corpse->weight = ch->Empty_Weight( );

  /* NAME CORPSE */ 

  if( !strncmp( corpse->pIndexData->singular, "corpse of", 9 ) ) {
    char* tmp = static_string( );

    if( ch->descr->name != empty_string ) {
      sprintf( tmp, "corpse of %s", ch->descr->name );
      corpse->singular = alloc_string( tmp, MEM_OBJECT );
      sprintf( tmp, "%s corpses", ch->descr->name );
      corpse->plural = alloc_string( tmp, MEM_OBJECT );
      }
    else {
      sprintf( tmp, "corpse of %s", ch->Name( NULL ) );
      corpse->singular = alloc_string( tmp, MEM_OBJECT );
      sprintf( tmp, "%s corpses", seperate( ch->descr->singular, TRUE ) );
      corpse->plural = alloc_string( tmp, MEM_OBJECT );
      }
    }
 
  /* TRANSFER ITEMS TO CORPSE */

  for( int i = ch->wearing-1; i >= 0; i-- ) {
    if( ch->species != NULL || number_range( 0,10 ) == 0 ) {
      obj = (obj_data*) ch->wearing[i];
      obj->From( obj->number );
      obj->To( corpse );
      }
    }

  for( int i = ch->contents-1; i >= 0; i-- ) {
    if( ( obj = object( ch->contents[i] ) ) != NULL
      && ( ch->species != NULL || number_range( 0,10 ) == 0 ) ) {
      obj = (obj_data*) obj->From( obj->number );
      obj->To( corpse );
      }
    }

  corpse->To( where );

  return corpse;
}


void loot_corpse( obj_data* corpse, char_data* ch, char_data* victim )
{
  if( ch == NULL || ch->pcdata == NULL
    || victim->species == NULL || corpse == NULL ) 
    return;

  int level = level_setting( &ch->pcdata->pfile->settings,
    SET_AUTOLOOT );

  if( level == 0 )
    return;

  if( level == 1 ) {
    send_priv( ch, &corpse->contents, "contains", corpse ); 
    return;
    }

  /*
  for( obj = corpse->contents; obj != NULL; obj = next ) {
    next = obj->next_content;
    if( obj->pIndexData->item_type == ITEM_MONEY ) {
      for( i = 0; i < MAX_COIN; i++ )
        if( obj->pIndexData->vnum == coin_vnum[i] )
          amount += obj->number*coin_value[i];
      }
    else {
      if( !is_set( ch->pcdata->pfile->flags, PLR_AUTO_LOOT ) ) 
        continue;
      }
    obj->next_list = get;
    get            = obj;
    obj->selected  = obj->number;
    }

  get_obj( ch, get, corpse );

  if( is_set( ch->pcdata->pfile->flags, PLR_AUTO_SPLIT ) ) 
    split_money( ch, amount, FALSE );
  */
}


/*
 *   SLAY ROUTINE
 */


void do_slay( char_data* ch, char* argument )
{
  char_data*  victim;
    
  if( ch->shdata->level < LEVEL_BUILDER ) {
    send( ch, "To prevent abuse you are unable to use slay in\
 mortal form.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Slay whom?\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "slay",
    ch->array ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "Suicide is a mortal sin.\n\r" );
    return;
    }
    
  if( victim->species == NULL
    && ( get_trust( victim ) >= get_trust( ch )
    || !has_permission( ch, PERM_PLAYERS ) ) ) {
    send( ch, "You failed.\n\r" );
    return;
    }

  send( ch, "You slay %s in cold blood!\n\r", victim );
  send( victim, "%s slays you in cold blood!\n\r", ch );
  send( *ch->array, "%s slays %s in cold blood!\n\r", ch, victim );

  make_corpse( victim, ch->array );

  if( victim->species == NULL ) 
    raw_kill( victim );
  else
    victim->Extract( );
}


/*
 *   EXTRACTION ROUTINES
 */


void dereference( char_data* wch, char_data* ch )
{
  player_data*   pc;
  wizard_data*  imm;

  if( !wch->Is_Valid( ) )
    return;

  if( wch->cast != NULL && wch->cast->target == ch ) 
    disrupt_spell( wch );

  wch->seen_by  -= ch;
  wch->known_by -= ch;

  if( wch->reply == ch ) {
    if( is_set( &wch->status, STAT_REPLY_LOCK ) ) {
      send( wch, "%s has quit - reply lock removed.\n\r", ch );
      remove_bit( &wch->status, STAT_REPLY_LOCK );
      }
    wch->reply = NULL;
    }

  if( ( imm = wizard( wch ) ) != NULL
    && imm->player_edit == ch ) {
    send( imm, "The player you were editing just quit.\n\r" );
    imm->player_edit = NULL;
    }

  if( ( pc = player( wch ) ) != NULL
    && pc->familiar == ch )
    pc->familiar = NULL;
} 


void Char_Data :: Extract( )
{
  affect_data*    affect;
  char_data*         wch;
  obj_array*       array;
  mob_data*          npc;
  player_data*        pc;
  wizard_data*       imm;

  /* REMOVE WORLD REFERENCES */

  if( !Is_Valid( ) ) {
    roach( "Extract Char: Extracting invalid character." ); 
    return;
    }

  if( species != NULL ) {
    if( link != NULL ) 
      do_return( this, "" );
    unregister_reset( this );
    }

  if( link != NULL ) {
    link->character = NULL;
    link->player    = NULL;
    link            = NULL;
    }

  if( this->array != NULL )
    From( );

  clear_queue( this );

  if( ( pc = player( this ) ) != NULL ) {
    remove( request_app, pc );
    remove( request_imm, pc );
    }

  /* CLEAR EVENTS */

  stop_events( this );
  unlink( &active );
  active.time = -1;

  /* EXTRACT OBJECTS */

  if( ( pc = player( this ) ) != NULL ) {
    array = &pc->save_list;
    for( int i = 0; i < array->size; i++ )
      if( array->list[i] != NULL )
        array->list[i]->save = NULL;
    clear( *array );
    extract( pc->locker );
    extract( pc->junked );
    }

  extract( contents );
  extract( wearing );

  /* REMOVE FOLLOWERS */

  if( leader != NULL ) {
    if( ( pc = player( leader ) ) != NULL 
      && pc->familiar == this )
      pc->familiar = NULL;
    leader->followers -= this;
    leader = NULL;
    }

  for( int i = 0; i < followers; i++ ) {
    wch         = followers[i];
    wch->leader = NULL;
    if( wch->species != NULL && is_set( &wch->status, STAT_PET ) ) {
      send_seen( wch,
        "%s goes into limbo and awaits %s master's return.\n\r",
        wch, wch->His_Her( ) );
      wch->Extract( );
      }
    }

  clear( followers );
  
  /* REMOVE REFERENCES ON OTHER PLAYERS */

  for( int i = 0; i < player_list; i++ ) 
    dereference( player_list[i], this );

  for( int i = 0; i < mob_list; i++ ) 
    dereference( mob_list[i], this );

  if( mount != NULL ) {
    mount->rider = NULL;
    mount = NULL;
    }

  if( rider != NULL ) {
    rider->mount = NULL;
    rider = NULL;
    }

  /*
   *   FREE MEMORY
   */

  remove_leech( this );

  for( int i = 0; i < affected; i++ ) {
    affect = affected[i]; 
    if( affect->leech != NULL )
      fsend( affect->leech, "Leech for %s on %s dissipated.",
        aff_char_table[affect->type].name, descr->name );
    remove_leech( affect );
    }
  clear( affected );

  extract( enemy );
  delete_list( prepare );

  clear( cmd_queue );
  clear( known_by );
  clear( seen_by );
  clear( seen_exits );

  if( species == NULL || descr != species->descr ) 
    delete descr;

  if( ( pc = player( this ) ) != NULL ) {
    delete_list( pc->alias );
    delete_list( pc->tell );
    delete_list( pc->gtell );
    delete_list( pc->ctell );
    delete_list( pc->whisper );
    delete_list( pc->chat );
    delete_list( pc->gossip );
    delete_list( pc->say );
    delete_list( pc->yell );
    delete_list( pc->shout );
    delete_list( pc->atalk );
    delete_list( pc->to );
    delete_list( pc->chant );
    }

  if( ( imm = wizard( this ) ) != NULL ) {
    delete_list( imm->build_chan );
    delete_list( imm->imm_talk   );
    delete_list( imm->god_talk   );
    delete_list( imm->avatar     );

    free_string( imm->bamfin,       MEM_WIZARD );
    free_string( imm->bamfout,      MEM_WIZARD );
    free_string( imm->level_title,  MEM_WIZARD );
    }

  if( ( npc = mob( this ) ) != NULL ) { 
    if( npc->pTrainer != NULL ) 
      npc->pTrainer->mob = NULL;
    free_string( npc->pet_name, MEM_MOBS );
    }

  if( pcdata != NULL ) {
    free_string( pcdata->title,        MEM_PLAYER );
    free_string( pcdata->buffer,       MEM_PLAYER ); 
    free_string( pcdata->tmp_short,    MEM_PLAYER );
    free_string( pcdata->tmp_keywords, MEM_PLAYER );
    free_string( pcdata->prompt,       MEM_PLAYER );

    delete pcdata->recognize; 
    delete pcdata;
    delete shdata;
    }

  pcdata   = NULL;
  shdata   = NULL;
  descr    = NULL;
  species  = NULL;
  position = POS_EXTRACTED;
  pShop    = NULL;
  reset    = NULL;
  valid    = -1;

  extracted += this;
}





