/*
 *   GAME PARAMETERS
 */

#ifndef __DEFINE_H__
#define __DEFINE_H__

#ifdef TRUE
#undef TRUE
#undef FALSE
#endif

#define TRUE                                 1
#define FALSE                                0
#define MAX_INTEGER                      64000
#define M_PI            3.14159265358979323846
#define M_E              2.7182818284590452354

#define TFE_PORT_NUMBER           23
#define TFE_DEBUG_PORT_NUMBER   4000

#define MAX_STRING_LENGTH       4096
#define MAX_INPUT_LENGTH         320
#define ONE_LINE                  80
#define TWO_LINES                160
#define THREE_LINES              240
#define FOUR_LINES               320
#define FIVE_LINES               400
#define SIX_LINES                480
#define EIGHT_LINES              640
  
#define MAX_PFILE               5000

#define NOWHERE                   -1
#define UNLEARNT                   0


/*
 *  ROOM & MOB NUMBERS OF INTEREST
 */

#define NUM_TOAD                   93
#define MOB_BLANK                 133
#define MOB_CLAY_GOLEM            842
#define MOB_DEMON                3500


/*
 *   DEFINITIONS
 */


#define MAX_PLYR_RACE              13
#define MAX_SPELL                 133
#define MAX_WEAPON                 11
#define MAX_SPELL_WAIT              5
#define MAX_ARMOR                   5
#define MAX_PALETTE                 6
#define MAX_CFLAG                   4
#define MAX_INGRED                  5
#define MAX_SPECIES              5000
#define MAX_TRAIN                   8
#define MAX_QUEST                 128
#define MAX_OBJ_INDEX            7500 
#define MAX_FAKE_MOB               29


#define ATT_SPELL                   1
#define ATT_YELL_HELP               2   


#define COPPER                      0
#define SILVER                      1 
#define GOLD                        2
#define PLATINUM                    3
#define MAX_COIN                    4


#define COND_ALCOHOL                0
#define COND_FULL                   1
#define COND_THIRST                 2
#define COND_DRUNK                  3

#define CONT_CLOSEABLE              0
#define CONT_PICKPROOF              1
#define CONT_CLOSED                 2
#define CONT_LOCKED                 3
#define CONT_HOLDING                4
#define MAX_CONT                    5


#define FORMAT_NORMAL               0
#define FORMAT_BOLD                 1
#define FORMAT_REVERSE              2
#define FORMAT_UNDERLINE            3
#define FORMAT_RED                  4
#define FORMAT_GREEN                5
#define FORMAT_YELLOW               6
#define FORMAT_BLUE                 7
#define FORMAT_MAGENTA              8
#define FORMAT_CYAN                 9
#define FORMAT_WHITE               10
#define FORMAT_B_RED               11
#define FORMAT_B_GREEN             12
#define FORMAT_B_YELLOW            13
#define FORMAT_B_BLUE              14
#define FORMAT_B_MAGENTA           15
#define FORMAT_B_CYAN              16
#define FORMAT_B_WHITE             17
#define FORMAT_BL_RED              18
#define FORMAT_BL_GREEN            19 
#define FORMAT_BL_YELLOW           20
#define FORMAT_BL_BLUE             21
#define FORMAT_BL_MAGENTA          22
#define FORMAT_BL_CYAN             23
#define FORMAT_BL_WHITE            24
#define FORMAT_BB_RED                 25
#define FORMAT_BB_GREEN               26
#define FORMAT_BB_YELLOW              27
#define FORMAT_BB_BLUE                28
#define FORMAT_BB_MAGENTA             29
#define FORMAT_BB_CYAN                30
#define FORMAT_BB_WHITE               31
#define FORMAT_H_RED                  32
#define FORMAT_H_GREEN                33
#define FORMAT_H_YELLOW               34
#define FORMAT_H_BLUE                 35
#define FORMAT_H_MAGENTA              36
#define FORMAT_H_CYAN                 37
#define FORMAT_H_WHITE                38
#define MAX_FORMAT                    39


#define RACE_HUMAN                  0
#define RACE_ELF                    1
#define RACE_GNOME                  2
#define RACE_DWARF                  3    
#define RACE_HALFLING               4
#define RACE_ENT                    5
#define RACE_CENTAUR                6
#define RACE_LIZARD                 7
#define RACE_OGRE                   8
#define RACE_TROLL                  9
#define RACE_ORC                   10
#define RACE_GOBLIN                11
#define RACE_VYAN                  12 
#define RACE_UNDEAD                28
#define RACE_PLANT                 31
#define RACE_GOLEM                 33


#define REP_ATTACKED                0
#define REP_KILLED                  1
#define REP_STOLE_FROM              2


#define TO_ROOM                     0
#define TO_NOTVICT                  1
#define TO_ALL                      4
#define TO_ROOM_SEEN                5
#define TO_AREA                     6
#define TO_NOTVICT_SEEN             7
#define TO_VICT_SEEN                8

#define TRAP_POISON                 0
#define TRAP_BLIND                  1
#define MAX_TRAP                    2

#define QUEST_SACRIFICE             0
#define QUEST_KILL                  1
#define QUEST_GIVE                  2
#define QUEST_PRAY                  3
#define MAX_QUEST_TYPE              4


#define RES_DEXTERITY              -1
#define RES_MAGIC                   0
#define RES_FIRE                    1
#define RES_COLD                    2
#define RES_SHOCK                   3
#define RES_MIND                    4
#define RES_ACID                    5
#define RES_POISON                  6
#define MAX_RESIST                  7


#define SEX_NEUTRAL                 0
#define SEX_MALE                    1
#define SEX_FEMALE                  2
#define SEX_RANDOM                  3
#define MAX_SEX                     4
 

#define SIZE_ANT                    0
#define SIZE_RAT                    1
#define SIZE_DOG                    2
#define SIZE_GNOME                  3
#define SIZE_HUMAN                  4
#define SIZE_OGRE                   5
#define SIZE_HORSE                  6
#define SIZE_GIANT                  7
#define SIZE_ELEPHANT               8
#define SIZE_DINOSAUR               9
#define MAX_SIZE                   10


#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3

#define STAT_PET                    0
#define STAT_IN_GROUP               1
#define STAT_FAMILIAR               2
#define STAT_LEAPING                3
#define STAT_FOLLOWER               4
#define STAT_STUNNED                5 
#define STAT_POLYMORPH              6
#define STAT_ALERT                  7
#define STAT_TAMED                  8
#define STAT_BERSERK                9
#define STAT_GAINED_EXP            10
#define STAT_SENTINEL              11
#define STAT_REPLY_LOCK            12
#define STAT_AGGR_ALL              13
#define STAT_AGGR_GOOD             14
#define STAT_AGGR_EVIL             15
#define STAT_ORDERED               16
#define STAT_HIDING                17
#define STAT_SNEAKING              18
#define STAT_WIMPY                 19
#define STAT_PASSIVE               20
#define STAT_FORCED                21
#define STAT_CAMOUFLAGED           22
#define MAX_STAT                   23


#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_LIGHT                   2
#define SUN_SET                     3 


/*
 *   DEFINED FUNCTIONS
 */

#define ABS( a )                 ( a < 0 ? -a : a )  
#define UMIN( a, b )             ( a < b ? a : b )
#define UMAX( a, b )             ( a > b ? a : b )
#define URANGE( a, b, c )        ( b < a ? a : ( b > c ) ? c : b )

#define IS_SET( var, bit )       ( ( var & ( 1 << bit ) ) != 0 )
#define SET_BIT( var, bit )      ( var |= ( 1 << bit ) )
#define REMOVE_BIT( var, bit )   ( var &= ~( 1 << bit ) )
#define SWITCH_BIT( var, bit )   ( var ^= ( 1 << bit ) )

#define IS_ACTING( ch, bit )     ( ch->shdata->act & ( 1 << bit ) )

#define IS_AFFECTED( ch, bit )   ( is_set( ch->affected_by, bit ) )

#define IS_STATUS( ch, bit )     ( ch->status & ( 1 << bit ) )

#define IS_NPC( ch )             ( ch->species != NULL )

#define IS_GOD( ch )             ( get_trust( ch ) >= LEVEL_GOD )
#define IS_DEMIGOD( ch )         ( get_trust( ch ) >= LEVEL_DEMIGOD )
#define IS_SUPREME( ch )         ( get_trust( ch ) >= LEVEL_SUPREME )
#define IS_IMMORTAL( ch )        ( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_ARCHITECT( ch )       ( get_trust( ch ) >= LEVEL_ARCHITECT )
#define IS_CONTRACTER( ch )      ( get_trust( ch ) >= LEVEL_CONTRACTER )
#define IS_BUILDER( ch )         ( get_trust( ch ) >= LEVEL_BUILDER )

#define IS_PRIEST( ch )            ( !IS_NPC( ch ) &&                                                        ( ch->pcdata->clss == CLSS_CLERIC ||                                           ch->pcdata->clss == CLSS_PALADIN ||                                          ch->pcdata->clss == CLSS_MONK ) )

#define IS_MAGE( ch )              ( !IS_NPC( ch ) &&                                                        ( ch->pcdata->clss == CLSS_MAGE ||                                             ch->pcdata->clss == CLSS_RANGER ||                                           ch->pcdata->clss == CLSS_BARD ||                                             ch->pcdata->clss == CLSS_PSIONIC ) )

#define IS_GOOD( ch )         ( ch->shdata->alignment == ALGN_GOOD )
#define IS_EVIL( ch )         ( ch->shdata->alignment == ALGN_EVIL )
#define IS_NEUTRAL( ch )      ( ch->shdata->alignment == ALGN_NEUTRAL )

#define IS_HUNGRY( ch )       ( ch->pcdata->condition[ COND_FULL ] > 10 )
#define IS_THIRSTY( ch )      ( ch->pcdata->condition[ COND_THIRST ] > 10 )
#define IS_DRUNK( ch )                    ( !IS_NPC( ch ) &&                                                 ch->pcdata->condition[ COND_DRUNK ] > 10 )

#define IS_AWAKE( ch )        ( ch->position > POS_SLEEPING ) 
#define IS_OUTSIDE( ch )      (!IS_SET(ch->in_room->room_flags,RFLAG_INDOORS))

#define WAIT_STATE(ch, npulse)   (( ch )->wait = UMAX(( ch )->wait, (npulse)))


#define IS_OBJ_STAT( obj, stat )   IS_SET( obj->pIndexData->extra_flags, stat )


#endif /* __DEFINE.H__ */
