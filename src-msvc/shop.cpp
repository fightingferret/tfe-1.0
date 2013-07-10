#include "define.h"
#include "struct.h"


shop_data*  shop_list     = NULL;
const char* shop_flags [] = { "buys.stolen" };

const int ingot_vnum[] = { 638, 639, 640, 641, 643, 642 };


/*
 *   LOCAL FUNCTIONS
 */


bool        will_trade    ( char_data*, obj_data* );
int         get_cost      ( char_data*, char_data*, obj_data*, bool );
int         melt_cost     ( char_data*, obj_data* );
int         repair_cost   ( char_data*, obj_data* );


/*
 *   SHOP OBJET PROPERTIES
 */


bool Obj_Data :: Damaged( )
{
  return( condition < 9*repair_condition( this )/10 );
};


/*
 *   UPDATE ROUTINE
 */


void shop_update( void )
{
  mob_data*    keeper;
  obj_data*       obj;

  for( int i = 0; i < mob_list; i++ ) {
    keeper = mob_list[i];

    if( keeper->pShop == NULL || keeper->reset == NULL
      || player_in_room( keeper->in_room ) )
      continue;

    reset_shop( keeper );

    for( int j = keeper->contents-1; j >= 0; j-- ) {
      obj = (obj_data*) keeper->contents[j];
      for( int k = obj->number; k > 0; k-- ) 
        if( number_range( 0, 10 ) == 0 && --obj->number == 0 )
          obj->Extract( );
      } 
    }
}


/*
 *   COST ROUTINES
 */


bool will_trade( char_data* keeper, obj_data* obj )
{
  if( obj->pIndexData->materials != 0 
    && keeper->pShop->materials && obj->pIndexData->materials == 0 )
    return FALSE;

  return is_set( keeper->pShop->buy_type, obj->pIndexData->item_type );
}


int get_cost( char_data* keeper, char_data* ch, obj_data* obj, bool fBuy )
{
  obj_data*    inv;
  int        total  = 0;
  int         cost  = 0;
  int       number  = 0;
  int i;

  if( obj == NULL || is_set( obj->extra_flags, OFLAG_NO_SELL )
    || !will_trade( keeper, obj ) )
    return 0;

  for( i = 0; i < keeper->contents; i++ ) {
    inv = object( keeper->contents[i] );
    if( inv->pIndexData == obj->pIndexData )
      number += inv->number;
    }

  cost = obj->Cost( );

  if( ch->species == NULL ) 
    cost += (1-2*fBuy)*ch->shdata->skill[SKILL_HAGGLE]*cost/100;

  if( ch->shdata->skill[SKILL_HAGGLE] > 0 )
    ch->improve_skill( SKILL_HAGGLE );

  if( !fBuy ) {
    if( obj->pIndexData->item_type == ITEM_ARMOR
      || obj->pIndexData->item_type == ITEM_WEAPON )
      cost /= 12;
    else
      cost /= 4;
    }

  cost = cost*obj->condition/obj->pIndexData->durability
    *sqr(4-obj->rust)/16;

  switch( obj->pIndexData->item_type ) {
    case ITEM_STAFF :
    case ITEM_WAND :
      if( obj->value[0] != 0 )
        cost = cost*(1+obj->value[3]/obj->value[0])/2;
      break;

    case ITEM_DRINK_CON :
      if( fBuy )
        cost += obj->value[1]*liquid_table[ obj->value[2] ].cost/100;
      break;
    }    

  for( i = 0; i < obj->selected; i++ )
    total += cost*12/(3+number+( fBuy ? -i : i ) );

  return total;
}


int repair_cost( char_data* keeper, obj_data* obj )
{
  obj_clss_data*   obj_clss  = obj->pIndexData;
  int                  cost;

  if( !will_trade( keeper, obj )
    || obj->pIndexData->repair > keeper->pShop->repair )
    return 0;

  cost = 50*sqr( obj_clss->repair )+obj_clss->cost
    *( obj_clss->durability-obj->condition )
    /( 2*obj_clss->durability );

  cost = cost*(20+obj->age)/20;  

  return cost;
}


/*
 *   BUY FUNCTION 
 */


bool buyable_pet( char_data* pet )
{
  if( pet->species == NULL || pet->leader != NULL
    || !is_set( &pet->species->act_flags, ACT_CAN_TAME ) )
    return FALSE;

  return TRUE;
}


thing_data* cantafford( thing_data* t1, char_data* ch, thing_data* t2 )
{
  obj_data*    obj  = (obj_data*) t1;
  mob_data* keeper  = (mob_data*) t2;

  obj->temp = get_cost( keeper, ch, obj, TRUE );

  return( remove_coins( ch, obj->temp, NULL ) ? obj : NULL );
}


thing_data* buy( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;
 
  obj = (obj_data*) obj->From( obj->selected );   
  set_owner( obj, ch, NULL );
  obj->To( ch );

  return obj;
}


void do_buy( char_data *ch, char *argument )
{
  char             buf  [ MAX_INPUT_LENGTH ];
  char_data*    keeper;
  char_data*       pet;
  obj_data*        obj;
  room_data*      room;
  thing_array*   array;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  /* PET SHOP */

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    if( ch->species != NULL  ) {
      send( ch, "Monsters can't buy pets." );
      return;
      }

    if( ( room = get_room_index( ch->in_room->vnum+1 ) ) == NULL ) {
      send( ch, "The pet shop is still under construction.\r\n" );
      return;
      }

    thing_array list;

    for( int i = 0; i < room->contents; i++ ) 
      if( ( pet = character( room->contents[i] ) ) != NULL
        && buyable_pet( pet ) )
        list += pet;

    if( ( pet = one_character( ch, argument, "buy", &list ) ) == NULL )
      return;

    if( pet->shdata->level > ch->shdata->level ) {
      send( ch, "%s is too high a level for you.\r\n", pet );
      return;
      }

    if( pet->species->price == 0 ) {
      send( ch,
        "That pet is not for sale until a god sets a price for it.\r\n" );
      return;
      }

    if( is_set( &pet->species->act_flags, ACT_MOUNT ) ) {
      if( has_mount( ch ) )
        return;
      }
    else {
      if( number_of_pets( ch ) >= 2 ) {
        send( ch, "You already have two pets.\r\n" );
        return;
        }
      }

    sprintf( buf, "You hand %s", keeper->descr->name ); 
    if( !remove_coins( ch, pet->species->price, buf ) ) {
      if( ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch, "You can't afford it.\r\n" );
        return;
        }
      send( ch, "You don't have enough gold, but it doesn't seem to\
 matter.\r\n" );
      }

    pet->From( );
    pet->To( ch->array );

    set_bit( &pet->status, STAT_PET );
    add_follower( pet, ch );

    send( ch, "Enjoy your pet.\r\n" );
    fsend( ch, "%s bought %s as a pet.\r\n", ch, pet );

    if( pet->reset != NULL ) {
      pet->reset->count--;
      pet->reset = NULL;
      }
    return;
    }

  /* OBJECT SHOP */

  thing_array list;

  for( int i = 0; i < keeper->contents; i++ ) {
    obj = (obj_data*) keeper->contents[i];
    if( will_trade( keeper, obj ) )
      list += obj;
    }
    
  if( ( array = several_things( ch, argument, "buy", &list ) ) == NULL )
    return;

  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { heavy, many, cantafford, buy };

  sort_objects( ch, *array, keeper, 4, subset, func );
  
  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't lift" );
  page_priv( ch, &subset[1], "can't handle" );
  page_priv( ch, &subset[2], "can't afford" );
  page_publ( ch, &subset[3], "buy", keeper, "from", "for" );

  delete array;
}


void do_list( char_data* ch, char* argument )
{
  char_data*   keeper;
  char_data*      pet;
  thing_array*  array;
  obj_data*       obj;
  room_data*     room;
  int i;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( *argument == '\0' )
    argument = "all";

  /* PET SHOP */

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    if( ( room = get_room_index( ch->in_room->vnum+1 ) ) == NULL ) {
      send( ch, "The pet shop is still under construction.\r\n" );
      return;
      }

    thing_array list;

    for( i = 0; i < room->contents; i++ ) 
      if( ( pet = character( room->contents[i] ) ) != NULL
        && buyable_pet( pet ) )
        list += pet;

    if( is_empty( list ) ) {
      process_tell( keeper, ch, "Sorry, I'm out of pets right now.\r\n" );
      return;
      }

    if( ( array = several_things( ch, argument, "list", &list ) ) == NULL )
      return;

    send( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
    send_underlined( ch,
      "Pet                          Cost    Level\r\n" );

    for( i = 0; i < *array; i++ ) {
      pet = (char_data*) array->list[i];
      send( ch, "%-25s%8d%8d\r\n", pet->Seen_Name( ch, 1, TRUE ),
        pet->species->price, pet->shdata->level );
      }

    delete array;

    return;
    }

  /* OBJECT SHOP */

  thing_array list;

  for( i = 0; i < keeper->contents; i++ ) {
    obj = (obj_data*) keeper->contents[i];
    obj->selected = 1;
    if( ( obj->temp = get_cost( keeper, ch, obj, TRUE ) ) > 0 )
      list += obj;
    }

  if( is_empty( list ) ) {
    process_tell( keeper, ch,
      "Sorry, I have nothing to sell right now.\r\n" );
    return;
    }

  if( ( array = several_things( ch, argument, "list",  &list ) ) == NULL )
    return;

  page( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
  page_underlined( ch, "Item                                     Cost\
    Level   Number   Condition\r\n" );

  char level [ 5 ];

  include_closed = FALSE;

  for( i = 0; i < *array; i++ ) {
    obj = (obj_data*) array->list[i];

    if( !can_use( ch, obj->pIndexData, obj ) )
      sprintf( level, "***" );
    else
      sprintf( level, "%d", obj->pIndexData->level );

    page( ch, "%-37s%8d%8s%8d%5s%-s\r\n",
      truncate( (char *) obj->Seen_Name( ch, 1, TRUE ), 37 ), obj->temp, level,
      obj->number, "", obj->condition_name( ch, TRUE ) );
    }

  include_closed = TRUE;

  delete array;
}


/*
 *   SELL
 */


thing_data* sold( thing_data* t1, char_data*, thing_data* )
{
  return t1;
}


thing_data* uninterested( thing_data* thing, char_data* ch,
  thing_data* keeper )
{
  obj_data* obj = (obj_data*) thing;

  if( ( obj->temp = get_cost( (char_data*) keeper, ch, obj, FALSE ) ) <= 0 ) 
    return NULL;

  return obj;
}


thing_data* not_empty( thing_data* obj, char_data*, thing_data* )
{
  return( obj->contents == NULL ? obj : NULL );
}


void do_sell( char_data* ch, char* argument )
{
  char_data*   keeper;
  thing_array*  array;
  obj_data*       obj;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    process_tell( keeper, ch, "We don't buy pets." );
    ch->reply = keeper;
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Sell what?\r\n" );
    return;
    }

  if( ( array = several_things( ch, argument, "sell", 
    &ch->contents ) ) == NULL ) 
    return;

  thing_array   subset  [ 5 ];
  thing_func*     func  [ 5 ]  = { cursed, stolen, uninterested,
                                   not_empty, sold };

  sort_objects( ch, *array, keeper, 5, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[1], "don't own" );
  page_priv( ch, &subset[2], "isn't interested in", keeper );
  page_priv( ch, &subset[0], "can't let go of" );
  page_priv( ch, &subset[3], NULL, NULL, "isn't empty", "aren't empty" );
  page_publ( ch, &subset[4], "sell", keeper, "to", "for" );

  for( int i = 0; i < subset[4]; i++ ) {
    obj = (obj_data*) subset[4][i];
    obj = (obj_data*) obj->From( obj->selected );

    set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
    set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

    if( obj->pIndexData->item_type == ITEM_WAND
      || obj->pIndexData->item_type == ITEM_STAFF ) 
      obj->value[3] = obj->pIndexData->value[3];

    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;

    obj->owner = NULL;
    add_coins( ch, obj->temp );
    obj->To( keeper );
    consolidate( obj );
    }

  delete array;
}


void do_value( char_data* ch, char* argument )
{
  char           buf  [ MAX_STRING_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int          rcost;
  int         blocks;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    process_tell( keeper, ch, "We don't buy pets" );
    return;
    }

  if( ( obj = one_object( ch, argument, "value",
    &ch->contents ) ) == NULL ) 
    return;

  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
    }
  
  if( !obj->Belongs( ch ) ) {
    sprintf( buf, "%s is stolen so I would never buy it.",
      obj->Seen_Name( ch ) );
    process_tell( keeper, ch, buf );
    return;
    } 

  cost  = get_cost( keeper, ch, obj, FALSE );
  rcost = repair_cost( keeper, obj );
 
  if( obj->Damaged( ) ) {
    if( rcost > 0 ) 
      sprintf( buf, "I see %s is damaged.  I can repair it for %d cp %s ",
        obj->Seen_Name( ch ), rcost, cost > 0 ? "or" : "but" ); 
    else 
      sprintf( buf, "I see %s is damaged.  I am unable to repair it %s ",
        obj->Seen_Name( ch ), cost > 0 ? "but" : "and" );
    if( cost > 0 ) 
      sprintf( buf+strlen( buf ), "would give you %d cp for it.", cost );
    else
      strcat( buf, "am uninterested in buying it." );
    }
  else {
    if( cost > 0 )
      sprintf( buf, "I would pay you %d cp for %s.",
        cost, obj->Seen_Name( ch ) ); 
    else
      sprintf( buf, "I am uninterested in buying %s.",
        obj->Seen_Name( ch ) );
    }

  blocks = obj->pIndexData->blocks;
  if( ( cost = melt_cost( keeper, obj ) ) != 0 ) 
    sprintf( buf+strlen( buf ),
      "  I would melt it down to produce %d block%s for %d cp.",
      blocks, blocks == 1 ? "" : "s", cost );
      
  process_tell( keeper, ch, buf );
}


/*
 *   REPAIR ROUTINES
 */


void do_repair( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int           cond;

  if( *argument == '\0' ) {
    send( ch, "Repair what?\r\n" );
    return;
    }

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( keeper->pShop->repair == 0 ) {
    process_tell( keeper, ch, "Sorry - I do not repair items." );
    return;
    }

  if( ( obj = one_object( ch, argument, "repair", &ch->contents ) ) == NULL ) 
    return;

  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
    }

  if( ( cost = repair_cost( keeper, obj ) ) <= 0 ) {
    process_tell( keeper, ch, "That isn't something I can repair" );
    return;
    }

  if( ( cond = repair_condition( obj ) ) < 0 ) {
    process_tell( keeper, ch,
      "That item is too old to be worth repairing." );
    return;
    }

  if( !obj->Damaged( ) ) {
    process_tell( keeper, ch,
      "That isn't damaged enough to be worth repairing." );
    return;
    }

  sprintf( buf, "%s repairs %s for you at a cost of",
    keeper->Name( ch ), obj->Seen_Name( ch ) );
  *buf = toupper( *buf );
  if( !remove_coins( ch, cost, buf ) ) {
    sprintf( buf, "You can't afford the cost of repairing %s.", 
      obj->Seen_Name( ch ) );
    process_tell( keeper, ch, buf );   
    return;
    }
    
  fsend( ch, "%s gets %s repaired.\r\n", ch, obj );

  obj = (obj_data*) obj->From( 1 );

  obj->age       += int( 1+obj->rust*pow(cond-obj->condition,1.5)/100. );
  obj->condition  = cond;

  obj->To( &ch->contents );
  consolidate( obj );
}


/*
 *   MELT FUNCTIONS
 */


int melt_cost( char_data *keeper, obj_data *obj )
{
  int metal;

  for( metal = MAT_BRONZE; metal <= MAT_GOLD; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_GOLD ) 
    return 0;

  if( obj->pIndexData->blocks == 0 
    || (keeper->pShop->materials & ( 1 << metal )) == 0 )
    return 0;

  return 100;
}


void do_melt( char_data *ch, char *argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int         blocks;
  int          metal;
  int         length;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( ( obj = one_object( ch, argument, "melt", &ch->contents ) ) == NULL ) 
    return;

  for( metal = MAT_BRONZE; metal <= MAT_GOLD; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_GOLD ) {
    fsend( ch, "%s is not made out of metal.", obj );
    return;
    }

  if( ( blocks = obj->pIndexData->blocks ) == 0 ) {
    fsend( ch, "%s does not contain enough metal to be worth melting.",
      obj );
    return;
    }

  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
    }

  if( (keeper->pShop->materials & ( 1 << metal )) == 0 ) {
    process_tell( keeper, ch,
      "Sorry that is not made out of a metal I deal with." );
    return;
    }

  cost = 100;

  sprintf( buf, "You hand %s to %s along with",
    obj->Seen_Name( ch ), keeper->Seen_Name( ch ) );
  if( !remove_coins( ch, cost, buf ) ) { 
    sprintf( buf, "You can't afford my fee of %d cp to melt %s.",
      cost, obj->Seen_Name( ch ) );
    process_tell( keeper, ch, buf );
    return;
    }
    
  fsend( ch, "%s has %s melted down.", ch, obj );
  sprintf( buf, "%s takes %s and places it in the furnace.  ",
    keeper->Seen_Name( ch ), obj->Seen_Name( ch ) );
  buf[0] = toupper( buf[0] );

  obj->Extract( 1 );

  if( ( obj = create( get_obj_index(
    ingot_vnum[metal-MAT_BRONZE] ) ) ) == NULL ) {
    bug( "Repair: Ingot for %s does not exist.", 
      material_table[metal].name );
    return;
    }

  obj->number = blocks;
  obj->shown  = blocks;
  set_bit( &obj->materials, metal );

  length = strlen( buf );
  sprintf( &buf[length],
  "%s then pulls it out and after much hammering and reheating hands you %s.",
    keeper->He_She( ), obj->Seen_Name( ch, blocks ) );
  buf[length] = toupper( buf[length] );
  fsend( ch, buf );

  obj->To( &ch->contents );
}


/*
 *  ONLINE EDITING OF SHOPS
 */


void do_shedit( char_data* ch, char* argument )
{
  char                  buf  [MAX_STRING_LENGTH ];
  mob_data*          keeper;
  shop_data*           shop;
  char_data*         victim;
  species_data*     species;
  int                number;
  
  for( shop = shop_list; shop != NULL; shop = shop->next )
    if( ch->in_room == shop->room ) 
      break;  

  if( *argument != '\0' && !can_edit( ch, ch->in_room ) )
    return;

  if( exact_match( argument, "new" ) ) {
    if( shop != NULL ) {
      send( ch, "There is already a shop here.\r\n" );
      return;
      }
    shop = new shop_data;
    shop->room = ch->in_room;
    shop->custom = NULL;
    shop->keeper = -1;
    shop->repair = 0;
    shop->materials = 0;
    shop->buy_type[0] = 0;
    shop->buy_type[1] = 0;
    shop->next = shop_list;
    shop_list = shop;
    send( ch, "New shop created here.\r\n" );
    return;
    }

  if( shop == NULL ) {
    send( ch, "There is no shop associated with this room.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    species = get_species( shop->keeper ); 
    sprintf( buf, "Shop Keeper: %s  [ Vnum: %d ]\r\n\r\n", ( species == NULL
      ? "none" : species->descr->name ), shop->keeper );
    sprintf( buf+strlen( buf ), "Repair: %d\r\n\r\n", shop->repair );
    send( buf, ch );
    return;
    }
  
  if( exact_match( argument, "delete" ) ) {
    remove( shop_list, shop );
    for( int i = 0; i < mob_list; i++ )
      if( mob_list[i]->pShop == shop )
        mob_list[i]->pShop = NULL;
    send( ch, "Shop deleted.\r\n" );
    return;
    }
   
  if( matches( argument, "keeper" ) ) {
    if( ( victim = one_character( ch, argument, "set keepr",
      ch->array ) ) == NULL )
      return;

    if( ( keeper = mob( victim ) ) == NULL ) {
      send( ch, "Players can not be shop keepers.\r\n" );
      return;
      }
    shop->keeper = keeper->species->vnum;
    keeper->pShop = shop;
    send( ch, "Shop keeper set to %s.\r\n", keeper->descr->name );
    return;
    }

  if( matches( argument, "repair" ) ) {
    if( ( number = atoi( argument ) ) < 0 || number > 10 ) {
      send( ch,
        "A shop's repair level must be between 0 and 10.\r\n" ); 
      return;
      }
    shop->repair = number;
    send( ch, "The shop's repair level is set to %d.\r\n", number );
    } 
}


void do_shflag( char_data* ch, char* argument )
{
  shop_data*       shop;
  const char*  response;

  for( shop = shop_list; shop != NULL; shop = shop->next )
    if( ch->in_room == shop->room ) 
      break;  

  if( shop == NULL ) {
    send( ch, "This room has no shop entry.\r\n" );
    return;
    }

#define types 3

  const char*  title [types] = { "Basic", "Obj_Types", "Materials" };
  int            max [types] = { MAX_SHOP, MAX_ITEM, MAX_MATERIAL };

  const char** name1 [types] = { &shop_flags[0], &item_type_name[0],
    &material_name[0] };
  const char** name2 [types] = { &shop_flags[1], &item_type_name[1],
    &material_name[1] };

  int*    flag_value [types] = { &shop->flags, shop->buy_type,
    &shop->materials };
  int      uses_flag [types] = { 1, 1, 1 };

  response = flag_handler( title, name1, name2, flag_value, max,
    uses_flag, (const char*) NULL, ch, argument, types );

#undef types

  if( response == NULL ) 
    send( ch, "No such flag.\r\n" );
  else
    room_log( ch, ch->in_room->vnum, response );

  return;
}


void do_shstat( char_data*, char* )
{
  return;
}


/*
 *   MISC SHOP ROUTINES
 */


char_data* active_shop( char_data* ch )
{
  room_data*    room  = ch->in_room;
  char_data*  keeper;

  if( !is_set( &room->room_flags, RFLAG_PET_SHOP )
    && !is_set( &room->room_flags, RFLAG_SHOP ) ) 
    return NULL;

  for( int i = 0; i < room->contents; i++ )
    if( ( keeper = mob( room->contents[i] ) ) != NULL
      && keeper->pShop != NULL && keeper->pShop->room == room 
      && IS_AWAKE( keeper ) && ch->Seen( keeper ) )
      return keeper;

  return NULL;
}


char_data* find_keeper( char_data* ch )
{
  char_data* keeper = NULL;

  for( int i = 0; ; i++ ) {
    if( i >= *ch->array ) {
      if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP )
        || is_set( &ch->in_room->room_flags, RFLAG_SHOP ) ) {
        send( ch, "The shop keeper is not around right now.\r\n" );
        return NULL;
        }
      send( ch, "You are not in a shop.\r\n" );
      return NULL;
      } 
    if( ( keeper = mob( ch->array->list[i] ) ) != NULL
      && keeper->pShop != NULL )
      break;
    }

  if( !IS_AWAKE( keeper ) ) {
    send( ch, "The shopkeeper seems to be asleep.\r\n" );
    return NULL;
    }
   
  if( !ch->Seen( keeper ) && ch->shdata->level < LEVEL_APPRENTICE ) {
    do_say( keeper, "I don't trade with folks I can't see." );
    return NULL;
    }

  if( ch->species != NULL
    && !is_set( &ch->species->act_flags, ACT_HUMANOID ) ) {
    send( ch,
      "You can't carry anything so shopping is rather pointless.\r\n" );
    return NULL;
    }

  return keeper;
}


/*
 *   DISK ROUTINES
 */


void load_shops( void )
{
  shop_data*      shop  = NULL;
  custom_data*  custom;
  FILE*             fp;
  int             i, j;

  printf( "Loading Shops...\r\n" );

  fp = open_file( SHOP_FILE, "r" );
  
  if( strcmp( fread_word( fp ), "#SHOPS" ) ) 
    panic( "Load_shops: header not found" );

  for( ; ; ) {
    if( ( i = fread_number( fp ) ) == -1 )
      break;

    if( i == 0 ) {
      custom        = new custom_data;
      custom->item  = get_obj_index( fread_number( fp ) );
      custom->cost  = fread_number( fp );
  
      for( i = 0; i < MAX_INGRED; i++ ) {
        if( ( j = fread_number( fp ) ) == 0 ) {
          fread_number( fp );
          continue;
	  }
        custom->ingred[i] = get_obj_index( j );
        custom->number[i] = fread_number( fp );
        }
 
      if( custom->item == NULL ) {
        roach( "Load_Shops: Removing null custom item." );
        delete custom;
        }
      else if( shop == NULL ) {
        roach( "Load_Shops: Custom in null shop?" );
        delete custom;
        }
      else {
        custom->next = shop->custom;
        shop->custom = custom;
        }
   
      fread_to_eol( fp );
      continue;
      }

    shop = new shop_data;

    shop->keeper  = fread_number( fp );
  
    shop->flags       = fread_number( fp );
    shop->buy_type[0] = fread_number( fp );
    shop->buy_type[1] = fread_number( fp );

    shop->repair      = fread_number( fp );
    shop->materials   = fread_number( fp );
    shop->open_hour   = fread_number( fp );
    shop->close_hour  = fread_number( fp );

    fread_to_eol( fp );
  
    if( ( shop->room = get_room_index( i ) ) == NULL ) {
      roach( "Load_Shops: Deleting shop in non-existent room %d.", i ); 
      delete shop;
      shop = NULL;
      }
    else {
      shop->next = shop_list;
      shop_list  = shop; 
      }
    }

  fclose( fp );
  return;
}


void save_shops( )
{
  shop_data*      shop;
  custom_data*  custom;
  FILE*             fp;
  int                i;

  if( ( fp = open_file( SHOP_FILE, "w" ) ) == NULL )
    return;

  fprintf( fp, "#SHOPS\n" );

  for( shop = shop_list; shop != NULL; shop = shop->next ) {
    fprintf( fp, "%5d %5d ", shop->room->vnum, shop->keeper );
    fprintf( fp, "%5d %5d %5d ", shop->flags, shop->buy_type[0],
      shop->buy_type[1] );
    fprintf( fp, "%2d %5d %5d %5d\n", shop->repair, shop->materials,
      shop->open_hour, shop->close_hour );
    for( custom = shop->custom; custom != NULL; custom = custom->next ) {
      fprintf( fp, "    0 %5d %5d ", custom->item->vnum, custom->cost );
      for( i = 0; i < MAX_INGRED; i++ )
        fprintf( fp, "%5d %2d ", ( custom->ingred[i] == NULL ? 0 :
          custom->ingred[i]->vnum ), custom->number[i] );
      fprintf( fp, "\n" );
      }
    }
  fprintf( fp, "-1\n\n#$\n\n" );
  fclose( fp );

  return;
}





