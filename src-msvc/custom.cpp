#include "define.h"
#include "struct.h"


/*
 *   LOCAL FUNCTIONS
 */


char*         item_name    ( custom_data* );
custom_data*  find_custom  ( char*, custom_data* );
int           get_metal    ( custom_data* ); 
 

/*
 *   SUPPORT ROUTINES
 */


char* item_name( custom_data* custom )
{
  obj_clss_data*  obj_clss  = custom->item;
  char*                tmp;

  if( !is_set( obj_clss->extra_flags, OFLAG_RANDOM_METAL ) ) 
    return name_brief( obj_clss );

  tmp = static_string( );

  sprintf( tmp, "%s %s", material_name[ get_metal( custom ) ],
    name_brief( obj_clss ) );

  return tmp;
} 


int get_metal( custom_data* custom )
{
  int   i, j;
  
  for( i = 0; i < MAX_INGRED; i++ ) {
    if( custom->ingred[i] == NULL )
      continue;
    for( j = MAT_BRONZE; j <= MAT_ADAMANTINE; j++ )
      if( is_set( &custom->ingred[i]->materials, j ) )
        return j;
    }

  return MAT_BRONZE;
};


custom_data* find_custom( char* argument, custom_data* custom )
{
  custom_data*  found  = NULL;
  int           value  = 0;
  int               i;   

  for( ; custom != NULL; custom = custom->next ) 
    if( ( i = subset( argument, item_name( custom ) ) ) > value ) {
      found = custom;
      value = i;
      }

  return found;
};


/*
 *   BUILDER CUSTOM COMMANDS
 */


void do_shcustom( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  shop_data*       shop;
  custom_data*   custom;
  char_data*     keeper;
  wizard_data*   wizard;
  obj_data*         obj;
  room_data*       room;
  int                 i;
  thing_array*    array;

  wizard = (wizard_data*) ch;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "You aren't in a room.\r\n" );
    return;
    }
 
  if( is_set( &room->room_flags, RFLAG_PET_SHOP ) ) {
    send( ch, "Pet shops cannot have customs.\r\n" );
    return;
    }

  shop   = keeper->pShop;
  custom = locate( shop->custom, wizard->custom_edit );

  if( matches( argument, "exit" ) || !strcmp( argument, ".." ) ) {
    if( custom == NULL )
      send( ch, "You are already not editing a custom.\r\n" );
    else {
      send( ch, "You no longer edit a custom.\r\n" );
      wizard->custom_edit = 0;
      }
    return;
    }

  if( custom == NULL ) {
    if( *argument == '\0' ) {
      if( ( custom = shop->custom ) == NULL ) {
        send( ch, "This shop is has no customs.\r\n" );
        return;
        }
      send_title( ch, "Customs - %s", keeper->descr->name );
      send_underlined( ch, "Index  Vnum   Obj\r\n" ); 
      for( i = 1; custom != NULL; custom = custom->next, i++ ) 
        send( ch, "%4d%7d   %s\r\n", i, custom->item->vnum,
          name_brief( custom->item ) );
      return;
      }

    if( number_arg( argument, i ) ) {
      if( ( custom = locate( shop->custom, i ) ) == NULL ) {
        send( ch, "No custom exists with that index.\r\n" );
        return;
        }
      if( *argument == '\0' ) {
        send( ch, "You now edit custom #%d.\r\n", i );
        wizard->custom_edit = i;
        return;
        }
      if( ( obj = one_object( ch, argument, 
        "custom", &ch->contents ) ) == NULL )
        return;
      custom->item = obj->pIndexData;
      send( ch, "Item created by custom set to %s.\r\n",
        custom->item->Name( ) );
      return;
      }

    if( matches( argument, "delete" ) ) {
      if( ( custom = locate( shop->custom, atoi( argument ) ) ) == NULL ) 
        send( ch, "No custom exists with that index.\r\n" );
      else {
        send( ch, "Custom for %s deleted.\r\n", custom->item->Name( ) );
        remove( shop->custom, custom );
        delete custom;
        }
      return;
      }

    if( ( obj = one_object( ch, argument,
      "custom", &ch->contents ) ) == NULL ) 
      return;

    custom       = new custom_data;
    custom->item = obj->pIndexData;
    custom->next = shop->custom;
    shop->custom = custom;
  
    send( ch, "%s added to shop custom list.\r\n",
      obj->pIndexData->Name( ) );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Item: %s\r\n", custom->item->Name( ) );
    send( ch, "Cost: %d\r\n\r\n", custom->cost );
    *buf = '\0';
    for( i = 0; i < MAX_INGRED; i++ )
      if( custom->ingred[i] != NULL )
        sprintf( buf + strlen( buf ), "[%2d]  %s\r\n",
          i+1, name_brief( custom->ingred[i], custom->number[i] ) );
    if( *buf == '\0' )
      sprintf( buf, "No ingredients.\r\n" );
    send( ch, buf );
    return;
    }

  if( matches( argument, "cost" ) ) {
    custom->cost = atoi( argument );
    send( ch, "Cost for labor on %s set to %d cp.\r\n",
      custom->item->Name( ), custom->cost );
    return;
    } 

  if( matches( argument, "delete" ) ) {
    i = atoi( argument )-1;
    if( i >= 0 && i < MAX_INGRED && custom->ingred[i] != NULL ) {
      custom->ingred[i] = NULL;
      send( ch, "Ingredient removed.\r\n" );
      return;
      }
    send( ch, "Ingredient not found to remove.\r\n" ); 
    return;
    }

  if( ( array = several_things( ch, argument,
    "add as an ingredient", &ch->contents ) ) == NULL ) 
    return;

  if( array->size != 1 ) {
    send( ch, "You may only add one type of ingredient at a time.\r\n" );
    return;
    }

  if( ( obj = object( array->list[0] ) ) == NULL ) {
    send( ch, "Only objects may be ingredients.\r\n" );
    return;
    }

  for( i = 0; i < MAX_INGRED; i++ ) 
    if( custom->ingred[i] == NULL ) {
      custom->ingred[i] = obj->pIndexData;
      custom->number[i] = obj->selected;
      send( ch, "%s is added as an ingredient for %s.\r\n",
        obj, custom->item->Name( ) );
      return;
      }

  send( ch, "%s has the maximum number of ingredients allowed.\r\n",
    custom->item->Name( ) );
} 


/*
 *   PLAYER CUSTOM COMMANDS
 */


void display_custom( char_data* ch, char_data* keeper )
{
  char             tmp  [ FIVE_LINES ];
  char           level  [ 10 ];
  shop_data*      shop  = keeper->pShop;
  custom_data*  custom;
  bool            flag;
  
  if( ( custom = shop->custom ) == NULL ) {
    process_tell( keeper, ch, "There is nothing I can custom for you." );
    return;
    }

  send( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
  send_underlined( ch,
    "Item                         Cost  Lvl   Ingredients\r\n" );
  
  for( custom = shop->custom; custom != NULL; custom = custom->next ) {
    if( !can_use( ch, custom->item, NULL, TRUE ) )
      strcpy( level, "***" );
    else
      sprintf( level, "%d", custom->item->level );
    sprintf( tmp, "%-27s%6d%5s   ",
      truncate( item_name( custom ), 27 ), custom->cost, level );
    flag = FALSE;
    for( int i = 0; i < MAX_INGRED; i++ ) {
      if( custom->ingred[i] == NULL )
        continue;
      if( strlen( tmp )+strlen( name_brief( custom->ingred[i],
        custom->number[i] ) ) > 78 ) {
        strcat( tmp, "\r\n" );
        send( ch, tmp );
        sprintf( tmp, "%41s", "" );
        flag = FALSE;
        }  
      sprintf( tmp+strlen( tmp ), "%s%s", ( flag ? ", " : "" ),
        name_brief( custom->ingred[i], custom->number[i] ) );
      flag = TRUE;
      }

    strcat( tmp, "\r\n" );
    send( ch, tmp );
    }
}


void do_custom( char_data* ch, char* argument )
{
  char                tmp  [ TWO_LINES ];
  char_data*       keeper;
  custom_data*     custom;
  obj_data*           obj;
  shop_data*         shop;
  room_data*         room;
  int             i, j, k;
  obj_array          list;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( ( room = Room( ch->array->where ) ) != NULL
    && is_set( &room->room_flags, RFLAG_PET_SHOP ) ) {
    process_tell( keeper, ch, "Frankenstein is not for sale." );
    return;
    }

  if( *argument == '\0' ) {
    display_custom( ch, keeper );
    return;
    }

  shop = keeper->pShop;

  if( ( custom = find_custom( argument, shop->custom ) ) == NULL ) {
    process_tell( keeper, ch, "I can make nothing of that name." );
    return;
    }

  for( i = 0; i < MAX_INGRED; i++ ) {
    if( custom->ingred[i] == NULL )
      continue;

    for( j = k = 0; ; j++ ) {
      if( j == ch->contents ) {
        sprintf( tmp, "Customing %s requires %s which you do not have.",
          custom->item->Name( ),
          custom->ingred[i]->Name( custom->number[i] ) );
        process_tell( keeper, ch, tmp );
        return;
        }
      if( ( obj = object( ch->contents[j] ) ) != NULL
        && obj->pIndexData == custom->ingred[i] ) {
        list += obj;
        if( ( obj->selected = custom->number[i]-k ) <= obj->number )
          break;
        obj->selected = obj->number;
        k            += obj->number; 
        }
      }
    }

  if( custom->cost > 0 ) {
    sprintf( tmp, "You hand %s", keeper->Name( ch ) );
    if( !remove_coins( ch, custom->cost, tmp ) ) {
      send( ch, "You can't afford to custom %s.\r\n",
        custom->item->Name( ) );
      return;
      }
    }

  page_priv( ch, NULL, empty_string );
  page_publ( ch, (thing_array*) &list, "give", keeper, "to" );

  for( i = 0; i < list; i++ ) {
    obj = list[i];
    obj->Extract( obj->selected );
    }

  obj = create( custom->item );

  if( is_set( obj->pIndexData->extra_flags, OFLAG_RANDOM_METAL ) ) {
    set_bit( &obj->materials, get_metal( custom ) );
    set_alloy( obj, 0 );
    }

  obj->size_flags = ( ch->pcdata == NULL ? -1
    : ch->pcdata->pfile->ident );

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

  if( ch->pcdata != NULL )
    obj->owner = ch->pcdata->pfile;

  send( *keeper->array, "%s creates %s.\r\n", keeper, obj );
  send( *keeper->array, "%s gives %s to %s.\r\n", keeper, obj, ch );
  send( ch, "%s gives you %s.\r\n", keeper, obj );

  process_tell( keeper, ch, "Good luck" );
  
  obj->To( ch );
}















