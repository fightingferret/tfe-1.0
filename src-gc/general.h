extern int      boot_stage;
extern bool       tfe_down;
extern bool   in_character;


/*
 *   INDEX ROUTINES
 */


class Index_Data
{
 public:
   const char*  singular;
   const char*    plural;
   int             value;
};


const char*  lookup     ( index_data*, int, bool = FALSE );


/*
 *   HELP ROUTINES
 */


help_data*  find_help    ( char_data*, const char* );


/*
 *   INFO ROUTINES
 */


void   info   ( const char*, int, const char*, int, int = 3,
                char_data* = NULL, clan_data* = NULL );


/*
 *   NOTE/MAIL ROUTINES
 */


void   recent_notes     ( char_data* );
void   mail_message     ( char_data* );


/*
 *   STRING FUNCTIONS
 */


#define strlen( string )    int( strlen( string ) )


class String
{
 public:
  char*      text;
  int      length;

  String( const char* msg ) {
    length = strlen( msg );
    text   = new char [ length+1 ];
    memcpy( text, msg, length+1 );
    return;
    }

  ~String( ) {
    delete [] text;
    return;
    }
};


inline const char* name( char* word )
{
  return word;
} 


inline const char* name( const char* word )
{
  return word;
}


/*
 *   THING TYPES
 */


#define THING_DATA     0
#define AUCTION_DATA   1
#define EXIT_DATA      2
#define EXTRA_DATA     3
#define ROOM_DATA      4
#define OBJ_DATA       5
#define CHAR_DATA      6
#define MOB_DATA       7
#define PLAYER_DATA    8
#define WIZARD_DATA    9


/*
 *   TRUST
 */


int    get_trust      ( char_data* );
