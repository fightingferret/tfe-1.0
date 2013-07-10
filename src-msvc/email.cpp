/*


bool has_confirmed( char_data* ch )
{
  if( is_set( ch->pcdata->
    return TRUE;

  send( "Global channels and practicing of skills is disabled until you have
entered\r\na valid email address and confirmed it.  Please see help email for more\r\ninformation.\r\n", ch );

  return FALSE;
}


void do_email( char_data *ch, char *argument )
{
  char tmp [ MAX_INPUT_LENGTH ];
  
  PFILE_DATA *pfile;

  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  pfile = ch->pcdata->pfile;

  if( argument[0] == '\0' ) {
    sprintf( tmp, "Your email address is currently %s.\r\nWhat do you wish to change it to?\r\n", pfile->email[0] == '\0' ? "blank" : pfile->email );
    send( tmp, ch );
    return;
    }

  if( !valid_email( ch->link, argument ) )
    return;

  sprintf( tmp, "Your email address is tentatively changed to %s.  Please confirm it before continuing play.\r\n", argument );
  send( tmp, ch );

  free_string( pfile->email, MEM_PFILE );
  pfile->email = alloc_string( argument, MEM_PFILE );

  send_email( ch );

  return;
}






*/