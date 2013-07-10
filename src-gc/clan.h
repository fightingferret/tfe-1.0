/*
 *   ALIGNMENT
 */


#define ALGN_LAWFUL_GOOD            0
#define ALGN_LAWFUL_NEUTRAL         1
#define ALGN_LAWFUL_EVIL            2
#define ALGN_NEUTRAL_GOOD           3
#define ALGN_PURE_NEUTRAL           4
#define ALGN_NEUTRAL_EVIL           5
#define ALGN_CHAOTIC_GOOD           6
#define ALGN_CHAOTIC_NEUTRAL        7
#define ALGN_CHAOTIC_EVIL           8
#define MAX_ALIGNMENT               9


extern flag_data         align_flags;
extern flag_data     abv_align_flags;


#define ib      inline bool
#define align   ch->shdata->alignment%3

ib is_good    ( char_data* ch ) { return( align == 0 ); }
ib is_neutral ( char_data* ch ) { return( align == 1 ); }
ib is_evil    ( char_data* ch ) { return( align == 2 ); }

#undef ib
#undef align


/*
 *   TITLES
 */


#define TITLE_SET_FLAGS       0
#define TITLE_EDIT_CHARTER    1
#define TITLE_RECRUIT         2
#define TITLE_REMOVE_NOTES    3
#define MAX_TITLE             4


class Title_Data
{
 public:
  char*          name;
  int           flags  [ 2 ];
  pfile_data*   pfile;

  Title_Data( char* s1, pfile_data* p1 ) {
    name     = alloc_string( s1, MEM_CLAN );
    flags[0] = 0;
    flags[1] = 0;
    pfile    = p1;
    }

  Title_Data( ) {
    name     = empty_string;
    flags[0] = 0;
    flags[1] = 0;
    pfile    = NULL;
    }

  ~Title_Data( ) {
    free_string( name, MEM_CLAN );
    }
};


title_data*    get_title         ( pfile_data* );


class Title_Array
{
 public:
  int           size;
  title_data**  list;

  Title_Array( ) {
    size = 0;
    list = NULL;
    }

  ~Title_Array( ) {
    if( size > 0 ) 
      delete [] list;
    }
};


/*
 *   CLAN CLASS
 */


#define CLAN_APPROVED         0
#define CLAN_KNOWN            1
#define CLAN_PUBLIC           2
#define MAX_CLAN_FLAGS        3


extern clan_data**    clan_list;
extern int             max_clan; 


class Clan_Data
{
 public:
  char*              name;
  char*            abbrev;
  char*           charter;
  int               flags  [ 2 ];
  int           min_level;
  int             classes;
  int               races;
  int          alignments;
  int               sexes;
  int                date;
  bool           modified;
  title_array      titles;
  pfile_array     members;
  note_data**   note_list;
  int            max_note;

  Clan_Data( char* s1 ) {
    record_new( sizeof( clan_data ), MEM_CLAN );
    abbrev      = alloc_string( s1, MEM_CLAN );
    name        = empty_string;
    charter     = empty_string;
    flags[0]    = 0;
    flags[1]    = 0;
    min_level   = 5;
    classes     = 0;
    races       = 0;
    alignments  = 0;
    sexes       = 0;
    max_note    = 0;
    modified    = FALSE;
    note_list   = NULL;
    date        = current_time;
    insert( clan_list, max_clan, this, max_clan );
    }

  ~Clan_Data( ) {
    record_delete( sizeof( clan_data ), MEM_CLAN );
    free_string( name, MEM_CLAN );
    free_string( abbrev, MEM_CLAN );
    free_string( charter, MEM_CLAN );
    }

  friend const char* name( clan_data* clan ) {
    return( clan->name == empty_string ? clan->abbrev
      : clan->name );
    }
};


inline bool same_clan( char_data* c1, char_data* c2 )
{
  return( c1->pcdata->pfile->clan != NULL
    && c1->pcdata->pfile->clan == c2->pcdata->pfile->clan );
}


inline bool knows_members( char_data* ch, clan_data* clan )
{
  return( ch->pcdata->pfile->clan == clan
    || ( is_set( clan->flags, CLAN_APPROVED )
    && is_set( clan->flags, CLAN_PUBLIC ) ) 
    || has_permission( ch, PERM_CLANS ) );
}  


void        add_member      ( clan_data*, pfile_data* );
void        remove_member   ( pfile_data* );
void        remove_member   ( player_data* );
void        save_clans      ( clan_data* = NULL );
void        load_clans      ( void );
clan_data*  find_clan       ( char_data*, char* );


