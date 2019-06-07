int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR = 0;
int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR = 5;
int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION = 1;

class PermissionsFramework
{
	protected ref array< PlayerIdentity > m_ServerIdentities;

	protected bool m_bLoaded;

	void PermissionsFramework()
	{
		MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY );

		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
		{
			m_ServerIdentities = new array< PlayerIdentity >;
		}

		m_bLoaded = false;

		GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayers", this, SingeplayerExecutionType.Server );
		GetRPCManager().AddRPC( "PermissionsFramework", "RemovePlayer", this, SingeplayerExecutionType.Client );
		GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayer", this, SingeplayerExecutionType.Client );
		GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayerData", this, SingeplayerExecutionType.Client );
		GetRPCManager().AddRPC( "PermissionsFramework", "UpdateRole", this, SingeplayerExecutionType.Client );
		GetRPCManager().AddRPC( "PermissionsFramework", "SetClientPlayer", this, SingeplayerExecutionType.Client );
		GetRPCManager().AddRPC( "PermissionsFramework", "CheckVersion", this, SingeplayerExecutionType.Server );

		GetPermissionsManager().RegisterPermission( "Admin.Player.Read" );
		GetPermissionsManager().RegisterPermission( "Admin.Roles.Update" );
	}

	void ~PermissionsFramework()
	{
		Print("PermissionsFramework::~PermissionsFramework");

		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
		{
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( this.ReloadPlayerList );

			delete m_ServerIdentities;
		}
	}
	
	void OnStart()
	{
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
		{
			GetPermissionsManager().LoadRoles();
		}
	}

	void OnFinish()
	{
	}

	void OnLoaded()
	{
		if ( GetGame().IsClient() && GetGame().IsMultiplayer() )
		{
			GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayers", new Param, true );
			GetRPCManager().SendRPC( "PermissionsFramework", "CheckVersion", new Param3< int, int, int >( JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR, JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR, JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION ), true );
		}

		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
		{
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( this.ReloadPlayerList, 1000, true );
		}
	}

	void Update( float timeslice )
	{
		if ( !m_bLoaded && !GetDayZGame().IsLoading() )
		{
			m_bLoaded = true;
			OnLoaded();
		} else 
		{
			OnUpdate( timeslice );
		}
	}

	void OnUpdate( float timeslice )
	{

	}

	void ReloadPlayerList()
	{
		GetGame().GetPlayerIndentities( m_ServerIdentities );

		for ( int j = 0; j < m_ServerIdentities.Count(); j++ )
		{
			GetPermissionsManager().GetPlayerByIdentity( m_ServerIdentities[j] ).UpdatePlayerData();
		}

		m_ServerIdentities.Clear();
	}

	void CheckVersion( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param3< int, int, int > data;
		if ( !ctx.Read( data ) ) return;

		if ( type == CallType.Server )
		{
			if ( data.param1 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR )
			{
				Print( "" + sender.GetPlainId() + " is running a different major version of Permissions Framework." );
				return;
			}

			if ( data.param2 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR )
			{
				Print( "" + sender.GetPlainId() + " is running a different minor version of Permissions Framework." );
				return;
			}

			if ( data.param3 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION )
			{
				Print( "" + sender.GetPlainId() + " is running a different revision of Permissions Framework." );	   
				return;
			}
		}
	}

	void UpdatePlayers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		if ( type == CallType.Server )
		{
			if ( GetGame().IsMultiplayer() )
			{
				for ( int i = 0; i < GetPermissionsManager().GetPlayers().Count(); i++ )
				{
					GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayerData", new Param1< PlayerData >( GetPermissionsManager().GetPlayers()[i].GetData() ), false, sender );
				}
			}
		}
	}

	void RemovePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		if ( type == CallType.Client )
		{
			if ( GetGame().IsMultiplayer() )
			{
				ref Param1< PlayerData > data;
				if ( !ctx.Read( data ) ) return;
				
				AuthPlayer player = DeserializePlayer( data.param1 );

				RemoveSelectedPlayer( player );
				GetPermissionsManager().AuthPlayers.RemoveItem( player );
			}
		}
	}

	void UpdatePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		if ( type == CallType.Client )
		{
			if ( GetGame().IsMultiplayer() )
			{
				ref Param3< string, string, string > data;
				if ( !ctx.Read( data ) ) return;

				ref AuthPlayer ap = GetPermissionsManager().GetPlayerBySteam64ID( data.param1 );

				ap.GetData().SName = data.param2;
				ap.GetData().SGUID = data.param3;

				if ( ClientAuthPlayer == NULL )
				{
					return;
				}

				if ( ClientAuthPlayer.GetData() == NULL )
				{
					return;
				}

				if ( ClientAuthPlayer.GetData().SGUID == data.param3 )
				{
					GetModuleManager().OnClientPermissionsUpdated();
				}
			}
		}
	}

	void UpdatePlayerData( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		if ( type == CallType.Server )
		{
			if ( !GetPermissionsManager().HasPermission( "Admin.Player.Read", sender ) )
				return;

			Param1< string > data;
			if ( !ctx.Read( data ) )
				return;

			if ( GetGame().IsMultiplayer() )
			{
				AuthPlayer player = GetPermissionsManager().GetPlayerByGUID( data.param1 );
				if ( !player )
					return;

				if ( sender && data.param1 == sender.GetId() )
				{
					GetRPCManager().SendRPC( "PermissionsFramework", "SetClientPlayer", new Param1< ref PlayerData >( SerializePlayer( player ) ), false, sender );
				} else 
				{
					GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayerData", new Param1< ref PlayerData >( SerializePlayer( player ) ), false, sender );
				}
			}
		}

		if ( type == CallType.Client )
		{
			if ( GetGame().IsMultiplayer() )
			{
				Param1< ref PlayerData > cdata;
				if ( !ctx.Read( cdata ) )
					return;

				DeserializePlayer( cdata.param1 );
			}
		}
	}

	void SetClientPlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		if ( type == CallType.Client )
		{
			if ( GetGame().IsMultiplayer() )
			{
				ref Param1< ref PlayerData > data;
				if ( !ctx.Read( data ) )
					return;

				ClientAuthPlayer = DeserializePlayer( data.param1 );
				
				GetModuleManager().OnClientPermissionsUpdated();
			}
		}
	}

	void UpdateRole( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		ref Param2< string, ref array< string > > data;
		if ( !ctx.Read( data ) )
			return;

		ref array< string > arr = new ref array< string >;
		arr.Copy( data.param2 );

		ref Role role = NULL;

		if ( type == CallType.Server )
		{
			if ( !GetPermissionsManager().HasPermission( "Admin.Roles.Update", sender ) )
				return;

			GetPermissionsManager().RolesMap.Find( data.param1, role );

			if ( role )
			{
				role.ClearPermissions();

				role.SerializedData = arr;

				role.Deserialize();
			} else 
			{
				role = GetPermissionsManager().LoadRole( data.param1, arr );
			}

			role.Serialize();
				
			GetRPCManager().SendRPC( "PermissionsFramework", "UpdateRole", new Param2< string, ref array< string > >( role.Name, role.SerializedData ), false );
		}

		if ( type == CallType.Client )
		{
			GetPermissionsManager().RolesMap.Find( data.param1, role );

			if ( role )
			{
				role.ClearPermissions();

				role.SerializedData = arr;

				role.Deserialize();
			} else 
			{
				GetPermissionsManager().LoadRole( data.param1, arr );
			}
		}
	}
}