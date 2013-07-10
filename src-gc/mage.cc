#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *   DAMAGE
 */


bool spell_meteor_swarm( char_data* ch, char_data*, void*, int level, int )
{
  char_data* rch;

  if( null_caster( ch, SPELL_METEOR_SWARM ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && can_kill( ch, rch, FALSE ) )
      damage_physical( rch, ch, spell_damage( SPELL_METEOR_SWARM, level ),
        "*The swarm of meteors" );

  return TRUE;
}


bool spell_maelstrom( char_data* ch, char_data*, void*, int level, int )
{
  char_data*   rch;

  if( null_caster( ch, SPELL_MAELSTROM ) )
    return TRUE;

  for( int i = ch->array->size; i >= 0; i-- ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch != ch && can_kill( ch, rch, FALSE ) )
      damage_magic( rch, ch, spell_damage( SPELL_MAELSTROM, level ),     
        "*The maelstrom" );     

  return TRUE;
}


/*
 *   MISSILE SPELLS
 */


bool spell_magic_missile( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_MAGIC_MISSILE ) )
    return TRUE;

  damage_magic( victim, ch, spell_damage( SPELL_MAGIC_MISSILE, level ),
    "*The magic missile" );

  return TRUE;
}


bool spell_prismic_missile( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_PRISMIC_MISSILE ) )
    return TRUE;

  damage_magic( victim, ch, spell_damage( SPELL_PRISMIC_MISSILE, level ),
    "*A multi-hued burst of light" );

  return TRUE;
}


/*
 *   SLEEP SPELLS
 */


void sleep_affect( char_data* ch, char_data* victim, int level, int duration )
{
  if( !can_kill( ch, victim, FALSE ) )
    return;
  
  if( is_set( victim->affected_by, AFF_SLEEP_RESIST ) )
    return; 

  if( ch != NULL
    && makes_save( victim, ch, RES_MAGIC, SPELL_SLEEP, level ) ) {
    if( victim->position > POS_SLEEPING ) 
      send( victim, "You feel drowsy but quickly shrug it off.\n\r" );
    return;
    }

  spell_affect( ch, victim, level, duration, SPELL_SLEEP, AFF_SLEEP );
  record_damage( victim, ch, 10 );
  stop_fight( victim );

  if( victim->position <= POS_SLEEPING ) 
    return;

  send( victim, "You feel sleepy and suddenly fall asleep!\n\r" );

  if( victim->mount != NULL ) {
    fsend( *victim->array,
      "%s tumbles off %s %s is riding and drops to the ground.",
      victim, victim->mount, victim->He_She( ) );
    victim->mount->rider = NULL;
    victim->mount = NULL;  
    }
  else {
    send_seen( victim, "%s drops to the ground dead asleep.\n\r",
      victim );
    }

  victim->position = POS_SLEEPING;

  return;
}


bool spell_sleep( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  sleep_affect( ch, victim, level, duration );

  return TRUE;
}


bool spell_mists_sleep( char_data* ch, char_data*, void*, int level,
  int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_MISTS_SLEEP ) )
    return TRUE;

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    send( ch, "You are unable to summon the mists underwater.\n\r" );
    return TRUE;
    }

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch )
      sleep_affect( ch, rch, level, duration );

  return TRUE;
}


/*
 *   STRENGTH SPELL
 */


bool spell_ogre_strength( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_OGRE_STRENGTH,
    AFF_OGRE_STRENGTH, min( 17, victim->shdata->strength+3 ) );

  return TRUE;
}


/*
 *   ENCHANTMENT SPELLS
 */


bool spell_detect_magic( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_MAGIC,
    AFF_DETECT_MAGIC );

  return TRUE;
}


bool spell_minor_enchantment( char_data* ch, char_data*, void* vo,
  int level, int )
{
  obj_data*   obj  = (obj_data*) vo;
  int        roll  = number_range( 0, 100 );

  if( null_caster( ch, SPELL_MINOR_ENCHANTMENT ) ) 
    return TRUE;

  obj->selected = 1;

  if( is_set( obj->extra_flags, OFLAG_NO_ENCHANT ) ) {
    send( ch, "This object cannot be enchanted.\n\r", obj );
    return TRUE;
    }

  if( obj->value[0] < 0 ) {
    send( *ch->array,
      "%s starts to glow, but the light turns black and then fades.\n\r",
      obj );
    return TRUE;
    }

  if( roll < 30-level+10*obj->value[0] ) {
    fsend( *ch->array,
      "%s glows briefly, but then suddenly explodes in a burst\
 of energy!\n\r", obj );
    obj->Extract( 1 );
    return TRUE;
    } 

  if( roll <= 60-2*level+20*obj->value[0] ) {
    fsend( *ch->array,
      "%s glows briefly but the enchantment fails.\n\r", obj );
    return TRUE;
    }

  send( *ch->array, "%s glows for a while.\n\r", obj );

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
    }
 
  obj->value[0]++;
  set_bit( obj->extra_flags, OFLAG_MAGIC );

  if( obj->array == NULL )
    ch->contents += obj;

  return TRUE;
}


bool spell_major_enchantment( char_data* ch, char_data*, void* vo, 
  int level, int )
{
  obj_data*   obj  = (obj_data*) vo;
  int        roll  = number_range( 0, 100 );
 
  if( null_caster( ch, SPELL_MAJOR_ENCHANTMENT ) ) 
    return TRUE;
  
  if( is_set( obj->extra_flags, OFLAG_NO_MAJOR ) ) {
    send( ch, "%s cannot be enchanted.\n\r", obj );
    return TRUE;
    }

  obj->selected = 1;

  if( obj->value[0] < 0 || obj->value[0] >= 3 ) {
    fsend( *ch->array,
      "%s starts to glow, but the light turns black and then fades.",
      obj );
    return TRUE;
    }

  if( roll <= 30-level+20*obj->value[0] ) {
    fsend( *ch->array,
      "%s glows briefly but the enchantment fails.\n\r", obj );
    return TRUE;
    }

  send( *ch->array, "%s glows for a while.\n\r", obj );

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
    }

  obj->value[0]++;
  set_bit( obj->extra_flags, OFLAG_MAGIC );

  if( obj->array == NULL )
    obj->To( ch );

  return TRUE;

}


/*
 *   REPLICATE
 */


bool spell_replicate( char_data* ch, char_data*, void* vo, int, int )
{
  obj_data*   obj  = (obj_data*) vo; 

  if( is_set( obj->extra_flags, OFLAG_COPIED ) ) {
    fsend( ch, "You feel %s has already been copied and lacks the essence\
 required for you to replicate it.", obj );
    return TRUE;
    }

  fsend( ch, "In a swirl of colors %s materializes in your hand.", obj );
  fsend_seen( ch, "In a swirl of colors %s materializes in %s's hand.",
    obj, ch );   

  obj->pIndexData->count++;

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
    }

  obj->number = 2;
  set_bit( obj->extra_flags, OFLAG_COPIED );

  if( obj->array == NULL )
    obj->To( ch );

  return TRUE;
}
 
 
/*
 *   IDENTIFY SPELL
 */


bool spell_identify( char_data* ch, char_data*, void* vo, int, int )
{
  char                  buf  [ MAX_INPUT_LENGTH ];
  affect_data*          paf;
  obj_data*             obj  = (obj_data*) vo;
  obj_clss_data*   obj_clss  = obj->pIndexData;
  bool                found  = FALSE;
  int                     i;

  if( null_caster( ch, SPELL_IDENTIFY ) )
    return TRUE;

  if( obj == NULL ) {
    bug( "Identify: NULL object." );
    return TRUE;
    }

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
    }
 
  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

  send( ch, scroll_line[0] );

  sprintf( buf, "%s%s", obj->Seen_Name( ch, 1 ),
    is_set( obj->extra_flags, OFLAG_NOREMOVE ) ? "  [CURSED]" : "" );
  send_title( ch, buf );

  send( ch, "     Base Cost: %-12d Level: %-11d Weight: %.2f lbs\n\r",
    obj->Cost( ), obj_clss->level, obj->Empty_Weight( 1 )/100. );
  sprintf( buf, "            " );

  switch( obj_clss->item_type ) {
   case ITEM_WEAPON :
    sprintf( buf+5,  "   Damage: %dd%d       ", 
      obj_clss->value[1], obj_clss->value[2] );
    sprintf( buf+25, "    Class: %s          ",
      ( obj_clss->value[3] >= 0 && obj_clss->value[3] < MAX_WEAPON ) ?
      skill_table[ WEAPON_UNARMED+obj_clss->value[3] ].name : "none" );
    sprintf( buf+45, "   Attack: %s\n\r",
      ( obj_clss->value[3] >= 0 && obj_clss->value[3] < MAX_WEAPON ) ?
      weapon_attack[ obj_clss->value[3] ] : "none" );
    break;

   case ITEM_WAND :
    sprintf( buf+5,  "    Spell: %s          ",
      ( obj_clss->value[0] >= 0 && obj_clss->value[0] < MAX_SPELL ) ?
      spell_table[ obj_clss->value[0] ].name : "none" );
    sprintf( buf+25, "  Charges: %d\n\r", obj->value[3] );
    break;

   case ITEM_WHISTLE:
    if( obj_clss->value[0] > 0 )
      sprintf( buf+5,  "   Range: %d\n\r", obj_clss->value[0] );
    break;

   case ITEM_SCROLL :
   case ITEM_POTION :
    sprintf( buf+5,  "    Spell: %s          ",
      ( obj_clss->value[0] >= 0 && obj_clss->value[0] < MAX_SPELL ) ?
      spell_table[ obj_clss->value[0] ].name : "none" );
    sprintf( buf+25, "    Level: %d\n\r",
      obj_clss->value[1] );
    break;

   case ITEM_TRAP :
    sprintf( buf, "Damage: %dd%d\n\r", obj_clss->value[1],
      obj_clss->value[2] );
    break;

   case ITEM_ARMOR :
    sprintf( buf, "   Armor Class: %-3d", armor_class( obj ) );
    if( obj->value[2] != 0 && can_wear( obj, ITEM_WEAR_WRIST ) ) 
      sprintf( buf+strlen( buf ), "Protection Aura: %d",
        obj->value[2] );
    strcat( buf, "\n\r" );
    break;

   default :
    buf[0] = '\0';
    break;
    } 

  if( *buf != '\0' ) {
    send( ch, buf );
    }

  sprintf( buf,    "%d%%", obj->vs_acid( ) );
  sprintf( buf+20, "%d%%", obj->vs_fire( ) );
  sprintf( buf+40, "%d%%", obj->vs_cold( ) );

  send( ch, "          Acid: %-13s Fire: %-13s Cold: %s\n\r\n\r",
    buf, buf+20, buf+40 );

  /* CONDITION */ 

  if( obj_clss->item_type == ITEM_ARMOR
    || obj_clss->item_type == ITEM_WEAPON )  
    send( ch, "     Condition: %s\n\r",
      obj->condition_name( ch, TRUE ) );

  /* MATERIALS */

  strcpy( buf, "     Materials: " );
  material_flags.sprint( &buf[16], &obj->materials );
  send( ch, buf );
  send( ch, "\n\r" );

  /* WEAR LOC */

  buf[0] = '\0';
  for( i = 1; i < MAX_ITEM_WEAR; i++ )
    if( is_set( &obj_clss->wear_flags, i ) ) {
      if( buf[0] == '\0' ) 
        sprintf( buf, "     Wear Loc.: %s", wear_name[i] );
      else
        sprintf( buf+strlen( buf ), ", %s", wear_name[i] );
      }
  if( *buf != '\0' ) {
    send( ch, buf );
    send( ch, "\n\r" );
    }

  /* ANTI-FLAGS */

  buf[0] = '\0';
  for( i = 0; i < MAX_ANTI; i++ )
    if( is_set( &obj_clss->anti_flags, i ) ) {
      if( buf[0] == '\0' ) 
        sprintf( buf, "\n\r    Anti-Flags: %s", anti_flags[i] );
      else
        sprintf( buf+strlen( buf ), ", %s", anti_flags[i] );
      }
  if( *buf != '\0' ) {
    strcat( buf, "\n\r" );
    send( ch, buf );
    }

  /* AFFECTS */

  for( i = 0; i < obj_clss->affected; i++ ) {
    paf = obj_clss->affected[i];
    if( paf->type == AFF_NONE ) { 
      if( !found ) {
        send( ch, "\n\r          Affects:\n\r" );
        found = TRUE;    
        }
      send( ch, "%12s%s by %d.\n\r", "",
        affect_location[ paf->location ], paf->modifier );
      }
    }

  for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) {
    if( is_set( obj_clss->affect_flags, i ) ) {
      if( !found ) {
        send( ch, "\n\r          Affects:\n\r" );
        found = TRUE;    
        }
      send( ch, "%12s%s\n\r", "", aff_char_table[i].id_line );
      }
    }
  
  for( i = 0; i < MAX_OFLAG; i++ ) {
    if( is_set( obj->extra_flags, i ) && oflag_ident[i][0] != '\0' ) {
      if( !found ) {
        send( ch, "\n\r          Affects:\n\r" );
        found = TRUE;    
        }
      send( ch, "%12s%s\n\r", "", oflag_ident[i] );
      } 
    }
  
  send( ch, "\n\r" );
  send( ch, scroll_line[0] );

  if( obj->array == NULL )
    obj->To( ch );

  return TRUE;
};


bool spell_obscure( char_data* ch, char_data*, void* vo, int, int )
{

  obj_data*             obj  = (obj_data*) vo;
 
  if( null_caster( ch, SPELL_OBSCURE  ) )
    return TRUE;

  remove_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  remove_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
  
  return TRUE;

}

/*
 *   FLASH OF LIGHT
 */


bool spell_blind( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  if( victim->species != NULL 
    && !is_set( &victim->species->act_flags, ACT_HAS_EYES ) ) {
    send( ch, "%s does not see with %s eyes so blind has no affect.\n\r",
      victim, victim->His_Her( ) );
    return TRUE;
    }

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_BLIND, level ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_BLIND, AFF_BLIND );

  return TRUE;
}


bool spell_blinding_light( char_data* ch, char_data*, void*,
  int level, int duration )
{
  char_data* rch;

  /*
  for( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room ) {
    if( rch == ch || !rch->Can_See( ) || ( rch->species != NULL
      && !is_set( &rch->species->act_flags, ACT_HAS_EYES ) ) )
      continue;
    send( rch, "### The room explodes in a flash of light. ###\n\r" );
    if( makes_save( rch, ch, RES_MAGIC, SPELL_BLINDING_LIGHT, level ) ) {
      send( rch, 
        "Fortunately you were not looking at the blast and\
 are unaffected.\n\r" );
      }
    else if( !can_kill( ch, rch ) ) {
      send( rch, "Oddly the flash has no affect on you.\n\r" );
      }
    else {
      spell_affect( ch, rch, level, duration,
        SPELL_BLINDING_LIGHT, AFF_BLIND );
      }
    }
  */

  return TRUE;
}


/*
 *   ENTROPY SPELLS
 */


bool spell_wither( char_data* ch, char_data* victim, void*, int level, int )
{
  affect_data affect;

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_WITHER, level ) ) {
    send( ch, "The spell fails.\n\r" );
    return TRUE;
    }

  send( victim, "You feel your skin shrivel.\n\r" );
  send_seen( victim, "%s seems to shrivel before you.\n\r", victim );
 
  affect.type      = AFF_NONE;
  affect.location  = APPLY_CON;
  affect.modifier  = -1; 
  affect.duration  = level*3;
  affect.level     = level;
  affect.leech     = NULL;

  add_affect( victim, &affect );

  damage_magic( victim, ch, spell_damage( SPELL_WITHER, level ),
    "*A withering stare" );

  return TRUE;
}


bool spell_drain_life( char_data* ch, char_data* victim, void*,
  int level, int )
{
  affect_data affect;

  if( victim->shdata->race == RACE_UNDEAD ) {
    send( ch, "You cannot drain life from the undead.\n\r" );
    return TRUE;
    }

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_DRAIN_LIFE, level ) ) {
    send( ch, "The spell fails.\n\r" );
    return TRUE;
    }

  send( victim, "The chill of death momentarily touchs your soul.\n\r" );
  send_seen( victim, "%s shivers for a brief instant.\n\r", victim );
 
  affect.type      = AFF_NONE;
  affect.location  = APPLY_CON;
  affect.modifier  = -1; 
  affect.duration  = level*3;
  affect.level     = level;
  affect.leech     = NULL;

  add_affect( victim, &affect );

  return TRUE;
}


/*
 *   WEB SPELLS
 */


bool spell_web( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  affect_data* paf;

  if( null_caster( ch, SPELL_WEB ) )
    return TRUE;

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( is_set( victim->affected_by, AFF_ENTANGLED ) ) {
    for( int i = 0; i < ch->affected; i++ ) {
      paf = ch->affected[i];
      if( paf->type == AFF_ENTANGLED ) {
        paf->level += 1+number_range( 0, level/3 );   
        paf->duration = max( level/2, paf->duration+1 ); 
        break;
        }
      }
    send( "The web around you thickens.\n\r", victim );
    send( *victim->array, "The web trapping %s thickens.\n\r", victim );
    return TRUE;
    }
  else {
    send( victim, "A web begins to form around you.\n\r" );
    send( *victim->array, "A web begins to form around %s.\n\r", victim );

    if( makes_save( victim, ch, RES_DEXTERITY, SPELL_WEB, level )  
      &&  makes_save( victim, ch, RES_MAGIC, SPELL_WEB, level ) )  {
      send( victim, "Luckily you avoid becoming entangled.\n\r" );
      send( *victim->array, "%s skillfully avoids the web.\n\r", victim );
      return TRUE;
      }
    }

  spell_affect( ch, victim, level, duration, SPELL_WEB, AFF_ENTANGLED );
  set_delay( victim, 32 );

  return TRUE;
}


/*
 *   MIND SPELLS
 */


bool spell_confuse( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  if( makes_save( victim, ch, RES_MIND, SPELL_CONFUSE, level/2 ) ) 
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_CONFUSE, AFF_CONFUSED );

  return TRUE;
}


bool spell_hallucinate( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  if( makes_save( victim, ch, RES_MIND, SPELL_HALLUCINATE, level ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_HALLUCINATE,
    AFF_HALLUCINATE );

  return TRUE;
}


/*
 *   VISION SPELLS
 */


bool spell_sense_danger( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SENSE_DANGER,
    AFF_SENSE_DANGER );

  return TRUE;
}


bool spell_eagle_eye( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_EAGLE_EYE ) )
    return FALSE;

  room = ch->in_room;

  if( victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\n\r" );
    return TRUE;
    }

  if( victim->in_room->area->status != AREA_OPEN 
    || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( victim->shdata->level >= LEVEL_BUILDER
    && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\n\r" );
    return TRUE;
    }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\n\r" );
    return TRUE;
    }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\n\r" );
    return TRUE;
    }

  ch->in_room = victim->in_room;
  send( ch, "\n\r" );
  do_look( ch, "" );
  ch->in_room = room;

  return TRUE;
}


bool spell_scry( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_SCRY ) )
    return FALSE;

  room = ch->in_room;

  if( victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\n\r" );
    return TRUE;
    }

  if( victim->in_room->area->status != AREA_OPEN 
    || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( victim->shdata->level >= LEVEL_BUILDER
    && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\n\r" );
    return TRUE;
    }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\n\r" );
    return TRUE;
    }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\n\r" );
    return TRUE;
    }

  ch->in_room = victim->in_room;
  send( ch, "\n\r" );
  do_look( ch, "" );
  ch->in_room = room;

  return TRUE;
}



bool spell_hawks_view( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_HAWKS_VIEW ) )
    return FALSE;

  room = ch->in_room;

  if( victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\n\r" );
    return TRUE;
    }

  if( victim->in_room->area->status != AREA_OPEN 
    || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( victim->shdata->level >= LEVEL_BUILDER
    && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\n\r" );
    return TRUE;
    }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\n\r" );
    return TRUE;
    }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\n\r" );
    return TRUE;
    }

  ch->in_room = victim->in_room;
  send( ch, "\n\r" );
  do_look( ch, "" );
  ch->in_room = room;

  return TRUE;
}



/*
 *   INVULNERABILITY
 */


bool spell_invulnerability( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INVULNERABILITY,
    AFF_INVULNERABILITY );

  return TRUE;
}


/*
 *   LEVITATION/FLY
 */


bool spell_float( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FLOAT, AFF_FLOAT );

  return TRUE;
}


bool spell_fly( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FLY, AFF_FLY );

  return TRUE;
}


/*
 *  SLOW/HASTE
 */


bool spell_haste( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_HASTE, AFF_HASTE );

  return TRUE;
}


bool spell_slow( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  if( makes_save( victim, ch, RES_MAGIC, SPELL_SLOW, level ) ) 
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_SLOW, AFF_SLOW );

  return TRUE;
}










