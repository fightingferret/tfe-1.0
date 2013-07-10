/*
 *   HOST ROUTINES
 */


extern link_data*   host_stack;
extern int           read_pntr;


bool   init_daemon          ( void );
void   write_host           ( link_data*, char* );
void   read_host            ( void );     
void   recover_links        ( void );
void   restart_links        ( void );


/* 
 *   BAN ROUTINES
 */


bool   is_banned            ( const char* );
bool   is_banned            ( account_data*, link_data* );


/*
 *   NETWORK ROUTINES
 */


extern link_data* link_list;


class Link_Data
{
 public:
  link_data*          next;
  link_data*      snoop_by;
  char_data*     character;
  player_data*      player;
  pfile_data*        pfile;
  account_data*    account;
  char*               host;
  int              channel;
  int            connected;
  int                 idle;
  text_data*       receive;
  text_data*          send;
  text_data*         paged;
  char*        rec_pending;
  char*       send_pending;
  char*           rec_prev;
  bool             command;
     
  Link_Data( ) {
    record_new( sizeof( link_data ), -MEM_LINK );
    rec_pending   = empty_string;
    send_pending  = empty_string;
    rec_prev      = empty_string;
    receive       = NULL;
    send          = NULL;
    paged         = NULL;
    character     = NULL;
    player        = NULL;
    pfile         = NULL;
    snoop_by      = NULL;
    host          = NULL;
    next          = NULL;
    account       = NULL;
    channel       = -1;
    idle          = 0;
    }

  ~Link_Data( ) {
    channel = -1;
    record_delete( sizeof( link_data ), -MEM_LINK );
    delete_list( receive );
    delete_list( send ); 
    delete_list( paged );
    }
};


class Text_Data
{
 public:
  text_data*    next;
  string     message;

  Text_Data( const char* text ) : message( text ) {
    record_new( sizeof( text_data ), MEM_LINK );
    record_new( message.length+1, -MEM_LINK );
    return;
    }

  ~Text_Data( ) {
    record_delete( sizeof( text_data ), MEM_LINK );
    record_delete( message.length+1, -MEM_LINK );
    return;
    }

  friend void overwrite( text_data* receive, const char* msg ) {
    int    length  = strlen( msg );
    char*     tmp  = receive->message.text;
    record_new( length-receive->message.length, -MEM_LINK );
    receive->message.length = length;
    receive->message.text   = new char [ length+1 ];
    memcpy( receive->message.text, msg, length+1 );
    delete [] tmp;
    return;
    }
};


extern const char  echo_off_str  [];
extern const char  echo_on_str   [];
extern const char  go_ahead_str  [];


extern int socket_one;
extern int socket_two;
extern int       port  [ 3 ];


int    open_port            ( int );
void   new_player           ( player_data* );
bool   check_parse_name     ( link_data*, char* );
void   prompt_ansi          ( link_data* );
void   prompt_nml           ( link_data* );
void   write_greeting       ( link_data* );
void   send_to_status       ( char_data*, char* );
void   set_playing          ( link_data* );
void   close_socket         ( link_data*, bool = FALSE );
void   extract              ( link_data* );
void   open_link            ( int );


/*
 *   LOGIN ROUTINES
 */


#define CON_ACNT_REQUEST           -8
#define CON_ACNT_MENU              -9
#define CON_ACNT_NAME             -10
#define CON_ACNT_PWD              -11
#define CON_ACNT_EMAIL            -12
#define CON_ACNT_ENTER            -13
#define CON_ACNT_CONFIRM          -14
#define CON_ACNT_CHECK            -15
#define CON_ACNT_CHECK_PWD        -16
#define CON_DIGITALNATION          -4
#define CON_POLICIES               -3
#define CON_FEATURES               -2
#define CON_PAGE                   -1
#define CON_PLAYING                 0
#define CON_INTRO                   1 
#define CON_PASSWORD_ECHO           2
#define CON_PASSWORD_NOECHO         3
#define CON_DISC_OLD                4
#define CON_GET_NEW_NAME            5
#define CON_GET_NEW_PASSWORD        6
#define CON_CONFIRM_PASSWORD        7
#define CON_SET_TERM                8
#define CON_READ_GAME_RULES        10
#define CON_AGREE_GAME_RULES       11
#define CON_GET_EMAIL              12
#define CON_HELP_SEX               13
#define CON_GET_NEW_SEX            14 
#define CON_HELP_CLSS              15
#define CON_GET_NEW_CLSS           16
#define CON_HELP_RACE              17
#define CON_GET_NEW_RACE           18
#define CON_DECIDE_STATS           19
#define CON_GET_NEW_ALIGNMENT      20
#define CON_HELP_ALIGNMENT         21
#define CON_READ_IMOTD             22
#define CON_READ_MOTD              23
#define CON_CLOSING_LINK           24
#define CON_CE_ACCOUNT             25
#define CON_CE_PASSWORD            26
#define CON_CE_EMAIL               27
#define CON_VE_ACCOUNT             28
#define CON_VE_VALIDATE            29
#define CON_VE_CONFIRM             30


extern bool  wizlock;    


inline bool Char_Data :: In_Game( )
{
  return( Is_Valid( ) && ( link == NULL
    || link->connected == CON_PLAYING ) );
}


inline bool past_password( link_data* link )
{
  return( link->connected != CON_PASSWORD_ECHO 
    && link->connected != CON_PASSWORD_NOECHO 
    && link->connected != CON_INTRO );
} 


void   press_return             ( link_data* );
bool   no_input                 ( link_data*, char* );
void   nanny                    ( link_data*, char* );
void   nanny_intro              ( link_data*, char* );
void   nanny_acnt_name          ( link_data*, char* );
void   nanny_acnt_password      ( link_data*, char* );
void   nanny_acnt_email         ( link_data*, char* );
void   nanny_acnt_enter         ( link_data*, char* );
void   nanny_acnt_confirm       ( link_data*, char* );
void   nanny_acnt_check         ( link_data*, char* );
void   nanny_acnt_check_pwd     ( link_data*, char* );
void   nanny_old_password       ( link_data*, char* );
void   nanny_motd               ( link_data*, char* );
void   nanny_imotd              ( link_data*, char* );
void   nanny_new_name           ( link_data*, char* );
void   nanny_acnt_menu          ( link_data*, char* );
void   nanny_acnt_request       ( link_data*, char* );
void   nanny_new_password       ( link_data*, char* ); 
void   nanny_confirm_password   ( link_data*, char* );     
void   nanny_set_term           ( link_data*, char* );
void   nanny_agree_rules        ( link_data*, char* );
void   nanny_show_rules         ( link_data*, char* );
void   nanny_alignment          ( link_data*, char* );
void   nanny_help_alignment     ( link_data*, char* );
void   nanny_disc_old           ( link_data*, char* );
void   nanny_help_class         ( link_data*, char* );
void   nanny_class              ( link_data*, char* );
void   nanny_help_race          ( link_data*, char* );
void   nanny_race               ( link_data*, char* );
void   nanny_stats              ( link_data*, char* );
void   nanny_help_sex           ( link_data*, char* );
void   nanny_sex                ( link_data*, char* );
void   nanny_ve_validate        ( link_data*, char* );











