int           strcasecmp     ( const char*, const char* );
int           strncasecmp    ( const char*, const char*, int );
int           strncmp        ( const char*, const char*, int );
int           rstrcasecmp    ( const char*, const char* );
int           rstrncasecmp   ( const char*, const char*, int );
char*         strcat         ( char*, const char* );

bool          matches        ( const char*&, const char* );
bool          exact_match    ( const char*&, const char* );
bool          fmatches       ( const char*, const char*, int = 0 );
bool          number_arg     ( const char*&, int& );

bool          contains_word  ( const char*&, const char*, char* );
bool          two_argument   ( const char*&, const char*, char* );
char*         one_argument   ( const char*, char* );

int           subset         ( const char*, const char*, bool = FALSE );
int           member         ( const char*, const char*, bool = FALSE );
int           compare        ( const char*, const char*, bool = FALSE,
                               int = 3 );

bool          isvowel           ( char letter );
bool          is_name           ( char*, const char*, bool = FALSE );
const char*   break_line        ( const char*, char*, int );
const char*   word_list         ( const char**, int, bool = TRUE );
void          smash_spaces      ( char* );
char*         capitalize        ( char* );
char*         capitalize_words  ( const char* );
void          add_spaces        ( char*, int );
char*         seperate          ( char*, bool );


/*
 *  INLINE UTILITY ROUTINES
 */


inline int count( const char* s )
{
  int i;

  for( ; *s == ' '; s++ );

  for( i = 0; *s != '\0'; i++ ) {
    for( ; *s != '\0' && *s != ' '; s++ );
    for( ; *s == ' '; s++ );
    }

  return i;
}


inline int count( char* s )
{
  return count( (const char*) s );
}


inline char* truncate( char* string, int length )
{
  if( strlen( string ) > length ) 
    strcpy( string+length-3, "..." );

  return string;
}


inline void skip_spaces( char*& arg )
{
  for( ; isspace( *arg ); arg++ );
  return;
}


inline void skip_spaces( const char*& arg )
{
  for( ; isspace( *arg ); arg++ );
  return;
}


inline bool matches( const char*& argument, const char* word, bool exact )
{
  return( exact ? exact_match( argument, word )
    : matches( argument, word ) );
}


/*
 *   STATIC STRINGS
 */


extern char  static_storage  [ 10*THREE_LINES ]; 
extern int       store_pntr;


inline char* static_string( void )
{
  store_pntr = ( store_pntr+1 )%10;
  return &static_storage[store_pntr*THREE_LINES];
}


inline char* static_string( const char* msg )
{
  char* tmp  = static_string( );

  strcpy( tmp, msg );

  return tmp;
}


