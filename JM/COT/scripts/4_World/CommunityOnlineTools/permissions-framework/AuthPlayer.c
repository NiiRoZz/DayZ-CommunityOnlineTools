class AuthPlayer: Managed
{
	ref Permission RootPermission;
	ref array< Role > Roles;

	protected PlayerIdentity m_PlayerIdentity;

	protected ref PlayerData m_Data;
	protected ref PlayerFile m_PlayerFile;

	protected bool m_HasPermissions;
	protected bool m_HasPlayerData;

	void AuthPlayer( PlayerIdentity identity )
	{
		m_PlayerIdentity = identity;

		RootPermission = new ref Permission( "" );
		Roles = new array< Role >;

		m_Data = new PlayerData();

		if ( m_PlayerIdentity )
			UpdatePlayerData();
	}

	void ~AuthPlayer()
	{
		delete RootPermission;
	}

	void UpdateData( PlayerData newData )
	{
		m_Data.Copy( newData );

		Deserialize();
	}

	PlayerIdentity GetPlayerIdentity()
	{
		return m_PlayerIdentity;
	}

	PlayerBase GetPlayerBase()
	{
		PlayerBase player = GetPlayerObjectByIdentity( m_PlayerIdentity );
		
		if ( player )
			player.authenticatedPlayer = this;

		return player;
	}

	PlayerData GetData()
	{
		return m_Data;
	}

	void UpdatePlayerData()
	{
		if ( m_PlayerIdentity == NULL )
			return;

		GetData().IPingMin		= m_PlayerIdentity.GetPingMin();
		GetData().IPingMax		= m_PlayerIdentity.GetPingMax();
		GetData().IPingAvg		= m_PlayerIdentity.GetPingAvg();
		
		GetData().SSteam64ID	= m_PlayerIdentity.GetPlainId();
		GetData().SGUID			= m_PlayerIdentity.GetId();
		GetData().SName			= m_PlayerIdentity.GetName();

		if ( GetPlayerBase() == NULL )
			return;

		GetData().Load( GetPlayerBase() );
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

		RootPermission = new ref Permission( GetData().SSteam64ID, NULL );

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

		GetLogger().Log( "" +  GetData().SSteam64ID + " returned " + has + " for permission " + permission + " with perm type " + permType, "JM_COT_PermissionFramework" );

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
		GetData().APermissions.Clear();
		GetData().ARoles.Clear();

		RootPermission.Serialize( GetData().APermissions );

		for ( int j = 0; j < Roles.Count(); j++ )
		{
			GetData().ARoles.Insert( Roles[j].Name );
		}
	}

	void Deserialize()
	{
		ClearRoles();
		ClearPermissions();
		
		for ( int i = 0; i < GetData().APermissions.Count(); i++ )
		{
			AddPermission( GetData().APermissions[i] );
		}

		for ( int j = 0; j < GetData().ARoles.Count(); j++ )
		{
			AddStringRole( GetData().ARoles[j] );
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

			GetLogger().Log( "Saving player data: " + GetData().SSteam64ID, "JM_COT_PermissionFramework" );

			for ( int j = 0; j < GetData().ARoles.Count(); j++ )
			{
				m_PlayerFile.Roles.Insert( GetData().ARoles[j] );
			}

			m_PlayerFile.Save();
		}

		if ( m_HasPermissions )
		{
			GetLogger().Log( "Saving permissions: " + GetData().SSteam64ID, "JM_COT_PermissionFramework" );
			FileHandle file = OpenFile( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt", FileMode.WRITE );

			if ( file != 0 )
			{
				string line;

				for ( int i = 0; i < GetData().APermissions.Count(); i++ )
				{
					FPrintln( file, GetData().APermissions[i] );
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
		m_HasPlayerData = PlayerFile.Load( GetData(), m_PlayerFile );

		for ( int j = 0; j < m_PlayerFile.Roles.Count(); j++ )
		{
			AddStringRole( m_PlayerFile.Roles[j], false );
		}

		GetLogger().Log( "Loading permissions for " + GetData().SSteam64ID, "JM_COT_PermissionFramework" );

		if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt" ) )
		{
			m_HasPermissions = ReadPermissions( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt" );
		} else if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt.txt" ) )
		{
			m_HasPermissions = ReadPermissions( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt.txt" );
			
			DeleteFile( PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\" + GetData().SSteam64ID + ".txt.txt" );
			
			Save();
		} else
		{
			m_HasPermissions = false;
		}
	}

	void DebugPrint()
	{
		GetLogger().Log( "Printing permissions for " + GetData().SSteam64ID, "JM_COT_PermissionFramework" );

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