static ref AuthPlayer ClientAuthPlayer;

static ref array< ref AuthPlayer > SELECTED_PLAYERS;

static const string PERMISSION_FRAMEWORK_DIRECTORY = "$profile:PermissionsFramework\\";

ref array< ref AuthPlayer > GetSelectedPlayers()
{
	if ( SELECTED_PLAYERS == NULL )
	{
		SELECTED_PLAYERS = new ref array< ref AuthPlayer >;
	}
	return SELECTED_PLAYERS;
}

bool PlayerAlreadySelected( AuthPlayer player )
{
	int position = GetSelectedPlayers().Find( player );

	return position > -1;
}

int RemoveSelectedPlayer( AuthPlayer player )
{
	int position = GetSelectedPlayers().Find( player );

	if ( position > -1 )
	{
		GetSelectedPlayers().Remove( position );
	}

	return position;
}

int AddSelectedPlayer( AuthPlayer player )
{
	int position = GetSelectedPlayers().Find( player );
	
	if ( position > -1 )
		return position;

	return GetSelectedPlayers().Insert( player );
}

PlayerData SerializePlayer( AuthPlayer player )
{
	player.Serialize();

	return player.GetData();
}

AuthPlayer DeserializePlayer( PlayerData data )
{
	return GetPermissionsManager().GetPlayer( data );
}

array< PlayerData > SerializePlayers( array< ref AuthPlayer > players )
{
	array< PlayerData > output = new array< PlayerData >;

	for ( int i = 0; i < players.Count(); i++)
	{
		output.Insert( SerializePlayer( players[i] ) );
	}

	return output;
}

array< ref AuthPlayer > DeserializePlayers( array< ref PlayerData > players )
{
	array< ref AuthPlayer > output = new array< ref AuthPlayer >;

	for ( int i = 0; i < players.Count(); i++)
	{
		output.Insert( DeserializePlayer( players[i] ) );
	}

	return output;
}

array< string > SerializePlayersID( array< ref AuthPlayer > players )
{
	array< string > output = new array< string >;

	for ( int i = 0; i < players.Count(); i++)
	{
		output.Insert( players[i].GetData().SSteam64ID );
	}

	return output;
}

array< ref AuthPlayer > DeserializePlayersID( ref array< string > steam64Ids )
{
	return GetPermissionsManager().GetPlayers( steam64Ids );
}

AuthPlayer GetPlayerForID( string steam )
{
	return GetPermissionsManager().GetPlayerBySteam64ID( steam );
}