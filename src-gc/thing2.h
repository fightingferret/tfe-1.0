/*
 *   LOCATION
 */


inline char* location( room_data* room )
{
  char* tmp = static_string( );

  sprintf( tmp, "In room %d", room->vnum );

  return tmp;
}


inline char* location( obj_data* obj )
{
  char* tmp = static_string( );

  sprintf( tmp, "In %s", obj->Seen_Name( NULL ) );

  return tmp;
}


inline char* location( char_data* ch )
{
  char* tmp = static_string( );

  sprintf( tmp, "Carried by %s", ch->descr->name );  

  return tmp;
}


inline char* location( locker_data* locker )
{
  char* tmp = static_string( );

  sprintf( tmp, "In %s's locker", locker->owner->descr->name );

  return tmp;
}


inline char* location( thing_data* thing ) 
{
  if( thing == NULL ) 
    return static_string( "Nowhere!?" );

  switch( thing->type( ) ) {
    case ROOM_DATA   :  return location( (room_data*)   thing );
    case OBJ_DATA    :  return location( (obj_data*)    thing );
    case LOCKER_DATA :  return location( (locker_data*) thing );
    case MOB_DATA    :
    case CHAR_DATA   :
    case PLAYER_DATA :   
    case WIZARD_DATA :  return location( (char_data*)   thing );
    }

  return static_string( "In something unknown?!" );
}


