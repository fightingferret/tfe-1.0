#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   LOCAL VARIABLES
 */


spell_data      spell_table         [ MAX_SPELL ];     
social_type     social_table        [ MAX_PLYR_RACE+1 ][ MAX_SOCIAL ];
spell_act_type  spell_act_table     [ MAX_SPELL_ACT ];
liquid_type     liquid_table        [ MAX_LIQUID ];
town_type       town_table          [ MAX_TOWN ];
skill_type      skill_table         [ MAX_SKILL ];
metal_type      material_table      [ MAX_METAL ];
nation_data     nation_table        [ MAX_NATION ];
group_data      group_table         [ MAX_GROUP ];     
race_data       race_table          [ MAX_RACE ];
plyr_race_data  plyr_race_table     [ MAX_PLYR_RACE ];
aff_char_type   aff_char_table      [ MAX_AFF_CHAR ];
aff_obj_type    aff_obj_table       [ MAX_AFF_OBJ ];
command_type    command_table       [ MAX_COMMAND ];
category_data   cmd_cat_table       [ MAX_CMD_CAT ];
clss_type       clss_table          [ MAX_CLSS ];
starting_data   starting_table      [ MAX_CLSS+MAX_PLYR_RACE+1 ];
tedit_data      tedit_table         [ MAX_TABLE ];
recipe_data     build_table         [ MAX_BUILD ];
category_data   help_cat_table      [ MAX_HELP_CAT ];
town_type       astral_table        [ MAX_ASTRAL ];
religion_data   religion_table      [ MAX_RELIGION ];
alignment_data  alignment_table     [ MAX_ALIGNMENT ];

int table_max [ MAX_TABLE ];


/*
 *   LOCAL CONSTANTS
 */


const int table_abs_max [ MAX_TABLE ] =
{
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL,
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL,
  MAX_SOCIAL, MAX_SOCIAL, MAX_SPELL_ACT, MAX_LIQUID, MAX_SPELL, MAX_TOWN,
  MAX_SKILL, MAX_METAL, MAX_NATION, MAX_GROUP, MAX_RACE, MAX_PLYR_RACE,
  MAX_AFF_CHAR, MAX_AFF_OBJ, MAX_COMMAND, MAX_CMD_CAT, MAX_CLSS,
  MAX_CLSS+MAX_PLYR_RACE+1,
  MAX_TABLE, MAX_BUILD, MAX_HELP_CAT, MAX_ASTRAL, MAX_RELIGION,
  MAX_ALIGNMENT
};


const int table_size [ MAX_TABLE ] =
{
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( social_type ),
  sizeof( class spell_act_type ),
  sizeof( class liquid_type ),
  sizeof( class spell_data ), 
  sizeof( class town_type ),
  sizeof( class skill_type ), 
  sizeof( class metal_type ),
  sizeof( nation_data ),
  sizeof( group_data ),
  sizeof( race_data ),
  sizeof( class plyr_race_data ),
  sizeof( class aff_char_type ),
  sizeof( class aff_obj_type ),
  sizeof( class command_type ),
  sizeof( class category_data ),
  sizeof( class clss_type ),
  sizeof( class starting_data ),
  sizeof( class tedit_data ),
  sizeof( recipe_data ),
  sizeof( class category_data ),
  sizeof( class town_type ),
  sizeof( religion_data ),
  sizeof( alignment_data )
};



entry_data table_entry [ MAX_TABLE-MAX_PLYR_RACE ][ MAX_FIELD ] =
{
  {
    {  &social_table[0][0].name,           VAR_CHAR   },
    {  &social_table[0][0].position,       VAR_POS    },
    {  &social_table[0][0].aggressive,     VAR_BOOL   },
    {  &social_table[0][0].reveal,         VAR_BOOL   },
    {  &social_table[0][0].disrupt,        VAR_BOOL   },
    {  &social_table[0][0].char_no_arg,    VAR_CHAR   },
    {  &social_table[0][0].others_no_arg,  VAR_CHAR   },
    {  &social_table[0][0].char_found,     VAR_CHAR   },
    {  &social_table[0][0].others_found,   VAR_CHAR   },
    {  &social_table[0][0].vict_found,     VAR_CHAR   },
    {  &social_table[0][0].vict_sleep,     VAR_CHAR   },
    {  &social_table[0][0].char_auto,      VAR_CHAR   },
    {  &social_table[0][0].others_auto,    VAR_CHAR   },
    {  &social_table[0][0].obj_self,       VAR_CHAR   },
    {  &social_table[0][0].obj_others,     VAR_CHAR   },
    {  &social_table[0][0].dir_self,       VAR_CHAR   },
    {  &social_table[0][0].dir_others,     VAR_CHAR   },
    {  &social_table[0][0].ch_obj_self,    VAR_CHAR   },
    {  &social_table[0][0].ch_obj_victim,  VAR_CHAR   },
    {  &social_table[0][0].ch_obj_others,  VAR_CHAR   },
    {  &social_table[0][0].ch_obj_sleep,   VAR_CHAR   },
    {  &social_table[0][0].self_obj_self,  VAR_CHAR   },
    {  &social_table[0][0].self_obj_others,VAR_CHAR  }
  },

  {
    {  &spell_act_table[0].name,           VAR_CHAR  },
    {  &spell_act_table[0].self_other,     VAR_CHAR  },
    {  &spell_act_table[0].victim_other,   VAR_CHAR  },
    {  &spell_act_table[0].others_other,   VAR_CHAR  },
    {  &spell_act_table[0].self_self,      VAR_CHAR  },
    {  &spell_act_table[0].others_self,    VAR_CHAR  }
  },

  { 
    {  &liquid_table[0].name,           VAR_CHAR  },
    {  &liquid_table[0].color,          VAR_CHAR  },   
    {  &liquid_table[0].hunger,         VAR_INT   },
    {  &liquid_table[0].thirst,         VAR_INT   },
    {  &liquid_table[0].alcohol,        VAR_INT   },
    {  &liquid_table[0].cost,           VAR_INT   },
    {  &liquid_table[0].create,         VAR_BOOL  },
    {  &liquid_table[0].spell,          VAR_SKILL }
  },

  {
    {  &spell_table[0].name,              CNST_CHAR   },
    {  &spell_table[0].prepare,           VAR_INT     },
    {  &spell_table[0].wait,              VAR_INT     },
    {  &spell_table[0].type,              VAR_STYPE   },
    {  &spell_table[0].damage,            VAR_FORMULA },
    {  &spell_table[0].cast_mana,         VAR_FORMULA },
    {  &spell_table[0].leech_mana,        VAR_FORMULA },
    {  &spell_table[0].regen,             VAR_FORMULA },
    {  &spell_table[0].duration,          VAR_FORMULA },
    {  &spell_table[0].location,          VAR_LOC     },
    {  &spell_table[0].action[0],         VAR_SA      },
    {  &spell_table[0].action[1],         VAR_SA      },
    {  &spell_table[0].action[2],         VAR_SA      },
    {  &spell_table[0].action[3],         VAR_SA      },
    {  &spell_table[0].action[4],         VAR_SA      },
    {  &spell_table[0].reagent[0],        VAR_INT     },
    {  &spell_table[0].reagent[1],        VAR_INT     },
    {  &spell_table[0].reagent[2],        VAR_INT     },
    {  &spell_table[0].reagent[3],        VAR_INT     },
    {  &spell_table[0].reagent[4],        VAR_INT     }
  },

  {
    { &town_table[0].name,              VAR_CHAR  },
    { &town_table[0].recall,            VAR_INT   }
  },

  { 
    { &skill_table[0].name,                         VAR_CHAR  },
    { &skill_table[0].pre_skill[0],                 VAR_SKILL },
    { &skill_table[0].pre_level[0],                 VAR_INT   },
    { &skill_table[0].pre_skill[1],                 VAR_SKILL },
    { &skill_table[0].pre_level[1],                 VAR_INT   },
    { &skill_table[0].category,                     VAR_SCAT  },
    { &skill_table[0].prac_cost[ CLSS_MAGE ],       VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_CLERIC ],     VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_THIEF ],      VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_WARRIOR ],    VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_PALADIN ],    VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_RANGER ],     VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_MONK ],       VAR_INT   },  
    { &skill_table[0].prac_cost[ CLSS_PSIONIC ],    VAR_INT   },
    { &skill_table[0].prac_cost[ CLSS_BARD ],       VAR_INT   },
    { &skill_table[0].level[ CLSS_MAGE ],           VAR_INT   },
    { &skill_table[0].level[ CLSS_CLERIC ],         VAR_INT   },  
    { &skill_table[0].level[ CLSS_THIEF ],          VAR_INT   },  
    { &skill_table[0].level[ CLSS_WARRIOR ],        VAR_INT   },  
    { &skill_table[0].level[ CLSS_PALADIN ],        VAR_INT   },  
    { &skill_table[0].level[ CLSS_RANGER ],         VAR_INT   },  
    { &skill_table[0].level[ CLSS_MONK ],           VAR_INT   },  
    { &skill_table[0].level[ CLSS_PSIONIC ],        VAR_INT   },
    { &skill_table[0].level[ CLSS_BARD ],           VAR_INT   },
  },

  { 
    { &material_table[0].name,                      CNST_CHAR  },
    { &material_table[0].cost,                      VAR_INT    },
    { &material_table[0].weight,                    VAR_INT    },
    { &material_table[0].mana,                      VAR_INT    },
    { &material_table[0].armor,                     VAR_INT    },
    { &material_table[0].enchant,                   VAR_INT    },
    { &material_table[0].save_fire,                 VAR_INT    },
    { &material_table[0].save_cold,                 VAR_INT    },
    { &material_table[0].save_acid,                 VAR_INT    },
    { &material_table[0].msg_fire,                  VAR_CHAR   },
    { &material_table[0].msg_cold,                  VAR_CHAR   },
    { &material_table[0].msg_acid,                  VAR_CHAR   },
    { &material_table[0].rust_name,                 VAR_CHAR   },
    { &material_table[0].rust[0],                   VAR_CHAR   },
    { &material_table[0].rust[1],                   VAR_CHAR   },
    { &material_table[0].rust[2],                   VAR_CHAR   },
  },

  {
    { &nation_table[0].name,                        VAR_CHAR  },
    { &nation_table[0].abbrev,                      VAR_CHAR  },
    { &nation_table[0].temple,                      VAR_INT   },
    { &nation_table[0].room[0],                     VAR_INT   },
    { &nation_table[0].room[1],                     VAR_INT   },
    { &nation_table[0].race[0],                     VAR_INT   },
    { &nation_table[0].race[1],                     VAR_INT   },
    { &nation_table[0].race[2],                     VAR_INT   },
    { &nation_table[0].race[3],                     VAR_INT   },
    { &nation_table[0].race[4],                     VAR_INT   },
    { &nation_table[0].race[5],                     VAR_INT   },
    { &nation_table[0].race[6],                     VAR_INT   },
    { &nation_table[0].race[7],                     VAR_INT   },
    { &nation_table[0].race[8],                     VAR_INT   },
    { &nation_table[0].race[9],                     VAR_INT   },
    { &nation_table[0].race[10],                    VAR_INT   },
    { &nation_table[0].race[11],                    VAR_INT   },
    { &nation_table[0].race[12],                    VAR_INT   },
    { &nation_table[0].alignment[0],                VAR_INT   },
    { &nation_table[0].alignment[1],                VAR_INT   },
    { &nation_table[0].alignment[2],                VAR_INT   },
    { &nation_table[0].alignment[3],                VAR_INT   },
    { &nation_table[0].alignment[4],                VAR_INT   },
    { &nation_table[0].alignment[5],                VAR_INT   },
    { &nation_table[0].alignment[6],                VAR_INT   },
    { &nation_table[0].alignment[7],                VAR_INT   },
    { &nation_table[0].alignment[8],                VAR_INT   }
  },  

  {
    { &group_table[0].name,                       VAR_CHAR  },
  },

  {
    { &race_table[0].name,                        VAR_CHAR  },
    { &race_table[0].abbrev,                      VAR_CHAR  },
    { &race_table[0].track,                       VAR_CHAR  },
  },

  {
    { &plyr_race_table[0].name,                   VAR_CHAR  },
    { &plyr_race_table[0].plural,                 VAR_CHAR  },
    { &plyr_race_table[0].hp_bonus,               VAR_INT   },
    { &plyr_race_table[0].mana_bonus,             VAR_INT   },
    { &plyr_race_table[0].move_bonus,             VAR_INT   },
    { &plyr_race_table[0].size,                   VAR_SIZE  },
    { &plyr_race_table[0].weight,                 VAR_CENT  },
    { &plyr_race_table[0].stat_bonus[0],          VAR_INT   },
    { &plyr_race_table[0].stat_bonus[1],          VAR_INT   },
    { &plyr_race_table[0].stat_bonus[2],          VAR_INT   },
    { &plyr_race_table[0].stat_bonus[3],          VAR_INT   },
    { &plyr_race_table[0].stat_bonus[4],          VAR_INT   },
    { &plyr_race_table[0].resist[0],              VAR_INT   }, 
    { &plyr_race_table[0].resist[1],              VAR_INT   }, 
    { &plyr_race_table[0].resist[2],              VAR_INT   }, 
    { &plyr_race_table[0].resist[3],              VAR_INT   }, 
    { &plyr_race_table[0].resist[4],              VAR_INT   }, 
    { &plyr_race_table[0].resist[5],              VAR_INT   }, 
    { &plyr_race_table[0].resist[6],              VAR_INT   }, 
    { &plyr_race_table[0].affect[0],              VAR_AFF   },
    { &plyr_race_table[0].start_room[0],          VAR_INT   },
    { &plyr_race_table[0].start_room[1],          VAR_INT   },
    { &plyr_race_table[0].start_room[2],          VAR_INT   },
    { &plyr_race_table[0].portal,                 VAR_INT   },
    { &plyr_race_table[0].start_age,              VAR_INT   },
    { &plyr_race_table[0].life_span,              VAR_INT   },
    { &plyr_race_table[0].alignments,             VAR_ALIGN },
    { &plyr_race_table[0].language,               VAR_LANG  },
    { &plyr_race_table[0].open,                   VAR_BOOL  },
  },

  {
    { &aff_char_table[0].name,                    VAR_CHAR    },
    { &aff_char_table[0].id_line,                 VAR_CHAR    },
    { &aff_char_table[0].score_name,              VAR_CHAR    },
    { &aff_char_table[0].msg_on,                  VAR_CHAR    },
    { &aff_char_table[0].msg_on_room,             VAR_CHAR    },
    { &aff_char_table[0].msg_off,                 VAR_CHAR    },
    { &aff_char_table[0].msg_off_room,            VAR_CHAR    },
    { &aff_char_table[0].location,                VAR_AFF_LOC },
    { &aff_char_table[0].modifier,                VAR_CHAR    },
  },

  {
    { &aff_obj_table[0].name,                     VAR_CHAR    },
    { &aff_obj_table[0].msg_on,                   VAR_CHAR    },
    { &aff_obj_table[0].msg_off,                  VAR_CHAR    },
    { &aff_obj_table[0].location,                 VAR_INT     },
  },

  {
    { &command_table[0].name,                     CNST_CHAR },
    { &command_table[0].help,                     VAR_CHAR  },
    { &command_table[0].func_name,                VAR_FUNC  },
    { &command_table[0].level[0],                 VAR_PERM  },
    { &command_table[0].reqlen,                   VAR_INT   },
    { &command_table[0].position,                 VAR_POS   },
    { &command_table[0].category,                 VAR_CC    },
    { &command_table[0].disrupt,                  VAR_BOOL  },
    { &command_table[0].reveal,                   VAR_BOOL  },
    { &command_table[0].queue,                    VAR_BOOL  },
  },
 
  {
    { &cmd_cat_table[0].name,                     VAR_CHAR  },
    { &cmd_cat_table[0].level,                    VAR_INT   },
  },

  {
    { &clss_table[0].name,                        VAR_CHAR  },
    { &clss_table[0].abbrev,                      VAR_CHAR  },
    { &clss_table[0].hit_min,                     VAR_INT   },
    { &clss_table[0].hit_max,                     VAR_INT   },
    { &clss_table[0].mana_min,                    VAR_INT   },
    { &clss_table[0].mana_max,                    VAR_INT   },
    { &clss_table[0].move_min,                    VAR_INT   },
    { &clss_table[0].move_max,                    VAR_INT   },
    { &clss_table[0].hit_bonus,                   VAR_INT   },
    { &clss_table[0].mana_bonus,                  VAR_INT   },
    { &clss_table[0].move_bonus,                  VAR_INT   },
    { &clss_table[0].resist[0],                   VAR_INT   },
    { &clss_table[0].resist[1],                   VAR_INT   },
    { &clss_table[0].resist[2],                   VAR_INT   },
    { &clss_table[0].resist[3],                   VAR_INT   },
    { &clss_table[0].resist[4],                   VAR_INT   },
    { &clss_table[0].resist[5],                   VAR_INT   },
    { &clss_table[0].resist[6],                   VAR_INT   },
    { &clss_table[0].alignments,                  VAR_ALIGN },
    { &clss_table[0].open,                        VAR_BOOL  },
  },

  {
    { &starting_table[0].name,                    VAR_CHAR  },
    { &starting_table[0].object[0],               VAR_OBJ   },
    { &starting_table[0].object[2],               VAR_OBJ   },
    { &starting_table[0].object[4],               VAR_OBJ   },
    { &starting_table[0].object[6],               VAR_OBJ   },
    { &starting_table[0].object[8],               VAR_OBJ   },
    { &starting_table[0].skill[0],                VAR_SKILL },
    { &starting_table[0].skill[1],                VAR_SKILL },
    { &starting_table[0].skill[2],                VAR_SKILL },
    { &starting_table[0].skill[3],                VAR_SKILL },
    { &starting_table[0].skill[4],                VAR_SKILL },
    { &starting_table[0].level[0],                VAR_INT   },
    { &starting_table[0].level[1],                VAR_INT   },
    { &starting_table[0].level[2],                VAR_INT   },
    { &starting_table[0].level[3],                VAR_INT   },
    { &starting_table[0].level[4],                VAR_INT   },
  },
  
  {
    { &tedit_table[0].name,                       CNST_CHAR },
    { &tedit_table[0].edit,                       VAR_INT   },
    { &tedit_table[0].new_delete,                 VAR_INT   },
  },

  {
    { &build_table[0].name,                       VAR_CHAR  },
    { &build_table[0].result[0],                  VAR_OBJ   },
    { &build_table[0].ingredient[0],              VAR_OBJ   },
    { &build_table[0].ingredient[2],              VAR_OBJ   },
    { &build_table[0].ingredient[4],              VAR_OBJ   },
    { &build_table[0].ingredient[6],              VAR_OBJ   },
    { &build_table[0].ingredient[8],              VAR_OBJ   },
    { &build_table[0].ingredient[10],             VAR_OBJ   },
    { &build_table[0].ingredient[12],             VAR_OBJ   },
    { &build_table[0].ingredient[14],             VAR_OBJ   },
    { &build_table[0].ingredient[16],             VAR_OBJ   },
    { &build_table[0].ingredient[18],             VAR_OBJ   },
    { &build_table[0].skill[0],                   VAR_SKILL },
    { &build_table[0].skill[1],                   VAR_SKILL },
    { &build_table[0].skill[2],                   VAR_SKILL },
    { &build_table[0].tool[0],                    VAR_OBJ   },
    { &build_table[0].tool[1],                    VAR_OBJ   },
  },

  {
    { &help_cat_table[0].name,                    VAR_CHAR  },
    { &help_cat_table[0].level,                   VAR_INT   },
  },

  {
    { &astral_table[0].name,                      VAR_CHAR  },
    { &astral_table[0].recall,                    VAR_INT   }
  },

  {
    { &religion_table[0].name,                    VAR_CHAR   },
    { &religion_table[0].sex,                     VAR_SEX    },
    { &religion_table[0].alignments,              VAR_ALIGN  },
    { &religion_table[0].classes,                 VAR_INT    }
  },

  { 
    { &alignment_table[0].name,                   VAR_CHAR   },
    { &alignment_table[0].abbrev,                 VAR_CHAR   },
  }
};


const char* table_name [ MAX_TABLE ] = { "Soc.Default",
  "Soc.Human", "Soc.Elf", "Soc.Gnome", "Soc.Dwarf",
  "Soc.Halfling", "Soc.Ent", "Soc.Centaur", "Soc.Lizard",
  "Soc.Ogre", "Soc.Troll", "Soc.Orc", "Soc.Goblin", "Soc.Vyan",
  "Spell.Actions",
  "Liquids", "Spell.Data", "Towns", "Skills", "Materials", "Nations",
  "Groups", "Races", "Player.Races", "Aff.Char", "Aff.Obj",
  "Commands",
  "Cmd.Categories", "Classes", "Starting", "Tables", "Build",
  "Help.Categories", "Astral", "Religions", "Alignments" };


const char* social_fields [] =
{
  "name",
  "position",
  "aggressive",
  "disrupt",
  "reveal",
  "no_arg.self",
  "no_arg.others",
  "ch.self",
  "ch.others",
  "ch.victim",
  "ch.sleep",
  "self.self",
  "self.others",
  "obj.self",
  "obj.others",
  "dir.self",
  "dir.others",
  "ch/obj.self",
  "ch/obj.victim",
  "ch/obj.others",
  "ch/obj.sleep",
  "self/obj.self",
  "self/obj.others",
  ""
};


const char* spell_action_fields [] =
{
  "name",
  "self_other",
  "victim_other",
  "others_other",
  "self_self",
  "others_self",
  ""
};


const char* liquid_fields [] =
{
  "name",
  "color",
  "hunger",
  "thirst",
  "alcohol",
  "cp/liter",
  "creatable", 
  "spell",
  ""
};


const char* spell_data_fields [] =
{
  "name",
  "prepare",
  "wait",
  "type",
  "damage",
  "cast_mana",
  "leech_mana",
  "regen",
  "duration",
  "location",
  "action[1]",
  "action[2]",
  "action[3]",
  "action[4]",
  "action[5]",
  "reagent[1]",
  "reagent[2]",
  "reagent[3]",
  "reagent[4]",
  "reagent[5]",
  ""
};


const char* town_fields [] =
{
  "name",
  "recall_loc",
  ""
};


const char* skill_fields [] =
{
  "Name",
  "Prereq[1]",
  "Level[1]",
  "Prereq[2]",
  "Level[2]",
  "Category",
  "Cost[Mage]",
  "Cost[Cleric]",
  "Cost[Thief]",
  "Cost[Warrior]",
  "Cost[Paladin]",
  "Cost[Ranger]",
  "Cost[Monk]",
  "Cost[Psionic]",
  "Cost[Bard]",
  "Level[Mage]",
  "Level[Cleric]",
  "Level[Thief]",
  "Level[Warrior]",
  "Level[Paladin]",
  "Level[Ranger]",
  "Level[Monk]",
  "Level[Psionic]",
  "Level[Bard]",
  ""
};


const char* material_fields [] = 
{
  "Name",
  "Cost",
  "Weight",
  "Mana",
  "Armor", 
  "Enchant",
  "Save[Fire]",
  "Save[Cold]",
  "Save[Acid]",
  "Msg[Fire]",
  "Msg[Cold]", 
  "Msg[Acid]",
  "Rust_Name",   
  "Rust[1]",
  "Rust[2]",
  "Rust[3]",
  ""
};


const char* nation_fields [] =
{
  "Name",
  "Abbrev.",
  "Temple",
  "Room[1]",
  "Room[2]",
  "Rela[Human]",
  "Rela[Elf]",
  "Rela[Gnome]",
  "Rela[Dwarf]",
  "Rela[Halfling]",
  "Rela[Ent]",
  "Rela[Centaur]",
  "Rela[Lizardman]",
  "Rela[Ogre]",
  "Rela[Troll]",
  "Rela[Orc]",
  "Rela[Goblin]",
  "Rela[Vyan]",
  "Rela[LG]",
  "Rela[LN]",
  "Rela[LE]",
  "Rela[NG]",
  "Rela[PN]",
  "Rela[NE]",
  "Rela[CG]",
  "Rela[CN]",
  "Rela[CE]",
  ""
};


const char* group_fields [] =
{
  "Name",
  ""
};


const char* race_fields [] =
{ 
  "Name",
  "Abbrev.",
  "Tracks",
  ""
};


const char* player_race_fields [] = 
{
  "Name",
  "Plural",
  "Hitpoints",
  "Energy",
  "Move",
  "Size",
  "Weight",
  "Strength",
  "Intelligence",
  "Wisdom",
  "Dexterity",
  "Constitution",
  "Magic",
  "Fire",
  "Cold",
  "Electricity",
  "Mind",
  "Acid",
  "Poison",
  "Affect",
  "Start[Good]",
  "Start[Neutral]",
  "Start[Evil]",
  "Portal",
  "Start.Age",
  "Life.Span",
  "Alignments",
  "Language",
  "Open",
  ""
};


const char* aff_char_fields [] =
{
  "Name",
  "Id.line",
  "Score.name",
  "Msg.on",
  "Msg.on_room",
  "Msg.off",
  "Msg.off_room",
  "Location",
  "Modifier",
  ""
};


const char* aff_obj_fields [] =
{
  "Name",
  "Msg.On",
  "Msg.Off",
  "Location",
  ""
};


const char* command_fields [] =
{
  "name",
  "help",
  "function",
  "level",
  "reqlen",
  "position",
  "category",
  "disrupt",
  "reveal",
  "queue",
  ""
};


const char* cmd_cat_fields [] =
{
  "name",
  "level",
  ""
};


const char* class_fields [] =
{
  "Name",
  "Abbrev.",
  "Hit_Min",
  "Hit_Max",
  "Energy_Min",
  "Energy_Max",
  "Move_Min",
  "Move_Max",
  "Hit_Regen",
  "Energy_Regen",
  "Move_Regen",
  "Magic",
  "Fire",
  "Cold",
  "Electricity",
  "Mind",
  "Acid",
  "Poison",
  "Alignments",
  "Open", 
  ""
};


const char* starting_fields [] =
{
  "Class",
  "1_Object",
  "2_Object",
  "3_Object",
  "4_Object",
  "5_Object",
  "1_Skill",
  "2_Skill",
  "3_Skill",
  "4_Skill",
  "5_Skill",
  "1_Level",
  "2_Level",
  "3_Level",
  "4_Level",
  "5_Level",
  ""
};


const char* table_fields [] =
{
  "Name",
  "Edit",
  "New_Delete",
  ""
};


const char* build_fields [] =
{
  "name",
  "result",
  "ingred[1]",
  "ingred[2]",
  "ingred[3]",
  "ingred[4]",
  "ingred[5]",
  "ingred[6]",
  "ingred[7]",
  "ingred[8]",
  "ingred[9]",
  "ingred[10]",
  "skill[1]",
  "skill[2]",
  "skill[3]",
  "tool[1]",
  "tool[2]",
  ""
};


const char* help_cat_fields [] =
{
  "name",
  "level",
  ""
};


const char* astral_fields [] =
{
  "name",
  "location",
  ""
};


const char* religion_fields [] =
{
  "name",
  "sex",
  "alignments",
  "classes",
  ""
};


const char* alignment_fields [] =
{
  "name",
  "abbrev",
  ""
};


const char** table_field [ MAX_TABLE ] = 
{
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  social_fields,
  spell_action_fields,
  liquid_fields,  
  spell_data_fields,
  town_fields,
  skill_fields,
  material_fields,
  nation_fields,
  group_fields,
  race_fields,
  player_race_fields,
  aff_char_fields,
  aff_obj_fields,
  command_fields,
  cmd_cat_fields,
  class_fields,
  starting_fields,
  table_fields,
  build_fields,
  help_cat_fields,
  astral_fields,
  religion_fields,
  alignment_fields
};


/*
 *   LOCAL FUNCTIONS
 */


int    find_table      ( char_data*, char*&, const char*,
                         const char* = empty_string );
int    find_entry      ( char_data*, char*&, int, const char* = empty_string );
void   init_entry      ( int, char* );
void   init_commands   ( void );
void   remove_entry    ( char_data*, int, int );
void   sort_table      ( char_data*, int );
void   swap_entries    ( int, int, int );


inline char*   entry_name   ( int, int );


/*
 *   SUPPORT ROUTINES
 */


bool edit_table( char_data* ch, int table )
{
  int flag;

  switch( table ) {
    case TABLE_COMMAND     :  flag = PERM_COMMANDS;       break;
    case TABLE_SPELL_DATA  :
    case TABLE_SPELL_ACT   :  flag = PERM_SPELLS;         break;
    default                :  flag = PERM_MISC_TABLES;    break; 
    }

  if( table <= MAX_PLYR_RACE  && table == ch->shdata->race+1 )
    flag = PERM_SOCIALS;

  if( !has_permission( ch, flag ) ) {
    send( ch, "You do not have permission to edit that table.\n\r" );
    return FALSE;
    }

  return TRUE;
}


int find_table( char_data* ch, char*& argument, const char* msg1,
  const char* msg2 )
{
  int       i;

  if( *argument == '\0' ) {
    if( msg2 != empty_string ) 
      send( ch, msg2 );
    return -2;
    }

  for( i = 0; i < MAX_TABLE; i++ ) 
    if( matches( argument, table_name[i] ) )  
      return i;

  send( msg1, ch );
  return -1;
}


int find_entry( char_data* ch, char*& argument, int table, const char* msg )
{
  int       i;

  if( *argument == '\0' ) {
    if( msg != empty_string )
      send( ch, msg );
    return -2;
    }

  for( i = 0; i < table_max[table]; i++ ) 
    if( exact_match( argument, entry_name( table, i ) ) ) 
      return i;

  for( i = 0; i < table_max[table]; i++ ) 
    if( fmatches( argument, entry_name( table, i )) ) 
      return i;

  send( ch, "The %s table does not contain such an entry.\n\r",
    table_name[table] );

  return -1;
}


/*
 *   ENTRY INFO
 */


inline char* entry_name( int table, int col )
{
  void* pntr;

  if( table <= MAX_PLYR_RACE )
    return social_table[table][col].name;

  pntr = table_entry[ table-MAX_PLYR_RACE ][0].offset
    +col*table_size[table];

  return (char*) *((void**) pntr);
}


int entry_type( int table, int entry )
{
  table = max( 0,table-MAX_PLYR_RACE );
 
  return table_entry[table][entry].type;
}


void* table_pntr( int table, int entry, int col )
{
  if( table <= MAX_PLYR_RACE ) 
    return (void*) &social_table[table][col]-&social_table[0][0]
      +table_entry[0][entry].offset;

  return table_entry[ table-MAX_PLYR_RACE ][entry].offset
    +col*table_size[table];
}


/*
 *   DISK ROUTINES
 */


void load_tables( )
{
  FILE*         fp;
  int      i, j, k;
  char**      pntr;
  char*     string;
  int        table;

  echo( "Loading Tables ...\n\r" );
 
  for( i = -1; i < MAX_TABLE; i++ ) {
    table = ( i == -1 ? TABLE_SKILL : i );

    fprintf( stderr, "  -%s\n\r", table_name[table] );
    fp = open_file( TABLE_DIR, table_name[table], "r", TRUE );

    if( strcmp( fread_word( fp ), "#TABLE" ) ) 
      panic( "Load_tables: missing header" );

    table_max[table] = fread_number( fp );

    if( table_max[table] > table_abs_max[table] ) 
      panic( "Load_Tables: Number of entries in table %s > table max.", 
        table_name[table] );

    for( j = 0; j < table_max[table]; j++ )
      for( k = 0; *table_field[table][k] != '\0'; k++ ) {
        (void*) pntr = table_pntr( table, k, j );
        switch( entry_type( table, k ) ) {
         case VAR_TEMP :
          (int) *pntr = 0;
          break;

         case VAR_BLANK :
          *pntr = empty_string;
          break;

         case VAR_FORMULA :
         case VAR_CHAR :
         case CNST_CHAR : 
         case VAR_FUNC :
          *pntr = fread_string( fp, MEM_TABLE );
          break;

         case VAR_BOOL :
         case VAR_LEECH :
         case VAR_SIZE :
	 case VAR_SA :
         case VAR_INT :
         case VAR_CC :
         case VAR_SCAT :
	 case VAR_DICE :
         case VAR_STYPE :
         case VAR_ALIGN :
         case VAR_LANG :
         case VAR_AFF_LOC :
         case VAR_LOC :
         case VAR_CENT :
         case VAR_SEX :
          (int) *pntr = fread_number( fp );
          break;

         case VAR_POS :
          if( ( int( *pntr ) = fread_number( fp ) ) < 0 
	    || int( *pntr ) >= MAX_POSITION ) 
            panic( "Load_Tables: Impossible position." ); 
          break;         

         case VAR_DELETE :
          fread_number( fp );
          break;

         case VAR_SKILL :
          string = fread_string( fp, MEM_UNKNOWN );
          if( i != -1 ) 
            (int) *pntr = skill_index( string );
          free_string( string, MEM_UNKNOWN );
          break;

         case VAR_PERM :
         case VAR_OBJ  :
          (int) *pntr     = fread_number( fp );
          (int) *(pntr+1) = fread_number( fp );
          break;

         case VAR_AFF  :
          (int) *pntr     = fread_number( fp );
          (int) *(pntr+1) = fread_number( fp );
          (int) *(pntr+2) = fread_number( fp );
          break;
	  }
        }
    fclose( fp );
    }

  if( table_max[ TABLE_SKILL ] != MAX_SKILL ) {
    roach( "Load_Tables: Entries in skill table != max_skill." );
    roach( "-- Max_Skill = %d", MAX_SKILL );
    panic( "--   Entries = %d", table_max[ TABLE_SKILL ] );
    }

  init_commands( );
  init_spells( );
}


void save_tables(  )
{
  FILE*          fp;
  char**       pntr;
  int       i, j, k;

  for( i = 0; i < MAX_TABLE; i++ ) {
    fp = open_file( TABLE_DIR, table_name[i], "w" );

    fprintf( fp, "#TABLE\n\n" );
    fprintf( fp, "%d\n\n", table_max[i] );

    for( j = 0; j < table_max[i]; j++ ) {
      for( k = 0; *table_field[i][k] != '\0'; k++ ) {
        (void*) pntr = table_pntr( i,k,j );
        switch( entry_type( i,k ) ) {
          case VAR_DELETE :
            break;

          case VAR_BLANK :
          case CNST_CHAR :
          case VAR_FUNC :
          case VAR_CHAR :
          case VAR_FORMULA :
            fprintf( fp, "%s~\n", (char*) *pntr );
            break;

          case VAR_BOOL :
          case VAR_SIZE :
          case VAR_INT : 
          case VAR_SA :
          case VAR_CC:
          case VAR_POS:
	  case VAR_DICE :
          case VAR_SCAT :
          case VAR_STYPE :
          case VAR_LEECH :
          case VAR_TEMP :
          case VAR_ALIGN :
          case VAR_LANG :
          case VAR_AFF_LOC :
	  case VAR_LOC :
	  case VAR_CENT :
          case VAR_SEX :
            fprintf( fp, "%d\n", (int) *pntr ); 
            break;

          case VAR_SKILL :
            if( (int) *pntr <= 0 )
              fprintf( fp, "~\n" );
            else
              fprintf( fp, "%s~\n", skill_table[ (int) *pntr ].name );
            break;

          case VAR_PERM :
          case VAR_OBJ :
            fprintf( fp, "%d %d\n", (int) *pntr, (int) *(pntr+1) );
            break;

          case VAR_AFF:
            fprintf( fp, "%d %d %d\n", (int) *pntr,
              (int) *(pntr+1), (int) *(pntr+2) );
            break;
          }  
        }
      fprintf( fp, "\n" ); 
      }
    fprintf( fp, "\n" ); 
    fclose( fp );
    }
}


/*
 *   TEDIT ROUTINE
 */


void do_tedit( char_data* ch, char* argument )
{
  wizard_data*  imm  = (wizard_data*) ch;
  int             i  = -1;
  int             j  = -1;

  if( *argument == '\0' ) {
    display_array( ch, "Tables", &table_name[0], &table_name[1],
      MAX_TABLE );
    return;
    }

  if( exact_match( argument, "new" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.",
      "Syntax: tedit new <table> <entry>" ) ) < 0 )
      return;
    if( table_max[i] == table_abs_max[i] ) {
      send( ch, "That table has no open slots.\n\r" );
      return;
      }
    if( !edit_table( ch, i ) )
      return;
    init_entry( i, argument );
    imm->table_edit[0] = i;
    imm->table_edit[1] = table_max[i]-1;
    send( ch, "Table entry added and you are now editing it.\n\r" );
    return;
    }

  if( exact_match( argument, "delete" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.\n\r" ) ) != -1 
      && ( j = find_entry( ch, argument, i,
      "Syntax: tedit delete <table> <name>\n\r" ) ) >= 0 ) 
      remove_entry( ch, i, j );
    return;
    }

  if( exact_match( argument, "sort" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.\n\r",
      "Which table do you wish to sort?\n\r" ) ) < 0 )
      return;
    sort_table( ch, i );
    return;
    }        

  if( ( i = find_table( ch, argument, "Table not found.\n\r" ) ) == -1 )
    return;

  if( *argument == '\0' ) {
    display_array( ch, table_name[i], 
      (const char**) table_pntr( i,0,0 ), (const char**) table_pntr( i,0,1 ),
      table_max[i] );
    return;
    }

  if( ( j = find_entry( ch, argument, i ) ) == -1 )
    return;

  imm->table_edit[0] = i;
  imm->table_edit[1] = j;

  send( ch, "Tstat and tset now work on the %s %s.\n\r",
    table_name[i], entry_name( i, j ) );
} 


void do_tset( char_data* ch, char* argument )
{
  char          arg  [ MAX_INPUT_LENGTH ];
  char          tmp  [ MAX_INPUT_LENGTH ];  
  void**       pntr;
  wizard_data*  imm  = (wizard_data*) ch;
  int        length;
  int             i  = imm->table_edit[0];
  int             j  = imm->table_edit[1];
  int          k, n;
  int*        ipntr; 
  bool        error  = FALSE;

  if( *argument == '\0' ) {
    do_tstat( ch, argument );
    return;
    }

  if( !edit_table( ch, i ) )
    return;

  argument = one_argument( argument, arg );

  length = strlen( arg );
  for( k = 0; *table_field[i][k] != '\0'; k++ ) 
    if( !strncasecmp( arg, table_field[i][k], length ) )
      break;

  if( *table_field[i][k] == '\0' ) {
    send( "Syntax: tset <field> <value>\n\r", ch );
    return;
    }

  (void*) pntr = (void*) table_pntr( i,k,j );

  switch( entry_type( i,k ) ) {
    case VAR_TEMP :
      send( "That is a unsorted variable and may not be set.\n\r", ch );
      return;

    case VAR_SIZE : {
      class type_field size_field = {
        "size", MAX_SIZE, &size_name[0], &size_name[1], (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      size_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_STYPE : {
      class type_field stype_field = {
      "type", MAX_STYPE, &stype_name[0], &stype_name[1], (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      stype_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_AFF_LOC : {
      class type_field stype_field = {
        "type", MAX_AFF_LOCATION, &affect_location[0], &affect_location[1],
        (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      stype_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_BOOL :
      ipntr = (int*) pntr;
      set_bool( ch, argument, table_field[i][k], *ipntr ); 
      return;

    case VAR_ALIGN :
      align_flags.set( ch, argument, (int*) pntr );
      return;     

    case VAR_LANG :
      lang_flags.set( ch, argument, (int*) pntr );
      return;      

    case VAR_AFF :
      affect_flags.set( ch, argument, (int*) pntr );
      return;

    case VAR_PERM :
      permission_flags.set( ch, argument, (int*) pntr );
      return;

    case VAR_LOC :
      location_flags.set( ch, argument, (int*) pntr );
      return;

    case VAR_POS : {
      class type_field pos_field = {
        "position", MAX_POSITION,
        &position_name[0], &position_name[1], (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      pos_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_SEX : {
      class type_field sex_field = {
        "sex", MAX_SEX-1,
        &sex_name[0], &sex_name[1], (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      sex_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_FORMULA :
      evaluate( argument, error );
      if( error ) {
        send( ch, "Expression fails to evaluate.\n\r" );
        return;
        }

    case VAR_BLANK :
    case VAR_CHAR :
    case VAR_FUNC  :
      free_string( (char*) *pntr, MEM_TABLE );
      (char*) *pntr = alloc_string( argument, MEM_TABLE );
      (void*) pntr = table_pntr( i,0,j );
      sprintf( tmp, "%s on %s %s set to:\n\r%s\n\r",
        table_field[i][k], table_name[i], (char*) *pntr, argument );
      break;

    case VAR_INT :
      n            = atoi( argument );
      (int)  *pntr = n;
      (void*) pntr = table_pntr( i,0,j );
      sprintf( tmp, "%s on %s %s set to %d.\n\r",
        table_field[i][k], table_name[i], (char*) *pntr, n );
      break;

    case VAR_CENT :
      n            = (int) 100*atof( argument );
      (int)  *pntr = n;
      (void*) pntr = table_pntr( i,0,j );
      sprintf( tmp, "%s on %s %s set to %.2f.\n\r",
        table_field[i][k], table_name[i], (char*) *pntr,
        (float) n/100 );
      break;

    case VAR_CC : {
      class type_field cat_field = {
        "category", MAX_ENTRY_CMD_CAT,
        &cmd_cat_table[0].name, &cmd_cat_table[1].name, (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      cat_field.set( ch, (char*) *pntr, argument ); 
      return;
      } 
    
    case VAR_SKILL :
      length = strlen( argument );
      for( n = 0; n < table_max[TABLE_SKILL]; n++ ) 
        if( !strncasecmp( argument, skill_table[n].name, length ) )
          break;
      if( n == table_max[ TABLE_SKILL ] ) {
        if( strncasecmp( argument, "none", length ) ) {
          send( "Unknown skill.\n\r", ch );
          return;
          } 
        n = -1;
        }
      (int)  *pntr = n;
      (void*) pntr = table_pntr( i,0,j );
      sprintf( tmp, "%s on %s %s set to %s.\n\r",
        table_field[i][k], table_name[i], (char*) *pntr,
        n == -1 ? "none" : skill_table[n].name );
      break;

    case VAR_SA :
      length = strlen( argument );
      for( n = 0; n < table_max[TABLE_SPELL_ACT]; n++ ) 
        if( !strncasecmp( argument, spell_act_table[n].name, length ) ) 
          break;
      if( n == table_max[TABLE_SPELL_ACT] ) {
        send( "No spell action matching that name found.\n\r", ch );
        return;
        }     
      (int)  *pntr = n;
      (void*) pntr = table_pntr( i,0,j );
      sprintf( tmp, "%s on %s %s set to %s.\n\r",
        table_field[i][k], table_name[i], (char*) *pntr,
        spell_act_table[n].name );
      break;

    case VAR_SCAT : {
      class type_field scat_field = {
        "category", MAX_SKILL_CAT,
        &skill_cat_name[0],  &skill_cat_name[1], (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      scat_field.set( ch, (char*) *pntr, argument ); 
      return;
      }

    case VAR_OBJ :
      if( !strcasecmp( argument, "nothing" ) ) {
        send( ch, "Field set to nothing.\n\r" );
        (int) *pntr     = 0;
        (int) *(pntr+1) = 1; 
        return;
        }
      /*
      if( ( obj = one_object( ch, argument ) ) == NULL ) {
        send( "You don't have that object.\n\r", ch );
        return;
        }
      (int) *pntr     = obj->pIndexData->vnum;
      (int) *(pntr+1) = obj->selected; 
      send( ch, "Object set.\n\r" );
      */
      return;

    case VAR_LEECH :
    case VAR_DICE : {
      class dice_field entry = {
        table_field[i][k], LEVEL_MOB, (int*) pntr };
      (void*) pntr = table_pntr( i,0,j );
      entry.set( ch, (char*) *pntr, argument );
      return;
      }

    case CNST_CHAR :
      send( ch, "That entry is not setable.\n\r" );
      return;
    }

  *tmp = toupper( *tmp );
  send( ch, tmp );  
}


void do_tstat( char_data* ch, char* )
{
  char                 tmp  [ FOUR_LINES ];
  wizard_data*         imm  = (wizard_data*) ch;
  obj_clss_data*  obj_clss;
  int                table  = imm->table_edit[0];
  int                entry  = imm->table_edit[1];
  int            col, k, n;
  void**              pntr;
  bool              single;

  single = ( table != TABLE_SKILL && table != TABLE_CLSS
    && table != TABLE_PLYR_RACE && table != TABLE_SPELL_DATA
    && table != TABLE_NATION );

  for( k = col = 0; *table_field[table][k] != '\0'; k++, col++ ) {
    (void*) pntr = table_pntr( table,k,entry );
    sprintf( tmp, "%15s : ", table_field[table][k] );

    switch( entry_type( table,k ) ) {
      case VAR_FORMULA :
      case VAR_CHAR :
      case CNST_CHAR :
      case VAR_FUNC :
      case VAR_BLANK :
        strcpy( &tmp[18], (char*) *pntr );
        break;

      case VAR_BOOL :
        sprintf( &tmp[18], "%s", (bool) *pntr ? "true" : "false" ); 
        break; 
   
      case VAR_TEMP :
      case VAR_INT :
        sprintf( &tmp[18], "%d", (int) *pntr );
        break;
   
      case VAR_CENT :
        n = (int) *pntr; 
        sprintf( &tmp[18], "%.2f", (float) n/100 );
        break;
   
      case VAR_AFF_LOC :
        strcpy( &tmp[18], affect_location[ int( *pntr ) ] );
        break;

      case VAR_SIZE :
        strcpy( &tmp[18], size_name[ (int) *pntr ] );
        break;

      case VAR_SCAT :
        strcpy( &tmp[18], skill_cat_name[ (int) *pntr ] );
        break;

      case VAR_STYPE :
        strcpy( &tmp[18], stype_name[ (int) *pntr ] );
        break;

      case VAR_SKILL :
        sprintf( &tmp[18], "%s", (int) *pntr == -1 
          ? "none" : skill_table[(int) *pntr].name );
        break;

      case VAR_SA :
        if( int( *pntr ) >= 0 && int( *pntr ) < table_max[ TABLE_SPELL_ACT ] )   
          strcpy( &tmp[18], spell_act_table[(int) *pntr].name );
        else
          sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
        break;

      case VAR_CC :
        strcpy( &tmp[18], cmd_cat_table[(int) *pntr].name );
        break;

      case VAR_POS :
        strcpy( &tmp[18], position_name[ int(*pntr) ] );
        break;

      case VAR_SEX :
        strcpy( &tmp[18], sex_name[ int(*pntr) ] );
        break;

      case VAR_OBJ :
        obj_clss = get_obj_index( int( *pntr ) );
        strcpy( &tmp[18], obj_clss == NULL ? "nothing"
          : obj_clss->Name( int( *(pntr+1) ) ) );
        break;

      case VAR_AFF :
        affect_flags.sprint( &tmp[18], (int*) pntr );
        break;

      case VAR_ALIGN :
        abv_align_flags.sprint( &tmp[18], (int*) pntr );
        break;

      case VAR_LANG :
        lang_flags.sprint( &tmp[18], (int*) pntr );
        break;

      case VAR_PERM :
        permission_flags.sprint( &tmp[18], (int*) pntr );
        break;

      case VAR_LOC :
        if( *((int*) pntr) == 0 ) 
          strcpy( &tmp[18], "anywhere" ); 
        else
          location_flags.sprint( &tmp[18], (int*) pntr );
        break;

      case VAR_DICE :
        sprintf_dice( &tmp[18], int( *pntr ) );
        break; 

      case VAR_LEECH :
        sprintf_leech( &tmp[18], int( *pntr ) );
        break;
      }

    if( single ) {
      strcat( tmp, "\n\r" );
      page( ch, tmp );
      }
    else {
      if( strlen( tmp ) > 30 ) {
        page( ch, "%s     %s\n\r", col%2 ==1 ? "\n\r" : "", tmp );  
        col = 1;
        }
      else if( col%2 != 1 ) 
        page( ch, "     %-30s", tmp );
      else
        page( ch, "%s\n\r", tmp );
      }
    }

  if( !single && col%2 == 1 )
    page( ch, "\n\r" );

  if( table != TABLE_SPELL_ACT )
    return;

  bool found = FALSE;

  page( ch, "\n\rUsed By:\n\r" );

  for( int i = 0; i < MAX_SPELL; i++ )
    for( int j = 0; j < spell_table[i].wait; j++ )
      if( spell_table[i].action[j] == entry ) {
        found = TRUE;
        page( ch, "  %s\n\r", spell_table[i].name );
        break;
        }

  if( !found ) 
    page( ch, "  nothing\n\r" );
}


/*
 *   ADDING/REMOVING ENTRIES
 */


void init_entry( int i, char* name )
{
  void**  pntr;
  int        j  = table_max[i];
  int        k;

  (void*) pntr = table_pntr( i,0,j );
         *pntr = (void*) alloc_string( name, MEM_TABLE );

  for( k = 1; *table_field[i][k] != '\0'; k++ ) { 
    (void*) pntr = table_pntr( i,k,j );
    switch( entry_type( i,k ) ) {
     case VAR_CHAR :
     case VAR_FUNC :
      (const char*) *pntr = empty_string;
      break;
  
     case VAR_INT :
     case VAR_SA :
     case VAR_SIZE :
     case VAR_CC :
     case VAR_OBJ :
     case VAR_STYPE :
      *pntr = 0;
      break;
      }
    }

  table_max[i]++;

  return;
}


void sort_table( char_data* ch, int table )
{
  int  max  = table_max[table];
  int  min;
  int    i;
  int    j;

  if( table > TABLE_SOC_VYAN  && table != TABLE_COMMAND 
    && table != TABLE_SPELL_ACT ) {
    send( ch, "That would destroy the universe as we now know it.\n\r" );
    return;
    }

  for( i = 0; i < max-1; i++ ) {
    min = i;
    for( j = i; j < max; j++ ) 
      if( strcasecmp( entry_name( table, j ), entry_name( table, min ) ) < 0 )
        min = j;
    if( min != i ) 
      swap_entries( table, i, min );
    }

  send( ch, "%s table sorted.\n\r", table_name[table] );

  return;
}


/*
 *  DELETING ENTRIES
 */


bool extract_nation( char_data* ch, int nation )
{
  int i;

  if( nation+1 != table_max[ TABLE_NATION ] ) {
    send( ch, "You may only delete the last entry of that table.\n\r" );
    return FALSE;
    }

  for( i = 0; i < MAX_SPECIES; i++ )
    if( species_list[i] != NULL
      && species_list[i]->nation == nation ) {
      send( ch, "Mobs belonging to that nation still exist.\n\r" );
      return FALSE;
      }

  return TRUE;
}


void remove_entry( char_data* ch, int table, int entry )
{
  wizard_data*  imm;

  if( table > TABLE_SOC_VYAN && table != TABLE_COMMAND
    && table != TABLE_NATION ) {
    send( ch, "That table cannot have entries deleted from it.\n\r" );
    return;
    }

  if( table == TABLE_NATION && !extract_nation( ch, entry ) ) 
    return;

  send( ch, "Entry %s removed from table %s.\n\r",
    entry_name( table, entry ), table_name[table] );

  for( int i = 0; i < player_list; i++ ) {
    if( player_list[i]->Is_Valid( )
      && ( imm = wizard( player_list[i] ) ) != NULL
      && imm->table_edit[0] == table
      && imm->table_edit[1] == entry ) {
      send( imm, "The table entry you were editing was deleted.\n\r" );
      imm->table_edit[0] = 0;
      imm->table_edit[1] = 0;
      }
    }
   
  for( int i = entry+1; i < table_max[table]; i++ )
    swap_entries( table, i-1, i );

  table_max[table]--;
}


/*
 *   SWAPPING OF ENTRIES
 */


void swap_entries( int table, int e1, int e2 )
{
  int           size  = table_size[table];
  char*         temp;
  void*        pntr1  = table_pntr( table,0,e1 );
  void*        pntr2  = table_pntr( table,0,e2 );
  wizard_data*   imm;

  temp = new char[size];

  memcpy( temp,  pntr1, size );
  memcpy( pntr1, pntr2, size );
  memcpy( pntr2, temp,  size );

  delete [] temp;

  for( int i = 0; i < player_list; i++ )
    if( player_list[i]->Is_Valid( ) 
      && ( imm = wizard( player_list[i] ) ) != NULL
      && imm->table_edit[0] == table )
      exchange( imm->table_edit[1], e1, e2 ); 
    
  if( table == TABLE_SPELL_ACT ) 
    for( int i = 0; i < MAX_SPELL; i++ ) 
      for( int j = 0; j < 5; j++ )
        exchange( spell_table[i].action[j], e1, e2 );
}










