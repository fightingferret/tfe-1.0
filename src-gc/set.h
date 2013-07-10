/*
 *   FIELD CLASSES
 */


class int_field {
 public:
  const char*    name;
  int             min;
  int             max;
  int*          value;     

  const char*    set          ( char_data*, const char*, char* );    
};


class cent_field {
 public:
  const char*    name;
  int             min;
  int             max;
  int*          value;     

  const char*    set          ( char_data*, const char*, char* );    
};


class dice_field {
 public:
  const char*     name;
  int            level;
  int*           value;

  const char*      set       ( char_data*, const char*, char* );
};


class string_field {
 public:
  const char*        name;
  int            mem_type;
  const char**      value;
  set_func*          func;

  const char*         set    ( char_data*, const char*, char* );
};


class type_field {
 public:
  const char*    name;
  int            max;
  const char**   first;
  const char**   second;
  int*           value;

  const char*              set    ( char_data*, const char*, char* );
  inline const char*   element    ( int i );
};


inline const char* type_field :: element( int i )
{
  return *(first+i*(second-first));
}; 
         

/*
 *   TEMPLATES
 */


template < class T >
const char* process( T* field, char_data* ch, const char* subject,
  char* argument )
{
  int  i;

  for( i = 0; field[i].name[0] != '\0'; i++ ) 
    if( matches( argument, field[i].name ) )
      return field[i].set( ch, subject, argument );

  return NULL;
}     


template < class T >
bool process( T* field, char_data* ch, const char* subject, char* argument,
  species_data* species, player_data* player )
{
  const char*  response;

  if( ( response = process( field, ch, subject, argument ) ) != NULL ) { 
    if( species != NULL ) {
      mob_log( ch, species->vnum, response );
      species->date = current_time;
      zero_exp( species );
      }
    else if( player != NULL ) {
      player_log( player, response );
      } 
    return TRUE;
    }

  return FALSE;
}     


template < class T >
bool process( T* field, char_data* ch, const char* subject, char* argument,
  species_data* species )
{
  return process( field, ch, subject, argument, species, NULL );
}     


template < class T >
bool process( T* field, char_data* ch, const char* subject, char* argument,
  obj_clss_data* obj )
{
  const char*  response;

  if( ( response = process( field, ch, subject, argument ) ) != NULL ) { 
    if( obj != NULL )
      obj_log( ch, obj->vnum, response );
    return TRUE;
    }

  return FALSE;
}     


/*
 *   FUNCTIONS
 */


void set_string   ( char_data*, char*, char*&, const char*, int );
void set_type     ( char_data*, char*, int&, const char*,
                    const char*, int,
                    const char**, const char** = NULL );












