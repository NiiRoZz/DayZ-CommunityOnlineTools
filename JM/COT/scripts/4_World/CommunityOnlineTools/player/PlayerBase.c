
modded class PlayerBase
{
	ref AuthPlayer authenticatedPlayer;

	protected bool m_HasGodMode;

	override void Init()
	{
		super.Init();
	}

	bool HasGodMode()
	{
		return m_HasGodMode;
	}

	void SetGodMode( bool mode )
	{
		m_HasGodMode = mode;

		if ( m_HasGodMode )
		{
			SetAllowDamage(false);
			m_HasGodMode = true;
			//Notify( authenticatedPlayer, "You now have god mode." );
		} else
		{
			SetAllowDamage(true);
			m_HasGodMode = false;
			//Notify( authenticatedPlayer, "You no longer have god mode." );
		}
	}
}