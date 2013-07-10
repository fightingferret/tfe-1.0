/*
 *  CHARACTER FUNCTIONS 
 */


#define IB  inline bool
#define CD  char_data* ch

IB is_mage( CD )      { return ch->pcdata->clss == CLSS_MAGE; }
IB is_cleric( CD )    { return ch->pcdata->clss == CLSS_CLERIC; }

#undef CD


inline const char* char_data :: seen_pers( char_data* victim )
{
  if( is_set( affected_by, AFF_HALLUCINATE ) )
    return fake_mob[ number_range( 0, MAX_FAKE_MOB-1 ) ];

  if( *victim->pet_name != '\0' && victim->leader == this )
    return victim->pet_name; 

  if( victim->species != NULL || Recognizes( victim ) )
    return victim->descr->name;

  return victim->descr->short_descr;
}


inline const char* char_data :: pers( char_data* victim )
{
  return can_see( victim ) ? seen_pers( victim ) : "someone";
}


inline const char* who_pers( char_data* ch, char_data* victim )
{
  return can_see_who( ch, victim ) ? ch->seen_pers( victim ) : "someone";
}


/*
 *  OBJECT FUNCTIONS
 */


#define OB  obj_data* obj
#define OE  obj->extra_flags

IB is_identified( OB )     { return is_set( OE, OFLAG_IDENTIFIED ); }

#undef IB
#undef OB
#undef OE



