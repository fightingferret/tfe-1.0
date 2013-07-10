#include "define.h"
#include "struct.h"


const char* material_name [] = { "paper", "wood",
  "leather", "unused", "cloth", "glass", "stone", "bone", "flesh", "organic",
  "unused", "bronze", "iron", "steel", "mithril", "adamantine",
  "electrum", "silver", "gold", "copper", "platinum", "krynite" };

const char* oflag_name [ MAX_OFLAG ] = { "glow", "hums", "dark", "lock",
  "evil", "invis", "magic", "no.drop", "sanctified", "flaming", "backstab",
  "no.disarm", "noremove", "inventory", "no.shield", "no.major", "no.show",
  "no.sacrifice", "water.proof", "appraised", "no.sell", "no.junk",
  "identified", "rust.proof", "uses_skin", "is.chair", "no.save",
  "burning", "additive", "good", "use.the", "replicate",
  "known.liquid", "poison.coated", "no.auction", "no.enchant",
  "copied", "random.metal", "covers" };

const char* layer_name [ MAX_LAYER ] = { "bottom", "under",
  "base", "over", "top" };

const char* oflag_ident [ MAX_OFLAG ] = { "", "", "", "", "", "", "",
  "Is cursed (undropable).", "Has been sanctified.", "", "", "",
  "Is cursed (noremove).", "", "", "",  
  "", "", "", "", "", "", "", "Is rust proof", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "" };

const char* anti_flags [ MAX_ANTI ] = { "anti-human",
  "anti-elf", "anti-gnome", "anti-dwarf", "anti-halfling",
  "anti-ent", "anti-centaur", "anti-lizardman",
  "anti-ogre", "anti-troll", "anti-orc", "anti-goblin", "anti-vyan",
  "unused",
  "anti-mage", "anti-cleric", "anti-thief", "anti-warrior",
  "anti-paladin", "anti-ranger", "anti-psionic", "anti-monk", "anti-bard",
  "unused", "unused", "unused",
  "anti-good", "anti-neutral", "anti-evil",
  "anti-lawful", "anti-n2??", "anti-chaotic" };
  
const char* size_flags [ MAX_SFLAG ] = { "custom_fit", "size_specific",
  "race_specific", "random_size",
  "tiny", "small", "medium", "large", "giant",
  "human", "elf", "gnome", "dwarf", "halfling", "ent", "centaur",
  "lizardman", "ogre", "troll", "orc", "goblin", "vyan" };

const char* restriction_flags [ MAX_RESTRICTION ] = { "bladed",
  "no_hide", "no_sneak", "dishonorable" };

static const char* no_permission =
  "You don't have permission to alter that object.\r\n";


int   select         ( obj_clss_data*, char_data*, char* );
void  display        ( obj_clss_data*, char_data*, char*, int& );
void  oset_affect    ( char_data*, obj_clss_data*, char* );
void  oedit_replace  ( char_data*, char* );


/*
 *   PERMISSION ROUTINE
 */


bool char_data :: can_edit( obj_clss_data* obj_clss )
{
  if( has_permission( this, PERM_ALL_OBJECTS ) 
    || is_name( descr->name, obj_clss->creator ) )
    return TRUE;

  return FALSE;
}


/*
 *   OFIND ROUTINES
 */


int select_ofind( obj_clss_data* obj, char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  oprog_data*      oprog;
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int               i, j;
  int             length;

#define types 7

  char flag [types+1] = "sfowirm";

  int  max [types] = { MAX_SFLAG, MAX_ENTRY_AFF_CHAR, MAX_OFLAG,
    MAX_ITEM_WEAR, MAX_ANTI,
    MAX_RESTRICTION, MAX_MATERIAL };

  const char** name1 [types] = { &size_flags[0], &aff_char_table[0].name,
    &oflag_name[0], &wear_name[0], &anti_flags[0], &restriction_flags[0],
    &material_name[0] };
  const char** name2 [types] = { &size_flags[1], &aff_char_table[1].name,
    &oflag_name[1], &wear_name[1], &anti_flags[1], &restriction_flags[1],
    &material_name[1] };

  int* flag_value [types] = { &obj->size_flags, obj->affect_flags,
    obj->extra_flags, &obj->wear_flags, &obj->anti_flags,
    &obj->restrictions, &obj->materials };

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
      }
    else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help ofind.\r\n" );
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
      send( ch, "All flags require an argument - See help ofind.\r\n" );
      return -1;
      }
  
    for( i = 0; strncmp( argument-1, " -", 2 ) && *argument != '\0'; ) {
      if( i > ONE_LINE-2 ) {
        send( ch, "Flag arguments must be less than one line.\r\n" );
        return -1;
        } 
      tmp[i++] = *argument++;
      }

    for( ; isspace( tmp[i-1] ); i-- );

    tmp[i] = '\0';
    string = NULL;
    length = i;

    switch( letter ) {
      case 't' :  string = item_type_name[ obj->item_type ];     break;
      case 'c' :  string = obj->creator;                         break;
      }

    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      continue;
      }

    if( letter == 'a' || letter == 'n' ) {
      if( !is_name( tmp, obj->Name( ) ) )
        return 0;
      continue;
      }

    if( letter == 'b' ) {
      if( !is_name( tmp, name_before( obj ) ) ) 
        return 0;
      continue;
      }

    if( letter == 'T' ) {
      for( i = 0; !fmatches( tmp, oprog_trigger[i] ); i++ ) 
        if( i == MAX_OPROG_TRIGGER-1 ) {
          send( ch, "Unknown trigger type, see help ofind.\r\n" );
          return -1;
	  } 
      for( oprog = obj->oprog; oprog != NULL && oprog->trigger != i;
        oprog = oprog->next );
      if( ( oprog != NULL ) != negative )
        continue;
      return 0;
      }

    if( letter == 'W' ) {
      if( obj->item_type != ITEM_WEAPON ) 
        return 0;
      for( i = 0; i < MAX_SKILL-WEAPON_FIRST; i++ ) 
        if( fmatches( tmp, skill_table[ WEAPON_FIRST+i ].name ) ) {
          if( ( obj->value[3] == i ) == negative )
            return 0;
          break;
          }
      if( i == MAX_SKILL-WEAPON_FIRST ) {
        send( ch, "Unknown weapon class - See help ofind.\r\n" );
        return -1;
        }
      continue;
      }

    for( i = 0; i < types; i++ ) 
      if( letter == flag[i] )
        break;

    if( i != types ) {
      for( j = 0; j < max[i]; j++ ) 
        if( fmatches( tmp, *(name1[i]+j*(name2[i]-name1[i])) ) ) {
          if( is_set( flag_value[i], j ) == negative )
            return 0;
          break;
          }
      if( j != max[i] )
        continue;
      }

    send( ch, "Unknown flag - See help ofind.\r\n" );
    return -1;
    }

#undef types
}


void display_ofind( obj_clss_data* obj, char_data* ch, char* buf,
  int& length )
{
  sprintf( &buf[length], "[%4d] %-33s %s %s %s%4d%4d %s ",
    obj->vnum, truncate( (char *) obj->Name( ), 33 ), int4( obj->cost ),
    int4( obj->weight ), int4( obj->count ),
    obj->blocks, obj->level, int4( obj->durability ) ); 
  length += strlen( &buf[length] );

  switch( obj->item_type ) {
   case ITEM_ARMOR :
    sprintf( &buf[length], "AC: %d", obj->value[1] );
    break;
   case ITEM_WEAPON :
    sprintf( &buf[length], "Dm: %dd%d", obj->value[1], obj->value[2] );
    break;
    }

  strcat( buf, "\r\n" );
  length += strlen( &buf[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, buf );
    length = 0;
    *buf = '\0';
    } 
}


void do_ofind( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  obj_clss_data*      obj;
  int                   i;
  int              length  = 0;
  bool              found  = FALSE;

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj = obj_index_list[i] ) != NULL ) { 
      switch( select_ofind( obj, ch, argument ) ) {
       case -1 : return;
       case  1 :
        if( !found ) {
          found = TRUE;
          page_underlined( ch, "Vnum   Name                         \
     Cost  Wgt  Num Ing Lvl Dur\r\n" );
	  }
        display_ofind( obj, ch, buf, length );
        }
      }
    }

  if( !found ) 
    send( ch, "No object class matching search was found.\r\n" );
  else
    page( ch, buf );
}


void do_identify( char_data* ch, char* argument )
{
  obj_data* obj;

  if( *argument == '\0' ) {
    send( ch, "Identify what?\r\n" );
    return;
    }

  if( ( obj = one_object( ch, argument,
    "identity", &ch->contents, &ch->wearing ) ) == NULL ) 
    return;

  spell_identify( ch, NULL, obj, 10, -1 );
}


/*
 *   OBJECT ONLINE COMMANDS
 */


void do_oedit( char_data* ch, char* argument )
{
  char                 buf  [ MAX_INPUT_LENGTH ];
  obj_data*            obj;
  obj_clss_data*  obj_clss;
  obj_clss_data*  obj_copy; 
  wizard_data*      wizard;
  int                    i;

  wizard    = (wizard_data*) ch;
  obj_clss  = wizard->obj_edit;

  if( *argument == '\0' ) {
    send( ch, "Which object do you want to edit?\r\n" );
    return;
    }

  if( matches( argument, "new" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What do you want to name the new object?\r\n" );
      return;
      }

    if( number_arg( argument, i ) ) {
      if( ( obj_copy = get_obj_index( i ) ) == NULL ) {
        send( ch, "The vnum %d cooresponds to no existing object.\r\n", i );
        return;
        }
      }
    else
      obj_copy = NULL;
     
    for( i = 1; ; i++ ) {
      if( i >= MAX_OBJ_INDEX ) {
        send( ch, "Mud is out of object vnums.\r\n" );
        return;
        }
      if( obj_index_list[i] == NULL )
        break;
      }

    obj_clss          = new obj_clss_data;
    obj_index_list[i] = obj_clss;

    if( obj_copy != NULL ) 
      memcpy( obj_clss, obj_copy, sizeof( obj_clss_data ) );

    obj_clss->vnum  = i;
    obj_clss->fakes = i;

    obj_clss->singular = alloc_string( argument, MEM_OBJ_CLSS );
    sprintf( buf, "%ss", obj_clss->singular );
    obj_clss->plural  = alloc_string( buf, MEM_OBJ_CLSS );

    if( obj_copy == NULL ) {
      obj_clss->before          = empty_string;
      obj_clss->after           = empty_string;
      obj_clss->long_p          = empty_string;
      obj_clss->long_s          = empty_string;
      obj_clss->prefix_plural   = empty_string;
      obj_clss->prefix_singular = empty_string;
      }
    else {
      obj_clss->before   = alloc_string( obj_copy->before, MEM_OBJ_CLSS );
      obj_clss->after    = alloc_string( obj_copy->after,  MEM_OBJ_CLSS );
      obj_clss->long_p   = alloc_string( obj_copy->long_p, MEM_OBJ_CLSS );
      obj_clss->long_s   = alloc_string( obj_copy->long_s, MEM_OBJ_CLSS );
      obj_clss->prefix_singular = alloc_string( obj_copy->prefix_singular,
        MEM_OBJ_CLSS );
      obj_clss->prefix_plural   = alloc_string( obj_copy->prefix_plural,
        MEM_OBJ_CLSS );
      }

    obj_clss->creator  = alloc_string( ch->descr->name, MEM_OBJ_CLSS );     
    obj_clss->last_mod = alloc_string( ch->descr->name, MEM_OBJ_CLSS );
    obj_clss->oprog    = NULL;
    obj_clss->date     = current_time;

    if( obj_copy == NULL ) {
      obj_clss->item_type       = 1;
      obj_clss->size_flags      = 0;
      obj_clss->anti_flags      = 0;
      obj_clss->restrictions    = 0;   
      obj_clss->materials       = 0;
      obj_clss->wear_flags      = 1;
      obj_clss->weight          = 1;
      obj_clss->cost            = 0;
      obj_clss->level           = 1;
      obj_clss->limit           = -1;
      obj_clss->repair          = 10;
      obj_clss->durability      = 1000;
      obj_clss->blocks          = 0;

      vzero( obj_clss->extra_flags, 2 );
      vzero( obj_clss->value, 4 );
      vzero( obj_clss->affect_flags, AFFECT_INTS );
      }
     
    create( obj_clss )->To( ch );

    wizard->obj_edit     = obj_clss;
    wizard->oprog_edit   = NULL;
    wizard->oextra_edit  = NULL;

    send( ch, "Object created: %s (vnum %d)\r\n",
      obj_clss->Name( ), obj_clss->vnum );
    return;
    }

  if( exact_match( argument, "replace" ) ) {
    oedit_replace( ch, argument );
    return;
    }

  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      if( ( obj_clss = wizard->obj_edit ) == NULL ) {
        send( "You aren't editing any object.\r\n", ch );
        return;
        }
      }
    else {
      if( ( obj_clss = get_obj_index( atoi( argument ) ) ) == NULL ) {
        send( ch, "There is no object by that number.\r\n" );
        return;
        }
      } 

    if( !ch->can_edit( obj_clss ) ) {
      send( ch, no_permission );
      return;
      }

    if( !can_extract( obj_clss, ch ) )
      return;

    send( ch, "You delete %s.\r\n", obj_clss->Name( ) );
    sprintf( buf, "Object deleted: %s (%s)", obj_clss->Name( ),
      ch->descr->name );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES ); 

    wizard->obj_edit = obj_clss;
    extract( wizard, offset( &wizard->obj_edit, wizard ), "object" );
    obj_index_list[obj_clss->vnum] = NULL;
    delete obj_clss;

    return;
    }

  if( number_arg( argument, i ) ) {
    if( ( obj_clss = get_obj_index( i ) ) == NULL ) {
      send( ch, "No object has that vnum.\r\n" );
      return;
      }
    wizard->obj_edit = obj_clss;
    }
  else {
    if( ( obj = one_object( ch, argument, 
      "oedit", &ch->contents, &ch->wearing, 
      ch->array ) ) == NULL )
      return;
    wizard->obj_edit = obj->pIndexData;
    }

  wizard->oextra_edit = NULL;
  wizard->oprog_edit  = NULL;

  send( ch, "Odesc and oset now operate on %s.\r\n",
    wizard->obj_edit->Name( ) );
}


void oedit_replace( char_data* ch, char* argument )
{
  area_data*          area;
  reset_data*        reset;
  room_data*          room;
  species_data*    species;
  obj_clss_data*      obj1  = NULL;
  obj_clss_data*      obj2  = NULL;
  int                count  = 0;
  int                 i, j;

  if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
    send( ch, "Syntax: oedit replace <vnum_old> <vnum_new>.\r\n" );
    return;
    }

  if( ( obj1 = get_obj_index( i ) ) == NULL
    || ( obj2 = get_obj_index( j ) ) == NULL ) {
    send( ch, "Vnum %d doesn't coorespond to an existing object.\r\n",
      obj1 == NULL ? i : j );
    return;
    }

  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( reset->vnum == i 
          && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          reset->vnum = j;
          count++;
          area->modified = TRUE;
	  }

  for( int k = 0; k < MAX_SPECIES; k++ ) 
    if( ( species = species_list[k] ) != NULL ) 
      for( reset = species->reset; reset != NULL; reset = reset->next ) 
        if( reset->vnum == i 
	  && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          reset->vnum = j;
          count++;
	  }

  send( ch, "Vnum %d replaced by %d in %d reset%s.\r\n",
    i, j, count, count != 1 ? "s" : "" );
}


/*
 *   OBJECT DESCRIPTIONS
 */


void do_odesc( char_data* ch, char* argument )
{
  wizard_data*  wizard;

  wizard = (wizard_data*) ch;

  if( wizard->obj_edit == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
    }

  if( wizard->oextra_edit == NULL ) {
    send( "You aren't editing any extra.\r\n", ch );
    return;
    }
  else
    wizard->oextra_edit->text = edit_string( ch,
      argument, wizard->oextra_edit->text, MEM_OBJ_CLSS );
}


void do_oextra( char_data* ch, char* argument )
{
  obj_clss_data*  obj_clss;
  wizard_data*      wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( "You aren't editing any object.\r\n", ch );
    return;
    }

  if( ch->can_edit( obj_clss ) ) 
    edit_extra( obj_clss->extra_descr, wizard, offset( &wizard->oextra_edit,
      wizard ), argument, "odesc" );
}


void do_oflag( char_data* ch, char* argument )
{
  obj_clss_data*   obj_clss;
  wizard_data*       wizard;
  const char*        string;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    page( ch, "You aren't editing any object.\r\n" );
    return;
    }

  #define types 10

  const char* title [types] = { "Size", "Affect", "Object", "Wear", "Layer",
    "Anti", "Restriction", "Material", "Container", "Trap" }; 

  int  max [types] = { MAX_SFLAG, MAX_ENTRY_AFF_CHAR, MAX_OFLAG,
    MAX_ITEM_WEAR, MAX_LAYER, MAX_ANTI,
    MAX_RESTRICTION, MAX_MATERIAL, MAX_CONT, MAX_TRAP };

  const char** name1 [types] = { &size_flags[0], &aff_char_table[0].name,
    &oflag_name[0], &wear_name[0], &layer_name[0],
    &anti_flags[0], &restriction_flags[0],
    &material_name[0], &cont_flag_name[0], &trap_flags[0] };
  const char** name2 [types] = { &size_flags[1], &aff_char_table[1].name,
    &oflag_name[1], &wear_name[1], &layer_name[1],
    &anti_flags[1], &restriction_flags[1],
    &material_name[1], &cont_flag_name[1], &trap_flags[1] };

  int *flag_value [types] = { &obj_clss->size_flags, obj_clss->affect_flags,
    obj_clss->extra_flags, &obj_clss->wear_flags, &obj_clss->layer_flags,
    &obj_clss->anti_flags,
    &obj_clss->restrictions, &obj_clss->materials, &obj_clss->value[1],
    &obj_clss->value[0] };

  int uses_flag [types] = { 1, 1, 1, 1, 1, 1, 1, 1, 
    obj_clss->item_type == ITEM_CONTAINER,
    obj_clss->item_type == ITEM_TRAP };

  string = flag_handler( title, name1, name2, flag_value, max, uses_flag,
    ch->can_edit( obj_clss ) ? NULL : no_permission, ch, argument, types ); 

  if( string != NULL )
    obj_log( ch, obj_clss->vnum, string );

  #undef types
}


void do_oload( char_data* ch, char *argument )
{  
  obj_clss_data*  obj_clss;
  obj_data*            obj;
  int                 vnum;

  if( *argument == '\0' || !number_arg( argument, vnum ) ) {
    send( ch, "Syntax: oload <vnum>.\r\n" );
    return;
    }

  if( ( obj_clss = get_obj_index( vnum ) ) == NULL ) {
    send( ch, "No object has that vnum.\r\n" );
    return;
    }

  obj         = create( obj_clss );
  obj->source = alloc_string( ch->real_name( ), MEM_OBJECT );

  enchant_object( obj );
  set_alloy( obj, 100 );

  obj->owner = ch->pcdata->pfile;

  send( ch, "You have created %s!\r\n", obj );
  send( *ch->array, "%s has created %s!\r\n", ch, obj );

  obj->To( can_wear( obj, ITEM_TAKE ) ? ch : ch->array->where );
  consolidate( obj );
}


/*
 *   OSET
 */


bool oset_drink_container( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "capacity",        -1,    10000,   &obj->value[0]       },
    { "",                 0,        0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_container( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "capacity",         0,    10000,   &obj->value[0]       },
    { "",                 0,        0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_reagent( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "charges",          0,      100,   &obj->value[0]       },
    { "",                 0,        0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_food( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "nourishment",      0,      100,   &obj->value[0]       },
    { "",                 0,        0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_armor( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "enchantment",     -5,        5,   &obj->value[0]       },
    { "ac",               0,       50,   &obj->value[1]       },
    { "global ac",        0,       50,   &obj->value[2]       },
    { "",                 0,        0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_corpse( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "halflife",   0,  QUEUE_LENGTH-1,   &obj->value[0]       },
    { "species",    0,     MAX_SPECIES,   &obj->value[1]       },
    { "",           0,               0,   NULL                 },
    };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_weapon( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "enchantment",     -5,        5,   &obj->value[0]       },
    { "damdice",          0,       50,   &obj->value[1]       },
    { "damside",          0,       50,   &obj->value[2]       },
    { "",                 0,        0,   NULL                 },
    };

  if( process( int_list, ch, obj->Name( ), argument, obj ) )
    return TRUE;

  #define wn(i) skill_table[ WEAPON_UNARMED+i ].name

  class type_field type_list[] = {
    { "class", MAX_WEAPON, &wn(0),  &wn(1),  &obj->value[3] },
    { "",      0,          NULL,    NULL,    NULL        }
    };

  #undef wn

  return process( type_list, ch, obj->Name( ), argument, obj );
}


void do_oset( char_data* ch, char* argument )
{
  char                 buf  [ MAX_INPUT_LENGTH ];
  obj_clss_data*  obj_clss;
  obj_data*            obj;
  wizard_data*      wizard;
  char*             string;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
    }

  if( !ch->can_edit( obj_clss ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "Syntax: oset <field> <value>\r\n" );
    return;
    }

  free_string( obj_clss->last_mod, MEM_OBJ_CLSS );
  obj_clss->last_mod = alloc_string( ch->descr->name, MEM_OBJ_CLSS );

  class int_field int_list[] = {
    { "fakes",            0,    10000,   &obj_clss->fakes       },
    { "level",            0,       99,   &obj_clss->level       },
    { "value0",     -100000,   100000,   &obj_clss->value[0]    },
    { "value1",     -100000,   100000,   &obj_clss->value[1]    },
    { "value2",     -100000,   100000,   &obj_clss->value[2]    },
    { "value3",     -100000,   100000,   &obj_clss->value[3]    },
    { "limit",           -1,   100000,   &obj_clss->limit       },
    { "repair",           0,       10,   &obj_clss->repair      },
    { "durability",       1,    10000,   &obj_clss->durability  },
    { "ingots",           0,      100,   &obj_clss->blocks      },
    { "cost",             0,  1000000,   &obj_clss->cost        },
    { "light",            0,      100,   &obj_clss->light       },
    { "",                 0,        0,   NULL                   },
    };

  if( process( int_list, ch, obj_clss->Name( ), argument, obj_clss ) )
    return;

  class cent_field cent_list[] = {
    { "weight",           0,    10000,   &obj_clss->weight      },
    { "",                 0,        0,   NULL                   },
    };

  if( process( cent_list, ch, obj_clss->Name( ), argument, obj_clss ) )
    return;

  char *word[] = { "singular", "plural", "after", "before",
    "long_s", "long_p", "creator", "prefix_singular",
    "prefix_plural" };

  char **pChar[] = { &obj_clss->singular, &obj_clss->plural,
    &obj_clss->after, &obj_clss->before, &obj_clss->long_s,
    &obj_clss->long_p, &obj_clss->creator,
    &obj_clss->prefix_singular, &obj_clss->prefix_plural };

  for( int i = 0; i < 9; i++ ) {
    if( matches( argument, word[i] ) ) {
      sprintf( buf, "%s set to %s.", word[i], argument );
      obj_log( ch, obj_clss->vnum, buf );
      send( ch, "The %s of %s is now:\r\n%s\r\n",
        word[i], obj_clss->Name( ), argument );

      string = alloc_string( argument, MEM_OBJ_CLSS );

      if( i < 4 ) {
        for( int j = 0; j < obj_list; j++ ) {
          obj = obj_list[j];
          if( obj->pIndexData == obj_clss ) {
            char **pChar2[] = { &obj->singular, &obj->plural,
              &obj->after, &obj->before };
            if( *pChar2[i] == *pChar[i] )
              *pChar2[i] = string;
            }
	  }
        }
      free_string( *pChar[i], MEM_OBJ_CLSS );
      *pChar[i] = string;
      return;
      }          
    }

#define itn         item_type_name

  class type_field type_list [] = {
    { "type",   MAX_ITEM,    &itn[0], &itn[1],  &obj_clss->item_type },
    { "",       0,           NULL,    NULL,     NULL                 },
    };

  if( process( type_list, ch, obj_clss->Name( ), argument, obj_clss ) )
    return;

#undef itn

  switch( obj_clss->item_type ) {
    case ITEM_WEAPON :
      if( oset_weapon( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_ARMOR :
      if( oset_armor( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_CONTAINER :
      if( oset_container( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_REAGENT :
      if( oset_reagent( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_FOOD :
      if( oset_food( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_DRINK_CON :
      if( oset_drink_container( ch, obj_clss, argument ) )
        return;
      break;
    case ITEM_CORPSE :
      if( oset_corpse( ch, obj_clss, argument ) )
        return;
      break;
    }

  if( matches( argument, "affect" ) ) {
    oset_affect( ch, obj_clss, argument );
    return;
    }
      
  send( ch, "Unknown field.\r\n" );
}


void oset_affect( char_data* ch, obj_clss_data* obj_clss, char* argument )
{
  int                  i;
  int                col;
  int                num;
  affect_data*       paf  = NULL;
      
  if( *argument == '\0' ) {
    send( ch, "Affect Types:\r\n" );
    for( i = 1, col = 0; i < MAX_AFF_LOCATION; i++ ) {
      send( ch, "%15s", affect_location[i] );
      if( ++col%3 == 0 )
        send( ch, "\r\n" );
      }
    if( col%3 != 0 )
      send( ch, "\r\n" );
    return;
    }
      
  for( i = 1; i < MAX_AFF_LOCATION; i++ ) {
    if( matches( argument, affect_location[i] ) ) {
      for( int j = 0; j < obj_clss->affected; j++ ) 
        if( obj_clss->affected[j]->location == i ) {
          paf = obj_clss->affected[j];
          break;
	  }
      if( ( num = atoi( argument ) ) == 0 ) {
        if( paf != NULL ) 
          obj_clss->affected -= paf;
        }
      else {
        if( paf == NULL ) {
          paf                 = new affect_data;
          paf->type           = AFF_NONE;
          paf->duration       = -1;
          paf->location       = i;
          obj_clss->affected += paf;
          }
        paf->modifier      = num;
        }
      send( ch, "%s modifier of %d set on %s.\r\n", 
        affect_location[i], num, obj_clss->Name( ) );
      return;
      }
    }
       
  send( ch, "Unknown affect location.\r\n" );
}


/*
 *  OSTAT ROUTINE
 */


void do_ostat( char_data* ch, char* argument )
{
  char                   tmp  [ 3*MAX_STRING_LENGTH ];
  affect_data*           paf;
  obj_data*              obj  = NULL;
  obj_clss_data*         key;
  obj_clss_data*    obj_clss;
  wizard_data*        wizard  = (wizard_data*) ch;

  if( *argument == '\0' ) {
    if( ( obj_clss = wizard->obj_edit ) == NULL ) {
      send( ch, "Specify object or select one with oedit.\r\n" );
      return;
      }
    }
  else if( is_number( argument ) ) {
    if( ( obj_clss = get_obj_index( atoi( argument ) ) ) == NULL ) {
      send( ch, "No object has that vnum.\r\n" );
      return;
      }
    }
  else {
    if( ( obj = one_object( ch, argument, "ostat",
       &ch->contents, &ch->wearing,
       ch->array ) ) == NULL )
      return;
    obj_clss = obj->pIndexData;
    }

  page_title( ch, obj_clss->Name( ) );

  page( ch, "          Vnum: %-10d Creator: %s%s%s\r\n",
    obj_clss->vnum,  color_code( ch, COLOR_BOLD_YELLOW ),
    obj_clss->creator, normal( ch ) );

  page( ch, "         Fakes: %-10s",
    obj_clss->fakes == obj_clss->vnum
    ? "--" : atos( obj_clss->fakes ) );
  if( is_angel( ch ) ) 
    page( ch, "Last Mod: %s", obj_clss->last_mod ); 
  page( ch, "\r\n" );

  page( ch, "          Type: %s\r\n",
    item_type_name[ obj_clss->item_type ] );

  page( ch, "        Weight: %-12.2f Light: %-8d Base Cost: %d\r\n",
    (float) obj_clss->weight/100,
    obj_clss->light, obj_clss->cost );
  page( ch, "         Level: %-11d Number: %-12d Limit: %d\r\n",
    obj_clss->level, obj_clss->count, obj_clss->limit );
  page( ch, "        Repair: %-10d Durabil: %-11d Ingots: %d\r\n\r\n",
    obj_clss->repair, obj_clss->durability, obj_clss->blocks );

  switch( obj_clss->item_type ) {
    case ITEM_WEAPON :
      page( ch, "         Class: %-11s Attack: %s\r\n",
        weapon_class( obj_clss ),
        ( obj_clss->value[3] >= 0 && obj_clss->value[3] < MAX_WEAPON ) ?
        weapon_attack[ obj_clss->value[3] ] : "none" );
      page( ch, "       Damdice: %-10d Damside: %-6d Enchantment: %d\r\n\r\n",
        obj_clss->value[1], obj_clss->value[2], obj_clss->value[0] );
      break;

    case ITEM_ARMOR :
      page( ch, "            AC: %-8d Global AC: %d\r\n",
        obj_clss->value[1], obj_clss->value[2] );
      page( ch, "   Enchantment: %d\r\n\r\n",
        obj_clss->value[0] );
      break;

    case ITEM_CONTAINER :   
      page( ch, "      Capacity: %-14d Key: %d (%s)\r\n\r\n",
        obj_clss->value[0], obj_clss->value[2],
        ( key = get_obj_index( obj_clss->value[2] ) ) == NULL
        ? "none" : key->Name( ) );  
      break;

    case ITEM_DRINK_CON :
      page( ch, "      Capacity: %d\r\n\r\n",
        obj_clss->value[0] );
      break;

    case ITEM_REAGENT :
      page( ch, "       Charges: %d\r\n\r\n", obj_clss->value[0] );
      break;

    case ITEM_FOOD :
      page( ch, "   Nourishment: %d\r\n\r\n", obj_clss->value[0] );
      break;

    case ITEM_CORPSE:
      species_data*   species  = get_species( obj_clss->value[1] );

      page( ch, "      Halflife: %d\r\n", obj_clss->value[0] );
      page( ch, "       Species: %-5d (%s)\r\n\r\n",
	obj_clss->value[1], species == NULL ? "none" : species->Name( ) ); 
      break;
    }

  if( obj != NULL ) {
    page( ch, "     Condition: %-14d Age: %-12d Value: %d\r\n",
      obj->condition, obj->age, obj->Cost( ) );
    page( ch, "        Weight: %.2f lbs\r\n",
      (float) obj->Weight( 1 )/100 );
    page( ch, "         Owner: %s\r\n",
      obj->owner == NULL ? "noone" : obj->owner->name ); 
    page( ch, "\r\n" );
    }

  page( ch, "Prefix_S: %s\r\n", obj_clss->prefix_singular );
  page( ch, "Singular: %s\r\n", obj_clss->singular );
  page( ch, "  Long_S: %s\r\n", obj_clss->long_s );
  page( ch, "Prefix_P: %s\r\n", obj_clss->prefix_plural );
  page( ch, "  Plural: %s\r\n", obj_clss->plural );
  page( ch, "  Long_P: %s\r\n", obj_clss->long_p );
  page( ch, "  Before: %s\r\n", obj_clss->before );
  page( ch, "   After: %s\r\n", obj_clss->after );

  show_extras( ch, obj_clss->extra_descr );

  if( obj_clss->fakes != obj_clss->vnum ) {
    sprintf( tmp, "[ before ][ from Obj %d ]\r\n%s",
      obj_clss->fakes, before_descr( obj_clss ) );
    page( ch, tmp );
    }   

  for( int i = 0; i < obj_clss->affected; i++ ) {
    paf = obj_clss->affected[i];
    if( paf->type == AFF_NONE )  
      page( ch, "Affects %s by %d.\r\n",
        affect_location[ paf->location ], paf->modifier );
    }
}


/*
 *   OWHERE
 */


int select_owhere( obj_clss_data* obj_clss, obj_data* obj,
  char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int             length;

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
      }
    else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help owhere.\r\n" );
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
      send( ch, "All flags require an argument - See help owhere.\r\n" );
      return -1;
      }
  
    for( int i = 0; ; ) {
      if( !strncmp( argument-1, " -", 2 ) || *argument == '\0' ) {
        for( ; isspace( tmp[i-1] ); i-- );
        tmp[i] = '\0';
        length = i;
        break;
        }
      if( i > ONE_LINE-2 ) {
        send( ch, "Flag arguments must be less than one line.\r\n" );
        return -1;
        } 
      tmp[i++] = *argument++;
      }

    string = NULL;

    if( obj != NULL ) {
      switch( letter ) {
        case 's' :  string = obj->source;     break;
        case 'l' :  string = obj->label;      break;
        }
      }
  
    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      continue;
      }

    if( letter == 'n' ) {
      if( !is_name( tmp, obj != NULL 
        ? obj->Seen_Name( ch ) : obj_clss->Name( ) ) )
        return 0;
      continue;
      }

    send( ch, "Unknown flag - See help owhere.\r\n" );
    return -1;
    }
}


inline void display_owhere( obj_data* obj, char_data* ch, char* tmp,
  int& length )
{
  char                tmp1  [ ONE_LINE ];
  char                tmp2  [ ONE_LINE ];
  char                tmp3  [ ONE_LINE ];
  room_data*          room  = NULL;
  char_data*         owner  = NULL;   
  obj_data*      container  = NULL;
  thing_data*        where;

  for( where = obj; ; ) {
    if( where->array == NULL
      || ( where = where->array->where ) == NULL )
      break; 
    if( Room( where ) != NULL )
      room = (room_data*) where;
    else if( object( where ) != NULL )
      container = (obj_data*) where;
    else if( character( where ) != NULL )
      owner = (char_data*) where;
    } 

  strcpy( tmp1, room == NULL ? "Nowhere??" : room->name );
  strcpy( tmp2, owner == NULL ? "" : owner->Name( ) );
  strcpy( tmp3, container == NULL ? "" : container->Name( ) );

  truncate( tmp1, 25 );
  truncate( tmp2, 20 );
  truncate( tmp3, 20 );

  sprintf( &tmp[length], "%26s%7d%5s  %-20s %s\r\n",
    tmp1,
    room == NULL ? -1 : room->vnum,
    int4( obj->number ),
    tmp2,
    tmp3 );

  length += strlen( &tmp[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, tmp );
    length = 0;
    *tmp = '\0';
    } 
}


void do_owhere( char_data* ch, char* argument )
{
  char                  tmp  [ MAX_STRING_LENGTH ];
  obj_data*             obj;
  obj_clss_data*   obj_clss;
  int                length  = 0;
  bool                found;
  bool                first;
  bool             anything  = FALSE;
  int                  i, j;

  sprintf( tmp, "%26s%7s%5s  %-20s %s\r\n",
    "Room", "Vnum", "Nmbr", "Carried By", "Container" );
  page_underlined( ch, tmp );

  for( i = j = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    if( ( found = (select_owhere( obj_clss, NULL, ch, argument ) != 0 ? TRUE : FALSE) ) ) 
      page_divider( ch, obj_clss->Name( ), i );

    first = TRUE;

    for( ; j < obj_list && obj_list[j]->pIndexData == obj_clss; j++ )  
      if( ( obj = obj_list[j] )->Is_Valid( ) )  
        switch( select_owhere( obj_clss, obj, ch, argument ) ) {
          case -1 : return;
          case  1 : 
            if( !found ) {
              found = TRUE;
              page_divider( ch, obj_clss->Name( ), i );
	      }
            first = FALSE;
            display_owhere( obj, ch, tmp, length );
	  }

    if( found ) {
      if( first )
        page_centered( ch, "None found" );
      else {
        length = 0;
        page( ch, tmp );
        } 
      anything = TRUE;
      }
    }

  if( !anything )
    page_centered( ch, "Nothing found" );
}
















