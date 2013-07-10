/*
 *   FILE LOCATIONS
 */


#define AREA_DIR           "area/"
#define AREA_PREV_DIR      "prev/area/"
#define BACKUP_DIR         "reimb/"
#define CLAN_DIR           "clans/"
#define CLAN_NOTE_DIR      "notes/clans/"
#define IMMORTAL_LOG_DIR   "logs/immortal/"
#define MOB_LOG_DIR        "logs/mob/"
#define MAIL_DIR           "mail/"
#define NOTE_DIR           "notes/"
#define OBJ_LOG_DIR        "logs/object/"
#define PLAYER_DIR         "player/"  
#define PLAYER_LOG_DIR     "logs/player/" 
#define PLAYER_PREV_DIR    "prev/player/"
#define ROOM_DIR           "rooms/"
#define ROOM_PREV_DIR      "prev/rooms/" 
#define ROOM_LOG_DIR       "logs/room/"
#define TABLE_DIR          "tables/"
#define W3_DIR             "www/html/"

#define ACCOUNT_FILE     "files/accounts"
#define AREA_LIST        "area/area.lst"   
#define BADNAME_FILE     "files/badname.txt"
#define BANNED_FILE      "files/banned.txt"   
#define BUG_FILE         "files/bugs.txt"   
#define FACTION_FILE     "area/factions.dat"   
#define HELP_FILE        "area/help.are"
#define IMM_LOG_FILE     "files/immortal.log"
#define LOAD_FILE        "area/load.dat"
#define NULL_FILE        "/dev/null"  
#define MOB_FILE         "mob.mob"
#define OBJECT_FILE      "obj.obj"
#define PLAYER_LOG_FILE  "files/player.log" 
#define ABUSE_LOG_FILE   "files/abuse.log"
#define QUEST_FILE       "area/quest"
#define RTABLE_FILE      "files/rtables"
#define SHOP_FILE        "area/shop.dat"
#define TRAINER_FILE     "area/trainer.dat"
#define TYPO_FILE        "files/typos.txt" 
#define WORD_FILE        "files/words"


/*
 *   BUG FUNCTIONS
 */


void   panic       ( const char* );
void   bug         ( int, const char* );


#define BUG_APHID   3
#define BUG_BEETLE  2
#define BUG_ROACH   1


inline void aphid  ( const char* text )   {  bug( 3, text ); } 
inline void beetle ( const char* text )   {  bug( 2, text ); } 
inline void roach  ( const char* text )   {  bug( 1, text ); }
inline void bug    ( const char* text )   {  bug( 2, text ); }


template < class T >
void aphid( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item ) );
  corrupt( tmp, TWO_LINES, "Aphid" );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 3, tmp );

  return;
}


template < class T >
void roach( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, NULL ) );
  corrupt( tmp, TWO_LINES, "Roach" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 1, tmp );

  return;
}


template < class S, class T >
void roach( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item1 ), tostring( item2 ) );
  corrupt( tmp, TWO_LINES, "Roach" );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 1, tmp );

  return;
}


template < class T >
void bug( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, NULL ) );
  corrupt( tmp, TWO_LINES, "Bug" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( tmp );

  return;
}


template < class S, class T >
void bug( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item1, NULL ),
    tostring( item2, NULL ) );
  bug( tmp );
}


template < class T >
void panic( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, item );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  panic( tmp );
}


template < class S, class T >
void panic( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, item1, item2 );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  panic( tmp );
}


inline void corrupt( const char* msg, int length, const char* func )
{
  if( int( strlen( msg ) ) >= length ) 
    roach( "%s: Randomly corrupting memory.", func );
}


/*
 *   FILE ROUTINES
 */


FILE*   open_file         ( const char*, const char*, const char*,
                            bool = FALSE );
FILE*   open_file         ( const char*, const char*,
                            bool = FALSE ); 
void    delete_file       ( const char*, const char*, bool = TRUE );
void    rename_file       ( const char*, const char*,
                            const char*, const char* );

char    fread_letter      ( FILE *fp );
int     fread_number      ( FILE *fp );
void    fread_to_eol      ( FILE *fp );
char*   fread_block       ( FILE* );
char*   fread_string      ( FILE*, int );
char*   fread_word        ( FILE* );


void write_all  ( bool = FALSE );


/*
 *   LOG ROUTINES
 */


void   immortal_log       ( char_data*, char*, char* );
void   mob_log            ( char_data*, int, const char* ); 
void   obj_log            ( char_data*, int, const char* ); 
void   room_log           ( char_data*, int, const char* ); 
void   player_log         ( char_data*, const char* );


template < class S >
void player_log( char_data* ch, const char* text, S item1 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ) );
  player_log( ch, tmp );

  return;
}

  
template < class S, class T >
void player_log( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
  player_log( ch, tmp );

  return;
}


/*
 *   HELP ROUTINES
 */


bool   save_help          ( char_data* = NULL );
  

/*
 *   QUITTING ROUTINES
 */


void   forced_quit        ( player_data*, bool = FALSE );


/*
 *   WEBPAGE ROUTINES
 */


void w3_help  ( void );
void w3_who   ( void );





