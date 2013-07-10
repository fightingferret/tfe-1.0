inline const char* liquid_name( int i, bool known )
{
  if( i < 0 || i >= MAX_LIQUID ) {
    roach( "Liquid_Name: Impossible liquid." );   
    return "non-existent liquid";
    }

  return( known ? liquid_table[i].name : liquid_table[i].color );
}


inline const char* liquid_name( obj_data* obj )
{
  int i  =  obj->value[2];

  if( i < 0 || i >= MAX_LIQUID ) {
    roach( "Liquid_Name: Impossible liquid." );   
    return "non-existent liquid";
    }

  return( is_set( obj->extra_flags, OFLAG_KNOWN_LIQUID ) 
    ? liquid_table[i].name : liquid_table[i].color );
}


bool react_filled   ( char_data*, obj_data*, int );
