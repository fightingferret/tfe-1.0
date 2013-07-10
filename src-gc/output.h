/*
 *
 */


void  fsend         ( char_data*, const char* );  
void  fpage         ( char_data*, const char* );  

extern const char*   scroll_line     [ 3 ];


/*
 *   ACT ROUTINES
 */


#define cc const char
#define cd char_data
#define od obj_data
#define vd visible_data

void  act_print     ( char*, cc*, vd*, vd*, vd*, vd*, cc*, cc*, cd* );

void  act           ( cd*, cc*, vd*, vd* = NULL, vd* = NULL, vd* = NULL );
void  act           ( cd*, cc*, vd*, vd*, cc*, cc* = NULL ); 
void  act_notchar   ( cc*, cd*, vd* = NULL, vd* = NULL );
void  act_notchar   ( cc*, cd*, cc*, cc* );
void  act_notchar   ( cc*, cd*, vd*, cc*, cc* );
void  act_room      ( room_data*, cc*, cc* = NULL, cc* = NULL );
void  act_room      ( room_data*, cc*, vd*, vd* = NULL );
void  act_area      ( cc*, cd*, cd*, vd* );
void  act_neither   ( cc*, cd*, cd*, vd*, vd* = NULL );

#undef cc
#undef cd
#undef od


/*
 *   GENERIC DISPLAY ROUTINES
 */


void  display_array  ( char_data*, const char*, const char**,
                           const char**, int );

 
/*
 *   STRING CONVERSION ROUTINES
 */


inline const char* tostring( visible_data* visible, char_data* ch )
{
  return visible->Name( ch, visible->shown );
}


inline const char* tostring( species_data* species, char_data* )
{
  return species->Name( );
}


inline const char* tostring( obj_clss_data* obj_clss, char_data* )
{
  return obj_clss->Name( );
}


inline const char* tostring( thing_array* array, char_data* ch )
{  
  return list_name( ch, array );
}
  

inline int         tostring  ( int a, ... )          { return a; } 
inline float       tostring  ( float a, ... )        { return a; } 
inline double      tostring  ( double a, ... )       { return a; } 
inline const char* tostring  ( const char* a, ... )  { return a; } 


/*
 *   ECHO
 */


void   echo                 ( const char* );


template < class T >
void echo( const char* text, T item )
{
  char tmp  [ TWO_LINES ];

  sprintf( tmp, text, item );
  echo( tmp );

  return;
};


/*
 *   PAGE
 */


void    page              ( char_data*, const char* );
void    page_centered     ( char_data*, const char* );
void    page_underlined   ( char_data*, const char* );
void    next_page         ( link_data* );


template < class T >
void page( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }
};


template < class S, class T >
void page( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }
};


template < class S, class T, class U >
void page( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }
};


template < class S, class T, class U, class V >
void page( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text,
      tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }

  return;
};


template < class S, class T, class U, class V, class X >
void page( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4, X item5 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text,
      tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ),
      tostring( item5, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }

  return;
};


template < class S, class T, class U, class V, class X, class W >
void page( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4, X item5, W item6 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text,
      tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ),
      tostring( item5, ch ), tostring( item6, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page( ch, tmp );
    }

  return;
};


inline void page_header( char_data* ch, const char* text )
{
  link_data*   link;
  text_data*  paged;

  if( ch != NULL && ( link = ch->link ) != NULL ) {
    paged       = link->paged;
    link->paged = NULL; 
    page( ch, text );
    cat( link->paged, paged );
    }

  return;
}


template < class T >
void page_header( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    page_header( ch, tmp );
    }

  return;
};


/*
 *   PAGE_UNDERLINED
 */


template < class T >
void page_underlined( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item, ch ) );
    page_underlined( ch, tmp );
    }
};


template < class S, class T >
void page_underlined( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
    page_underlined( ch, tmp );
    }

  return;
};


/*
 *   PAGE_CENTERED
 */


template < class S >
void page_centered( char_data* ch, const char* text, S item )
{
  char tmp  [ TWO_LINES ];

  sprintf( tmp, text, tostring( item, ch ) );
  page_centered( ch, tmp );
}


/*
 *   PAGE_DIVIDER
 */


inline void page_divider( char_data* ch, const char* text, int i )
{
  char tmp  [ TWO_LINES ];

  page( ch, "\n\r" );
  sprintf( tmp, "-- %s (%d) --",  text, i );
  tmp[3] = toupper( tmp[3] );
  page_centered( ch, tmp );
  page( ch, "\n\r" );
}


/*
 *   FORMATTED PAGE
 */


template < class T >
void fpage( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


template < class S, class T >
void fpage( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


template < class S, class T, class U >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


template < class S, class T, class U, class V >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


template < class S, class T, class U, class V, class X >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4, X item5 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ),
      tostring( item5, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


template < class S, class T, class U, class V, class X, class Y >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4, X item5, Y item6 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ),
      tostring( item5, ch ), tostring( item6, ch ) );
    if( *text == '%' )
      *tmp = toupper( *tmp );
    fpage( ch, tmp );
    }

  return;
};


/*
 *   SEND
 */


void    send              ( link_data*, const char* );


inline void write_to_buffer( link_data* link, const char* message )
{
  send( link, message );
  return;
}


inline void send( char_data* ch, const char* text )
{
  if( ch != NULL )
    send( ch->link, text );

  return;
}


inline void send( const char* text, char_data* ch )
{
  send( ch, text );
  return;
}


template < class T >
inline void send( link_data* link, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, item );
  corrupt( tmp, SIX_LINES, "Send" );
  send( link, tmp );

  return;
}


template < class T >
void send( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


template < class S, class T >
void send( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


template < class S, class T, class U >
inline void send( link_data* link, const char* text, S item1, T item2,
  U item3 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, item1, item2, item3 );
  corrupt( tmp, SIX_LINES, "Send" );
  send( link, tmp );

  return;
}


template < class S, class T, class U >
void send( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char tmp  [ SIX_LINES ];

  if( ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


template < class S, class T, class U, class V >
void send( link_data* link, const char* text, S item1, T item2, U item3,
  V item4 )
{
  send( link->player, text, item1, item2, item3, item4 );
  return;
}


template < class S, class T, class U, class V >
void send( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4 )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


template < class S, class T, class U, class V, class W >
void send( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4, W item5 )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


template < class S, class T, class U, class V, class W, class X >
void send( char_data* ch, const char* text, S item1, T item2, U item3,
  V item4, W item5, X item6 )
{
  char tmp  [ SIX_LINES ];

  if( ch != NULL && ch->link != NULL ) {
    sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
      tostring( item3, ch ), tostring( item4, ch ),
      tostring( item5, ch ), tostring( item6, ch ) );
    corrupt( tmp, SIX_LINES, "Send" );
    send( ch, *text == '%' ? capitalize( tmp ) : tmp );
    }

  return;
};


/*
 *   SEND_SEEN
 */


template < class T >
void send_seen( char_data* ch, const char* text, T item )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && ch->Seen( rch ) && rch != (char_data*) item )
        send( rch, text, item );

  return;
};


template < class S, class T >
void send_seen( char_data* ch, const char* text, S item1, T item2 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && ch->Seen( rch )
        && rch != (char_data*) item1 && rch != (char_data*) item2 )
        send( rch, text, item1, item2 );

  return;
};


template < class S, class T, class U >
void send_seen( char_data* ch, const char* text, S item1, T item2,
  U item3 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && ch->Seen( rch )
        && rch != (char_data*) item1 && rch != (char_data*) item2
        && rch != (char_data*) item3 )
        send( rch, text, item1, item2, item3 );

  return;
};


template < class S, class T, class U, class V >
void send_seen( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && ch->Seen( rch )
        && rch != (char_data*) item1 && rch != (char_data*) item2
        && rch != (char_data*) item3 && rch != (char_data*) item4 )
        send( rch, text, item1, item2, item3, item4 );

  return;
};


/*
 *   SEND_ROOM
 */


inline void send( thing_array& array, const char* text )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING )
      send( rch, text );

  return;
};


template < class T >
void send( thing_array& array, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item )
      send( rch, text, item );

  return;
};


template < class S, class T >
void send( thing_array& array, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 )
      send( rch, text, item1, item2 );
    }

  return;
};


template < class S, class T, class U >
void send( thing_array& array, const char* text, S item1, T item2,
  U item3 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3 )
      send( rch, text, item1, item2, item3 );
    }

  return;
};


template < class S, class T, class U, class V  >
void send( thing_array& array, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL 
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 )
      send( rch, text, item1, item2, item3, item4  );
    }

  return;
};


template < class S, class T, class U, class V, class W >
void send( thing_array& array, const char* text, S item1, T item2,
  U item3, V item4, W item5 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 && rch != (char_data*) item5 )
      send( rch, text, item1, item2, item3, item4, item5 );
    }

  return;
};


template < class S, class T, class U, class V, class W, class X >
void send( thing_array& array, const char* text, S item1, T item2,
  U item3, V item4, W item5, X item6 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 && rch != (char_data*) item5
      && rch != (char_data*) item6 )
      send( rch, text, item1, item2, item3, item4, item5, item6 );
    }

  return;
};


/*
 *   SEND_UNDERLINED
 */


void send_underlined( char_data*, const char* );


template < class S >
void send_underlined( char_data* ch, const char* text, S item )
{
  char tmp  [ TWO_LINES ];

  sprintf( tmp, text, tostring( item, ch ) );
  send_underlined( ch, tmp );

  return;
}


/*
 *   SEND_CENTERED
 */


void send_centered( char_data*, const char* );


template < class S >
void send_centered( char_data* ch, const char* text, S item )
{
  char tmp  [ TWO_LINES ];

  sprintf( tmp, text, tostring( item, ch ) );
  send_centered( ch, tmp );

  return;
}


/*
 *   FSEND_ALL
 */


template < class T >
void fsend_all( room_data* room, const char* text, T item )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ ) 
    if( ( rch = character( room->contents[i] ) ) != NULL
      && rch->position > POS_SLEEPING )
      fsend( rch, text, item );

  return;
}


/*
 *   FORMATTED SEND 
 */


template < class T >
void fsend( char_data* ch, const char* text, T item )
{
  char tmp  [ SIX_LINES ];
 
  sprintf( tmp, text, tostring( item, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );

  return;
} 


template < class S, class T >
void fsend( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );
};


template < class S, class T, class U >
void fsend( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
    tostring( item3, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );
};


template < class S, class T, class U, class V >
void fsend( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
    tostring( item3, ch ), tostring( item4, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );
};


template < class S, class T, class U, class V, class W >
void fsend( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4, W item5 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
    tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );
};


template < class S, class T, class U, class V, class W, class X >
void fsend( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4, W item5, X item6 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ),
    tostring( item3, ch ), tostring( item4, ch ),
    tostring( item5, ch ), tostring( item6, ch ) );
  corrupt( tmp, SIX_LINES, "Fsend" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  fsend( ch, tmp );
};


/*
 *   FORMATTED SEND ROOM
 */


template < class T >
void fsend( thing_array& array, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item )
      fsend( rch, text, item );

  return;
} 


template < class S, class T >
void fsend( thing_array& array, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL  
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 )
      fsend( rch, text, item1, item2 );
    }

  return;
} 


template < class S, class T, class U >
void fsend( thing_array& array, const char* text, S item1, T item2,
  U item3 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3 )
      fsend( rch, text, item1, item2, item3 );
    }

  return;
} 


template < class S, class T, class U, class V >
void fsend( thing_data& array, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 )
      fsend( rch, text, item1, item2, item3, item4 );
    }

  return;
} 


template < class S, class T, class U, class V, class W >
void fsend( thing_array& array, const char* text, S item1, T item2,
  U item3, V item4, W item5 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 && rch != (char_data*) item5 )
      fsend( rch, text, item1, item2, item3, item4, item5 );
    }
} 


template < class S, class T, class U, class V, class W, class X >
void fsend( thing_array& array, const char* text, S item1, T item2,
  U item3, V item4, W item5, X item6 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) {
    if( ( rch = character( array[i] ) ) != NULL
      && rch->position > POS_SLEEPING && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 && rch != (char_data*) item5
      && rch != (char_data*) item6 )
      fsend( rch, text, item1, item2, item3, item4, item5, item6 );
    }
} 


/*
 *   FORMATTED_SEND_SEEN
 */


template < class T >
void fsend_seen( char_data* ch, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && ch->Seen( rch ) && rch != (char_data*) item )
      fsend( rch, text, item );
    }
};


template < class S, class T >
void fsend_seen( char_data* ch, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && ch->Seen( rch )
      && rch != (char_data*) item1 && rch != (char_data*) item2 )
      fsend( rch, text, item1, item2 );
    }
};


template < class S, class T, class U >
void fsend_seen( char_data* ch, const char* text, S item1, T item2,
  U item3 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3 )
      fsend( rch, text, item1, item2, item3 );
     }
};


template < class S, class T, class U, class V >
void fsend_seen( char_data* ch, const char* text, S item1, T item2,
  U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULl
      && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1
      && rch != (char_data*) item2 && rch != (char_data*) item3
      && rch != (char_data*) item4 )
      fsend( rch, text, item1, item2, item3, item4 );
    }
};


/*
 *   SEND_COLOR
 */


template < class T >
inline void send_color( char_data* ch, int color, const char* text, T item )
{
  char tmp  [ SIX_LINES ];
 
  sprintf( tmp, text, tostring( item, ch ) );
  send_color( ch, color, tmp );
}


template < class S, class T >
inline void send_color( char_data* ch, int color, const char* text,
  S item1, T item2 )
{
  char tmp  [ SIX_LINES ];
 
  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
  send_color( ch, color, tmp );
}


/*
 *   TITLE
 */


void  page_title    ( char_data*, const char* );
void  send_title    ( char_data*, const char* );


template < class S >
inline void page_title( char_data* ch, const char* text, S item )
{
  char tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, ch ) );
  page_title( ch, tmp );
  
  return;
}


template < class S >
inline void send_title( char_data* ch, const char* text, S item )
{
  char tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, ch ) );
  send_title( ch, tmp );
  
  return;
}
