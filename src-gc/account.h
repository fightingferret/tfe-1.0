/*
 *   ACCOUNT CLASS
 */


class Account_Data
{
 public:
  account_data*   next;
  char*           name;
  char*          email;
  char*            pwd;
  char*        confirm;
  char*      new_email; 
  char*          notes; 
  int       last_login;
  int          balance;
  int          players;
  int           banned;

  Account_Data( );
  ~Account_Data( );

  friend char* name( account_data* account ) {
    return account->name;
    }
};


extern int              max_account;
extern account_data**  account_list;


/*
 *   BAN ROUTINES
 */


void  load_badname       ( void );
void  save_badname       ( void );
void  load_banned        ( void );
void  save_banned        ( void );


extern const char**  badname_array;
extern int           max_badname;
       

/*
 *   GLOBAL ROUTINES
 */


void            load_accounts       ( void );
void            save_accounts       ( void );
void            display_account     ( char_data*, account_data*, bool& );
void            extract             ( account_data* ); 
account_data*   find_account        ( char*, bool = FALSE );
account_data*   account_arg         ( char*& );

