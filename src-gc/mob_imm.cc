#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


const char* plr_name [MAX_PLR] = { "Autoassist", "Autoexit",
  "??", "??", "??", "Autosplit", "Brief", "Chant",
  "Chat", "Ctell", "Email.Public", "Gossip", "Group.Incog", "Numeric",
  "Idle", "??", "Info", "Lang.Id.", "??", "No.Summon", "Ooc",
  "Parry", "Pet.Assist", "Prompt", "Reverse", "Safe.Kill", "Say.Repeat",
  "??","Searching", "Sneak", "Status.Bar", "Track",
  "Portal", "Atalk", "Unused1", "Unused2", "No.Emote", "No.Shout",
  "No.Tell",
  "Deny", "Freeze", "Edit_Help", "Familiar", "Leader", "Enrolled",
  "Is.Approved", "Donated", "Valid.Email", "Crash.Quit", "Avatar.Chnnl",
  "Build.Chnnl", "Wizinvis", "Holylight", "Logged", "Imm.Chnnl" };

const char* act_name [] = { "Is_Npc", "Sentinel", "Scavenger", "Can_Tame",
  "Aggressive", "Stay_Area", "Wimpy", "Humanoid", "Warm_blooded",
  "Summonable", "Assist_Group", "Can_Fly", "Can_Swim", "Mount",
  "Can_Climb", "Can_Carry", "Has_Eyes", "Has_Skeleton", "Ghost",
  "Zero_Regen", "Slithers", "Oozes", "No_Bash", "Mimic", "Predator",
  "Ice_Crystal", "Aggr_Evil", "Aggr_Good", "Carnivore", "Elemental",
  "Use.The" };

const char* no_permission =
  "You do not have permission to alter that species\n\r";

bool mobs_modified  = TRUE;


int   select         ( species_data*, char_data*, char* );
void  display        ( species_data*, char_data*, char*, int& );
void  medit_replace  ( char_data*, char* );

/*
 *   MLOAD FUNCTION
 */


void do_mload( char_data* ch, char* argument )
{
  species_data*  species;
  mob_data*          mob;
    
  if( *argument == '\0' || !is_number( argument ) ) {
    send( ch, "Usage: mload <vnum>.\n\r" );
    return;
    }

  if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
    send( ch, "No mob has that vnum.\n\r" );
    return;
    }

  mob = create_mobile( species );
  mreset_mob( mob );

  mob->To( ch->array );
  send( ch, "You create %s.\n\r", mob );
  send( *ch->array, "%s has created %s!\n\r", ch, mob );
}


/*
 *   MFIND ROUTINES
 */


int select( species_data* species, char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int                  i;
  int             length;
  int           min, max;
  mprog_data*      mprog;

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
      }
    else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help mfind.\n\r" );
        return -1;
        }
      }

    negative = FALSE;
    skip_spaces( argument );

    if( *argument == '!' ) {
      negative = TRUE;
      argument++;
      }

    if( *argument == '-' || isspace( *argument ) || *argument == '\0' ) {
      send( ch, "All flags require an argument - See help mfind.\n\r" );
      return -1;
      }
  
    for( i = 0; strncmp( argument-1, " -", 2 ) && *argument != '\0'; ) {
      if( i > ONE_LINE-2 ) {
        send( ch, "Flag arguments must be less than one line.\n\r" );
        return -1;
        } 
      tmp[i++] = *argument++;
      }

    for( ; isspace( tmp[i-1] ); i-- );

    tmp[i] = '\0';
    string = NULL;
    length = strlen( tmp );

    switch( letter ) {
      case 'g' :  string = group_table[species->group].name;        break;
      case 'c' :  string = species->creator;                        break;
      case 'r' :  string = race_table[species->shdata->race].name;  break; 
      case 'N' :  string = nation_table[species->nation].name;      break;
      }

    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      continue;
      }

    if( letter == 'n' ) {
      if( !is_name( tmp, species->Name( ) ) )
        return 0;
      continue;
      }

    if( letter == 'l' ) {
      atorange( tmp, min, max );
      if( species->shdata->level < min || species->shdata->level > max )
        return 0;
      continue;
      }

    if( letter == 'w' ) {
      for( i = 0; i < MAX_WEAR; i++ ) 
        if( !strncasecmp( tmp, wear_part_name[i], length ) ) {
          if( is_set( &species->wear_part, i ) == negative )
            return 0;
          break;
	  }
      continue;
      }

    if( letter == 'f' ) {
      for( i = 0; i < MAX_ACT; i++ ) 
        if( !strncasecmp( tmp, act_name[i], length ) ) {
          if( is_set( &species->act_flags, i ) == negative )
            return 0;
          break;
	  }
      continue;
      }

    if( letter == 'a' ) {
      for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) 
        if( !strncasecmp( tmp, aff_char_table[i].name, length ) ) {
          if( is_set( species->affected_by, i ) == negative )
            return 0;
          break;
	  }
      continue;
      }

    if( letter == 'T' ) {
      for( i = 0; !fmatches( tmp, mprog_trigger[i] ); i++ ) 
        if( i == MAX_MPROG_TRIGGER-1 ) {
          send( ch, "Unknown trigger type, see help mfind.\n\r" );
          return -1;
	  } 
      for( mprog = species->mprog; mprog != NULL && mprog->trigger != i;
        mprog = mprog->next );
      if( ( mprog != NULL ) != negative )
        continue;
      return 0;
      }

    send( ch, "Unknown flag - See help mfind.\n\r" );
    return -1;
    }
}


void display( species_data* species, char_data* ch, char* buf, int& length )
{
  dice_data  dice;
  int          hp;
 
  dice = species->hitdice;
  hp   = average( dice );
      
  sprintf( &buf[length], "[%5d] %-30s %5d %5s %5d %5d\n\r",
    species->vnum, truncate( species->Name( ), 30 ),
    species->shdata->level,
    species->shdata->deaths == 0 ? "??"
    : int5( species->exp/species->shdata->deaths ),
    hp, species->shdata->deaths );
  length += strlen( &buf[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, buf );
    length  = 0;
    *buf    = '\0';
    }        
}


void do_mfind( char_data* ch, char* argument )
{
  const char* title_msg =
    "Vnum    Name                           Level   Exp    Hp  Dths\n\r";

  char                buf  [ MAX_STRING_LENGTH ];
  species_data*   species;
  int                   i;
  int              length  = 0;
  bool              found  = FALSE;

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) != NULL ) {
      switch( select( species, ch, argument ) ) {
       case -1 : return;
       case  1 :
        if( !found ) {
          found = TRUE;
          page_underlined( ch, title_msg );
	  }
        display( species, ch, buf, length );
        }
      }
    }

  if( !found ) 
    send( ch, "No creature matching search was found.\n\r" );
  else
    page( ch, buf );
}
        

/*
 *   MWHERE ROUTINE
 */


char* status( mob_data* mob )
{
  char* tmp;

  if( mob->leader != NULL
    && is_set( &mob->status, STAT_PET ) ) {
    tmp = static_string( );
    sprintf( tmp, "Pet of %s", mob->leader->Name( ) );
    return tmp;
    }

  if( mob->reset != NULL )
    return "Reset";

  return "Mloaded?";
}


void do_mwhere( char_data* ch, char* argument )
{
  char          tmp1  [ TWO_LINES ];
  char          tmp2  [ TWO_LINES ];
  mob_data*   victim;
  int           vnum;
  bool         found;
  char*         name;

  if( *argument == '\0' ) {
    send( ch, "Usage: mwhere <mob>\n\r" );
    return;
    }

  vnum = atoi( argument );

  sprintf( tmp1, "%26s  %6s  %-15s  %s\n\r", "Room",
    "Vnum", "Status", "Area" );
  page_underlined( ch, tmp1 );

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( species_list[i] == NULL )
      continue;

     name = species_list[i]->Name( );

     if( vnum != i && !is_name( argument, name ) )
      continue;

    page_divider( ch, name, i );
    found = FALSE;

    for( int j = 0; j < mob_list; j++ ) {
      victim = mob_list[j];
      if( victim->Is_Valid( )
        && victim->species->vnum == i ) {
        strcpy( tmp1, victim->in_room->name );
        strcpy( tmp2, victim->in_room->area->name ); 
        truncate( tmp1, 25 );
        truncate( tmp2, 20 );
        page( ch, "%26s  %6d  %-15s  %s\n\r",
          tmp1, victim->in_room->vnum,
          status( victim ), tmp2 );
        found = TRUE;
        }
      }

    if( !found )
      page_centered( ch, "None found" );
    }

  return;
}


/*
 *   MEDIT
 */


bool char_data :: can_edit( species_data* species )
{
  if( species == NULL || has_permission( this, PERM_ALL_MOBS ) 
    || !strcmp( species->creator, descr->name ) )
    return TRUE;

  return FALSE;
}


void do_medit( char_data* ch, char* argument )
{
  char                     arg  [ MAX_INPUT_LENGTH ];
  char_data*            victim;
  descr_data*            descr;
  share_data*           shdata;
  species_data*        species;
  species_data*   species_copy  = NULL;
  wizard_data*          imm;
  int                        i;

  imm = (wizard_data*) ch;

  if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      if( ( species = imm->mob_edit ) == NULL ) {
        send( ch, "Which mob do you want to delete?\n\r" );
        return;
        }
      if( !ch->can_edit( species ) )
        return;
      }
    else 
      if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
        send( ch, "There is no species with that number.\n\r" );
        return;
        }

    if( !can_extract( species, ch ) )
      return;

    imm->mob_edit = species;
    extract( imm, offset( &imm->mob_edit, imm ), "species" );
    species_list[ species->vnum ] = NULL;
    delete species;

    send( ch, "You genocide mob %d, %s.\n\r",
      species->vnum, species );
    return;
    }
  
  if( exact_match( argument, "new" ) ) {
    if( isdigit( *argument ) ) {
      argument = one_argument( argument, arg );
      species_copy = get_species( atoi( arg ) );
      if( species_copy == NULL ) {
        send( ch, "No mob has given vnum to copy.\n\r" );
        return;
        }
      }
    else 
      species_copy = get_species( MOB_BLANK );    

    if( *argument == '\0' ) {
      send( ch, "You need to give the mob a name.\n\r" );
      return;
      }

    species = new species_data;
    shdata  = new share_data;
    descr   = new descr_data;
   
    for( i = 1; get_species( i ) != NULL; i++ );

    memcpy( species, species_copy, sizeof( *species ) );
    memcpy( shdata, species_copy->shdata, sizeof( *species->shdata ) );
    
    species->shdata = shdata;
    species->descr  = descr;
    species->vnum   = i;

    descr->keywords      = alloc_string( argument, MEM_DESCR );
    descr->singular      = alloc_string( argument, MEM_DESCR );
    descr->plural        = alloc_string( argument, MEM_DESCR );
    descr->complete      = alloc_string( species_copy->descr->complete,
                           MEM_DESCR );
    descr->long_s = alloc_string( "is here", MEM_DESCR );
    descr->long_p   = alloc_string( "are here", MEM_DESCR );

    species->creator = alloc_string( ch->descr->name, MEM_DESCR );     

    species->attack       = new program_data;
    species->attack->code = alloc_string( species_copy->attack->code,
      MEM_SPECIES );

    species->mprog = NULL;
    species->reset = NULL;
 
    zero_exp( species );

    for( i = 0; i < MAX_ARMOR; i++ )
      species->part_name[i] = alloc_string( species_copy->part_name[i],
        MEM_SPECIES );
    species->wear_part = 0;

    species_list[ species->vnum ] = species;

    create_mobile( species )->To( ch->array );
    send( ch, "New mob created, assigned vnum %d.\n\r", species->vnum );

    imm->mob_edit    = species;
    imm->mpdata_edit = NULL;
    imm->player_edit = NULL;

    return;
    }

  if( exact_match( argument, "replace" ) ) {
    medit_replace( ch, argument );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Who do you want to edit?\n\r" );
    return;
    }

  if( isdigit( *argument ) ) {
    if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
      send( ch, "No mob has that vnum.\n\r" );
      return;
      }
    }
  else {
    if( ( victim = one_character( ch, argument, "medit",
      ch->array, (thing_array*) &player_list ) ) == NULL )
      return;
    
    if( victim->species == NULL ) {
      if( ch != victim ) {
        if( is_builder( victim ) && !is_demigod( ch ) ) {
          send( ch, "You can't edit immortals.\n\r" );
          return;
          }
        if( get_trust( victim ) >= get_trust( ch ) ) {
          send( ch, "You can't edit them.\n\r" );
          return;
          }
        if( !has_permission( ch, PERM_PLAYERS ) ) {
          send( ch, "You can't edit players.\n\r" );
          return;
          }
        }

      imm->player_edit = (player_data*) victim;
      imm->mob_edit    = NULL;
      imm->mprog_edit  = NULL;
      imm->mpdata_edit = NULL;

      send( ch, "Mset and mflag now operate on %s.\n\r", victim );
      return;
      }
    species = victim->species;    
    }

  imm->player_edit  = NULL;
  imm->mob_edit     = species;
  imm->mprog_edit   = NULL;
  imm->mpdata_edit  = NULL;

  send( ch, "Mset and mflag now operate on %s.\n\r", species );
}


void medit_replace( char_data* ch, char* argument )
{
  area_data*          area;
  reset_data*        reset;
  room_data*          room;
  species_data*   species1  = NULL;
  species_data*   species2  = NULL;
  int                count  = 0;
  int                 i, j;

  if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
    send( ch, "Syntax: medit replace <vnum_old> <vnum_new>.\n\r" );
    return;
    }

  if( ( species1 = get_species( i ) ) == NULL
    || ( species2 = get_species( j ) ) == NULL ) {
    send( ch, "Vnum %d doesn't coorespond to an existing species.\n\r",
      species1 == NULL ? i : j );
    return;
    }

  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( reset->vnum == i 
          && is_set( &reset->flags, RSFLAG_MOB ) ) {
          reset->vnum = j;
          count++;
          area->modified = TRUE;
	  }

  send( ch, "Species %d replaced by %d in %d reset%s.\n\r",
    i, j, count, count != 1 ? "s" : "" );

  return;
}


/*
 *   MARMOR
 */


void do_marmor( char_data *ch, char* argument )
{
  char               arg  [ MAX_STRING_LENGTH ];
  char               buf  [ MAX_STRING_LENGTH ];
  species_data*  species;
  wizard_data*    imm;
  int             chance;
  int              armor; 
  int                  i;

  imm = (wizard_data*) ch;

  if( ( species = imm->mob_edit ) == NULL ) {
    send( "You aren't editing any mob.\n\r", ch );
    return;
    }

  if( IS_SET( species->act_flags, ACT_HUMANOID ) ) {
    sprintf( buf, "%s is a humanoid so marmor has no affect.\n\r",
      species->descr->name );
    buf[0] = toupper( buf[0] );
    send( buf, ch );
    return;
    }

  if( argument[0] == '\0' ) {
    for( i = 0; i < MAX_ARMOR; i++ ) {
      sprintf( buf, "[%2d] %5d %5d %s\n\r", i+1,
        species->chance[i], species->armor[i], species->part_name[i] );
      send( buf, ch );
      } 
    return;
    }

  if( !ch->can_edit( species ) )
    return;

  argument = one_argument( argument, arg );
  i = atoi( arg )-1;

  if( i < 0 || i >= MAX_ARMOR ) {
    send( "Part number out of range.\n\r", ch );
    return;
    }

  argument = one_argument( argument, arg );
  chance =  atoi( arg );
  
  if( chance < 1 || chance > 1000 ) {
    send( "Chance out of range.\n\r", ch );
    return;
    }

  argument = one_argument( argument, arg );
  armor =  atoi( arg );
  
  if( armor < -100 || armor > 10000 ) {
    send( "Armor out of range.\n\r", ch );
    return;
    }

  zero_exp( species );

  species->armor[i]     = armor;
  species->chance[i]    = chance;
  species->part_name[i] = alloc_string( argument, MEM_SPECIES );

  send( ch, "Ok.\n\r" );
}


/*
 *   MDESC
 */


void do_mdesc( char_data* ch, char* argument )
{
  char               tmp  [ MAX_STRING_LENGTH ];
  species_data*  species;
  wizard_data*    imm;

  imm = (wizard_data*) ch;

  if( ( species = imm->mob_edit ) == NULL ) {
    send( ch, "You aren't editing any mob - use medit <mob>.\n\r" );
    return;
    }

  if( *argument != '\0' && !ch->can_edit( species ) )
    return;

  species->descr->complete = edit_string( ch, argument,
    species->descr->complete, MEM_DESCR );

  if( *argument != '\0' ) {
    sprintf( tmp, "mde: %s", argument );
    mob_log( ch, species->vnum, tmp );
    }

  return;
}


/*
 *   MFLAG
 */


void do_mflag( char_data* ch, char* argument )
{
  char_data*            pc;
  species_data*        mob;
  wizard_data*         imm;
  const char*     response;

  #define types 3
  
  imm = (wizard_data*) ch;  
  pc = imm->player_edit;
  mob    = imm->mob_edit;

  if( pc == NULL && mob == NULL ) {
    send( ch, "You aren't editing a mob or player.\n\r" );
    return;
    }

  if( pc == NULL ) {    
    const char* title [types] = { "Act", "Affect", "Wear" };
    int max [types] = { MAX_ACT, MAX_ENTRY_AFF_CHAR, MAX_WEAR };

    const char** name1 [types] = { &act_name[0], &aff_char_table[0].name,
      &wear_part_name[0] }; 
    const char** name2 [types] = { &act_name[1], &aff_char_table[1].name,
      &wear_part_name[1] };

    int* flag_value [types] = { &mob->act_flags, mob->affected_by,
      &mob->wear_part };
    int uses_flag [types] = { 1, 1, 1 };

    if( ( response = flag_handler( title, name1, name2, flag_value, max,
      uses_flag, ch->can_edit( mob ) ? (const char*) NULL : no_permission,
      ch, argument, types ) ) != NULL ) 
      mob_log( ch, mob->vnum, response );
    return;
    }

  imm = wizard( pc );

  const char* title [types] = { "Act", "Affect", "Permission" };
  int max [types] = { is_demigod( ch ) ? MAX_PLR : MAX_PLR_OPTION,
    MAX_ENTRY_AFF_CHAR, MAX_PERMISSION };

  const char** name1 [types] = { &plr_name[0], &aff_char_table[0].name,
    &permission_name[1] }; 
  const char** name2 [types] = { &plr_name[1], &aff_char_table[1].name,
    &permission_name[2] };

  int* flag_value [types] = { pc->pcdata->pfile->flags,
    pc->affected_by, imm == NULL ? NULL : imm->permission };
  int uses_flag [types] = { -1, 1, is_demigod( ch ) ? -1 : 1 };

  flag_handler( title, name1, name2, flag_value, max, uses_flag,
    "That flag isn't setable or you don't have permission.\n\r",
    ch, argument, imm == NULL ? 2 : types );

  #undef types

  return;
}


/*
 *   MSET
 */


void do_mset( char_data* ch, char* argument )
{
  char_data*      victim;
  descr_data*      descr;
  pc_data*        pcdata;
  share_data*     shdata;
  species_data*  species;
  player_data*        pc;
  wizard_data*       imm;
  account_data*  account;
  clan_data*        clan;
  const char*       name;

  imm     = (wizard_data*) ch;
  species = imm->mob_edit;
  victim  = imm->player_edit;

  if( species == NULL && victim == NULL ) {
    send( ch, "You aren't editing any mob - use medit <mob>.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    do_mstat( ch, "" );
    return;
    }

  if( !ch->can_edit( species ) )
    return;

  if( species == NULL ) {
    shdata = victim->shdata;
    pcdata = victim->pcdata;
    descr  = victim->descr;
    pc     = player( victim );
    imm    = wizard( victim );
    name   = descr->name;
    }
  else {
    shdata = species->shdata;
    pcdata = NULL;
    pc     = NULL;
    descr  = species->descr;
    name   = species->Name( );
    } 

  { 
    class int_field int_list[] = {
      { "strength",          0,    30,  &shdata->strength      },
      { "intelligence",      0,    30,  &shdata->intelligence  },
      { "wisdom",            0,    30,  &shdata->wisdom        },
      { "dexterity",         0,    30,  &shdata->dexterity     },
      { "constitution",      0,    30,  &shdata->constitution  },
      { "fame",              0,  1000,  &shdata->fame          },
      { "",                  0,     0,  NULL                   },
      };

    if( process( int_list, ch, name, argument, species, pc ) ) {
      update_maxes( victim );
      return;
      }

#define an( i )   alignment_table[i].name
#define rn( i )   race_table[i].name
#define mea       MAX_ENTRY_ALIGNMENT
#define mer       MAX_ENTRY_RACE

    class type_field type_list[] = {
      { "alignment", mea,   &an(0), &an(1), &shdata->alignment  },
      { "race",      mer,   &rn(0), &rn(1), &shdata->race       },
      { "",          0,     NULL,   NULL,   NULL                }
      };

#undef mea
#undef mer
#undef an
#undef rn

    if( process( type_list, ch, name, argument, species, pc ) )
      return;
    }

  class string_field string_list[] = {
    { "keywords",      MEM_DESCR,  &descr->keywords,    NULL },
    { "singular",      MEM_DESCR,  &descr->singular,    NULL },
    { "",              0,          NULL,                NULL },   
    };

  if( process( string_list, ch, name, argument, species ) )
    return;

  if( species != NULL ) {
    class string_field string_list[] = {
      { "name",         MEM_DESCR,  &descr->name,          NULL },
      { "plural",       MEM_DESCR,  &descr->plural,        NULL },
      { "long_s",       MEM_DESCR,  &descr->long_s,        NULL },
      { "long_p",       MEM_DESCR,  &descr->long_p,        NULL },
      { "prefix_s",     MEM_DESCR,  &descr->prefix_s,      NULL },
      { "prefix_p",     MEM_DESCR,  &descr->prefix_p,      NULL },
      { "adj_s",        MEM_DESCR,  &descr->adj_s,         NULL },
      { "adj_p",        MEM_DESCR,  &descr->adj_p,         NULL },
      { "creator",      MEM_DESCR,  &species->creator,     NULL },
      { "",             0,          NULL,                  NULL },   
      };

    if( process( string_list, ch, name, argument, species ) )
      return;

    class int_field int_list[] = {
      { "coins",               0,       100000,  &species->gold        },
      { "level",               0,           90,  &shdata->level        },
      { "magic res.",       -200,          100,  &shdata->resist[0]    },
      { "fire res.",        -200,          100,  &shdata->resist[1]    },
      { "cold res.",        -200,          100,  &shdata->resist[2]    },
      { "electricity res.", -200,          100,  &shdata->resist[3]    },
      { "mind res.",        -200,          100,  &shdata->resist[4]    },
      { "acid res.",        -200,          100,  &shdata->resist[5]    },
      { "poison res.",      -200,          100,  &shdata->resist[6]    },
      { "maturity",            1,         1000,  &species->maturity    },
      { "adult",               0,  MAX_SPECIES,  &species->adult       },
      { "skeleton",            0,  MAX_SPECIES,  &species->skeleton    },
      { "zombie",              0,  MAX_SPECIES,  &species->zombie      },
      { "corpse",              0,        10000,  &species->corpse      },
      { "price",               1,     10000000,  &species->price       },
      { "wander delay",      200,         4999,  &species->wander      },  
      { "light",               0,          100,  &species->light       },  
      { "",                    0,            0,  NULL                  }, 
      };
               
    if( process( int_list, ch, name, argument, species ) )
      return;

    class cent_field cent_list[] = {
      { "weight",              0,     10000000,  &species->weight      },  
      { "",                    0,            0,  NULL                  }, 
      };
               
    if( process( cent_list, ch, name, argument, species ) )
      return;

#define gtn( i )    group_table[i].name
#define nn( i )     nation_table[i].name
#define sn( i )     size_name[i]
#define sxn( i )    sex_name[i]
#define cn( i )     color_fields[i]

    class type_field type_list[] = {
      { "group",    MAX_ENTRY_GROUP,  &gtn(0), &gtn(1), &species->group    },
      { "nation",   MAX_ENTRY_NATION, &nn(0),  &nn(1),  &species->nation   },
      { "sex",      MAX_SEX,          &sxn(0), &sxn(1), &species->sex      },
      { "size",     MAX_SIZE,         &sn(0),  &sn(1),  &species->size     },
      { "color",    MAX_COLOR,        &cn(0),  &cn(1),  &species->color    },
      { "",         0,                NULL,    NULL,    NULL               }
      };

#undef gtn
#undef nn
#undef sn
#undef sxn
#undef cn

    if( process( type_list, ch, name, argument, species ) )
      return;

    class dice_field dice_list[] = {
      { "movedice",    LEVEL_MOB,     &species->movedice },
      { "hitdice",     LEVEL_MOB,     &species->hitdice },    
      { "",            -1,            NULL }
      };

    if( process( dice_list, ch, name, argument, species ) )
      return; 
    }

  if( imm != NULL ) {
    class int_field int_list [] = {
      { "office",          1,       500,  &imm->office    },
      { "",                0,         0,  NULL               },
      };

    if( process( int_list, ch, name, argument ) )
      return;
    }

#define ctn( i )    clss_table[i].name
#define rn( i )     religion_table[i].name
#define sxn( i )    sex_name[i]
#define max_relig   MAX_ENTRY_RELIGION

  if( pc != NULL ) {
    class type_field type_list[] = {
      { "sex",       MAX_SEX-1,   &sxn(0),  &sxn(1),  &pc->sex           },
      { "class",     MAX_CLSS,    &ctn(0),  &ctn(1),  &pcdata->clss      },
      { "religion",  max_relig,   &rn(0),   &rn(1),   &pcdata->religion  },
      { "",          0,           NULL,     NULL,     NULL               },
      };

#undef max_relig
#undef ctn
#undef rn
#undef sxn

    if( process( type_list, ch, name, argument, NULL, pc ) )
      return;

    class int_field int_list[] = {
      { "deaths",          0,     10000,  &shdata->deaths                   },
      { "exp",             0, 100000000,  &victim->exp                      },
      { "gsp_points",  -1000,      1000,  &pc->gossip_pts               },
      { "piety",           0,      1000,  &pcdata->piety                    },
      { "prayer",          0,      1000,  &pc->prayer                   },
      { "prac_points",  -100,      1000,  &pcdata->practice                 },
      { "hunger",       -100,       100,  &pcdata->condition[COND_FULL]     },
      { "thirst",       -100,       100,  &pcdata->condition[COND_THIRST]   },
      { "drunk",        -100,       100,  &pcdata->condition[COND_DRUNK]    },
      { "alcohol",      -100,       100,  &pcdata->condition[COND_ALCOHOL]  },
      { "move",            0,      1000,  &pc->move                     },
      { "hits",            0,      1000,  &pc->hit                      },
      { "mana",            0,      1000,  &pc->mana                     },
      { "base_move",       0,      1000,  &pc->base_move                },
      { "base_hits",       0,      1000,  &pc->base_hit                 },
      { "base_mana",       0,      1000,  &pc->base_mana                },
      { "",                0,         0,  NULL                              },
      };

    if( process( int_list, ch, name, argument, NULL, pc ) )
      return;
    
    if( matches( argument, "clan" ) ) {
      if( *argument == '\0' ) {
        send( ch, "Set %s to which clan?\n\r", pc );
        return;
        }  
      if( ( clan = find_clan( ch, argument ) ) != NULL ) {
        remove_member( pc );   
        remove_member( pc->pcdata->pfile );
        add_member( clan, pc->pcdata->pfile );
        save_clans( clan );
        if( ch == pc ) {
          send( ch, "You set yourself into clan %s.\n\r", clan->name );
	   }
        else {
          send( ch, "You set %s into clan %s.\n\r", pc, clan->name );
          send( pc, "%s sets you into clan %s.\n\r", ch, clan->name );
	  }
        }
      return;
      }

    if( matches( argument, "account" ) ) {
      if( *argument == '\0' ) {
        send( ch, "To which account do you wish to switch %s.\n\r", pc );
        return;
        }
      if( ( account = find_account( argument ) ) == NULL ) {
        send( ch, "No such account exists.\n\r" );
        return;
        }
      send( ch, "%s switched to account %s.\n\r", 
        pc, account->name );
      pc->pcdata->pfile->account = account;
      return;
      }

    if( is_god( ch ) && matches( argument, "balance" ) ) {
      if( pcdata->pfile->account == NULL ) {
        send( ch, "Null Account.\n\r" );
        return;
        }
      pcdata->pfile->account->balance = atoi( argument );
      send( ch, "Balance set to $%.2f.\n\r", 
        (float) pcdata->pfile->account->balance/100 );
      save_accounts( );
      return;
      }
    }

  send( ch, "Unknown field - see help mset.\n\r" );
}


void do_mstat( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char_data*       victim;
  descr_data*       descr;
  pc_data*         pcdata;
  share_data*      shdata;
  species_data*   species;
  wizard_data*        imm;
  dice_data          dice;
  player_data*         pc;
  int                   i;
  int               flags;

  imm = wizard( ch );

  if( !get_flags( ch, argument, &flags, "a", "Mstat" ) )
    return;
  
  if( *argument == '\0' ) {
    if( ( species = imm->mob_edit ) == NULL ) {
      if( imm->player_edit == NULL ) {
        send( ch, "Specify victim or use medit to select one.\n\r" );
        return;
        }
      victim = imm->player_edit;
      shdata = victim->shdata;
      pcdata = victim->pcdata;
      descr  = victim->descr;
      pc     = player( imm->player_edit );
      }
    else {
      victim = NULL;
      shdata = species->shdata;
      pcdata = NULL;  
      descr  = species->descr;
      pc = NULL;
      }
    }
  else {
    if( isdigit( *argument ) ) {
      i = atoi( argument );
      if( ( species = get_species( i ) ) == NULL ) {
        send( ch, "There is no species with that number.\n\r" );
        return;
        }
      victim = NULL;
      shdata = species->shdata;
      pcdata = NULL;  
      descr  = species->descr;
      pc = NULL;
      }
    else {
      if( ( victim = one_character( ch, argument, "mstat",
        ch->array, (thing_array*) &player_list ) ) == NULL )
        return;

      shdata  = victim->shdata;
      pcdata  = victim->pcdata;
      species = victim->species;   
      descr   = victim->descr;
      pc  = player( victim );
      }
    }

  sprintf( buf, "        " );

  if( !is_set( &flags, 0 ) ) {
    page_title( ch, victim == NULL
      ? species->Name( ) : victim->Seen_Name( ch ) );

    if( pc != NULL ) {
      sprintf( buf+5,  "    Class: %s        ",
        clss_table[ pcdata->clss ].name );
      sprintf( buf+25, "  Exp.Lvl: %d\n\r", exp_for_level( victim )
        -victim->exp );
      page( ch, buf );
      }
    else {
      page( ch, "          Vnum: %-10d Creator: %s%s%s\n\r",
        species->vnum, color_code( ch, COLOR_BOLD_YELLOW ),
        species->creator, normal( ch ) );
      }

    if( pc == NULL ) 
      page( ch, "        Nation: %-12s Group: %s\n\r",
        nation_table[species->nation].name,
        group_table[species->group].name );

    page( ch, "         Level: %-12d Kills: %-11d Deaths: %d\n\r",
      shdata->level, shdata->kills, shdata->deaths );
    page( ch, "          Fame: %-5d\n\r", shdata->fame );

    page( ch,  "           Sex: %-13s Race: %-8s Alignment: %s\n\r",
      sex_name[ victim == NULL ? species->sex : victim->sex ],
      race_table[ shdata->race ].name,
      alignment_table[ shdata->alignment ].name );

    if( pc != NULL ) {
      page( ch, "         Piety: %-11d Prayer: %d\n\r",
        pcdata->piety, pc->prayer );
      page( ch, "           Age: %-12d Pracs: %-12d Total: %d (%d)\n\r",
        pc->Age( ), pcdata->practice,
        total_pracs( victim ), expected_pracs( victim ) );
      page( ch, "      Qst_Pnts: %-9d Gsp_Pnts: %d\n\r",
        pcdata->quest_pts, pc->gossip_pts );
      page( ch, "          Bank: %-12d Coins: %s\n\r",
        pc->bank, coin_phrase( victim ) );
      }
    else {
      page( ch, "         Coins: %-11d Wander: %-12d Light: %d\n\r",
        species->gold, species->wander, species->light );
      page( ch, "          Size: %-11s Weight: %.2f lbs\n\r",
        size_name[ species->size ], (float) species->weight/100 );

      dice = species->hitdice;
      sprintf( buf+5,  "  HitDice: %dd%d+%d            ",
        dice.number, dice.side, dice.plus );
      dice = species->movedice;
      sprintf( buf+27,   " MvDice: %dd%d+%d\n\r",
        dice.number, dice.side, dice.plus );
      page( ch, buf );

      page( ch, "      Maturity: %-12d Price: %d\n\r",
        species->maturity, species->price );

      species_data*     adult  = get_species( species->adult );
      species_data*  skeleton  = get_species( species->skeleton );
      species_data*    zombie  = get_species( species->zombie );
      obj_clss_data*   corpse  = get_obj_index( species->corpse );

      page( ch, "         Adult: %-5d (%s)\n\r",
        species->adult,
        adult == NULL ? "none" : adult->Name( ) );
      page( ch, "      Skeleton: %-5d (%s)\n\r",
        species->skeleton,
        skeleton == NULL ? "none" : skeleton->Name( ) );
      page( ch, "        Zombie: %-5d (%s)\n\r",
        species->zombie,
        zombie == NULL ? "none" : zombie->Name( ) );
      page( ch, "        Corpse: %-5d (%s)\n\r",
        species->corpse,
        corpse == NULL ? "none" : corpse->Name( ) );
      }

    if( victim != NULL ) {
      page( ch, "\n\r" );

      sprintf( buf+5,  "  Hit Pts: %d/%d       ", victim->hit,
        victim->max_hit );
      sprintf( buf+25, "   Energy: %d/%d       ", victim->mana,
        victim->max_mana );
      sprintf( buf+45, "     Move: %d/%d\n\r", victim->move,
        victim->max_move );
      page( ch, buf );

      page( ch, "     Hit Regen: %-8d Ene Regen: %-9d Mv Regen: %d\n\r",  
        victim->Hit_Regen( ), victim->Mana_Regen( ),
        victim->Move_Regen( ) );

      page( ch, "      Wght Inv: %-8.2f Wght Worn: %.2f\n\r",
        (float) victim->contents.weight/100,
        (float) victim->wearing.weight/100 );

      page( ch, "        Leader: %s\n\r",
        victim->leader == NULL ? "noone" : victim->leader->Name( ch ) );

      if( victim->species != NULL ) 
        page( ch, "         Reset: %s\n\r", name( victim->reset ) );
      }

    if( pc != NULL ) { 
      sprintf( buf+5,  "   Hunger: %d          ",
        pcdata->condition[COND_FULL] );
      sprintf( buf+25, "   Thirst: %d          ",
        pcdata->condition[COND_THIRST] );
      sprintf( buf+45, "    Drunk: %d (%d)\n\r",
        pcdata->condition[ COND_DRUNK ], pcdata->condition[ COND_ALCOHOL ] );
      page( ch, buf );
      }

    page( ch, "\n\r" );  

    if( victim != NULL ) {
      sprintf( buf+5, 
        "Str: %2d(%2d)  Int: %2d(%2d)  Wis: %2d(%2d)  Dex: %2d(%2d)\
  Con: %2d(%2d).\n\r\n\r",
        victim->Strength( ), shdata->strength,
        victim->Intelligence( ), shdata->intelligence,
        victim->Wisdom( ), shdata->wisdom,
        victim->Dexterity( ), shdata->dexterity,
        victim->Constitution( ), shdata->constitution );
      page( ch, buf );

      sprintf( buf+5,
        "Mag: %2d(%2d)  Fir: %2d(%2d)  Col: %2d(%2d)  Min: %2d(%2d)\n\r",
        shdata->resist[RES_MAGIC],    shdata->resist[RES_MAGIC],
        shdata->resist[RES_FIRE],     shdata->resist[RES_FIRE],
        shdata->resist[RES_COLD],     shdata->resist[RES_COLD],
        shdata->resist[RES_MIND],     shdata->resist[RES_MIND] );   
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %2d(%2d)  Aci: %2d(%2d)  Poi: %2d(%2d)\n\r\n\r",
        shdata->resist[RES_SHOCK],    shdata->resist[RES_SHOCK],
        shdata->resist[RES_ACID],     shdata->resist[RES_ACID],
        shdata->resist[RES_POISON],   shdata->resist[RES_POISON] );
      }
    else {
      sprintf( buf+5,
        "Str: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\n\r\n\r",
        shdata->strength, shdata->intelligence, shdata->wisdom,
        shdata->dexterity, shdata->constitution );
      page( ch, buf );

      sprintf( buf+5,
        "Mag: %2d  Fir: %2d  Col: %2d  Min: %2d\n\r",
        shdata->resist[RES_MAGIC], shdata->resist[RES_FIRE],
        shdata->resist[RES_COLD],  shdata->resist[RES_MIND] );
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %2d  Aci: %2d  Pos: %2d\n\r\n\r",
        shdata->resist[RES_SHOCK], shdata->resist[RES_ACID],
        shdata->resist[RES_POISON] );
      }
    page( ch, buf );
    }

  page( ch, "       Name: %s\n\r", descr->name );
  page( ch, "   Singular: %s\n\r", descr->singular );

  if( pc != NULL ) {
    if( pcdata->tmp_short != NULL ) 
      page( ch, " Unapproved: %s\n\r", pcdata->tmp_short );
    page( ch, "   Keywords: %s\n\r", descr->keywords );
    if( pcdata->tmp_keywords != NULL )
      page( ch, " Unapproved: %s\n\r", pcdata->tmp_keywords );
    }
  else {
    page( ch, "   Prefix_S: %s\n\r", descr->prefix_s );
    page( ch, "      Adj_S: %s\n\r", descr->adj_s );
    page( ch, "     Long_S: %s\n\r", descr->long_s );
    page( ch, "     Plural: %s\n\r", descr->plural );
    page( ch, "   Prefix_P: %s\n\r", descr->prefix_p );
    page( ch, "      Adj_P: %s\n\r", descr->adj_p );
    page( ch, "     Long_P: %s\n\r", descr->long_p );
    page( ch, "   Keywords: %s\n\r", descr->keywords );
    page( ch, "      Color: %s\n\r", color_fields[ species->color ] );
    }

  sprintf( buf, "\n\rDescription: \n\r%s",
    *descr->complete != '\0' ? descr->complete : "(none).\n\r" );
  page( ch, buf );
}






















