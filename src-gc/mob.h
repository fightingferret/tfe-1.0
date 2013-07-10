#define ACT_IS_NPC                  0
#define ACT_SENTINEL                1
#define ACT_SCAVENGER               2
#define ACT_CAN_TAME                3
#define ACT_AGGR_ALL                4
#define ACT_STAY_AREA               5
#define ACT_WIMPY                   6
#define ACT_HUMANOID                7
#define ACT_WARM_BLOODED            8
#define ACT_SUMMONABLE              9
#define ACT_ASSIST_GROUP           10
#define ACT_CAN_FLY                11
#define ACT_CAN_SWIM               12
#define ACT_MOUNT                  13 
#define ACT_CAN_CLIMB              14
#define ACT_CAN_CARRY              15
#define ACT_HAS_EYES               16
#define ACT_HAS_SKELETON           17
#define ACT_GHOST                  18
#define ACT_ZERO_REGEN             19
#define ACT_SLITHERS               20
#define ACT_OOZES                  21
#define ACT_NO_BASH                22
#define ACT_MIMIC                  23
#define ACT_PREDATOR               24
#define ACT_MELT                   25
#define ACT_AGGR_EVIL              26
#define ACT_AGGR_GOOD              27
#define ACT_CARNIVORE              28
#define ACT_ELEMENTAL              29
#define ACT_USE_THE                30
#define MAX_ACT                    31


class Species_Data
{
 public:
  descr_data*         descr;
  mprog_data*         mprog;
  reset_data*         reset;
  share_data*        shdata;
  program_data*      attack;
  const char*       creator;
  int                  date;
  int                  vnum;
  int               hitdice;
  int              movedice;
  int                   sex;
  int                 adult;
  int              maturity;
  int              language;
  int                nation;
  int                 group;
  int           affected_by  [ 2 ];
  int                  gold;
  int                 color;
  int                  size;
  int                weight;
  int                chance  [ MAX_ARMOR ];
  int                 armor  [ MAX_ARMOR ];
  char*           part_name  [ MAX_ARMOR ];
  int             act_flags;
  int             wear_part;
  int              skeleton;
  int                zombie;
  int                corpse;
  int                 price;

  int                damage;
  int                rounds;
  int               special;
  int          damage_taken;
  int                   exp;      
  int                wander;
  int                 light;

  Species_Data( ) {
    record_new( sizeof( species_data ), MEM_SPECIES );
    mprog  = NULL;
    reset  = NULL;
    nation = NATION_NONE;
    wander = 4000;
    }

  ~Species_Data( ) {
    record_delete( sizeof( species_data ), MEM_SPECIES );
    }

  const char* Name ( );
};


bool            can_extract    ( species_data*, char_data* );
void            fix_species    ( void );
mob_data*       create_mobile  ( species_data* );


/*
 *   GET_SPECIES ROUTINE
 */


extern  species_data*  species_list [ MAX_SPECIES ];


inline species_data* get_species( int vnum )
{
  if( vnum < 0 || vnum >= MAX_SPECIES )
    return NULL;

  return species_list[ vnum ];
}


