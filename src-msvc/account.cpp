#include "define.h"
#include "struct.h"


account_data**  account_list  = NULL;
int             max_account;


/*
 *   LOCAL FUNCTIONS
 */


account_data*   existing_email      ( const char* );
bool            valid_acnt_name     ( char* );
bool            valid_email         ( char* );
void            send_email          ( link_data*, bool );
void            save_accounts       ( void );


/*
 *   ACCOUNT_DATA 
 */


Account_Data :: Account_Data( )
{
  record_new( sizeof( account_data ), MEM_ACCOUNT );
  
  name      = empty_string;
  email     = empty_string;
  pwd       = empty_string;
  confirm   = empty_string;
  new_email = empty_string; 
  notes     = empty_string;

  last_login = -1;
  balance    = 0;
  players    = 0;
  banned     = -1;
} 


Account_Data :: ~Account_Data( )
{
  record_delete( sizeof( account_data ), MEM_ACCOUNT );

  free_string( name,      MEM_ACCOUNT );
  free_string( email,     MEM_ACCOUNT );
  free_string( new_email, MEM_ACCOUNT );
  free_string( pwd,       MEM_ACCOUNT );
  free_string( confirm,   MEM_ACCOUNT );
  free_string( notes,     MEM_ACCOUNT );
}


/*
 *   ACCOUNT FUNCTIONS
 */


void add_list( account_data* account )
{
  int pos;

  pos = pntr_search( account_list, max_account, account->name );

  if( pos < 0 )
    pos = -pos-1;

  insert( account_list, max_account, account, pos );
}


void extract( account_data* account )
{
  int pos;

  pos = pntr_search( account_list, max_account, account->name );

  if( pos >= 0 ) {
    if( account_list[pos] != account )
      bug( "Extract_Account: Wrong position!" );
    else
      remove( account_list, max_account, pos );
    }

  delete account;
}


/*
 *   FIND ACCOUNT ROUTINES
 */


account_data* find_account( char* name, bool all )
{
  int pos;

  pos = pntr_search( account_list, max_account, name );

  if( pos < 0 ) 
    pos = -pos-1; 

  if( !all ) 
    for( ; pos < max_account
      && account_list[pos]->last_login == -1; pos++ );

  return( pos != max_account
    && !strncasecmp( name, account_list[pos]->name, strlen( name ) ) 
    ? account_list[pos] : NULL );
}


account_data* account_arg( char*& argument )
{
  int  pos;
  int    i;

  for( i = strlen( argument ); i > 0; ) { 
    pos = pntr_search( account_list, max_account, argument, i );
    if( pos < 0 )
      pos = -pos-1;
    for( ; pos < max_account; pos++ )
      if( account_list[pos]->last_login != -1 ) {
        if( !strncasecmp( argument, account_list[pos]->name, i ) ) {
          argument += i;
          skip_spaces( argument );
          return account_list[pos];
          }
        break;
        }
    for( ; --i > 0 && isgraph( argument[i] ); );   
    } 

  return NULL;
}


/*
 *   VALID ACCOUNT NAME
 */


bool valid_acnt_name( char* argument )
{
  if( strlen( argument ) < 5 )
    return FALSE;
  
  return TRUE;
}


/*
 *   EMAIL ROUTINES
 */


account_data* existing_email( const char* argument )
{
  int i;

  for( i = 0; i < max_account; i++ )
    if( !strcasecmp( account_list[i]->email, argument ) 
      || !strcasecmp( account_list[i]->new_email, argument ) )
      return account_list[i];

  return NULL;
}  


bool valid_email( char* addr )
{
  int i;

  if( strlen( addr ) < 5 ) 
    return FALSE;

  for( i = 1; i < strlen( addr )-1; i++ )
    if( addr[i] == (char) '@' )
      break;

  if( i == strlen( addr )-1 )
    return FALSE;

  for( i = 0; i < strlen( addr ); i++ ) 
    if( !isalnum( addr[i] ) && addr[i] != (char) '@'
      && addr[i] != (char) '.' && addr[i] != (char) '_'
      && addr[i] != (char) '-' && addr[i] != (char) '!'
      && addr[i] != (char) '+' ) 
      return FALSE;

  return TRUE;
}


void send_email( link_data* link, bool change )
{  
  char                 tmp  [ MAX_INPUT_LENGTH ];
  char             confirm  [ 10 ];
  account_data*    account;
  FILE*                 fp;
  int                    i;

  account = link->account;

  for( i = 0; i < 8; i++ )
    confirm[i] = 'a'+number_range( 0, 25 );
  confirm[8] = '\0';

  account->confirm    = alloc_string( confirm, MEM_ACCOUNT );
  account->last_login = current_time;

  if( ( fp = open_file( "email.msg", "w" ) ) == NULL ) 
    return;

  if( change ) {
    fprintf( fp, "A request was entered at forestsedge.com to change\n" );
    fprintf( fp, "the email address of an account to this address.  The\n" );
    fprintf( fp, "confirmation code needed to complete this is given\n" );
    fprintf( fp, "below.\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "            Account: %s\n", account->name );   
    fprintf( fp, "         Prev Email: %s\n", account->email );   
    fprintf( fp, "          New Email: %s\n", account->new_email );   
    fprintf( fp, "  Confirmation Code: %s\n", confirm );   
    fprintf( fp, "    Site of Request: %s\n", link->host );
    }
  else {
    fprintf( fp, "A request was entered at forestsedge.com to open an\n" );
    fprintf( fp, "account for this email address.  If this is indeed\n" );
    fprintf( fp, "the case your confirmation code is '%s'.\n", confirm );
    fprintf( fp, "The request was from a user connected from %s.\n", 
      link->host );
    }

  fclose( fp );

  sprintf( tmp, "./softmail %s", account->new_email );
  system( tmp );

  return;
}


/*
 *   NANNY ROUTINES
 */


void nanny_acnt_enter( link_data* link, char* argument )
{
  account_data*  account;

  if( *argument == '\0' ) {
    write_greeting( link );
    link->connected = CON_INTRO;
    return;
    }

  if( ( account = find_account( argument ) ) == NULL ) {
    help_link( link, "Unfound_Acnt" );
    send( link, "Account: " );
    return;
    }

  if( is_banned( account, link ) )
    return;

  link->account = account;

  if( account->email == empty_string ) {
    help_link( link, "Confirm_Acnt" );
    send( link, "Confirmation Code: " );
    link->connected = CON_ACNT_CONFIRM;
    return;
    }

  help_link( link, "Acnt_Check_Pwd" );
  send( link, "Password: " );

  switch( link->connected ) {
    case CON_VE_ACCOUNT :  link->connected = CON_VE_VALIDATE;     break;
    case CON_CE_ACCOUNT :  link->connected = CON_CE_PASSWORD;     break;
    default             :  link->connected = CON_ACNT_CHECK_PWD;  break;
    }

  return;  
}


void nanny_acnt_check_pwd( link_data* link, char* argument )
{
  if( strcmp( link->account->pwd, argument ) ) {
    help_link( link, "Acnt_Bad_Pwd" );
    link->connected = CON_PAGE;
    return;
    }

  if( link->connected == CON_CE_PASSWORD ) {
    help_link( link, "New_Email" );
    send( link, "Current: %s\r\n\r\n", link->account->email );
    send( link, "New Email: " );
    link->connected = CON_CE_EMAIL;
    }
  else {
    help_link( link, "Login_new_name" );
    send( link, "Name: " );
    link->connected = CON_GET_NEW_NAME;
    }

  return;
}


void nanny_acnt_confirm( link_data* link, char* argument ) 
{
  account_data* account = link->account;

  if( no_input( link, argument ) )
    return;

  if( strcmp( argument, link->account->confirm ) ) {
    help_link( link, "Bad_Confirm" );
    send( link, "Confirmation Code: " );
    return;
    }

  if( link->connected == CON_VE_CONFIRM ) {
    help_link( link, "Email_Changed" );
    press_return( link );
    link->connected = CON_PAGE;
    free_string( account->email, MEM_ACCOUNT );
    }
  else {
    send( link, "\r\n>> Account Validated. <<\r\n" );
    help_link( link, "Login_new_name" );
    send( link, "Name: " );
    link->connected = CON_GET_NEW_NAME;
    }

  free_string( account->confirm, MEM_ACCOUNT );

  account->email     = account->new_email;
  account->new_email = empty_string;
  account->confirm   = empty_string;

  save_accounts( );
}


void nanny_acnt_name( link_data* link, char* argument )
{
  if( no_input( link, argument ) )
    return;

  if( !valid_acnt_name( argument ) ) {
    help_link( link, "Invalid_Acnt_Name" );
    send( link, "Account: " );
    return;
    }

  if( find_account( argument, TRUE ) != NULL ) {
    help_link( link, "Existing_Acnt" );
    send( link, "Account: " );
    return;
    }
   
  link->account       = new account_data;
  link->account->name = alloc_string( argument, MEM_ACCOUNT ); 

  add_list( link->account );

  help_link( link, "Acnt_Pwd" );
  send( link, "Password: " );

  link->connected = CON_ACNT_PWD;

  return;
}


void nanny_acnt_password( link_data* link, char* argument )
{
  if( strlen( argument ) < 5 ) {
    help_link( link, "Short_Acnt_Pwd" );
    send( link, "Password: " );
    return;
    }

  link->account->pwd = alloc_string( argument, MEM_ACCOUNT );

  help_link( link, "Acnt_Email" );
  send( link, "Email: " );

  link->connected = CON_ACNT_EMAIL;

  return;
}


void nanny_acnt_email( link_data* link, char* argument )
{
  account_data*  account  = link->account;

  if( no_input( link, argument ) )
    return;

  if( !valid_email( argument ) ) {
    help_link( link, "Invalid_Email" );
    send( link, "Email: " );
    return;
    }

  if( existing_email( argument ) != NULL ) {
    help_link( link, "Existing_Email" );
    link->connected = CON_PAGE;
    if( link->account->last_login == -1 ) {
      extract( link->account );       
      link->account = NULL;
      }
    return;
    }

  if( is_banned( argument ) ) {
    send( link,
      "New accounts with email from that site are banned.\r\n" );
    close_socket( link, TRUE );
    return;        
    }

  if( link->connected == CON_CE_EMAIL ) {
    account->new_email = alloc_string( argument, MEM_ACCOUNT );
    help_link( link, "ChangeEmail_Sent" );
    press_return( link );
    link->connected = CON_PAGE;
    send_email( link, TRUE );
    return;
    }  

  link->connected     = CON_ACNT_CHECK;
  account->new_email  = alloc_string( argument, MEM_ACCOUNT );

  help_link( link, "Acnt_Check" );
 
  send( link,
    "   Account: %s\r\n  Password: %s\r\n     Email: %s\r\n\r\n", 
    account->name, account->pwd, account->new_email );
  send( link, "Is the correct? " );

  return;
}


void nanny_acnt_check( link_data* link, char* argument )
{
  if( toupper( *argument ) != 'Y'  ) {
    help_link( link, "Acnt_Cancel" );
    extract( link->account );
    link->account   = NULL;
    link->connected = CON_PAGE;
    return;
    }

  link->connected = CON_PAGE;

  send_email( link, FALSE );
  save_accounts( );

  help_link( link, "Acnt_Done" );

  return;
}


void nanny_acnt_menu( link_data* link, char* argument )
{
  switch( atoi( argument ) ) {
    case 1:
      if( is_banned( link->host ) ) {
        send( link, "The site you are connected from is banned.\r\n" );
        close_socket( link, TRUE );
        return;        
        }
      help_link( link, "Create_Account" );
      link->connected = CON_ACNT_NAME;
      break;

    case 2:
      help_link( link, "CE_AccountName" );
      link->connected = CON_CE_ACCOUNT;
      break;

    case 3:
      help_link( link, "CE_AccountName" );
      link->connected = CON_VE_VALIDATE;
      break;

    case 4:
      help_link( link, "Acnt_Request" );
      link->connected = CON_ACNT_REQUEST;
      send( link, "Email: " );
      return;

    default:
      write_greeting( link );
      link->connected = CON_INTRO;
      return;
    }

  send( link, "Account: " );

  return;
}


void nanny_acnt_request( link_data* link, char* argument )
{
  char               tmp  [ THREE_LINES ];
  account_data*  account;
  bool             found  = FALSE;
  FILE*               fp;

  if( ( account = existing_email( argument ) ) == NULL ) {
    help_link( link, "Acnt_No_Email" );
    link->connected = CON_PAGE;
    return;
    }

  if( ( fp = open_file( "email.msg", "w" ) ) == NULL ) 
    return;

  fprintf( fp, "A user connected from %s requested this\n", link->host );
  fprintf( fp, "information be emailed to this address.\n\n" );
  
  fprintf( fp, "       Account Name: %s\n", account->name );
  fprintf( fp, "   Account Password: %s\n", account->pwd );

  if( account->confirm != empty_string )
    fprintf( fp, "  Confirmation Code: %s\n", account->confirm );

  fprintf( fp, "\n" );

  for( int i = 0; i < max_pfile; i++ ) 
    if( pfile_list[i]->account == account ) {
      if( !found ) {
        fprintf( fp, "%-20s%-15s%s\n", "Player", "Password", "Last Login" );
        fprintf( fp, "%-20s%-15s%s\n", "------", "--------", "----------" );
        found = TRUE;
        }
      fprintf( fp, "%-20s%-15s%s\n", pfile_list[i]->name,
        pfile_list[i]->pwd, pfile_list[i]->last_host );
      }

  if( !found ) 
    fprintf( fp, "No existing players on this account.\n" );

  fclose( fp );

  sprintf( tmp, "./softmail %s", account->email );
  system( tmp );

  help_link( link, "Acnt_Sent" );
  link->connected = CON_PAGE;

  return;
}


void nanny_ve_validate( link_data* link, char* argument )
{
  account_data*  account;

  if( *argument == '\0' ) {
    write_greeting( link );
    link->connected = CON_INTRO;
    return;
    }

  if( ( account = find_account( argument ) ) == NULL ) {
    help_link( link, "Unfound_Acnt" );
    send( link, "Account: " );
    return;
    }

  if( account->new_email == empty_string ) {
    help_link( link, "CE_norequest" );
    link->connected = CON_PAGE;
    return;
    }

  help_link( link, "CE_Code" );
  send( link, "Confirmation Code: " );

  link->account   = account;
  link->connected = CON_VE_CONFIRM;
  
  return;
}


/*
 *   DISK ROUTINES
 */


void save_accounts( )
{
  account_data*   account;
  FILE*                fp;

  if( ( fp = open_file( ACCOUNT_FILE, "w" ) ) == NULL ) 
    return;

  fprintf( fp, "%d\n\n", max_account );

  for( int i = 0; i < max_account; i++ ) {
    account = account_list[i];
    fprintf( fp, "%s~\n", account->name );
    fprintf( fp, "%s~\n", account->email );
    fprintf( fp, "%s~\n", account->pwd );
    fprintf( fp, "%s~\n", account->new_email );
    fprintf( fp, "%s~\n", account->confirm );
    fprintf( fp, "%s~\n", account->notes );
    fprintf( fp, "%d %d %d\n\n", account->last_login,
      account->balance, account->banned );
    }

  fclose( fp );
}


void load_accounts( )
{
  account_data*   account;
  FILE*                fp;

  printf( "Loading Accounts...\r\n" );

  if( ( fp = fopen( ACCOUNT_FILE, "r" ) ) == NULL ) {
    bug( "Load_Accounts: Fopen Error" );
    max_account  = 0;
    account_list  = NULL;
    return;
    }

  max_account = fread_number( fp );
  account_list = new account_data* [ max_account ];

  for( int i = 0; i < max_account; i++ ) {
    account = new account_data;

    account->name      = fread_string( fp, MEM_ACCOUNT );
    account->email     = fread_string( fp, MEM_ACCOUNT );
    account->pwd       = fread_string( fp, MEM_ACCOUNT );
    account->new_email = fread_string( fp, MEM_ACCOUNT );
    account->confirm   = fread_string( fp, MEM_ACCOUNT );
    account->notes     = fread_string( fp, MEM_ACCOUNT );
 
    account->last_login = fread_number( fp );
    account->balance     = fread_number( fp );
    account->banned     = fread_number( fp );

    account_list[i] = account;
    }

  fclose( fp );
}


/*
 *   LIST ACCOUNTS ROUTINE
 */


void display_pfile( char_data* ch, pfile_data* pfile, bool& first )
{
  char                tmp  [ TWO_LINES ];
  char               host  [ ONE_LINE ];
  account_data*   account;

  account = pfile->account;

  if( first ) {
    first = FALSE;
    sprintf( tmp, "%-14s%-30s%-18s%-10s%s\r\n",
      "Character", "Site", "Last On", "Class", "Level" );
    page_underlined( ch, tmp );
    }

  strncpy( host, pfile->last_host, 30 );
  truncate( host, 28 );

  sprintf( tmp, "%-14s%-30s%-18s%-10s%5d\r\n",
    pfile->name, host,
    ltime( pfile->last_on )+4,
    clss_table[pfile->clss].name,
    pfile->level );
  page( ch, tmp );
}


void display_account( char_data* ch, account_data* account, bool& first )
{
  char  tmp  [ TWO_LINES ];

  if( first ) {
    first = FALSE;
    sprintf( tmp, "%-20s%-30s\r\n",
      "Account", "Email" );
    page_underlined( ch, tmp );
    }

  page( ch, "%-20s%-30s\r\n", account->name, account->email );

  return;
}


void do_accounts( char_data* ch, char* argument )
{
  account_data*   account  = NULL;
  pfile_data*       pfile;
  wizard_data*        imm  = (wizard_data*) ch;
  int               flags;
  bool              first  = TRUE;
  int                   i;

  if( !get_flags( ch, argument, &flags, "spP", "Accounts" ) )
    return;;

  if( is_set( &flags, 0 ) ) {
    if( *argument == '\0' ) {
      send( ch, "For what site do you wish to list the players?\r\n" );
      return;
      }
    if( ( i = site_search( argument ) ) < 0 )
      i = -i-1;
    for( ; i < site_entries && !rstrncasecmp( site_list[i]->last_host,
      argument, strlen( argument ) ); i++ ) 
      if( imm->See_Account( site_list[i] ) )
        display_pfile( ch, site_list[i], first );
    if( first )
      send( ch, "No players from that site.\r\n" );
    return;
    }

  if( is_set( &flags, 2 ) ) {
    if( *argument == '\0' ) {
      send( ch,
        "For which character to you wish to run a password match?\r\n" );
      return;
      }
    if( ( pfile = find_pfile( argument, ch ) ) == NULL ) 
      return;
    for( i = 0; i < max_pfile; i++ ) 
      if( !strcasecmp( pfile_list[i]->pwd, pfile->pwd )
        && imm->See_Account( pfile_list[i] ) ) 
        display_pfile( ch, pfile_list[i], first );
    return;
    }          

  if( *argument == '\0' ) {
    send( ch,
      "For what player %sdo you wish to list an account summary?\r\n",
      is_set( &flags, 2 ) ? "" : "or account " );
    return;
    }

  if( is_set( &flags, 1 ) || ( account = find_account( argument ) ) == NULL ) {
    if( ( pfile = find_pfile( argument ) ) == NULL ) {
      send( ch, "No matching player %sfound.\r\n",
        is_set( &flags, 1 ) ? "" : "or account " );
      return;
      }
    if( !imm->See_Account( pfile ) ) {
      send( ch, "You cannot view the account of %s.\r\n", pfile->name );
      return;
      }
    if( ( account = pfile->account ) == NULL ) {
      send( ch, "%s has no account.\r\n", pfile->name );
      return;
      }
    }

  page( ch, "  Account: %s\r\n", account->name );
  page( ch, "    Email: %s\r\n", account->email );

  if( is_god( ch ) ) 
    page( ch, " Password: %s\r\n", account->pwd );

  if( account->banned != -1 ) 
    page( ch, "   Banned: %s\r\n",
      account->banned == 0 ? "forever" :
      ltime( account->banned ) ); 

  page( ch, "\r\n" );

  for( i = 0; i < max_pfile; i++ ) 
    if( pfile_list[i]->account == account
      && imm->See_Account( pfile_list[i] ) ) 
      display_pfile( ch, pfile_list[i], first );

  if( account->notes != empty_string ) {
    page( ch, "\r\n" );
    page_underlined( ch, "Pbug\r\n" );
    page( ch, account->notes );
    }
}


/*
 *   PURCHASE COMMAND
 */


bool lower_balance( char_data* ch, int amount )
{
  account_data* account;

  if( ( account = ch->pcdata->pfile->account ) == NULL ) {
    send( ch, "You don't have an account.\r\n" );
    return FALSE;
    }

  if( account->balance < amount ) {
    send( ch, "Your account balance is $%.2f - requested purchase\
 is $%.2f.\r\n", account->balance, amount );
    return FALSE;
    } 

  account->balance -= amount;

  return TRUE;
}


void do_purchase( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  player_data*        pc  = (player_data*) ch;
  account_data*  account;

  if( ( account = ch->pcdata->pfile->account ) == NULL ) {
    send( ch, "You lack an account.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Balance: $%.2f\r\n", (float) account->balance/100 );
    return;
    }

  int     i;
  int  cost;

  if( matches( argument, "gsp" ) ) {
    if( !number_arg( argument, i ) ) {
      send( pc, "How many gossip points do you wish to purchase at 500 per\
 dollar?\r\n" );
      return;
      } 
    if( i <= 0 ) {
      send( pc, "The resale value of gsps is nothing.\r\n" );
      return;
      }
    if( pc->gossip_pts+i > 1000 ) {
      send( pc, "You may have at maximum 1000 gossip points.\r\n" );
      return;
      }
    cost = (4+i)/5;
    if( lower_balance( ch, cost ) ) { 
      i = min( 5*cost, 1000-pc->gossip_pts );
      pc->gossip_pts += i;
      send( ch, "%s gossip points purchased for $%.2f.\r\n",
        number_word( i ), (float) cost/100 );
      }
    return;
    } 
}

