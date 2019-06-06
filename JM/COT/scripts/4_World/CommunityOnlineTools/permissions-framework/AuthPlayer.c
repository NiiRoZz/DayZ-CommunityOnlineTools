class AuthPlayer
{
	ref Permission RootPermission;
	ref array< Role > Roles;

	PlayerBase PlayerObject;
	PlayerIdentity IdentityPlayer;

	ref PlayerData Data;

	protected ref PlayerFile m_PlayerFile;

	protected bool m_HasPermissions;
	protected bool m_HasPlayerData;

	void AuthPlayer( ref PlayerData data )
	{
		PlayerObject = NULL;

		Data = data;

		RootPermission = new ref Permission( Data.SSteam64ID );
		Roles = new array< Role >;
	}

	void ~AuthPlayer()
	{
		delete RootPermission;
	}

	void SwapData( PlayerData newData )
	{
		Data.Copy( newData );

		Deserialize();
	}

	string GetGUID()
	{
		return Data.SGUID;
	}

	string GetSteam64ID()
	{
		return Data.SSteam64ID;
	}

	string GetName()
	{
		return Data.SName;
	}

	void UpdatePlayerData()
	{
		if ( IdentityPlayer == NULL ) return;

		Data.IPingMin = IdentityPlayer.GetPingMin();
		Data.IPingMax = IdentityPlayer.GetPingMax();
		Data.IPingAvg = IdentityPlayer.GetPingAvg();
		
		Data.SSteam64ID = IdentityPlayer.GetPlainId();
		Data.SGUID = IdentityPlayer.GetId();
		Data.SName = IdentityPlayer.GetName();

		if ( PlayerObject == NULL ) return;

		Data.Load( PlayerObject );
	}

	void CopyPermissions( ref Permission copy )
	{
		ref array< string > data = new ref array< string >;
		copy.Serialize( data );

		for ( int i = 0; i < data.Count(); i++ )
		{
			RootPermission.AddPermission( data[i], PermissionType.INHERIT );
		}
	}

	void ClearPermissions()
	{
		delete RootPermission;

		RootPermission = new ref Permission( Data.SSteam64ID, NULL );

		m_HasPermissions = false;
	}

	void ClearRoles()
	{
		Roles.Clear();

		AddStringRole( "everyone" );
	}

	void AddPermission( string permission, PermissionType type = PermissionType.INHERIT )
	{
		RootPermission.AddPermission( permission, type );

		m_HasPermissions = true;
	}

	bool HasPermission( string permission )
	{
		// RootPermission.DebugPrint( 0 );

		PermissionType permType;

		bool has = RootPermission.HasPermission( permission, permType );

		GetLogger().Log( "" +  GetSteam64ID() + " returned " + has + " for permission " + permission + " with perm type " + permType, "JM_COT_PermissionFramework" );

		if ( has )
			return true;

		if ( permType == PermissionType.DISALLOW )
			return false;

		for ( int j = 0; j < Roles.Count(); j++ )
		{
			has = Roles[j].HasPermission( permission, permType );

			GetLogger().Log( "    Role " +  Roles[j].Name + " returned " + has + " for permission " + permission + " with perm type " + permType, "JM_COT_PermissionFramework" );

			if ( has )
			{
				return true;
			}
		}

		return false;
	}

	void AddStringRole( string role, bool shouldSerialize = true )
	{
		ref Role r = GetPermissionsManager().RolesMap.Get( role );

		if ( Roles.Find( r ) < 0 ) 
		{
			GetLogger().Log( "Adding role " + role + ": " + r, "JM_COT_PermissionFramework" );

			Roles.Insert( r );

			if ( shouldSerialize )
				m_HasPlayerData = true;
		}
	}

	void AddRole( Role role )
	{
		GetLogger().Log( "Adding role " + role.Name + ": " + role, "JM_COT_PermissionFramework" );

		m_HasPlayerData = true;

		Roles.Insert( role );
	}

	void Serialize()
	{
		Data.APermissions.Clear();
		Data.ARoles.Clear();

		RootPermission.Serialize( Data.APermissions );

		for ( int j = 0; j < Roles.Count(); j++ )
		{
			Data.ARoles.Insert( Roles[j].Name );
		}
	}

	void Deserialize()
	{
		ClearRoles();
		ClearPermissions();
		
		for ( int i = 0; i < Data.APermissions.Count(); i++ )
		{
			AddPermission( Data.APermissions[i] );
		}

		for ( int j = 0; j < Data.ARoles.Count(); j++ )
		{
			AddStringRole( Data.ARoles[j] );
		}
	}

	string FileReadyStripName( string name )
	{
		name.Replace( "\\", "" );
		name.Replace( "/", "" );
		name.Replace( "=", "" );
		name.Replace( "+", "" );

		return name;
	}

	void Save()
	{
		Serialize();

		if ( m_HasPlayerData )
		{   
			m_PlayerFile.Roles.Clear();

			GetLogger().Log( "Saving player data: " + Data.SSteam64ID, "JM_COT_PermissionFramework" );

			for ( int j = 0; j < Data.ARoles.Count(); j++ )
			{
				m_PlayerFile.Roles.Insert( Data.ARoles[j] );
			}

			m_PlayerFile.Save();
		}

		if ( m_HasPermissions )
		{
			GetLogger().Log( "Saving permissions: " + Data.SSteam64ID, "JM_COT_PermissionFramework" );
			FileHandle file = OpenFile( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt", FileMode.WRITE );

			if ( file != 0 )
			{
				string line;

				for ( int i = 0; i < Data.APermissions.Count(); i++ )
				{
					FPrintln( file, Data.APermissions[i] );
				}
				
				CloseFile(file);
			}
		}
	}

	protected bool ReadPermissions( string filename )
	{
		FileHandle file = OpenFile( filename, FileMode.READ );

		if ( file < 0 )
			return false;

		array< string > data = new array< string >;

		string line;

		while ( FGets( file, line ) > 0 )
		{
			data.Insert( line );
		}

		CloseFile( file );

		for ( int i = 0; i < data.Count(); i++ )
		{
			AddPermission( data[i] );
		}

		return true;
	}

	void Load()
	{

		m_HasPlayerData = PlayerFile.Load( Data, m_PlayerFile );

		for ( int j = 0; j < m_PlayerFile.Roles.Count(); j++ )
		{
			AddStringRole( m_PlayerFile.Roles[j], false );
		}

		GetLogger().Log( "Loading permissions for " + Data.SSteam64ID, "JM_COT_PermissionFramework" );

		if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt" ) )
		{
			m_HasPermissions = ReadPermissions( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt" );
		} else if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt.txt" ) )
		{
			m_HasPermissions = ReadPermissions( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt.txt" );
			
			DeleteFile( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + Data.SSteam64ID + ".txt.txt" );
			
			Save();
		} else
		{
			m_HasPermissions = false;
		}
	}

	void DebugPrint()
	{
		GetLogger().Log( "Printing permissions for " + Data.SSteam64ID, "JM_COT_PermissionFramework" );

		RootPermission.DebugPrint( 0 );
	}

	// TODO: Figure out how to make it work properly?
	void Kick()
	{
	}

	// TODO: Maybe actually ban the player?
	void Ban()
	{
	}
}