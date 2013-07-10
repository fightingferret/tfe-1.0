#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


const char*   seperate   ( const char*, bool );
void          append_liquid   ( char*, obj_data* );


char    static_storage  [ 10*THREE_LINES ]; 
int         store_pntr  = 0;


bool include_liquid = TRUE;
bool include_empty  = TRUE;
bool include_closed = TRUE;


/*
 *   SUPPORT ROUTINES
 */


const char* color_word [] = { "grey", "red", "orange", "yellow",
  "green", "cyan", "blue", "indigo", "violet", "purple", "white" };

const char* gem_quality [] = { "uncut", "flawless", "blemished",
  "fractured" };


void get_obj_adj( char* tmp, char_data* ch, obj_data* obj )
{
  int metal;

  *tmp = '\0';

  for( metal = MAT_BRONZE; metal <= MAT_ADAMANTINE; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  /*
  if( is_set( obj->pIndexData->size_flags, SFLAG_RANDOM ) ) {
    for( i = SFLAG_TINY; i < SFLAG_GIANT; i++ ) 
      if( is_set( &obj_clss->size_flags, i ) )
        break;
  */

  if( ch != NULL ) {
    if( is_set( obj->extra_flags, OFLAG_EVIL )
      && IS_AFFECTED( ch, AFF_DETECT_EVIL ) )
      sprintf( tmp+strlen( tmp ), "%s+evil+", tmp[0] == '\0' ? "" : ", " ); 

    if( is_set( obj->extra_flags, OFLAG_GOOD ) 
      && IS_AFFECTED( ch, AFF_DETECT_GOOD ) )
      sprintf( tmp+strlen( tmp ), "%s+good+", tmp[0] == '\0' ? "" : ", " ); 
    }

  if( obj->rust != 0 && metal <= MAT_ADAMANTINE )
    strcpy( tmp, material_table[ metal ].rust[ obj->rust-1 ] );

  if( is_set( obj->extra_flags, OFLAG_IS_INVIS ) )
    sprintf( tmp+strlen( tmp ), "%sinvisible", tmp[0] == '\0' ? "" : ", " ); 

  if( ch != NULL && IS_AFFECTED( ch, AFF_DETECT_MAGIC )
    && is_set( obj->extra_flags, OFLAG_MAGIC ) )
    sprintf( tmp+strlen( tmp ), "%senchanted", tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_BURNING ) )
    sprintf( tmp+strlen( tmp ), "%sburning",
      tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_FLAMING ) )
    sprintf( tmp+strlen( tmp ), "%sflaming",
      tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_POISON_COATED ) )
    sprintf( tmp+strlen( tmp ), "%spoison coated",
      tmp[0] == '\0' ? "" : ", " ); 

  switch( obj->pIndexData->item_type ) {
    case ITEM_DRINK_CON : 
      if( obj->value[1] == 0 && include_empty ) 
        sprintf( tmp+strlen( tmp ), "%sempty", *tmp == '\0' ? "" : ", " );
      break;

    case ITEM_FOOD :
      if( obj->value[1] >= 0 && obj->value[1] < 3 ) 
        sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
          cook_word[ obj->value[1] ] );
      break;

    case ITEM_CONTAINER :
      if( include_closed ) {
        if( is_set( &obj->value[1], CONT_CLOSED ) )
          sprintf( tmp+strlen( tmp ), "%sclosed", *tmp == '\0' ? "" : ", " );
        else if( obj->contents == NULL ) 
          sprintf( tmp+strlen( tmp ), "%sempty", *tmp == '\0' ? "" : ", " );
        }

      /*
    case ITEM_GEM :
      sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
        gem_quality[ obj->value[1] ] );
      break;        
      */
    }

  if( obj->pIndexData->vnum == 21 ) 
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
      color_word[ range( 0, obj->value[2]/3, 10 ) ] );

  if( is_set( obj->pIndexData->extra_flags, OFLAG_RANDOM_METAL )
    && metal <= MAT_ADAMANTINE ) 
    sprintf( tmp+strlen( tmp ), "%s%s", tmp[0] == '\0' ? "" : ", ",
      material_name[metal] ); 

  return;
}


void append_liquid( char* tmp, obj_data* obj )
{
  if( !include_liquid )
    return;

  if( obj->pIndexData->item_type != ITEM_DRINK_CON
    || ( obj->value[1] != -1 && obj->value[1] <= 0 )
    || obj->value[2] < 0 || obj->value[2] >= MAX_ENTRY_LIQUID )
    return;

  if( is_set( obj->extra_flags, OFLAG_KNOWN_LIQUID ) )
    sprintf( tmp+strlen( tmp ), " of %s",
      liquid_table[obj->value[2]].name );
  else {
    sprintf( tmp+strlen( tmp ), " containing %s",
      liquid_table[obj->value[2]].color );
    }

  return;
}


/*
 *   LOOK_SAME?
 */


bool char_data :: look_same( obj_data* obj1, obj_data* obj2 )
{
  bool id1;
  bool id2;

  if( obj1->pIndexData->fakes != obj2->pIndexData->fakes
    || ( obj1->contents == NULL ) != ( obj2->contents == NULL ) 
    || obj1->rust != obj2->rust )
    return FALSE;

  id1 = is_set( obj1->extra_flags, OFLAG_IDENTIFIED ); 
  id2 = is_set( obj2->extra_flags, OFLAG_IDENTIFIED );

  if( id2 ) {
    swap( obj1, obj2 );
    swap( id1, id2 );
    }

  if( id1 ) {
    if( !id2 ) {
      if( *obj1->singular == '{'
        || strcasecmp( obj1->after, obj2->before ) )
        return FALSE;
      if( ( obj1->pIndexData->item_type == ITEM_WEAPON
        || obj1->pIndexData->item_type == ITEM_ARMOR )
        && obj1->value[0] != 0 )
        return FALSE;
      }   
    if( obj1->pIndexData != obj2->pIndexData )
      return FALSE;
    if( ( obj1->pIndexData->item_type == ITEM_WEAPON
      || obj1->pIndexData->item_type == ITEM_ARMOR )
      && obj1->value[0] != obj2->value[0] )
      return FALSE;
    }

  if( obj1->pIndexData->vnum == 21
    && range( 0, obj1->value[2]/3, 10 ) != range( 0, obj2->value[2]/3, 10 ) )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_CORPSE
    && strcasecmp( obj1->singular, obj2->singular ) ) 
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_IS_INVIS )
    != is_set( obj2->extra_flags, OFLAG_IS_INVIS ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_BURNING )
    != is_set( obj2->extra_flags, OFLAG_BURNING ) )
    return FALSE;

  if( IS_AFFECTED( this, AFF_DETECT_MAGIC )
    && is_set( obj1->extra_flags, OFLAG_MAGIC )  
    != is_set( obj2->extra_flags, OFLAG_MAGIC ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_FLAMING )  
    != is_set( obj2->extra_flags, OFLAG_FLAMING ) )
    return FALSE;

  if( is_set( obj1->pIndexData->extra_flags, OFLAG_RANDOM_METAL )
    && obj1->materials != obj2->materials )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_DRINK_CON ) {
    if( obj1->value[1] == 0 ) {
      if( obj2->value[1] != 0 )
        return FALSE;
      }
    else {
      if( obj2->value[1] == 0 || obj2->value[2] != obj1->value[2] ) 
        return FALSE;
      }
    }    
  else if( obj1->pIndexData->item_type == ITEM_FOOD ) {
    if( obj1->value[1] != obj2->value[1] )
      return FALSE;
    }

  if( strcmp( obj1->label, obj2->label ) )
    return FALSE;

  return TRUE;
}


/*
 *   OBJECT CLASS NAME ROUTINES
 */


const char* obj_clss_data :: Keywords( )
{
  return Name( );
} 


const char* obj_clss_data :: Name( )
{
  char*             tmp;
  const char*  singular;

  tmp = static_string( );

  strcpy( tmp, is_set( extra_flags, OFLAG_THE ) ? "the " : "an " );

  if( prefix_singular != empty_string )
    sprintf( tmp+strlen( tmp ), "%s ", prefix_singular );

  singular = seperate( this->singular, TRUE );

  switch( *after ) {
    case '\0' :
      strcat( tmp, singular );
      break;

    case '+' :
      sprintf( tmp+strlen( tmp ), "%s %s", singular, &after[1] );
      break;

    default :
      sprintf( tmp+strlen( tmp ), "%s%s%s",
        after, *after == '\0' ? "" : " ", singular );
      break;
    }

  if( *tmp == 't' || isvowel( tmp[3] ) )
    return tmp;

  tmp[1] = 'a';
  return &tmp[1];
} 


const char* obj_clss_data :: Name( int number )
{
  char*           tmp;
  const char*  plural;
  
  if( number == 1 )
    return Name( );

  tmp = static_string( );

  strcpy( tmp, number_word( number ) );

  if( prefix_plural != empty_string )
    sprintf( tmp+strlen( tmp ), " %s ", prefix_plural );

  plural = seperate( this->plural, TRUE );

  switch( *after ) {
    case '\0' :
      sprintf( tmp+strlen( tmp ), " %s", plural );
      break;

    case '+' :
      sprintf( tmp+strlen( tmp ), " %s %s", plural, &after[1] );
      break;

    default :
      sprintf( tmp+strlen( tmp ), " %s %s", after, plural );
      break;
    }

  return tmp;
} 


char* name_before( obj_clss_data* obj )
{ 
  char*             tmp;
  const char*  singular;

  if( obj == NULL ) 
    return "## Null Pointer?? ##";

  tmp = static_string( );

  strcpy( tmp, is_set( obj->extra_flags, OFLAG_THE ) ? "the " : "an " );

  if( obj->prefix_singular != empty_string )
    sprintf( tmp+strlen( tmp ), "%s ", obj->prefix_singular );

  singular = seperate( obj->singular, FALSE );

  switch( *obj->before ) {
   case '\0' :
    strcat( tmp, singular );
    break;

   case '+' :
    sprintf( tmp+strlen( tmp ), "%s %s", singular, &obj->before[1] );
    break;

   default :
    sprintf( tmp+strlen( tmp ), "%s%s%s",
      obj->before, *obj->before == '\0' ? "" : " ", singular );
    break;
    }

  if( *tmp == 't' || isvowel( tmp[3] ) )
    return tmp;

  tmp[1] = 'a';
  return &tmp[1];
} 


char* name_brief( obj_clss_data* obj, int number )
{
  char*             tmp;
  const char*  singular;

  if( obj == NULL ) 
    return "## Null Pointer?? ##";

  if( number != 1 )
    return obj->Name( number );

  store_pntr = ( store_pntr+1 )%5;
  tmp        = &static_storage[store_pntr*THREE_LINES];

  singular = seperate( obj->singular, TRUE );

  switch( *obj->after ) {
   case '\0' :
    strcpy( tmp, singular );
    break;

   case '+' :
    sprintf( tmp, "%s %s", singular, &obj->after[1] );
    break;

   default :
    sprintf( tmp, "%s%s%s",
      obj->after, *obj->after == '\0' ? "" : " ", singular );
    break;
    }

  return tmp;
} 


/*
 *   OBJECT NAME ROUTINES
 */


const char* obj_data :: Name( char_data* ch, int number, bool brief )
{
  if( ch != NULL && is_set( extra_flags, OFLAG_IS_INVIS )
    && !ch->Sees_Invis( ) )
    return( number == 1 ? "something invisible"
      : "several invisible items" );

  return Seen_Name( ch, number, brief );
}


const char* obj_data :: Seen_Name( char_data* ch, int num, bool brief )
{
  char               adj  [ TWO_LINES ];
  char              plus  [ 6 ]; 
  char*           string;
  char*              tmp;
  const char*       noun;
  bool        identified  = is_set( extra_flags, OFLAG_IDENTIFIED );

  tmp  = static_string( );
  *tmp = '\0';

  get_obj_adj( adj, ch, this );

  if( num == 1 || brief ) {
    noun = seperate( singular, identified );
    }
  else {
    strcpy( tmp, number_word( num, ch ) );
    noun = seperate( plural, identified );
    }

  *plus = '\0';

  if( identified ) {
    string = after;
    if( ( pIndexData->item_type == ITEM_WEAPON
      || pIndexData->item_type == ITEM_ARMOR ) && value[0] != 0 )
      sprintf( plus, " %+d", value[0] );
    }
  else 
    string = before;

  if( num == 1 || brief ) {
    if( !brief ) {
      strcpy( tmp, is_set( pIndexData->extra_flags, OFLAG_THE )
        ? "the " : "an " );
      if( pIndexData->prefix_singular != empty_string )
        sprintf( tmp+strlen( tmp ), "%s ", pIndexData->prefix_singular );
      }
    if( *string != '+' )
      sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s",
        adj, adj[0] == '\0' ? "" : " ",
        string, ( *noun == '\0' || string[0] == '\0' ) 
        ? "" : " ", noun, plus );
    else
      sprintf( tmp+strlen( tmp ), "%s%s%s %s%s",
        adj,( *noun == '\0' || *adj == '\0' ) ? "" : " ",
        noun, &string[1], plus );
    if( num != 1 )
      sprintf( tmp+strlen( tmp ), " (x%d)", num );
    }
  else {
    if( pIndexData->prefix_plural != empty_string )
      sprintf( tmp+strlen( tmp ), " %s ", pIndexData->prefix_plural );
    else 
      strcat( tmp, " " );

    if( *string != '+' )
      sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s",
        adj, adj[0] == '\0' ? "" : " ",
        string, ( noun[0] == '\0' || string[0] == '\0' )
        ? "" : " ", noun, plus );
    else
      sprintf( tmp+strlen( tmp ), "%s%s%s %s%s",
        adj, ( *noun == '\0' || adj[0] == '\0' ) ?
        "" : " ", noun, &string[1], plus );
    }

  append_liquid( tmp, this );

  if( label != empty_string ) 
    sprintf( tmp+strlen( tmp ), " labeled %s", label );

  if( num == 1 && !brief && *tmp == 'a' && !isvowel( tmp[3] )
    && ( tmp[3] != '+' || !isvowel( tmp[4] ) ) ) {
    tmp[1] = 'a';
    return &tmp[1];
    }

  return tmp;
}


const char* obj_data :: Keywords( char_data* ch )
{
  if(  pIndexData->item_type != ITEM_WEAPON
    && pIndexData->item_type != ITEM_ARMOR
    && pIndexData->item_type != ITEM_FOOD
    && pIndexData->item_type != ITEM_REAGENT )
    return Seen_Name( ch );

  char* tmp = static_string( );

  sprintf( tmp, "%s %s %s", Seen_Name( ch ),
    condition_name( ch ), item_type_name[ pIndexData->item_type ] );

  return tmp;
}


const char* obj_data :: Show( char_data* ch, int num )
{ 
  char* tmp = static_string( );

  sprintf( tmp, "%s %s", Seen_Name( ch, num ),
    num == 1 ? ( *pIndexData->long_s == '\0' ?
    "lies here." : pIndexData->long_s )  
    : ( *pIndexData->long_p == '\0' ?
    "lie here." : pIndexData->long_p ) );

  return tmp;
}


/*
 *   LABEL ROUTINE
 */


bool valid_label( char_data* ch, const char* label )
{
  int i;

  for( i = 0; label[i] != '\0'; i++ )
    if( !isalpha( label[i] ) && label[i] != ' ' ) {
      send( ch, "Labels may only contain letters and spaces.\n\r" );
      return FALSE;
      }

  if( i >= 15 ) {
    send( ch, "Labels must be less than 15 characters.\n\r" );
    return FALSE;
    } 

  return TRUE;
}


void do_label( char_data* ch, char* argument )
{
  char        arg  [ MAX_INPUT_LENGTH ];
  obj_data*   obj;
  char*     label;
  bool     remove;

  remove = !contains_word( argument, "as", arg );

  if( *argument == '\0' || ( !remove && *arg == '\0' ) ) {
    send( ch, "Label what item and as what?\n\r" );
    return;
    }

  if( ( obj = one_object( ch, remove ? argument : arg,
    "label", &ch->contents ) ) == NULL ) 
    return;

  if( remove ) {
    if( obj->label == empty_string ) {
      send( ch, "%s isn't labeled.\n\r", obj );
      return;
      }
    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;
    send( ch, "You remove the label from %s.\n\r", obj );
    return;
    }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( ch, "You may only label containers.\n\r" );
    return;
    }

  if( ( label = obj->label ) != empty_string ) {
    obj->label = empty_string;
    send( ch, "%s is already labeled as '%s'.\n\r", obj, label );
    obj->label = label;
    return;
    }

  if( !valid_label( ch, argument ) )
    return;

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
    }
 
  if( obj->label == empty_string ) {
    send( ch, "You label %s '%s'.\n\r", obj, argument );
    }
  else {
    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;
    fsend( ch,
      "You remove the old label from %s and replace it with '%s'.\n\r", 
      obj, argument );
    }

  obj->label = alloc_string( argument, MEM_OBJECT );

  if( obj->array == NULL ) 
    obj->To( ch );

  consolidate( obj );
}












