#include "ctype.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


bool use_color = FALSE;


/* 
 *   INCOGNITO
 */


bool is_incognito( char_data* victim, char_data* ch )
{
  if( victim == NULL )
    return TRUE;

  if( victim->pcdata == NULL )
    return is_incognito( victim->leader, ch );
   
  return is_incognito( victim->pcdata->pfile, ch );
}


bool is_incognito( pfile_data* pfile, char_data* ch )
{
  int level;

  if( pfile == ch->pcdata->pfile || ch->shdata->level >= LEVEL_APPRENTICE )
    return FALSE;

  level = level_setting( &pfile->settings, SET_INCOGNITO );

  if( level == 0 )
    return FALSE;

  if( level == 3 )
    return TRUE; 

  return( pfile->clan != ch->pcdata->pfile->clan 
    || pfile->clan == NULL );
}


/*
 *   HE/SHE/IT
 */


const char* char_data :: He_She( char_data* to )
{
  const char* string [] = { "it",  "he", "she" };

  return( to == NULL || Seen( to )
    ? string[sex] : "they" );
}


const char* char_data :: Him_Her( char_data* to )
{
  const char* string [] = { "it",  "him", "her" };

  return( to == NULL || Seen( to )
    ? string[sex] : "them" );
}


const char* char_data :: His_Her( char_data* to )
{
  const char* string [] = { "its", "his", "her" };

  return( to == NULL || Seen( to )
    ? string[sex] : "their" );
}


/*
 *   ADJECTIVES
 */


char* adjectives( char_data* ch, char_data* victim )
{
  char* tmp = static_string( );

  *tmp = '\0';

  if( is_set( victim->affected_by, AFF_INVISIBLE ) )
    strcpy( tmp, "invisible" );

  if( is_evil( victim ) && ch->detects_evil( ) ) 
    sprintf( tmp+strlen( tmp ), "%sevil", *tmp == '\0' ? "" : ", " ); 
  else if( is_good( victim ) && ch->detects_good( ) ) 
    sprintf( tmp+strlen( tmp ), "%sgood", *tmp == '\0' ? "" : ", " ); 

  if( is_set( victim->affected_by, AFF_FIRE_SHIELD )
    && ( victim->species == NULL
    || !is_set( victim->species->affected_by, AFF_FIRE_SHIELD ) ) )
    sprintf( tmp+strlen( tmp ),
      "%sfire-shrouded", *tmp == '\0' ? "" : ", " ); 

  if( is_set( victim->affected_by, AFF_DISPLACE )
    && ( victim->species == NULL
    || !is_set( victim->species->affected_by, AFF_DISPLACE ) ) )
    sprintf( tmp+strlen( tmp ),
      "%sdisplaced", *tmp == '\0' ? "" : ", " ); 

  if( *tmp != '\0' )
    strcat( tmp, " " );

  return tmp; 
}


/* 
 *   KEYWORDS
 */


const char* char_data :: Keywords( char_data* ch )
{
  if( ch == this ) { 
    char* tmp = static_string( );
    sprintf( tmp, "self %s", Seen_Name( ch ) );
    return tmp;
    }

  if( !in_character )
    return descr->name;

  char* tmp = static_string( );
  sprintf( tmp, "%s %s", Seen_Name( ch ),
    seperate( descr->keywords, ch == NULL ? TRUE
    : includes( known_by, ch ) ) );
  return tmp;
}


/*
 *   NAME
 */


const char* Char_Data :: real_name( )
{
  if( link != NULL )
    return link->player->descr->name;

  return( species == NULL ? descr->name : descr->singular );
}


const char* species_data :: Name( )
{
  if( descr->name != empty_string ) 
    return descr->name;

  char*      tmp  = static_string( );
  char* singular  = seperate( descr->singular, TRUE ); 

  if( is_set( &act_flags, ACT_USE_THE ) ) 
    sprintf( tmp, "the %s", singular );
  else
    sprintf( tmp, "a%s %s",
      isvowel( *singular ) ? "n" : "", singular );

  return tmp;
}


const char* Char_Data :: Name( char_data* ch, int num, bool brief )
{
  if( !Seen( ch ) )
    return "someone";

  return Seen_Name( ch, num, brief );
}


const char* char_data :: Seen_Name( char_data* ch, int num, bool )
{
  if( ch != NULL && is_set( ch->affected_by, AFF_HALLUCINATE ) )
    return fake_mob[ number_range( 0, MAX_FAKE_MOB-1 ) ];

  char* tmp  = static_string( );

  if( species == NULL ) {
    if( ch == NULL || !in_character
      || ch->Recognizes( this ) ) 
      return descr->name;
    sprintf( tmp, "a%s %s",
      isvowel( *descr->singular ) ? "n" : "", descr->singular );
    return tmp;
    }

  if( ch != NULL && pet_name != empty_string && leader == ch ) 
    return pet_name;

  bool known  = ( ch == NULL ? TRUE : includes( known_by, ch ) );

  if( num > 1 ) {
    sprintf( tmp, "%s %s",
      number_word( num ),
      seperate( descr->plural, known ) );
    return tmp;
    }

  if( known && descr->name != empty_string )
    return descr->name;

  char* singular = seperate( descr->singular, known );

  if( is_set( &species->act_flags, ACT_USE_THE ) ) 
    sprintf( tmp, "the %s", singular );
  else 
    sprintf( tmp, "a%s %s",
      isvowel( *singular ) ? "n" : "", singular );

  return tmp;
}


/*
 *   LONG NAME
 */


const char* char_data :: Long_Name( char_data* ch, int num )
{
  char*        adj  = adjectives( ch, this );
  char*        tmp  = static_string( );
  char*    article;
  bool       known  = includes( known_by, ch );

  if( num > 1 ) {
    char* prefix_p  = seperate( descr->prefix_p, known );
    char*    adj_p  = seperate( descr->adj_p,    known );
    char*   plural  = seperate( descr->plural,   known );

    sprintf( tmp, "%s%s%s %s%s%s%s%s%s", 
      prefix_p,
      prefix_p == empty_string ? "" : " ",
      number_word( num ), adj,
      adj_p,
      adj_p == empty_string ? "" : " ",
      color_code( ch, species->color ),
      plural, normal( ch ) );
    return tmp;
    }

  char* prefix_s  = seperate( descr->prefix_s, known );
  char*    adj_s  = seperate( descr->adj_s,    known );
  char* singular  = seperate( descr->singular, known );

  if( adj_s != empty_string ) 
    sprintf( adj+strlen( adj ), "%s ", adj_s );

  if( species != NULL && is_set( &species->act_flags, ACT_USE_THE ) ) 
    article = "the";
  else {
    article = static_string( );
    sprintf( article, "a%s",
      isvowel( *adj == '\0' ? *singular : *adj ) ? "n" : "" );
    }

  if( species == NULL ) {
    if( ch->Recognizes( this ) ) {
      if( !is_set( &ch->pcdata->message, MSG_LONG_NAMES ) )
        return descr->name;
      sprintf( tmp, "%s, %s %s%s,",
        descr->name, article,
        adj, singular );
      }
    else {
      sprintf( tmp, "%s %s%s",
        article, adj, singular );
      }
    return tmp;
    }

  sprintf( tmp, "%s%s", 
    prefix_s,
    prefix_s == empty_string ? "" : " " );

  if( ( known && descr->name != empty_string )
    || ( pet_name != empty_string && leader == ch ) ) {
    if( *singular != '\0' 
      && is_set( &ch->pcdata->message, MSG_LONG_NAMES ) )
      sprintf( tmp+strlen(tmp), "%s%s%s, %s %s%s,",
        color_code( ch, species->color ),
        pet_name != empty_string ? pet_name : descr->name,
        normal( ch ),
        article, adj,
        singular );
    else  
      sprintf( tmp+strlen(tmp), "%s%s%s",
        color_code( ch, species->color ),
        pet_name != empty_string ? pet_name : descr->name,
        normal( ch ) );
    }
  else {
    sprintf( tmp+strlen(tmp), "%s %s%s%s%s",
      article, adj,
      color_code( ch, species->color ),
      singular, normal( ch ) );
    }

  return tmp;
}


const char* char_data :: Show( char_data* ch, int num )
{
  char*      tmp  = static_string( );
  char*   string  = empty_string;

  use_color = TRUE;

  if( mount != NULL ) 
    sprintf( tmp, "%s mounted on %s is here.",
      Long_Name( ch ), mount->Long_Name( ch ) );
 
  else if( rider == ch ) 
    sprintf( tmp, "%s which you are riding stands here.",
      Long_Name( ch ) );
  
  else if( position > POS_RESTING
    && is_set( affected_by, AFF_ENTANGLED ) )
    string = "struggling to escape a web."; 

  else if( position >= POS_RESTING
    && is_set( &status, STAT_HIDING ) )
    string = "trying to hide in the shadows here.";

  else {
    switch ( position ) {
      case POS_DEAD:       string = "dead.";                break;
      case POS_MORTAL:     string = "mortally wounded.";    break;
      case POS_INCAP:      string = "incapacitated.";       break;
      case POS_STUNNED:    string = "lying here stunned.";  break;
      case POS_SLEEPING:   string = "sleeping here.";       break;
      case POS_MEDITATING: string = "meditating here.";     break;
      case POS_RESTING:    string = "resting here.";        break;

      case POS_STANDING:
        if( fighting != NULL && !is_set( &status, STAT_LEAPING ) ) 
          sprintf( tmp, "%s is here, fighting %s.",
            Long_Name( ch, num ),
            fighting == ch ? "YOU!" : fighting->Name( ch ) );

        else if( species != NULL
          && !is_set( ch->affected_by, AFF_HALLUCINATE )
	  && !is_set( &status, STAT_PET ) ) 
          sprintf( tmp, "%s %s", Long_Name( ch, num ),
            seperate( num == 1 ? descr->long_s : descr->long_p, TRUE ) );

        else if( is_set( affected_by, AFF_FLOAT ) )
          string = "floating here.";

        else
          string = "standing here.";
      }
    }

  if( string != empty_string )
    sprintf( tmp, "%s %s %s", Long_Name( ch, num ),
      num == 1 ? "is" : "are", string );

  use_color = FALSE;

  return tmp;
}


/*
 *   LOOK_SAME
 */


bool char_data :: look_same( char_data* ch1, char_data* ch2 )
{
  if( ch1->species != ch2->species )
    return FALSE;

  if( ch1->species == NULL ) 
    return FALSE;

  if(  ch1->position != ch2->position
    || ch1->fighting != ch2->fighting )  
    return FALSE;

  if( includes( ch1->known_by, this )
    != includes( ch2->known_by, this ) )
    return FALSE;

  if( is_set( ch1->affected_by, AFF_FIRE_SHIELD )
    != is_set( ch2->affected_by, AFF_FIRE_SHIELD ) )
    return FALSE;

  if(  ( ch1->leader == this && ch1->pet_name != empty_string )
    || ( ch2->leader == this && ch2->pet_name != empty_string ) )
    return FALSE;

  return TRUE; 
}


/*
 *   DESCR_DATA
 */


Descr_Data :: Descr_Data( )
{
  record_new( sizeof( descr_data ), MEM_DESCR );

  name        = empty_string;
  complete    = empty_string;
  keywords    = empty_string;
  singular    = empty_string;
  prefix_s    = empty_string;
  adj_s       = empty_string;
  long_s      = empty_string;
  plural      = empty_string;
  prefix_p    = empty_string;
  adj_p       = empty_string;
  long_p      = empty_string;

  return;
}


Descr_Data :: ~Descr_Data( )
{
  record_delete( sizeof( descr_data ), MEM_DESCR );

  free_string( name,        MEM_DESCR );
  free_string( complete,    MEM_DESCR );
  free_string( keywords,    MEM_DESCR );
  free_string( singular,    MEM_DESCR );
  free_string( prefix_s,    MEM_DESCR );
  free_string( adj_s,       MEM_DESCR );
  free_string( long_s,      MEM_DESCR );
  free_string( plural,      MEM_DESCR );
  free_string( prefix_p,    MEM_DESCR );
  free_string( adj_p,       MEM_DESCR );
  free_string( long_p,      MEM_DESCR );

  return;
}


