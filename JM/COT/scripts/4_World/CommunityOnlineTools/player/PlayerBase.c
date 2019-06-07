
modded class PlayerBase
{
	ref AuthPlayer authenticatedPlayer;

	bool m_HasGodeMode;

	override void Init()
	{
		super.Init();

		//RegisterNetSyncVariableBool( "m_HasGodeMode" );
	}

	/*
	void HandleDeath( Object killerItem ) 
	{
		PlayerBase killer = PlayerBase.Cast( killerItem );

		if ( !authenticatedPlayer ) return;
		
		if ( !killer )
		{
			killer = PlayerBase.Cast( EntityAI.Cast( killerItem ).GetHierarchyParent() );

			if ( !killer )
			{
				return;
			}
		}

		if ( !killer.authenticatedPlayer ) return;

		if ( killer.authenticatedPlayer.GetData().SGUID == authenticatedPlayer.GetData().SGUID )
		{
			killer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( killer.authenticatedPlayer.GetName() + " has killed themselves." ), false, NULL);
			return;
		}

		authenticatedPlayer.GetData().Kills = 0;

		killer.authenticatedPlayer.GetData().Kills++;
		killer.authenticatedPlayer.GetData().TotalKills++;

		killer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "You have " + killer.authenticatedPlayer.GetData().Kills.ToString() + " kill(s)." ), false, killer.GetIdentity());
		
		string message = "";

		switch ( killer.authenticatedPlayer.GetData().Kills )
		{
			case 5:
				message = " is on a killing spree!";
				break;
			case 10:
				message = " is dominating!";
				break;
			case 15:
				message = " is Unstoppable!";
				break;
			case 20:
				message = " is Unstoppable!";
				break;
			case 25:
				message = " is Godlike!";
				break;
		}

		if ( killer.authenticatedPlayer.GetData().Kills > 25 ) 
		{
			message = " is Godlike!";
		}
		
		int distance = Math.Round( vector.Distance( killer.GetPosition(), GetPosition()) );

		string weapon = killerItem.GetType();

		killer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( killer.authenticatedPlayer.GetName() + " has killed " + authenticatedPlayer.GetName() + " (" + weapon + ")" + "(" + distance + "m)" ), false, NULL);
		
		if ( message.Length() > 0 )
		{
			killer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( killer.authenticatedPlayer.GetName() + message ), false, NULL );
		}
	}
	*/

	void SetGodMode( bool mode )
	{
		m_HasGodeMode = mode;

		if ( m_HasGodeMode )
		{
			SetAllowDamage(false);
			m_HasGodeMode = true;
			//Notify( authenticatedPlayer, "You now have god mode." );
		} else
		{
			SetAllowDamage(true);
			m_HasGodeMode = false;
			//Notify( authenticatedPlayer, "You no longer have god mode." );
		}
	}
}