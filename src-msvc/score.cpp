#include "define.h"
#include "struct.h"


char*   armor_name         ( int i );
char*   condition_name     ( char_data *ch );
char*   fame_name          ( int i );
char*   piety_name         ( int i );


/*
 *   STATUS WORD ROUTINES
 */


char* condition_name( char_data* ch )
{
  static char buf [ 50 ];
  
  if( IS_NPC( ch ) )
    return "blood-thirsty";
 
  sprintf( buf, "%s%s%s%s", 
    IS_DRUNK( ch ) ? "drunk" : "sober",
    ch->pcdata->condition[COND_THIRST] < 0 ? " thirsty" : "",
    ch->pcdata->condition[COND_FULL] < 0 ? " hungry" : "",
    IS_AFFECTED( ch, AFF_POISON ) ? " poisoned" : "" );

  return buf;
}


index_data fame_index [] =
{
  { "unknown",    "",    25 },
  { "obscure",    "",    75 },
  { "marginal",   "",   150 },
  { "familiar",   "",   300 },
  { "known",      "",   500 },
  { "famous",     "",   750 },
  { "renowned",   "",   900 },
  { "legendary",  "",    -1 }
};


index_data piety_index [] =
{ 
  { "sacrilegous", "",  -900 },
  { "blasphemous", "",  -700 },
  { "ungodly",     "",  -350 },
  { "impious",     "",  -100 },
  { "indevout",    "",     0 },
  { "moral",       "",   100 },
  { "pious",       "",   350 },
  { "religious.",  "",   700 },
  { "devout",      "",   900 },
  { "fanatical",   "",    -1 },
};


/* 
 *   SHOW IDENTITY ROUTINE
 */


void do_identity( char_data* ch, char* )
{
  char               tmp  [ MAX_STRING_LENGTH ];
  pc_data*        pcdata  = ch->pcdata;
  descr_data*      descr  = ch->descr; 
  player_data*        pc  = player( ch );
  wizard_data*       imm  = wizard( ch );

  if( is_confused_pet( ch ) )
    return;

  page_title( ch, descr->name );
  
  if( ch->species == NULL ) 
    page( ch, "         Class: %-13s Race: %-15s ID#: %d\r\n",
      clss_table[ pcdata->clss ].name, race_table[ ch->shdata->race ].name,
      pcdata->pfile->ident );
  else
    page( ch, "          Race: %s\r\n", 
      race_table[ ch->shdata->race ].name );

  page( ch, "           Sex: %-8s Alignment: %s\r\n",
    sex_name[ch->sex], alignment_table[ ch->shdata->alignment ].name );

  if( pc != NULL ) {
    page( ch, "      Religion: %-12s Piety: %-14s Fame: %s\r\n",
      religion_table[ pcdata->religion ].name,
      lookup( piety_index, pcdata->piety ),
      lookup( fame_index, ch->shdata->fame ) );
    page( ch, "        Deaths: %-7d Recog Char: %d\r\n",
      ch->shdata->deaths,
      pcdata->recognize == NULL ? 0 : pcdata->recognize->size );
    page( ch, "        Height: %-11s Weight: %.2f lbs\n\r\n",
      "??", (float) ch->Weight( )/100 );

    sprintf( tmp,  "-- Time Played: " );
    sprintf_minutes( tmp+16, ch->time_played( ) );
    sprintf( tmp+strlen( tmp ), " --" );
    page_centered( ch, tmp );
    page( ch, "\r\n" );

    page( ch, "     Appearance: %s\r\n", descr->singular ); 
    if( pcdata->tmp_short != empty_string ) 
      page( ch, "     Unapproved: %s\r\n", pcdata->tmp_short );
    } 

  page( ch, "       Keywords: %s\r\n", ch->descr->keywords );

  if( ch->species == NULL && pcdata->tmp_keywords != empty_string ) 
    page( ch, "     Unapproved: %s\r\n", pcdata->tmp_keywords );

  if( has_permission( ch, PERM_GOTO ) ) {
    page( ch, "         Bamfin: %s\r\n",
      imm->bamfin == empty_string ? "none" : imm->bamfin );
    page( ch, "        Bamfout: %s\r\n",
      imm->bamfout == empty_string ? "none" : imm->bamfout );
    }

  return;
}


/*
 *   SCORE ROUTINE
 */


void do_score( char_data* ch, char* )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_INPUT_LENGTH ];
  pc_data*       pcdata  = ch->pcdata;
  player_data*   pc  = player( ch );
  obj_data*       wield;
  obj_data*      wield2;
  int                 i;

  if( is_confused_pet( ch ) )
    return;

  page_title( ch, ch->descr->name );

  sprintf( buf,    "       " );
  sprintf( buf+5,  "    Level: %d          ", ch->shdata->level );

  if( pc != NULL ) {
    if( ch->shdata->level >= LEVEL_BUILDER ) 
      sprintf( buf+25, "    Trust: %d\r\n", get_trust( ch ) );
    else if( exp_for_level( ch ) > ch->exp )
      sprintf( buf+25, "  Exp.Lvl: %d\r\n", exp_for_level( ch )-ch->exp );
    else 
      sprintf( buf+25, "  Exp.Lvl: -\r\n" );
    }
  else
    sprintf( buf+25, "\r\n" );

  page( ch, buf );

  sprintf( buf+5,  "     Hits: %d/%d       ", ch->hit,  ch->max_hit );
  sprintf( buf+25, "   Energy: %d/%d       ", ch->mana, ch->max_mana );
  sprintf( buf+45, "    Moves: %d/%d\r\n",    ch->move, ch->max_move );
  page( ch, buf );

  sprintf( buf+5,  "    Items: %d/%d       ",
    ch->contents.number, ch->can_carry_n( ) );
  sprintf( buf+25, "   Weight: %d/%d       ",
    ch->contents.weight/100, ch->Capacity( )/100 );

  if( pc != NULL )
    sprintf( buf+45, "      Age: %d years\r\n", pc->Age( ) );
  else
    sprintf( buf+45, "\r\n" ); 
  page( ch, buf );

  if( pc != NULL ) 
    page( ch, "      Qst_Pnts: %-8d Prac_Pnts: %-9d Gsp_Pnts: %d\r\n",
      pcdata->quest_pts, pcdata->practice, pc->gossip_pts );

  page( ch, "     Hit Regen: %-8.1f Ene Regen: %-9.1f Mv Regen: %.1f\r\n",
    float( ch->Hit_Regen( )/10. ), float( ch->Mana_Regen( )/10. ),
    float( ch->Move_Regen( )/10. ) );

  wield = ch->Wearing( WEAR_HELD_R );

  if( ( wield2 = ch->Wearing( WEAR_HELD_L ) ) == NULL ) {
    page( ch, "       Hitroll: %-+10.2f Damroll: %+.2f\r\n\r\n", 
      ch->Hitroll( wield ), ch->Damroll( wield ) );
    }
  else {
    page( ch,
      "     Hitroll: %+3.2f/%+3.2f      Damroll: %+3.2f/%+3.2f\r\n\r\n", 
      ch->Hitroll( wield ), ch->Hitroll( wield2 ),
      ch->Damroll( wield ), ch->Damroll( wield2 ) );
    }

  /*  ABILITIES */

  sprintf( buf+5, "Str: %2d(%2d)  Int: %2d(%2d)  Wis: %2d(%2d)",
    ch->Strength( ),     ch->shdata->strength,
    ch->Intelligence( ), ch->shdata->intelligence,
    ch->Wisdom( ),       ch->shdata->wisdom );
  sprintf( buf+strlen( buf ), "  Dex: %2d(%2d)  Con: %2d(%2d)\r\n\r\n",
    ch->Dexterity( ),    ch->shdata->dexterity,
    ch->Constitution( ), ch->shdata->constitution );
  page( ch, buf );

  /*  RESISTANCES */

  const char* name [] = { "Magic", "Fire", "Cold", "Mind", "Electric", 
    "Acid", "Poison" }; 

  int resist [] = {
    ch->Save_Magic( ), ch->Save_Fire(  ),
    ch->Save_Cold( ),  ch->Save_Mind( ), 
    ch->Save_Shock( ), ch->Save_Acid( ), 
    ch->Save_Poison( ) };

  const char* color [] = { 
    "",  red( ch ), blue( ch ), "", yellow( ch ), green( ch ), "" };

  for( i = 0; i < 7; i++ ) {
    sprintf( tmp+40*i, "%10s", "" );
    sprintf( tmp+40*i+9-strlen( name[i] ), "%s%s: %+d%%%s%s",
      color[i], name[i], resist[i],
      normal( ch ), abs( resist[i] ) > 99 ? "" 
      : ( abs( resist[i] ) > 9 ? " " : "  " ) ); 
    }

  page( ch, "   %s%s%s%s\r\n", tmp, tmp+40, tmp+80, tmp+120 );
  page( ch, "   %s%s%s\r\n\r\n", tmp+160, tmp+200, tmp+240 );

  /*  MONEY, POSITION, CONDITION */

  if( pc != NULL && pc->bank != 0 ) 
    sprintf( tmp, "  [ %d cp in bank ]", pc->bank );
  else
    tmp[0] = '\0';

  page( ch, "        Coins:%s.%s\r\n", coin_phrase( ch ), tmp );
  page( ch, "     Position: [ %s ]  Condition: [ %s ]\r\n\r\n",
    position_name[ ch->position ], condition_name( ch ) );

  page_centered( ch,
    "[Also try the command identity for more information.]" ); 

  return;
}


/*
 *   STATISTICS ROUTINE
 */


void do_statistics( char_data* ch, char* argument )
{
  char           tmp  [ TWO_LINES ]; 
  char_data*  victim  = ch;

  if( *argument != '\0' && ch->shdata->level >= LEVEL_APPRENTICE ) {
    if( ( victim = one_player( ch, argument, "stat", 
      (thing_array*) &player_list ) ) == NULL )
      return;
    }

  page_title( ch, "Statistics" );
  page( ch, "\r\n" );

  sprintf( tmp, "%20s%20s%20s\r\n",
    "Hit Points", "Energy", "Move" );
  page_underlined( ch, tmp );

  page( ch, "%13s:%7d%13s:%7d%13s:%7d\r\n",
    "Base", victim->base_hit,
    "Base", victim->base_mana,
    "Base", victim->base_move );

  page( ch, "%13s:%7d)%12s:%7d)%12s:%7d)\r\n",
    "(Exp", (int) victim->Mean_Hp( ),
    "(Exp", (int) victim->Mean_Mana( ),
    "(Exp", (int) victim->Mean_Move( ) );

  page( ch, "%13s:%7d%13s:%7d%13s:%7d\r\n",
    "+Mod", victim->mod_hit,
    "+Mod", victim->mod_mana,
    "+Mod", victim->mod_move );

  page( ch, "%13s:%7d%13s:%7d%13s:%7d\r\n",
    "+Con", victim->shdata->level*(victim->Constitution( )-12)/2,
    "+Int", victim->shdata->level*victim->Intelligence( )/4,
    "Max",  victim->max_move );

  page( ch, "%13s:%7d%13s:%7d\r\n",
    "Max",    victim->max_hit,
    "-Leech", leech_max( victim ) );

  page( ch, "%34s:%7d\r\n",
    "-Prep", prep_max( victim ) );

  page( ch, "%34s:%7d\r\n",
    "Max", victim->max_mana );
}  


void damage( dice_data& dice, char_data*, obj_data* obj )
{
  if( obj == NULL ) {
    dice.number = 1;
    dice.side   = 4;
    dice.plus   = 0;
    }
  else {
    dice.number = obj->value[1];
    dice.side   = obj->value[2];
    dice.plus   = 0;
    }

  return;
};












