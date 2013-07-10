/*
 *   ALIAS HEADERS
 */


char*  subst_alias     ( link_data*, char* );
void   ampersand       ( text_data* );


class Alias_Data
{
 public:
  char*         abbrev;
  char*         command;

  Alias_Data       ( char*, char* );
  ~Alias_Data      ( );

  friend char* name( alias_data* alias ) {
    return alias->abbrev;
    }
};


/*
 *   ATTRIBUTES
 */


extern index_data        fame_index  [];
extern index_data       piety_index  [];
extern index_data  reputation_index  [];


/*
 *   PFILE ROUTINES
 */


#define MAX_VOTE    5


class Pfile_Array
{
 public:
  int           size;
  pfile_data**  list;

  Pfile_Array( ) {
    size = 0;
    list = NULL;
    }

  ~Pfile_Array( ) {
    if( size > 0 )
      delete [] list;
    }
};


class Pfile_Data
{
 public:
  char*              name;
  char*               pwd;
  account_data*   account;
  note_data*         mail;
  char*         last_host;
  pfile_data*        vote  [ MAX_VOTE ];
  int               flags  [ 2 ];
  int            settings;
  int               trust; 
  int              deaths;
  int               level;
  int                race;
  int                clss;
  int                 sex;
  int              bounty;
  int             last_on;
  int             created;
  int               ident;
  int                 exp;
  int                rank;
  int             guesses;   
  clan_data*         clan;
  char*          homepage; 

  Pfile_Data   ( const char* );
  ~Pfile_Data  ( );

  friend char* name( pfile_data* pfile ) {
    return pfile->name;
    }

  friend int   assign_ident   ( );
};


extern pfile_data*     ident_list  [ MAX_PFILE ];
extern pfile_data**    pfile_list;
extern pfile_data**     site_list;
extern int              max_pfile;
extern int           site_entries;


inline pfile_data* get_pfile( int i )
{
  if( i < 0 || i >= MAX_PFILE )
    return NULL;

  return ident_list[i];
}


int           reference              ( player_data*, thing_array&, int );
void          dereference            ( player_data* );
void          purge                  ( player_data* );
player_data*  find_player            ( pfile_data* );
pfile_data*   find_pfile             ( const char*, char_data* = NULL );
pfile_data*   find_pfile_exact       ( const char* );
pfile_data*   find_pfile_substring   ( const char* );
pfile_data*   player_arg             ( char*& );
int           site_search            ( const char* );
void          extract                ( pfile_data*, link_data* = NULL );
void          add_list               ( pfile_data**&, int&, pfile_data* );
void          remove_list            ( pfile_data**&, int&, pfile_data* );


/*
 *   PCDATA
 */


class Pc_Data
{
 public:
  char*            title;
  char*            tmp_keywords;
  char*            tmp_short;
  char*            buffer;
  char*            prompt;
  help_data*       help_edit;
  int              message;
  int              mess_settings; 
  int              trust;
  int              clss;
  int              religion;
  int              color [ 45 ];
  int              terminal;
  int              lines;
  int              condition [ 4 ];
  int              cflags [ 4 ];
  int              speaking;
  int              piety;
  int              practice;
  int              prac_timer;
  int              quest_pts;
  int              quest_flags [ MAX_QUEST ];
  int              level_hit;
  int              level_mana;
  int              level_move;
  int              max_level; 
  int              mod_age;
  int              wimpy;
  note_data*       mail_edit;
  pfile_data*      pfile;
  recognize_data*  recognize;  
};


/* 
 *   RECOGNIZE
 */


class Recognize_Data
{
 public:
  int    size;
  int*   list;

  Recognize_Data    ( int );
  ~Recognize_Data   ( );
};


void   remove                ( recognize_data*&, int );
void   reconcile_recognize   ( char_data* );
int    search                ( recognize_data*, int );
bool   consenting            ( char_data*, char_data*,
                               const char* = empty_string );


/*
 *   REQUESTS
 */


bool  remove      ( request_array&, char_data* );


extern request_array    request_imm;
extern request_array    request_app;



