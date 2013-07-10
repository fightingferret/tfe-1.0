/*
 *   ATTACK ROUTINES
 */


#define ATT_PHYSICAL     0
#define ATT_FIRE         1
#define ATT_COLD         2
#define ATT_ACID         3
#define ATT_SHOCK        4
#define MAX_ATTACK       5


#define ATT_LEAP         0
#define ATT_DEFAULT      1
#define ATT_KICK         2
#define ATT_PUNCH        3
#define ATT_BITE         4
#define ATT_SPIN_KICK    5
#define ATT_BASH         6
#define ATT_BACKSTAB     7
#define ATT_FLEE         8
#define ATT_GARROTE      9
#define ATT_CHARGE      10
#define ATT_GOUGE       11
#define ATT_MELEE       12


bool         attack         ( char_data*, char_data*, const char*, obj_data*,
                              int, int, int = ATT_PHYSICAL );
bool         is_entangled   ( char_data*, const char* );
char_data*   get_victim     ( char_data*, char*, char* );
const char*  get_attack     ( char_data*, obj_data* );

void         add_round      ( species_data*, int );
int          initiative     ( char_data* );
int          response       ( char_data* );
int          player_round   ( char_data*, char_data* );
int          mob_round      ( char_data*, char_data* );
int          bash_attack    ( char_data*, char_data* );
int          gouge_attack   ( char_data*, char_data* );

void         init_attack    ( char_data*, char_data* = NULL );
void         react_attack   ( char_data*, char_data* );
void         renter_combat  ( char_data* ); 
void         stop_fight     ( char_data* );
void         set_attack     ( char_data*, char_data* );
bool         jump_feet      ( char_data* );
void         leap_message   ( char_data*, char_data* );
void         fight_round    ( char_data* );

bool         can_kill       ( char_data*, char_data* = NULL, bool = TRUE );
bool         can_pkill      ( char_data*, char_data* = NULL, bool = TRUE );
bool         in_sanctuary   ( char_data*, bool = TRUE );
char_data*   opponent       ( char_data* );
char_data*   has_enemy      ( char_data* );


/*
 *   DAMAGE ROUTINES
 */


const char*  condition_short    ( char_data* );
const char*  condition_short    ( char_data*, char_data* );
const char*  word_physical      ( int );


#define cd char_data

bool  damage_fire        ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_cold        ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_mind        ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_physical    ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_shock       ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_magic       ( cd*, cd*, int, const char*, bool = FALSE );
bool  damage_acid        ( cd*, cd*, int, const char*, bool = FALSE );
bool  inflict            ( cd*, cd*, int, char* );

void  dam_message        ( cd*, cd*, int, const char*, const char* );
void  dam_local          ( cd*, cd*, int, const char*, const char* );

#undef cd

extern index_data  physical_index  [];
extern index_data  electric_index  [];
extern index_data      fire_index  [];
extern index_data      cold_index  [];
extern index_data      acid_index  [];


/*
 *   DEATH
 */


void        death_message      ( char_data* );


/*
 *   EXP ROUTINES
 */


void   gain_exp       ( char_data*, char_data*, int, const char* = NULL );
void   add_exp        ( char_data*, int, const char* = NULL );
int    exp_for_level  ( int );
int    death_exp      ( char_data*, char_data* );
void   disburse_exp   ( char_data* );
void   group_gain     ( char_data*, char_data*, float );


inline int exp_for_level( char_data* ch )
{
  return exp_for_level( ch->shdata->level ); 
}


/*
 *   ENEMY ROUTINES
 */


bool        is_enemy           ( char_data*, char_data* );
int         damage_done        ( char_data*, char_data* );
void        add_enemy          ( char_data*, char_data* );
void        clear_enemies      ( char_data* );
void        extract            ( enemy_data*& );
void        record_damage      ( char_data*, char_data*, int );
void        share_enemies      ( char_data*, char_data* );


/*
 *   FLEE ROUTINES
 */


bool check_wimpy     ( char_data* );
bool attempt_flee    ( char_data*, exit_data* = NULL );


/*
 *   WEAPONS
 */


inline const char* weapon_class( obj_clss_data* obj )
{
  if( obj->value[3] >= 0 && obj->value[3] < MAX_WEAPON )
    return skill_table[ WEAPON_UNARMED+obj->value[3] ].name;

  return "none";
}


/*
 *   SPAM ROUTINES
 */


void         spam_char          ( char_data*, const char* );
void         spam_room          ( const char*, char_data*, char_data* );


template < class T >
void spam_char( char_data* ch, const char* text, T item )
{
  if( ch->link == NULL || !is_set( &ch->pcdata->message, MSG_MISSES ) )
    return;

  send( ch, text, item );

  return;  
}


template < class S, class T >
void spam_char( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch->link == NULL || !is_set( &ch->pcdata->message, MSG_MISSES ) )
    return;

  send( ch, text, item1, item2 );

  return;  
}


template < class S, class T, class U >
void spam_char( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  if( ch->link == NULL || !is_set( &ch->pcdata->message, MSG_MISSES ) )
    return;

  send( ch, text, item1, item2, item3 );

  return;  
}











