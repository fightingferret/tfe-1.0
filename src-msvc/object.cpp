#include "define.h"
#include "struct.h"


/*
 *   LOCAL_CONSTANTS
 */


const char* item_type_name [] = { "other", "light", "scroll", "wand",
  "staff", "weapon", "gem", "spellbook", "treasure", "armor", "potion",
  "reagent", "furniture", "trash", "cross", "container", "lock pick", 
  "drink_container", "key", "food", "money", "key_ring", "boat",
  "corpse", "unused", "fountain", "whistle", "trap",
  "light_perm", "bandage", "bounty", "gate", "arrow", "skin",
  "body_part", "chair", "table", "book", "pipe", "tobacco", "deck_cards",
  "fire", "garrote" };

const char* item_values [] = { "unused",
  "unused | unused | life time | unused",
  "spell | level | duration | unused",
  "spell | level | duration | charges",
  "??",
  "enchantment | damdice | damside | weapon class",
  "hardness | condition",
  "??", "unused",
  "enchantment | armor value | bracer ac | unused",
  "spell | level | duration | unused",
  "charges | unused",
  "??", "??", "??",
  "capacity | container flags | key vnum | unused", 
  "pick modifier | unused",
  "capacity | contains | liquid | poisoned == -1", "??", 
  "food value | cooked? | unused | poisoned == -1",
  "??", "??", "??", "??", "??",
  "required to be -1 | unused | liquid | poisoned == -1",
  "range | unused",
  "trap flags | damdice | damside | unused",
  "unused | unused | lifetime | unused", "unused",
  "unused", "lifetime | unused", "unused | damdice | damside | unused",
  "unused", "unused",
  "seats | unused", "unused", "unused",
  "unused", "unused", "unused",
  "timer | next item | unused" };

const char *cont_flag_name [] = { "closeable", "pickproof", "closed",
  "locked", "holding" };


/*
 *   OBJ_CLSS_DATA CLASS
 */


Obj_Clss_Data :: Obj_Clss_Data( )
{
  record_new( sizeof( obj_clss_data ), MEM_OBJ_CLSS );

  oprog        = NULL;
  date         = -1;
  count        = 0;
}


Obj_Clss_Data :: ~Obj_Clss_Data( )
{
  record_delete( sizeof( obj_clss_data ), MEM_OBJ_CLSS );
}


/*
 *   OBJ_DATA
 */


int compare_vnum( obj_data* obj1, obj_data* obj2 )
{
  int a = obj1->pIndexData->vnum;
  int b = obj2->pIndexData->vnum;

  return( a < b ? -1 : ( a > b ? 1 : 0 ) );
}


Obj_Data :: Obj_Data( obj_clss_data* obj_clss )
{
  record_new( sizeof( obj_data ), MEM_OBJECT );

  valid         = OBJ_DATA;
  extra_descr   = NULL;
  array         = NULL;
  save          = NULL;
  owner         = NULL;
  source        = empty_string;
  label         = empty_string;
  position      = WEAR_NONE;
  layer         = 0;
  pIndexData    = obj_clss;

  insert( obj_list, this, 
    binary_search( obj_list, this, compare_vnum ) );
}


Obj_Data :: ~Obj_Data( )
{
  record_delete( sizeof( obj_data ), MEM_OBJECT );
  obj_list -= this;
}


/*
 *   SUPPORT FUNCTIONS
 */


bool can_extract( obj_clss_data* obj_clss, char_data* ch )
{
  custom_data*  custom;
  shop_data*      shop;
 
  for( int i = 0; i < obj_list; i++ ) 
    if( obj_list[i]->pIndexData == obj_clss ) {
      send( ch, "You must destroy all examples of %s first.\r\n",
        obj_clss );
      return FALSE;
      }

  if( has_reset( obj_clss ) ) { 
    send( ch, "You must remove all resets of that object.\r\n" );
    return FALSE;
    }

  for( shop = shop_list; shop != NULL; shop = shop->next ) 
    for( custom = shop->custom; custom != NULL; custom = custom->next ) {
      if( custom->item == obj_clss ) {
        send( ch, "A custom in room %d creates that item.\r\n",
          shop->room->vnum );
        return FALSE;
        }
      for( int i = 0; i < MAX_INGRED; i++ ) 
        if( custom->ingred[i] == obj_clss ) {
          send( ch,
            "A custom in room %d requires that item as an ingrediant.\r\n",
            shop->room->vnum );
          return FALSE;
	  }
      }

  return TRUE;
}        
  

/*
 *   LOW LEVEL OBJECT ROUTINES
 */


bool is_same( obj_data* obj1, obj_data* obj2 )
{
  obj_array*  array;

  if( obj1->pIndexData != obj2->pIndexData )
    return FALSE;

  if(  !is_empty( obj1->contents ) || !is_empty( obj2->contents )
    || !is_empty( obj1->affected ) || !is_empty( obj2->affected )
    || !is_empty( obj1->events )   || !is_empty( obj2->events )
    || obj1->extra_flags[0] != obj2->extra_flags[0] 
    || obj1->extra_flags[1] != obj2->extra_flags[1] 
    || obj1->timer != obj2->timer
    || obj1->condition != obj2->condition 
    || obj1->materials != obj2->materials
    || obj1->rust != obj2->rust
    || obj1->owner != obj2->owner
    || obj1->weight != obj2->weight )
    return FALSE;

  for( int i = 0; i < 4; i++ )
    if( obj1->value[i] != obj2->value[i] )
      return FALSE;

  if( obj2->save != NULL ) {
    if( obj1->save == NULL ) {
      obj1->save = obj2->save;
      obj2->save = NULL;
      array      = &obj1->save->save_list;
      for( int i = 0; ; i++ ) {
        if( i >= array->size ) 
          panic( "Is_Same: Object not in save array." );       
        if( obj2 == array->list[i] ) {
          array->list[i] = obj1;
          break;
	  }
        }
      }
    else if( obj1->save != obj2->save )
      return FALSE;
    }

  return( !strcmp( obj1->label, obj2->label ) );
}


/*
 *   OWNERSHIP
 */


bool Obj_Data :: Belongs( char_data* ch )
{
  return( owner == NULL || ( ch != NULL && ch->pcdata != NULL
    && ch->pcdata->pfile == owner ) );
}


void set_owner( pfile_data* pfile, thing_array& array )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ ) {
    if( ( obj = object( array[i] ) ) != NULL ) {
      if( obj->owner == NULL
        && obj->pIndexData->item_type != ITEM_MONEY )
        obj->owner = pfile; 
      set_owner( pfile, obj->contents );
      }
    }
}


void set_owner( obj_data* obj, pfile_data* buyer )
{
  obj_data* content;

  if( obj->pIndexData->item_type != ITEM_MONEY )
    obj->owner = buyer;

  for( int i = 0; i < obj->contents; i++ )
    if( ( content = object( obj->contents[i] ) ) != NULL )
      set_owner( content, buyer );
}

  
void set_owner( obj_data* obj, char_data* buyer, char_data* seller )
{
  obj_data* content;

  if( obj->Belongs( seller ) && obj->pIndexData->item_type != ITEM_MONEY )
    obj->owner = ( ( buyer != NULL && buyer->pcdata != NULL )
      ? buyer->pcdata->pfile : NULL );

  for( int i = 0; i < obj->contents; i++ )
    if( ( content = object( obj->contents[i] ) ) != NULL ) 
      set_owner( content, buyer, seller );
}


/*
 *   MISC ROUTINES
 */


void condition_abbrev( char* tmp, obj_data* obj, char_data* ch )
{
  const char* abbrev [] = { "wls", "dmg", "vwn", "wrn", "vsc", "scr",
    "rea", "goo", "vgo", "exc" };

  int i;

  i = 1000*obj->condition/obj->pIndexData->durability; 
  i = range( 0, i/100, 9 );

  sprintf( tmp, "%s%s%s", i > 4 ? ( i > 7 ? blue( ch ) : green( ch ) ) 
    : ( i > 2 ? yellow( ch ) : red( ch ) ), abbrev[i], normal( ch ) );

  return;
}


void age_abbrev( char* tmp, obj_data*, char_data* )
{
  sprintf( tmp, "   " );

  return;
}


const char* obj_data :: condition_name( char_data* ch, bool ansi )
{
  static char  tmp  [ ONE_LINE ];
  int            i;
  const char*  txt;

  i = 1000*condition/pIndexData->durability; 
  i = range( 0, i/100, 9 );

       if( i == 0 ) txt = "worthless";
  else if( i == 1 ) txt = "damaged";
  else if( i == 2 ) txt = "very worn";
  else if( i == 3 ) txt = "worn";
  else if( i == 4 ) txt = "very scratched";
  else if( i == 5 ) txt = "scratched";
  else if( i == 6 ) txt = "reasonable";
  else if( i == 7 ) txt = "good";
  else if( i == 8 ) txt = "very good";
  else              txt = "excellent";

  if( !ansi || ch->pcdata == NULL || ch->pcdata->terminal != TERM_ANSI )
    return txt;

  sprintf( tmp, "%s%s%s", i > 4 ? ( i > 7 ? blue( ch ) : green( ch ) ) 
    : ( i > 2 ? yellow( ch ) : red( ch ) ), txt, normal( ch ) );

  return tmp;
}


/*
 *   WEIGHT/NUMBER ROUTINES
 */


int Obj_Data :: Cost( )
{
  int   cost  = pIndexData->cost;
  int      i;

  if( ( pIndexData->item_type == ITEM_WEAPON
    || pIndexData->item_type == ITEM_ARMOR )
    && is_set( extra_flags, OFLAG_IDENTIFIED ) ) {
    if( value[0] < 0 )
      return 0;
    cost += cost*sqr( value[0] )/2;
    }

  if( ( pIndexData->item_type != ITEM_WEAPON
    && pIndexData->item_type != ITEM_ARMOR )
    || !is_set( pIndexData->extra_flags, OFLAG_RANDOM_METAL ) )
    return cost;

  for( i = MAT_BRONZE; i <= MAT_ADAMANTINE; i++ )
    if( is_set( &pIndexData->materials, i ) ) 
      return cost*material_table[i].cost;

  return cost;
}


/*
 *   OBJECT UTILITY ROUTINES
 */


void enchant_object( obj_data* obj )
{
  int i;

  if( obj->pIndexData->item_type == ITEM_WAND
    || obj->pIndexData->item_type == ITEM_STAFF ) 
    obj->value[3] = number_range( 0, obj->pIndexData->value[3] );

  if( ( obj->pIndexData->item_type == ITEM_WEAPON
    || obj->pIndexData->item_type == ITEM_ARMOR )
    && !is_set( obj->extra_flags, OFLAG_NO_ENCHANT ) ) {
    if( ( i = number_range( 0, 1000 ) ) >= 900 ) {
      obj->value[0] = ( i > 950 ? ( i > 990 ? ( i == 1000 ? 3 : 2 ) : 1 )
        : ( i < 910 ? ( i == 900 ? -3 : -2 ) : -1 ) );
      set_bit( obj->extra_flags, OFLAG_MAGIC );
      if( obj->value[0] < 0 )
        set_bit( obj->extra_flags, OFLAG_NOREMOVE );
      }
    }
}


void rust_object( obj_data* obj, int chance )
{
  int i;

  if( obj->metal( )
    && !is_set( obj->extra_flags, OFLAG_RUST_PROOF )
    && number_range( 0, 100 ) < chance ) {
    i = number_range( 0, 1000 );
    obj->rust = ( i > 700 ? 1 : ( i > 400 ? 2 : 3 ) );   
    }

  if( obj->pIndexData->item_type == ITEM_WEAPON
    || obj->pIndexData->item_type == ITEM_ARMOR ) {
    obj->age = number_range( 0, obj->pIndexData->durability/25-1 );
    obj->condition = number_range( 1, repair_condition( obj ) );
    }
}


void set_alloy( obj_data* obj, int level )
{
  int metal;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_RANDOM_METAL ) )
    return;

  for( metal = MAT_BRONZE; metal <= MAT_ADAMANTINE; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_ADAMANTINE ) {
    metal = MAT_BRONZE;
    for( ; ; ) {
      if( metal == MAT_ADAMANTINE
        || number_range( 0, level+75 ) > level-10*(metal-MAT_BRONZE) )
        break;
      metal++;
      }
    set_bit( &obj->materials, metal );
    }

  if( obj->pIndexData->item_type == ITEM_ARMOR )
    obj->value[1] = obj->pIndexData->value[1]-MAT_BRONZE+metal;
}


obj_data* create( obj_clss_data* obj_clss, int number )
{
  obj_data* obj;

  if( obj_clss == NULL ) {
    roach( "Create_object: NULL obj_clss." );
    return NULL;
    }

  obj = new obj_data( obj_clss );

  obj->singular = obj_clss->singular;
  obj->plural   = obj_clss->plural;
  obj->after    = obj_clss->after;
  obj->before   = obj_clss->before;

  obj->extra_flags[0] = obj_clss->extra_flags[0];
  obj->extra_flags[1] = obj_clss->extra_flags[1];
  
  if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) )
    obj->size_flags  = -1;
  else
    obj->size_flags = obj_clss->size_flags;

  obj->value[0]   = obj_clss->value[0];
  obj->value[1]   = obj_clss->value[1];
  obj->value[2]   = obj_clss->value[2];
  obj->value[3]   = obj_clss->value[3];
  obj->weight     = obj_clss->weight;
  obj->condition  = obj_clss->durability;
  obj->materials  = obj_clss->materials;
  obj->age   = 0; 
  obj->rust       = 0;
  obj->timer      = 0; 

  if( number > 0 )
    obj_clss->count += number;
  else
    number = -number;

  obj->number     = number;
  obj->selected   = number;

  switch( obj_clss->item_type ) {
    case ITEM_MONEY:
      obj->value[0]  = obj->pIndexData->cost;
      break;
    case ITEM_FIRE:
    case ITEM_GATE:
      obj->timer = obj->value[0];
      break;
    }

  if( obj_clss->item_type != ITEM_ARMOR && obj_clss->item_type != ITEM_WEAPON
    && !strcmp( obj_clss->before, obj_clss->after )
    && obj_clss->plural[0] != '{' && obj_clss->singular[0] != '{' )
    set_bit( obj->extra_flags, OFLAG_IDENTIFIED ); 

  return obj;
}


obj_data* duplicate( obj_data* copy, int num )
{
  obj_data*            obj;
  obj_clss_data*  obj_clss  = copy->pIndexData;

  obj = new obj_data( obj_clss );

  char* string_copy [] = { copy->singular, copy->plural,
    copy->before, copy->after }; 
  char** string_obj [] = { &obj->singular, &obj->plural,
    &obj->before, &obj->after }; 
  char* string_index [] = { obj_clss->singular,
    obj_clss->plural, obj_clss->before, obj_clss->after }; 

  for( int i = 0; i < 4; i++ ) 
    *string_obj[i] = ( string_copy[i] != string_index[i] 
      ? alloc_string( string_copy[i], MEM_OBJECT ) : string_index[i] );
  
  obj->age            = copy->age;
  obj->extra_flags[0] = copy->extra_flags[0];
  obj->extra_flags[1] = copy->extra_flags[1];
  obj->size_flags     = copy->size_flags;
  obj->value[0]       = copy->value[0];
  obj->value[1]       = copy->value[1];
  obj->value[2]       = copy->value[2];
  obj->value[3]       = copy->value[3];
  obj->weight         = copy->weight;
  obj->condition      = copy->condition;
  obj->rust           = copy->rust;
  obj->timer          = copy->timer; 
  obj->materials      = copy->materials;
  obj->owner          = copy->owner;

  obj->temp     = copy->temp;
  obj->number   = num;   
  obj->selected = num;   

  obj_clss->count += num;

  if( copy->save != NULL ) {
    copy->save->save_list += obj;
    obj->save = copy->save;
    }

  return obj;
}


/*
 *   OBJECT TRANSFER FUNCTIONS
 */


void drop_contents( obj_data* obj )
{
  /*
  int               i;
  list_data*     list;
  room_data*     room;
  obj_data*   content;

  if( obj->array == NULL
    || ( room = obj->array->room( ) ) == NULL )
    return;

  list = content_list( obj );

  for( i = 0; i < list->length; i++ ) {
    content = (obj_data*) list->pntr[i];
    if( content->timer == -2 || content->where != obj
      || obj->array != room )
      continue;
    remove( content, content->number );
    put_obj( content, room );
    }

  delete list;
  */
  return;
}


/*
 *   OBJECT EXTRACTION ROUTINES
 */


void Obj_Data :: Extract( int i )
{
  if( i < number ) {
    remove_weight( this, i );
    number -= i;
    if( boot_stage == 2 )
      pIndexData->count -= i;
    return;
    }

  if( i > number ) {
    roach( "Extract( Obj ): number > amount." );
    roach( "-- Obj = %s", this );
    roach( "-- Number = %d", i ); 
    roach( "-- Amount = %d", number );
    }

  Extract( );
}


void Obj_Data :: Extract( )
{
  obj_array*    array;
  int               i;

  if( !Is_Valid( ) ) {
    roach( "Extracting invalid object." );
    roach( "-- Valid = %d", valid );
    roach( "--   Obj = %s", this );
    return;
    }

  if( this->array != NULL ) 
    From( number );

  extract( contents );

  if( boot_stage == 2 )
    pIndexData->count -= number;

  clear_queue( this );
  stop_events( this );

  if( save != NULL ) {
    array = &save->save_list;
    for( i = 0; ; i++ ) {
      if( i >= array->size ) 
        panic( "Extract: Object not found in save array." );
      if( this == array->list[i] ) {
        array->list[i] = NULL;
        save = NULL;
        break;
        }
      }
    }

  delete_list( affected );
  free_string( source, MEM_OBJECT );

  if( singular != pIndexData->singular )
    free_string( singular, MEM_OBJECT );
  if( plural != pIndexData->plural )
    free_string( plural,   MEM_OBJECT );
  if( after != pIndexData->after )
    free_string( after,    MEM_OBJECT );
  if( before != pIndexData->before )
    free_string( before,   MEM_OBJECT );

  timer      = -2;
  valid      = -1;

  extracted += this;
}


/*
 *   DISK ROUTINES
 */


void fix( obj_clss_data* obj_clss )
{
  if( obj_clss->item_type == ITEM_SCROLL ) 
    set_bit( &obj_clss->materials, MAT_PAPER );

  for( int i = 0; i < MAX_ANTI; i++ )
    if( !strncasecmp( anti_flags[i], "unused", 6 ) )
      remove_bit( &obj_clss->anti_flags, i );

  return;
}


void load_objects( void )
{
  FILE*                 fp;
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  char              letter;
  int                    i;
  int                 vnum;

  echo( "Loading Objects ...\r\n" );
  vzero( obj_index_list, MAX_OBJ_INDEX );

  fp = open_file( AREA_DIR, OBJECT_FILE, "r", TRUE );

  if( strcmp( fread_word( fp ), "#OBJECTS" ) ) 
    panic( "Load_objects: header not found" );

  for( ; ; ) {
    letter = fread_letter( fp );

    if( letter != '#' ) 
      panic( "Load_objects: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
      break;
   
    if( vnum < 0 || vnum >= MAX_OBJ_INDEX ) 
      panic( "Load_objects: vnum out of range." );

    if( obj_index_list[vnum] != NULL ) 
      panic( "Load_objects: vnum %d duplicated.", vnum );

    obj_clss = new obj_clss_data;
 
    obj_index_list[vnum]  = obj_clss;
    obj_clss->vnum        = vnum;
    obj_clss->fakes       = vnum;

    obj_clss->singular         = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->plural           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->before           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->after            = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->long_s           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->long_p           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->prefix_singular  = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->prefix_plural    = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->creator          = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->last_mod         = fread_string( fp, MEM_OBJ_CLSS );      

    obj_clss->item_type       = fread_number( fp );
    obj_clss->fakes           = fread_number( fp );
    obj_clss->extra_flags[0]  = fread_number( fp );
    obj_clss->extra_flags[1]  = fread_number( fp );
    obj_clss->wear_flags      = fread_number( fp );
    obj_clss->anti_flags      = fread_number( fp );
    obj_clss->restrictions    = fread_number( fp );
    obj_clss->size_flags      = fread_number( fp );
    obj_clss->materials       = fread_number( fp );

    obj_clss->affect_flags[0] = fread_number( fp );
    obj_clss->affect_flags[1] = fread_number( fp );
    obj_clss->affect_flags[2] = fread_number( fp );
    obj_clss->layer_flags     = fread_number( fp );

    obj_clss->value[0]      = fread_number( fp );
    obj_clss->value[1]      = fread_number( fp );
    obj_clss->value[2]      = fread_number( fp );
    obj_clss->value[3]      = fread_number( fp );

    obj_clss->weight        = fread_number( fp );
    obj_clss->cost          = fread_number( fp );
    obj_clss->level         = fread_number( fp );
    obj_clss->limit         = fread_number( fp );
    obj_clss->repair        = fread_number( fp );
    obj_clss->durability    = fread_number( fp );
    obj_clss->blocks        = fread_number( fp );
    obj_clss->light         = fread_number( fp );

    obj_clss->date          = fread_number( fp );

    read_affects( fp, obj_clss ); 
    read_extra( fp, obj_clss->extra_descr );

    fread_letter( fp );

    for( ; ; ) {
      int number = fread_number( fp );

      if( number == -1 )
        break;

      oprog = new oprog_data;
      append( obj_clss->oprog, oprog );

      oprog->trigger  = number;
      oprog->obj_vnum = fread_number( fp );
      oprog->command  = fread_string( fp, MEM_OPROG );
      oprog->target   = fread_string( fp, MEM_OPROG );
      oprog->code     = fread_string( fp, MEM_OPROG );

      read_extra( fp, oprog->data );
      }       

    fix( obj_clss );
    }

  fclose( fp );

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) 
    if( obj_index_list[i] != NULL )
      for( oprog = obj_index_list[i]->oprog; oprog != NULL;
        oprog = oprog->next )
        if( oprog->obj_vnum > 0 )
          oprog->obj_act = get_obj_index( oprog->obj_vnum );
 
  return;
}







