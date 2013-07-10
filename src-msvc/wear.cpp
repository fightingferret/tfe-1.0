#include "define.h"
#include "struct.h"

// windows doesn't have gettimeofday, simulate
void gettimeofday(struct timeval *t, struct timezone *dummy)
{
  DWORD millisec = GetTickCount();

  t->tv_sec = (int) (millisec / 1000);
  t->tv_usec = (millisec % 1000) * 1000;
}

bool   remove           ( char_data*, obj_data*& );
void   wear_obj         ( char_data*, obj_data*, int );
void   wear_trigger     ( char_data*, obj_data* ); 
void   wield_obj        ( char_data*, obj_data* ); 
bool   can_remove       ( char_data*, obj_data*, bool = FALSE );


const char* wear_name [] = { "take", "finger", "neck", "body", "head",
  "legs", "feet", "hands", "arms", "unused0", "unused1", "waist", "wrist",
  "right.hand", "left.hand", "unused2", "unused3", "unused4", "unused5",
  "horse_body", "horse_back", "horse_foot" };

const char* curse_name [] = { "hand", "finger", "finger",
  "neck", "body", "head", "legs", "feet", "hand", "arms",
  "arm", "??", "waist", "wrist", "wrist", "hand", "hand", "??",
  "??", "??", "body", "back", "feet" };

const int wear_index [ MAX_WEAR ] = { ITEM_TAKE, ITEM_WEAR_FINGER,
  ITEM_WEAR_FINGER, ITEM_WEAR_NECK, ITEM_TAKE, ITEM_WEAR_BODY,
  ITEM_WEAR_HEAD, ITEM_WEAR_LEGS, ITEM_WEAR_FEET, ITEM_WEAR_HANDS,
  ITEM_WEAR_ARMS, ITEM_TAKE, ITEM_TAKE, ITEM_WEAR_WAIST,
  ITEM_WEAR_WRIST, ITEM_WEAR_WRIST, ITEM_HELD_R, ITEM_HELD_L,
  ITEM_TAKE, ITEM_TAKE, ITEM_TAKE, ITEM_TAKE,
  ITEM_WEAR_HORSE_BODY, ITEM_WEAR_HORSE_BACK, ITEM_WEAR_HORSE_FEET };
  
const char** wear_part_name = &reset_wear_name[2];

const char* wear_abbrev[] = { "Grnd", "Inv.", "Lght", "rFgr", "lFgr",
  "Neck", "??  ", "Body", "Head", "Legs", "Feet", "Hand", "Arms", "??  ",
  "??  ", "Wst.", "rWrs", "lWrs", "rHnd", "lHnd", "??  ", "??  ", "??  ",
  "??  ", "??  ", "??  ", "??  " };

const char* reset_wear_name[] = { "ground", "inventory",
  "floating", "right.finger", "left.finger",
  "neck", "unused1", "body", "head", "legs", "feet", "hands", "arms",
  "unused1", "unused2", "about.waist", "right.wrist", "left.wrist",
  "right.hand", "left.hand", "unused3", "unused4", "unused5", "unused6",
  "horse.body", "horse.back", "horse.foot" };


const char* wear_verb [] =
{ 
  "toss into the air",
  "place on your left hand",
  "place on your right hand",
  "place around your neck",
  "??",
  "wear",
  "place on your head",
  "cover your legs with", 
  "slip your feet into",
  "put your hands into",
  "cover your arms with",
  "??",
  "??",
  "place around your waist",
  "put on your right wrist",
  "put on your left wrist",
  "hold in your right hand",
  "hold in your left hand",
  "??",
  "??",
  "??",
  "??",
  "covering body", 
  "placed on back",
  "attached to feet",
};

 
const char* where_name [ MAX_WEAR ] =
{
  "floating nearby   ",
  "right hand finger ",
  "left hand finger  ",
  "worn around neck  ",
  "??                ",
  "worn on body      ",
  "worn on head      ",
  "worn on legs      ",
  "worn on feet      ",
  "worn on hands     ",
  "worn on arms      ",
  "??                ",
  "??                ",
  "worn about waist  ",
  "right wrist       ",
  "left wrist        ",
  "right hand        ",
  "left hand         ",
  "??                ",
  "??                ", 
  "??                ",
  "??                ",
  "covering body     ", 
  "placed on back    ",
  "attached to feet  ",
};


/*
 *   SUPPORT ROUTINES
 */ 

const char* wear_loc( obj_data* obj )
{
  if( obj->position == WEAR_HELD_R 
    || obj->position == WEAR_HELD_L ) {
    if( obj->pIndexData->item_type == ITEM_WEAPON )
      return "you are wielding";
    return "you are holding";
    }

  if( obj->position == WEAR_FLOATING )
    return "floating nearby";

  return "you are carrying";
}
  

obj_data* char_data :: Wearing( int slot, int layer )
{
  obj_data* obj;

  for( int j = 0; j < wearing; j++ ) {
    obj = (obj_data*) wearing[j];
    if( obj->position == slot 
      && ( layer == -1 || obj->layer == layer ) ) 
      return obj;
    }

  return NULL;
}


/*
 *   CAN_USE ROUTINES
 */


bool will_fit( char_data* ch, obj_data* obj, int i )
{
  if( !is_set( &obj->pIndexData->wear_flags, wear_index[i] ) ) 
    return FALSE;

  if( ch->species == NULL )
    return( wear_index[i] != ITEM_TAKE && i < MAX_WEAR_HUMANOID );

  return is_set( &ch->species->wear_part, i );
}


bool can_use( char_data* ch, obj_clss_data* obj_clss, obj_data* obj,
  bool custom )
{
  if( ch->species != NULL 
    && !is_set( &ch->species->act_flags, ACT_HUMANOID ) ) 
    return FALSE;

  /*  CUSTOM FIT  */

  if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM )
    && ( ch->pcdata == NULL || ( !custom
    && ch->pcdata->pfile->ident != obj->size_flags ) ) ) 
    return FALSE;

  /*  SIZE */

  if( is_set( &obj_clss->size_flags, SFLAG_RACE )
    && ch->shdata->race < MAX_PLYR_RACE 
    && !is_set( &obj_clss->size_flags, SFLAG_HUMAN+ch->shdata->race ) ) 
    return FALSE;

  if( is_set( &obj_clss->size_flags, SFLAG_SIZE )
    || ( obj != NULL && is_set( &obj_clss->size_flags, SFLAG_RANDOM ) ) ) 
    if( !is_set( obj == NULL ? &obj_clss->size_flags
      : &obj->size_flags, wear_size( ch ) ) ) 
      return FALSE;
 
  /* STOLEN */

  if( ch->shdata->alignment != ALGN_CHAOTIC_EVIL && obj != NULL
    && !obj->Belongs( ch ) ) 
    return FALSE;

  /*  CLASS RESTRICTIONS */

  if( ch->shdata->level < obj_clss->level ) 
    return FALSE;

  if( ch->shdata->level < LEVEL_BUILDER && ( ch->shdata->race < MAX_PLYR_RACE
    && is_set( &obj_clss->anti_flags, ANTI_HUMAN+ch->shdata->race ) )
    || ( ch->species == NULL 
    && is_set( &obj_clss->anti_flags, ANTI_MAGE+ch->pcdata->clss ) )
    || is_set( &obj_clss->anti_flags, ANTI_GOOD+ch->shdata->alignment%3 )
    || is_set( &obj_clss->anti_flags,
    ANTI_LAWFUL+ch->shdata->alignment/3 ) ) 
    return FALSE; 

  return TRUE;
}


/*
 *   WEAR ROUTINES
 */ 


thing_data* cantwear( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;
  int i;

  for( i = 0; !is_set( &obj->pIndexData->layer_flags, i ); )
    if( ++i == MAX_LAYER )
      return NULL; 
 
  for( i = 1; i < MAX_WEAR; i++ ) 
    if( will_fit( ch, obj, i ) )
      return obj;

  return NULL;
}


thing_data* bladed( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_CLERIC 
    && is_set( &obj->pIndexData->restrictions, RESTR_BLADED ) ) 
    return NULL;

  return obj;
}


thing_data* dishonorable( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_PALADIN 
    && is_set( &obj->pIndexData->restrictions, RESTR_DISHONORABLE ) ) 
    return NULL;

  return obj;
}


thing_data* levellimit( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj  = (obj_data*) t1;

  if( ch->shdata->level < obj->pIndexData->level ) 
    return NULL;

  return obj;
}


thing_data* antiobj( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data*            obj  = (obj_data*) t1;
  obj_clss_data*  obj_clss  = obj->pIndexData;

  if( ch->shdata->level < LEVEL_BUILDER && ( ch->shdata->race < MAX_PLYR_RACE
    && is_set( &obj_clss->anti_flags, ANTI_HUMAN+ch->shdata->race ) )
    || ( ch->species == NULL 
    && is_set( &obj_clss->anti_flags, ANTI_MAGE+ch->pcdata->clss ) )
    || is_set( &obj_clss->anti_flags, ANTI_GOOD+ch->shdata->alignment%3 )
    || is_set( &obj_clss->anti_flags,
    ANTI_LAWFUL+ch->shdata->alignment/3 ) ) 
    return NULL; 

  return obj;
}


thing_data* needremove( thing_data* t1, char_data* ch,
  thing_data* t2 )
{
  obj_data*       obj  = (obj_data*) t1;
  thing_array*  array  = (thing_array*) t2;
  obj_data*      worn;
  int               j;

  for( int i = 1; i < MAX_WEAR; i++ ) 
    if( will_fit( ch, obj, i ) ) {
      for( j = MAX_LAYER-1; j >= 0 && !ch->Wearing( i, j ); j-- );
      for( j++; j < MAX_LAYER; j++ )  
        if( is_set( &obj->pIndexData->layer_flags, j ) ) {
          worn = (obj_data*) obj->From( 1 );
          worn->position = i;
          worn->layer    = j;
          worn->To( &ch->wearing );
          if( obj->selected == 1 ) 
            return obj;
          obj->selected--;
          if( array != NULL )
            *array += worn;
          }
      }

  return NULL;  
}


void do_wear( char_data* ch, char* argument )
{
  thing_array* array;

  if( is_confused_pet( ch ) )
    return; 

  if( ( array = several_things( ch, argument, "wear",
    &ch->contents ) ) == NULL )
    return;

  thing_array   subset  [ 8 ];
  thing_func*     func  [ 8 ]  = { cantwear, stolen, bladed,
                                   dishonorable, levellimit,
                                   antiobj, needremove, NULL };

  sort_objects( ch, *array, (thing_data*) &subset[7], 8, subset, func );

  msg_type = MSG_EQUIP;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't wear" );
  page_priv( ch, &subset[1], NULL, NULL,
    "is stolen", "are stolen" );
  page_priv( ch, &subset[2], NULL, NULL,
    "is a bladed weapon and forbidden to you",
    "are bladed weapons and forbidden to you" ); 
  page_priv( ch, &subset[3], "you consider", NULL, "dishonorable" );
  page_priv( ch, &subset[4], "can't cope with" );
  page_priv( ch, &subset[5], "sense adversion from" );
  page_priv( ch, &subset[6], "need to remove something to equip" );
  list_wear( ch, &subset[7] );

  delete array;
}


void equip( char_data* ch, obj_data* obj )
{
  obj_data*    worn;
  affect_data    af;
  int i;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_ADDITIVE ) )
    for( i = 0; i < ch->wearing; i++ )
      if( ( worn = object( ch->wearing[i] ) ) != NULL
        && worn->pIndexData == obj->pIndexData 
        && worn != obj )
        return;

  for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ )
    if( is_set( obj->pIndexData->affect_flags, i ) ) {
      af.type = i;
      modify_affect( ch, &af, TRUE );
      }

  for( i = 0; i < obj->pIndexData->affected; i++ )
    modify_affect( ch, obj->pIndexData->affected[i], TRUE );
}


/*
 *   REMOVE ROUTINES
 */


thing_data* noremove( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  return( is_set( obj->extra_flags, OFLAG_NOREMOVE )
    ? NULL : obj );
}


thing_data* nocatch( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  return( obj->position == WEAR_FLOATING ? NULL : obj );
}


thing_data* remove( thing_data* thing, char_data* ch, thing_data* )
{
  thing = thing->From( 1 );
  thing->To( ch );

  return thing;
}


void do_remove( char_data* ch, char* argument )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  thing_array*  array;

  if( is_confused_pet( ch ) ) 
    return;

  if( contains_word( argument, "from", arg ) ) {
    return;
    }

  if( ( array = several_things( ch, argument, "remove", 
    &ch->wearing ) ) == NULL )
    return;

  thing_array   subset  [ 3 ];
  thing_func*     func  [ 3 ]  = { noremove, nocatch, remove };

  sort_objects( ch, *array, NULL, 3, subset, func );

  msg_type = MSG_EQUIP;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't remove" );
  page_priv( ch, &subset[1], "can't catch" );
  page_publ( ch, &subset[2], "remove" );

  consolidate( subset[2] );

  delete array;
}


void unequip( char_data* ch, obj_data* obj )
{
  obj_data*    worn;
  affect_data    af;
  int i;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_ADDITIVE ) )
    for( i = 0; i < ch->wearing; i++ )
      if( ( worn = object( ch->wearing[i] ) ) != NULL
        && worn->pIndexData == obj->pIndexData 
        && worn != obj )
        return;

  for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ )
    if( is_set( obj->pIndexData->affect_flags, i ) ) {
      af.type = i;
      modify_affect( ch, &af, FALSE  );
      }

  for( i = 0; i < obj->pIndexData->affected; i++ )
    modify_affect( ch, obj->pIndexData->affected[i], FALSE );
}








