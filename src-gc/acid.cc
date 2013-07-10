#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   ACID DAMAGE ROUTINES
 */


index_data acid_index [] = 
{
  { "blemishes",            "blemish",               3 },
  { "irritates",            "irritate",              7 },
  { "burns",                "burn",                 15 },
  { "erodes",               "erode",                30 },
  { "blisters",             "blister",              45 },
  { "MARS",                 "MAR",                  60 },
  { "WITHERS",              "WITHER",               90 },
  { "* CORRODES *",         "* CORRODE *",         120 },
  { "* SCARS *",            "* SCAR *",            150 }, 
  { "* DISFIGURES *",       "* DISFIGURE *",       190 }, 
  { "** MELTS **",          "** MELT **",          240 },
  { "** LIQUIFIES **",      "** LIQUIFY **",       290 },
  { "*** DISSOLVES ***",    "*** DISSOLVE ***",    350 },
  { "*** DELIQUESCES ***",  "*** DELIQUESCE ***",   -1 },
};


bool damage_acid( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  damage *= 100-victim->Save_Acid( );
  damage /= 100;

  dam_message( victim, ch, damage, string,
    lookup( acid_index, damage, plural ) );
  
  return inflict( victim, ch, damage, "acid" );
}


int obj_data :: vs_acid( )
{
  int save  = 100;
  int    i;

  for( i = 0; i < MAX_MATERIAL; i++ ) 
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_acid );

  if( pIndexData->item_type != ITEM_ARMOR 
    || pIndexData->item_type != ITEM_WEAPON ) 
    return save;

  return save+value[0]*(100-save)/(value[0]+2);
}


/* 
 *   ACID BASED SPELLS
 */


bool spell_resist_acid( char_data* ch, char_data* victim, void*,
  int level, int duration )
{

  spell_affect( ch, victim, level, duration, SPELL_RESIST_ACID,
    AFF_RESIST_ACID );
  
  return TRUE;
}


bool spell_acid_blast( char_data* ch, char_data* victim, void* vo,
  int level, int duration )
{
  obj_data*  obj  = (obj_data*) vo;
  int       save;

  /* Quaff */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain as the acid eats away at your\
 stomach and throat.  Luckily you don't feel it for long." );
    fsend_seen( victim, "%s grasps %s throat and spasms in pain - %s does\
 not survive long.", victim, victim->His_Her( ),
      victim->He_She( ) );
    death_message( victim );
    death( victim, NULL, "drinking acid" );
    return TRUE;
    }  

  /* Fill */

  if( ch == NULL ) {
    if( obj->metal( )
      || is_set( &obj->materials, MAT_STONE ) 
      || is_set( &obj->materials, MAT_GLASS ) )
      return FALSE;

    fsend( victim, "The acid bubbles and boils, eating its way through %s,\
 which you quickly drop and watch disappear into nothing.\n\r", obj );
    fsend( *victim->array, "%s quickly drops %s\
 as %s dissolved by the liquid.", victim, obj, 
      obj->selected > 1 ? "they are" : "it is" );

    obj->Extract( obj->selected );
    return TRUE;
    }

  /* Dip */

  if( duration == -3 ) {
    save = obj->vs_acid( );
    if( number_range( 0,99 ) > save ) {
      if( number_range( 0,99 ) > save ) {
        send( *ch->array, "%s is devoured.\n\r", obj );
        obj->Extract( 1 );
        return TRUE;
        }
      send( ch, "%s is partially destroyed.\n\r", obj );
      }
    if( obj->rust > 0 ) {
      send( ch, "%sthe %s on %s is removed.\n\r", 
        obj->rust-- > 1 ? "Some of " : "", "rust", obj );
      }
    return TRUE;
    }
  
  /* Throw-Cast */

  damage_acid( victim, ch, spell_damage( SPELL_ACID_BLAST, level ),
    "*the splatter of acid" );

  return TRUE; 
}


bool spell_acid_storm( char_data* ch, char_data* victim, void*,
  int level, int )
{
  damage_acid( victim, ch, spell_damage( SPELL_ACID_STORM, level ),
    "*the blast of acid" );

  return TRUE;
}
