/*
 *   PET ROUTINES
 */


bool   has_mount         ( char_data* );
bool   has_elemental     ( char_data* );
int    undead_pets       ( char_data* );
int    number_of_pets    ( char_data* );
int    pet_levels        ( char_data* );


inline bool is_pet( char_data* ch )
{
  return( ch->pcdata == NULL && is_set( &ch->status, STAT_PET ) );
}  


/*
 *   FOLLOWING ROUTINES
 */


char_array*  followers       ( char_data*, content_array* = NULL );
void         add_follower    ( char_data*, char_data*, bool = TRUE ); 
void         stop_follower   ( char_data* );


/*
 *   GROUP ROUTINES
 */


int min_group_move        ( char_data* );


inline char_data* group_leader( char_data* ch )
{
  if( !is_set( &ch->status, STAT_IN_GROUP ) )
    return NULL;

  for( ; ch->leader != NULL; ch = ch->leader );

  return ch;
}
