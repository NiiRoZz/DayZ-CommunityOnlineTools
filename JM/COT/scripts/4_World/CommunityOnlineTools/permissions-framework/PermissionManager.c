class PermissionManager
{
	ref array< ref AuthPlayer > AuthPlayers;
	ref array< ref Role > Roles;
	ref map< string, ref Role > RolesMap;

	ref Permission RootPermission;

	void PermissionManager()
	{
		if ( GetGame().IsServer() )
		{
			MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY );
			MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" );
			MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" );
		}

		AuthPlayers = new array< ref AuthPlayer >;
		Roles = new array< ref Role >;
		RolesMap = new map< string, ref Role >;

		RootPermission = new ref Permission( "ROOT" );
	}

	array< ref AuthPlayer > GetPlayers( ref array< string > steamIds = NULL )
	{
		if ( steamIds == NULL || !GetGame().IsMultiplayer() )
		{
			return AuthPlayers;
		}

		array< ref AuthPlayer > tempArray = new array< ref AuthPlayer >;

		for ( int i = 0; i < steamIds.Count(); i++ )
		{
			for ( int k = 0; k < AuthPlayers.Count(); k++ )
			{
				if ( steamIds[i] == AuthPlayers[k].GetData().SSteam64ID )
				{
					tempArray.Insert( AuthPlayers[k] );
				}
			}
		}

		return tempArray;
	}

	void RegisterPermission( string permission )
	{
		RootPermission.AddPermission( permission, PermissionType.INHERIT );
	}

	array< string > Serialize()
	{
		array< string > data = new array< string >;
		RootPermission.Serialize( data );
		return data;
	}

	ref Permission GetRootPermission()
	{
		return RootPermission;
	}

	bool HasPermission( string permission, PlayerIdentity identity = NULL )
	{
		if ( !GetGame().IsMultiplayer() )
			return true;

		if ( GetGame().IsClient() ) 
		{
			if ( ClientAuthPlayer == NULL )
			{
				GetLogger().Log( "ClientAuth is NULL!", "JM_COT_PermissionFramework" );
				return false;
			}

			return ClientAuthPlayer.HasPermission( permission );
		}
		
		if ( identity == NULL )
		{
			return false;
		}

		PlayerBase player = GetPlayerObjectByIdentity( identity );

		if ( player != NULL && player.authenticatedPlayer != NULL )
		{
			return player.authenticatedPlayer.HasPermission( permission );
		}

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SGUID == identity.GetId() )
			{
				return AuthPlayers[i].HasPermission( permission );
			}
		}

		return false;
	}

	AuthPlayer OnPlayerJoined( PlayerIdentity identity )
	{
		int index = AuthPlayers.Insert( new AuthPlayer( identity ) );

		AuthPlayers[index].UpdatePlayerData();

		AuthPlayers[index].CopyPermissions( RootPermission );
		AuthPlayers[index].Load();

		return AuthPlayers[index];
	}

	void OnPlayerLeft( PlayerIdentity player )
	{
		if ( player == NULL )
			return;

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SGUID == player.GetId() )
			{
				AuthPlayers[i].Save();

				GetRPCManager().SendRPC( "PermissionsFramework", "RemovePlayer", new Param1< ref PlayerData >( SerializePlayer( AuthPlayers[i] ) ), true );

				AuthPlayers.Remove( i );
				return;
			}
		}
	}

	void DebugPrint()
	{
		GetLogger().Log( "Printing all authenticated players!", "JM_COT_PermissionFramework" );
		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			AuthPlayers[i].DebugPrint();
		}
	}

	AuthPlayer GetPlayerByGUID( string guid )
	{
		if ( !GetGame().IsMultiplayer() )
			return AuthPlayers[0];

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SGUID == guid )
			{
				return AuthPlayers[i];
			}
		}

		if ( GetGame().IsClient() )
		{
			return AuthPlayers.Get( AuthPlayers.Insert( new AuthPlayer( NULL ) ) );
		}

		return NULL;
	}

	AuthPlayer GetPlayerBySteam64ID( string steam64 )
	{
		if ( !GetGame().IsMultiplayer() )
			return AuthPlayers[0];

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SSteam64ID == steam64 )
			{
				return AuthPlayers[i];
			}
		}

		if ( GetGame().IsClient() )
		{
			return AuthPlayers[ AuthPlayers.Insert( new AuthPlayer( NULL ) ) ];
		}

		return NULL;
	}

	AuthPlayer GetPlayerByIdentity( PlayerIdentity ident )
	{
		if ( !GetGame().IsMultiplayer() )
			return AuthPlayers[0];

		if ( !GetGame().IsServer() )
			return NULL;
		
		if ( ident == NULL )
			return NULL;

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SGUID == ident.GetId() )
			{
				return AuthPlayers[i];
			}
		}

		return OnPlayerJoined( ident );
	}

	AuthPlayer GetPlayer( PlayerData data )
	{
		Print(" test 1");
		if ( !GetGame().IsMultiplayer() )
			return AuthPlayers[0];
		Print(" test 2");
		
		if ( data == NULL )
			return NULL;
		Print(" test 3");

		for ( int i = 0; i < AuthPlayers.Count(); i++ )
		{
			if ( AuthPlayers[i].GetData().SGUID == data.SGUID )
			{
		Print(" test 4");
				AuthPlayers[i].UpdateData( data, GetGame().IsClient() );
				return AuthPlayers[i];
			}
		}
		Print(" test 5");

		if ( GetGame().IsClient() )
		{
		Print(" test 6");
			int index = AuthPlayers.Insert( new AuthPlayer( NULL ) );
			AuthPlayers[ index ].UpdateData( data, GetGame().IsClient() );
			return AuthPlayers[index];
		}

		Print(" test 7");
		return NULL;
	}

	protected bool IsValidFolderForRoles( string name, FileAttr attributes )
	{
		string extenstion = ".txt";
		int strLength = name.Length();

		if ( name == extenstion )
			return false;

		if ( (attributes & FileAttr.DIRECTORY ) )
			return false;

		if ( name == "" )
			return false;

		return true;
	}
	
	ref Role CreateRole( string name, ref array< string > data )
	{
		ref Role role = new ref Role( name );

		role.SerializedData.Copy( data );
		role.Deserialize();

		role.Save();

		Roles.Insert( role );
		RolesMap.Insert( name, role );

		return role;
	}

	ref Role LoadRole( string name, ref array< string > data = NULL )
	{
		ref Role role = new ref Role( name );

		if ( data == NULL )
		{
			if ( role.Load() )
			{
				Roles.Insert( role );
				RolesMap.Insert( name, role );
			}
		} else
		{
			role.SerializedData = data;
			role.Deserialize();

			if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			{
				role.Save();
			}

			Roles.Insert( role );
			RolesMap.Insert( name, role );
		}

		return role;
	}

	void LoadRoles()
	{
		string sName = "";
		FileAttr oFileAttr = FileAttr.INVALID;
		FindFileHandle oFileHandle = FindFile( PERMISSION_FRAMEWORK_DIRECTORY + "Roles\\*.txt", sName, oFileAttr, FindFileFlags.ALL );

		if (sName != "")
		{
			if ( IsValidFolderForRoles( sName, oFileAttr ) )
			{
				LoadRole( sName.Substring(0, sName.Length() - 4) );
			}

			while (FindNextFile(oFileHandle, sName, oFileAttr))
			{
				if ( IsValidFolderForRoles( sName, oFileAttr ))
				{
					LoadRole( sName.Substring(0, sName.Length() - 4) );
				}
			}
		}
	}

	bool RoleExists( string role )
	{
		return RolesMap.Contains( role );
	}
}

ref PermissionManager g_com_PermissionsManager;

ref PermissionManager GetPermissionsManager()
{
	if ( !g_com_PermissionsManager )
	{
		g_com_PermissionsManager = new ref PermissionManager();
	}

	return g_com_PermissionsManager;
}