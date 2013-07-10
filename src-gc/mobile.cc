#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   CAN_EXTRACT ROUTINE
 */
 

bool can_extract( species_data* species, char_data* ch )
{
  for( int i = 0; i < mob_list; i++ )
    if( mob_list[i]->species == species ) {
      send( ch, "There are still some creatures of that species alive.\n\r" );
      return FALSE;
      }
 
  return TRUE;
}


/*
 *   CREATION ROUTINES
 */


mob_data* create_mobile( species_data* species )
{
  mob_data*       mob;
  obj_data*       obj;
  int               i;
  int           coins;
  int            type;

  if( species == NULL ) 
    panic( "Create_mobile: NULL species." );

  mob          = new mob_data;
  mob->shdata  = species->shdata;
  mob->descr   = species->descr;
  mob->species = species;

  mob->affected_by[0] = species->affected_by[0];
  mob->affected_by[1] = species->affected_by[1];

  mob->base_hit  = max(1,roll_dice( species->hitdice ));
  mob->base_move = max(0,roll_dice( species->movedice ));
  mob->base_mana = 100;

  if( ( mob->sex = species->sex ) == SEX_RANDOM ) 
    mob->sex = ( number_range( 0, 1 ) == 0 ? SEX_MALE : SEX_FEMALE ); 

  update_max_hit( mob );
  update_max_mana( mob );

  mob->hit   = mob->max_hit;
  mob->mana  = mob->max_mana;

  update_max_move( mob );

  mob->move     = mob->max_move;
  mob->position = POS_STANDING;

  coins = species->gold;
  for( type = MAX_COIN - 1; type >= 0; type-- ) {
    if( ( i = number_range( 0, coins/coin_value[type] ) ) > 0 ) {
      obj = create( get_obj_index( coin_vnum[type] ), i );
      obj->To( mob );
      coins -= i*coin_value[type];
      }
    }

  assign_bit( &mob->status, STAT_SENTINEL,
    is_set( &species->act_flags, ACT_SENTINEL ) );
  assign_bit( &mob->status, STAT_AGGR_ALL,
    is_set( &species->act_flags, ACT_AGGR_ALL ) );
  assign_bit( &mob->status, STAT_AGGR_GOOD,
    is_set( &species->act_flags, ACT_AGGR_GOOD ) );
  assign_bit( &mob->status, STAT_AGGR_EVIL,
    is_set( &species->act_flags, ACT_AGGR_EVIL ) );

  mob->reset  = NULL;  
  mob->number = 1;

  return mob;
}


/*
 *   LOAD SPECIES
 */


void fix_species( )
{
  species_data*  species;

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL )
      continue;
    if( species->group < 0 || species->group >= MAX_ENTRY_GROUP ) {
      roach( "Fix_Species: Non-existent group." );
      roach( "-- Species = %s", species->Name( ) );
      species->group = 0;
      } 
    if( species->nation < 0 || species->nation >= MAX_ENTRY_NATION ) {
      roach( "Fix_Species: Non-existent nation." );
      roach( "-- Species = %s", species->Name( ) );
      species->nation = 0;
      }
    if( species->shdata->race < 0
      || species->shdata->race >= MAX_ENTRY_RACE ) {
      roach( "Fix_Species: Non-existant race." );
      roach( "-- Species = %s", species->Name( ) );
      species->shdata->race = 0;
      }
    }

  return;
}


void load_mobiles( void )
{
  descr_data*      descr;
  FILE*               fp;
  mprog_data*      mprog  = NULL;
  species_data*  species;
  share_data*     shdata;

  echo( "Loading Mobs ...\n\r" );
 
  fp = open_file( AREA_DIR, MOB_FILE, "r", TRUE ); 

  if( strcmp( fread_word( fp ), "#MOBILES" ) ) 
    panic( "Load_mobiles: missing header" );

  for( ; ; ) {
    int vnum;
    char letter;

    if( ( letter = fread_letter( fp ) ) != '#' ) 
      panic( "Load_mobiles: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
       break;

    if( ( species = get_species( vnum ) ) != NULL ) {
      bug( "Load_mobiles: vnum %d duplicated.", vnum );
      bug( fread_string( fp, MEM_UNKNOWN ) );
      bug( species->descr->name );
      exit( 1 );
      }

    species         = new species_data;
    species->shdata = new share_data;
    species->descr  = new descr_data;

    shdata = species->shdata;
    descr  = species->descr;

    species->vnum             = vnum;

    descr->name          = fread_string( fp, MEM_DESCR );
    descr->keywords      = fread_string( fp, MEM_DESCR );
    descr->singular      = fread_string( fp, MEM_DESCR );
    descr->prefix_s      = fread_string( fp, MEM_DESCR );
    descr->adj_s         = fread_string( fp, MEM_DESCR );
    descr->long_s        = fread_string( fp, MEM_DESCR );
    descr->plural        = fread_string( fp, MEM_DESCR );
    descr->prefix_p      = fread_string( fp, MEM_DESCR );
    descr->adj_p         = fread_string( fp, MEM_DESCR );
    descr->long_p        = fread_string( fp, MEM_DESCR );
    descr->complete      = fread_string( fp, MEM_DESCR );

    species->creator       = fread_string( fp, MEM_SPECIES ); 
    species->attack        = new program_data;
    species->attack->code  = fread_string( fp, MEM_MPROG );

    read_extra( fp, species->attack->data );

    species->nation      = fread_number( fp );
    species->group       = fread_number( fp );
    shdata->race         = fread_number( fp );
    species->adult       = fread_number( fp );
    species->maturity    = fread_number( fp );
    species->skeleton    = fread_number( fp );
    species->zombie      = fread_number( fp );
    species->corpse      = fread_number( fp );
    species->price       = fread_number( fp );
    shdata->kills        = fread_number( fp );
    shdata->deaths       = fread_number( fp );
    species->wander      = fread_number( fp );
    species->date        = fread_number( fp );
    species->light       = fread_number( fp );
    species->color       = fread_number( fp );

    species->act_flags      = fread_number( fp ) | ( 1 << ACT_IS_NPC );
    species->affected_by[0] = fread_number( fp );
    species->affected_by[1] = fread_number( fp );
    species->affected_by[2] = fread_number( fp );

    shdata->alignment     = fread_number( fp );
    shdata->level         = fread_number( fp );
    shdata->strength      = fread_number( fp );
    shdata->intelligence  = fread_number( fp );
    shdata->wisdom        = fread_number( fp );
    shdata->dexterity     = fread_number( fp );
    shdata->constitution  = fread_number( fp );

    shdata->resist[RES_MAGIC]    = fread_number( fp ); 
    shdata->resist[RES_FIRE]     = fread_number( fp ); 
    shdata->resist[RES_COLD]     = fread_number( fp ); 
    shdata->resist[RES_SHOCK]    = fread_number( fp ); 
    shdata->resist[RES_MIND]     = fread_number( fp ); 
    shdata->resist[RES_ACID]     = fread_number( fp );
    shdata->resist[RES_POISON]   = fread_number( fp );
 
    for( int i = 0; i < MAX_ARMOR; i++ ) {
      species->chance[i] = fread_number( fp );
      species->armor[i] = fread_number( fp );
      species->part_name[i] = fread_string( fp, MEM_SPECIES );
      }

    species->wear_part = fread_number( fp );
    species->hitdice   = fread_number( fp );
    species->movedice  = fread_number( fp );

    species->damage       = fread_number( fp );
    species->rounds       = fread_number( fp );
    species->special      = fread_number( fp );
    species->damage_taken = fread_number( fp );
    species->exp          = fread_number( fp );

    switch( fread_letter( fp ) ) {
      case 'M' : species->sex = SEX_MALE;       break;
      case 'F' : species->sex = SEX_FEMALE;     break;
      case 'R' : species->sex = SEX_RANDOM;     break;
      default  : species->sex = SEX_NEUTRAL;    break;
      }

    species->gold   = fread_number( fp );
    species->size   = fread_number( fp );
    species->weight = fread_number( fp );

    load( fp, species->reset );
  
    for( ; ; ) {
      int number = fread_number( fp );

      if( number == -1 )
        break;

      mprog = new mprog_data;
      append( species->mprog, mprog );

      mprog->next    = NULL;
      mprog->trigger = number;
      mprog->value   = fread_number( fp );
      mprog->string  = fread_string( fp, MEM_MPROG );
      mprog->code    = fread_string( fp, MEM_MPROG );
      mprog->species = species;

      read_extra( fp, mprog->data );
      }       


    species_list[vnum] = species;
    }

  fclose( fp );
}
















