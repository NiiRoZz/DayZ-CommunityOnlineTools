class PlayerMenu extends Form
{
    ref array< ref PlayerRow >      m_PlayerList;
    ref array< ref PermissionRow >  m_PermissionList;

    ref TextWidget                  m_PlayerCount;
    ref GridSpacerWidget            m_PlayerScriptListFirst;
    int                             m_PlayersInFirst;
    ref GridSpacerWidget            m_PlayerScriptListSecond;
    int                             m_PlayersInSecond;
    int                             m_UserID;

    ref Widget                      m_ActionsWrapper;
    ref Widget                      m_ActionsForm;

    ref Widget                      m_PermissionsWrapper;
    ref Widget                      m_PermsContainer;
    ref ButtonWidget                m_SetPermissionsButton;
    ref ButtonWidget                m_PermissionsBackButton;

    bool                            m_CanUpdateList;

    ref Permission                  m_LoadedPermission;
    ref PermissionRow               m_PermissionUI;

    private bool                    m_PermissionsLoaded;

    private bool                    m_DataJustUpdated;

    ref UIActionText m_GUID;
    ref UIActionText m_Name;
    ref UIActionText m_Steam64ID;

    ref UIActionText m_PosX;
    ref UIActionText m_PosY;
    ref UIActionText m_PosZ;

    ref UIActionText m_PingMin;
    ref UIActionText m_PingMax;
    ref UIActionText m_PingAvg;

    ref UIActionEditableText m_Health;
    ref UIActionEditableText m_Blood;
    ref UIActionEditableText m_Energy;
    ref UIActionEditableText m_Water;
    ref UIActionEditableText m_Shock;
    ref UIActionEditableText m_HeatComfort;
    ref UIActionEditableText m_Wet;
    ref UIActionEditableText m_Tremor;
    ref UIActionEditableText m_Stamina;
    ref UIActionSelectBox m_LastShaved;
    ref UIActionCheckbox m_BloodyHands;

    ref UIActionButton m_KickTransport;
    ref UIActionButton m_RepairTransport;
    ref UIActionButton m_TeleportToMe;
    ref UIActionButton m_TeleportMeTo;

    ref UIActionButton m_ModifyPermissions;
    ref UIActionButton m_BanPlayer;
    ref UIActionButton m_KickPlayer;
    ref UIActionCheckbox m_Freecam;
    ref UIActionCheckbox m_GodMode;
    ref UIActionButton m_SpectatePlayer;

    void PlayerMenu()
    {
        m_CanUpdateList = true;

        m_PermissionsLoaded = false;

        m_PlayerList = new ref array< ref PlayerRow >;
        m_PermissionList = new ref array< ref PermissionRow >;
    }

    void ~PlayerMenu()
    {
        delete m_PlayerList;
        delete m_PermissionList;
    }

    override string GetTitle()
    {
        return "#STR_PLAYER_MANAGEMENT_TITLE";
    }
    
    override string GetIconName()
    {
        return "P";
    }

    override bool ImageIsIcon()
    {
        return false;
    }

    override void OnInit( bool fromMenu )
    {
        m_PlayerScriptListFirst = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("player_list_first"));
        m_PlayerScriptListSecond = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("player_list_second"));
        m_PlayersInFirst = 0;
        m_PlayersInSecond = 0;
        m_UserID = 2000;

        m_PlayerCount = TextWidget.Cast(layoutRoot.FindAnyWidget("player_count"));

        m_ActionsForm = layoutRoot.FindAnyWidget("actions_form");
        m_ActionsWrapper = layoutRoot.FindAnyWidget("actions_wrapper");
        
        m_GUID = UIActionManager.CreateText( m_ActionsWrapper, "#STR_PLAYER_MANAGEMENT_GUID", "" );
        m_Name = UIActionManager.CreateText( m_ActionsWrapper, "#STR_PLAYER_MANAGEMENT_NAME", "" );
        m_Steam64ID = UIActionManager.CreateText( m_ActionsWrapper, "#STR_PLAYER_MANAGEMENT_STEAM", "" );

        ref Widget playerPosition = UIActionManager.CreateGridSpacer( m_ActionsWrapper, 1, 3 );
        m_PosX = UIActionManager.CreateText( playerPosition, "#STR_PLAYER_MANAGEMENT_POS_X", "" );
        m_PosY = UIActionManager.CreateText( playerPosition, "#STR_PLAYER_MANAGEMENT_POS_Y", "" );
        m_PosZ = UIActionManager.CreateText( playerPosition, "#STR_PLAYER_MANAGEMENT_POS_Z", "" );

        ref Widget pings = UIActionManager.CreateGridSpacer( m_ActionsWrapper, 1, 3 );
        m_PingMin = UIActionManager.CreateText( pings, "#STR_PLAYER_MANAGEMENT_PING_MIN", "" );
        m_PingMax = UIActionManager.CreateText( pings, "#STR_PLAYER_MANAGEMENT_PING_MAX", "" );
        m_PingAvg = UIActionManager.CreateText( pings, "#STR_PLAYER_MANAGEMENT_PING_AVERAGE", "" );
        pings.Show( false ); // because this doesn't work

        ref Widget playerActions = UIActionManager.CreateGridSpacer( m_ActionsWrapper, 3, 2 );
        m_Health = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_HEALTH", this, "Click_SetHealth", "", "Set" );
        m_Shock = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_SHOCK", this, "Click_SetShock", "", "Set" );
        m_Blood = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_BLOOD", this, "Click_SetBlood", "", "Set" );
        m_Energy = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_ENERGY", this, "Click_SetEnergy", "", "Set" );
        m_Water = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_WATER", this, "Click_SetWater", "", "Set" );
        //m_HeatComfort = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_TEMPERATURE_CHANGE", this, "Click_SetHeatComfort", "", "Set" );
        //m_Wet = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_WETNESS", this, "Click_SetWet", "", "Set" );
        //m_Tremor = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_TREMOR", this, "Click_SetTremor", "", "Set" );
        m_Stamina = UIActionManager.CreateEditableText( playerActions, "#STR_PLAYER_MANAGEMENT_STAMINA", this, "Click_SetStamina", "", "Set" );

        ref array< string > lifeSpanOptions = new ref array< string >;
        lifeSpanOptions.Insert( "#STR_PLAYER_MANAGEMENT_BEARD_NONE" );
        lifeSpanOptions.Insert( "#STR_PLAYER_MANAGEMENT_BEARD_MEDIUM " );
        lifeSpanOptions.Insert( "#STR_PLAYER_MANAGEMENT_BEARD_LARGE" );
        lifeSpanOptions.Insert( "#STR_PLAYER_MANAGEMENT_BEARD_EXTRA" );

        // m_LastShaved = UIActionManager.CreateSelectionBox( playerActions, "#STR_PLAYER_MANAGEMENT_BEARD", lifeSpanOptions, this, "Click_SetLifeSpanState" );
        m_BloodyHands = UIActionManager.CreateCheckbox( playerActions, "STR_PLAYER_MANAGEMENT_BLOODY_HANDS", this, "Click_SetBloodyHands", false );
        // m_KickTransport = UIActionManager.CreateButton( playerActions, "#STR_PLAYER_MANAGEMENT_KICK_TRANSPORT", this, "Click_KickTransport" );
        m_RepairTransport = UIActionManager.CreateButton( playerActions, "#STR_PLAYER_MANAGEMENT_REPAIR_TRANSPORT", this, "Click_RepairTransport" );
        m_TeleportToMe = UIActionManager.CreateButton( playerActions, "#STR_PLAYER_MANAGEMENT_TELEPORT_TO_ME", this, "Click_TeleportToMe" );
        m_TeleportMeTo = UIActionManager.CreateButton( playerActions, "#STR_PLAYER_MANAGEMENT_TELEPORT_ME_TO", this, "Click_TeleportMeTo" );

        ref Widget serverActions = UIActionManager.CreateGridSpacer( m_ActionsWrapper, 1, 2 );
        m_ModifyPermissions = UIActionManager.CreateButton( serverActions, "#STR_PLAYER_MANAGEMENT_MODIFY_PERMISSIONS", this, "Click_ModifyPermissions" );
        //m_Freecam = UIActionManager.CreateCheckbox( serverActions, "#STR_PLAYER_MANAGEMENT_FREECAM", this, "Click_ToggleFreecam", false );
        // m_GodMode = UIActionManager.CreateCheckbox( serverActions, "#STR_PLAYER_MANAGEMENT_GODMODE", this, "Click_GodMode", false );
        m_SpectatePlayer = UIActionManager.CreateButton( serverActions, "#STR_PLAYER_MANAGEMENT_SPECTATE_PLAYER", this, "Click_SpectatePlayer" );

        //m_BanPlayer = UIActionManager.CreateButton( serverActions, "#STR_PLAYER_MANAGEMENT_BAN_PLAYER", this, "Click_BanPlayer" );
        //m_KickPlayer = UIActionManager.CreateButton( serverActions, "#STR_PLAYER_MANAGEMENT_KICK_PLAYER", this, "Click_KickPlayer" );

        m_PermissionsWrapper = layoutRoot.FindAnyWidget("permissions_wrapper");
        m_PermsContainer = layoutRoot.FindAnyWidget("permissions_container");
        m_SetPermissionsButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("permissions_set_button"));
        m_PermissionsBackButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("permissions_back_button"));
    }

    void Click_ModifyPermissions()
    {
        m_ActionsForm.Show( false );
        m_PermissionsWrapper.Show( true );
    }

    void Click_SpectatePlayer( UIEvent eid, ref UIActionButton action )
    {
        if ( GetSelectedPlayers().Count() != 1 ) return;

        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;

        bool shouldSpectate = true;

        if ( CurrentActiveCamera )
        {
            SpectatorCamera cam = SpectatorCamera.Cast( CurrentActiveCamera );
            
            if ( cam )
            {
                shouldSpectate = false;
            }
        }

        GetRPCManager().SendRPC( "COT_Admin", "SpectatePlayer", new Param2< bool, ref array< string > >( shouldSpectate, SerializePlayersID( GetSelectedPlayers() ) ), true, NULL, GetPlayer() );
    }

    void Click_BanPlayer( UIEvent eid, ref UIActionButton action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "BanPlayer", new Param1< ref array< string > >( SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_KickPlayer( UIEvent eid, ref UIActionButton action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "KickPlayer", new Param1< ref array< string > >( SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_KickTransport( UIEvent eid, ref UIActionButton action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_KickTransport", new Param1< ref array< string > >( SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_RepairTransport( UIEvent eid, ref UIActionButton action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_RepairTransport", new Param1< ref array< string > >( SerializePlayersID( GetSelectedPlayers() ) ), true );
    }
    
    void Click_TeleportToMe( UIEvent eid, ref UIActionButton action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;

        if ( CurrentActiveCamera && CurrentActiveCamera.IsActive() )
        {
            GetRPCManager().SendRPC( "COT_Admin", "Player_TeleportToMe", new Param2< vector, ref array< string > >( CurrentActiveCamera.GetPosition(), SerializePlayersID( GetSelectedPlayers() ) ), true );
        } else 
        {
            GetRPCManager().SendRPC( "COT_Admin", "Player_TeleportToMe", new Param2< vector, ref array< string > >( GetPlayer().GetPosition(), SerializePlayersID( GetSelectedPlayers() ) ), true );
        }
    }

    void Click_TeleportMeTo( UIEvent eid, ref UIActionButton action )
    {
        if ( GetSelectedPlayers().Count() != 1 ) return;

        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;

        GetRPCManager().SendRPC( "COT_Admin", "Player_TeleportMeTo", new Param1< ref array< string > >( SerializePlayersID( GetSelectedPlayers() ) ), true, NULL, GetPlayer() );
    }

    void Click_SetHealth( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetHealth", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetShock( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetShock", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetBlood( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetBlood", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetEnergy( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetEnergy", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetWater( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetWater", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetHeatComfort( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetHeatComfort", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetWet( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetWet", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetTremor( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetTremor", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetStamina( UIEvent eid, ref UIActionEditableText action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetStamina", new Param2< float, ref array< string > >( ToFloat( action.GetText() ), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetLifeSpanState( UIEvent eid, ref UIActionSelectBox action )
    {
        if ( eid != UIEvent.CHANGE ) return;

        int state = action.GetSelection();

        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetLifeSpanState", new Param2< int, ref array< string > >( state, SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_SetBloodyHands( UIEvent eid, ref UIActionCheckbox action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "Player_SetBloodyHands", new Param2< float, ref array< string > >( action.IsChecked(), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_ToggleFreecam( UIEvent eid, ref UIActionCheckbox action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "ToggleFreecam", new Param2< bool, ref array< string > >( action.IsChecked(), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void Click_GodMode( UIEvent eid, ref UIActionCheckbox action )
    {
        if ( eid != UIEvent.CLICK ) return;
        m_DataJustUpdated = true;
        GetRPCManager().SendRPC( "COT_Admin", "GodMode", new Param2< bool, ref array< string > >( action.IsChecked(), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void UpdateActionsFields( ref PlayerData data )
    {
        if ( m_DataJustUpdated )
        {
            m_DataJustUpdated = false;
            return;
        }

        if ( data )
        {
            m_GUID.SetText( data.SGUID );
            m_Name.SetText( data.SName );
            m_Steam64ID.SetText( data.SSteam64ID );

            m_Health.SetText( data.FHealth.ToString() );
            m_Blood.SetText( data.FBlood.ToString() );
            m_Energy.SetText( data.FEnergy.ToString() );
            m_Water.SetText( data.FWater.ToString() );
            m_Shock.SetText( data.FShock.ToString() );
            //m_HeatComfort.SetText( data.FHeatComfort.ToString() );
            //m_Wet.SetText( data.FWet.ToString() );
            //m_Tremor.SetText( data.FTremor.ToString() );
            m_Stamina.SetText( data.FStamina.ToString() );
            //m_LastShaved.SetSelection( data.ILifeSpanState );
            m_BloodyHands.SetChecked( data.BBloodyHands );

            m_PosX.SetText( "" + data.VPosition[0] );
            m_PosY.SetText( "" + data.VPosition[1] );
            m_PosZ.SetText( "" + data.VPosition[2] );

            m_PingMin.SetText( data.IPingMin.ToString() );
            m_PingMax.SetText( data.IPingMax.ToString() );
            m_PingAvg.SetText( data.IPingAvg.ToString() );

            m_ActionsForm.FindAnyWidget( "disabled" ).Show( false );
        } else 
        {
            m_GUID.SetText( "" );
            m_Name.SetText( "" );
            m_Steam64ID.SetText( "" );

            m_Health.SetText( "" );
            m_Blood.SetText( "" );
            m_Energy.SetText( "" );
            m_Water.SetText( "" );

            m_PingMin.SetText( "" );
            m_PingMax.SetText( "" );
            m_PingAvg.SetText( "" );

            m_ActionsForm.Show( true );
            m_PermissionsWrapper.Show( false );

            m_ActionsForm.FindAnyWidget("disabled").Show( true );
        }
    }

    override void OnShow()
    {
        ReloadPlayers();

        if ( m_PermissionsLoaded == false )
        {
            SetupPermissionsUI();

            CreatePlayerList();

            m_PermissionsLoaded = true;
        }

        m_PermissionsWrapper.Show( false );
        m_ActionsWrapper.Show( true );

        if ( GetSelectedPlayers().Count() != 0 )
        {
            UpdateActionsFields( GetSelectedPlayers()[0].Data );
        } else
        {
            UpdateActionsFields( NULL );
        }

        GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( UpdateList, 1000, true );
    }

    override void OnHide() 
    {
        GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove( UpdateList );
    }

    void UpdateList()
    {
        if ( m_CanUpdateList )
        {
            m_CanUpdateList = false;
            UpdatePlayerList();
            m_CanUpdateList = true;

            if ( GetSelectedPlayers().Count() > 0 )
            {
                GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayerData", new Param1< string >( GetSelectedPlayers()[0].GetGUID() ), true );
            }
        }

        ReloadPlayers();
    }

    override bool OnClick( Widget w, int x, int y, int button )
    {
        if ( w == m_SetPermissionsButton )
        {
            SetPermissions();
        }

        if ( w == m_PermissionsBackButton )
        {
            m_PermissionsWrapper.Show( false );
            m_ActionsForm.Show( true );
        }

        return false;
    }

    void OnPlayer_Checked( ref PlayerRow row )
    {
        OnPlayerSelected( row.GetPlayer(), row.Checkbox.IsChecked() );
    }

    void OnPlayer_Button( ref PlayerRow row )
    {
        OnPlayerSelected( NULL );

        if ( OnPlayerSelected( row.GetPlayer() ) )
        {
            row.Checkbox.SetChecked( true );
        }
    }

    bool OnPlayerSelected( ref AuthPlayer player, bool select = true )
    {
        if ( player == NULL )
        {
            UpdateActionsFields( NULL );

            GetSelectedPlayers().Clear();

            for ( int i = 0; i < m_PlayerList.Count(); i++ )
            {
                m_PlayerList[i].Checkbox.SetChecked( false );
            }

            LoadPermissions( NULL );

            return false;
        } else 
        {
            int position = -1;

            if ( select )
            {
                position = AddSelectedPlayer( player );

                if ( GetSelectedPlayers().Count() == 1 )
                {
                    UpdateActionsFields( player.Data );

                    LoadPermissions( GetSelectedPlayers()[0].Data.APermissions );
                }

                return true;
            } else
            {
                position = RemoveSelectedPlayer( player );

                if ( position == 0 )
                {
                    if (GetSelectedPlayers().Count() > 0 )
                    {
                        UpdateActionsFields( GetSelectedPlayers()[0].Data );
                        LoadPermissions( GetSelectedPlayers()[0].Data.APermissions );
                    } else 
                    {
                        UpdateActionsFields( NULL );
                        LoadPermissions( NULL );
                    }
                }

                return true;
            }
        }
    }

    void ReloadPlayers()
    {
        GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayers", new Param, true );
    }

    void SetupPermissionsUI()
    {
        ref Permission rootPerm = GetPermissionsManager().GetRootPermission() 

        Widget permRow = GetGame().GetWorkspace().CreateWidgets( "JM/COT/gui/layouts/player/permissions/PermissionRow.layout", m_PermsContainer );

        permRow.GetScript( m_PermissionUI );

        permRow.Show( false );
        permRow.SetSize( 0, 0 );

        if ( m_PermissionUI )
        {
            m_PermissionList.Clear();

            m_PermissionUI.InitPermission( rootPerm, 0 );

            InitPermissionUIRow( rootPerm, 0, m_PermissionUI );
        }
    }

    void ResetPermissionUI()
    {
        m_PermissionUI.Disable();
    }

    ref array< string > SerializePermissionUI()
    {
        ref array< string > output = new ref array< string >;
        m_PermissionUI.Serialize( output );
        return output;
    }

    private void InitPermissionUIRow( ref Permission perm, int depth, ref PermissionRow parentRow )
    {
        for ( int i = 0; i < perm.Children.Count(); i++ )
        {
            ref Permission cPerm = perm.Children[i];

            Widget permRow = GetGame().GetWorkspace().CreateWidgets( "JM/COT/gui/layouts/player/permissions/PermissionRow.layout", m_PermsContainer );

            ref PermissionRow rowScript;
            permRow.GetScript( rowScript );

            if ( rowScript )
            {
                m_PermissionList.Insert( rowScript );
                rowScript.InitPermission( cPerm, depth );

                parentRow.Children.Insert( rowScript );
                rowScript.Parent = parentRow;

                InitPermissionUIRow( cPerm, depth + 1, rowScript );
            }
        }
    }

    void LoadPermissions( ref array< string > permissions = NULL )
    {
        if ( permissions == NULL )
        {
            for ( int i = 0; i < m_PermissionList.Count(); i++ )
            {
                m_PermissionList[i].Disable();
            }
        } else 
        {
            for ( int j = 0; j < permissions.Count(); j++ )
            {
                m_PermissionUI.SetPermission( permissions[j] );
            }
            m_PermissionUI.Enable();
        }
    }

    void SetPermissions()
    {
        GetRPCManager().SendRPC( "COT_Admin", "SetPermissions", new Param2< ref array< string >, ref array< string > >( SerializePermissionUI(), SerializePlayersID( GetSelectedPlayers() ) ), true );
    }

    void CreatePlayerList()
    {
        ref Widget playerRow = NULL;
        ref PlayerRow rowScript = NULL;

        for ( int i = 0; i < 100; i++ )
        {
            rowScript = NULL;
            playerRow = NULL;

            playerRow = GetGame().GetWorkspace().CreateWidgets( "JM/COT/gui/layouts/player/PlayerRow.layout", m_PlayerScriptListFirst );
            
            if ( playerRow == NULL ) continue;

            m_UserID++;

            playerRow.SetUserID( m_UserID );
            playerRow.GetScript( rowScript );

            if ( rowScript == NULL ) continue;

            rowScript.SetPlayer( NULL );

            rowScript.Menu = this;

            m_PlayerList.Insert( rowScript );
        }

        for ( i = 0; i < 100; i++ )
        {
            rowScript = NULL;
            playerRow = NULL;

            playerRow = GetGame().GetWorkspace().CreateWidgets( "JM/COT/gui/layouts/player/PlayerRow.layout", m_PlayerScriptListSecond );
            
            if ( playerRow == NULL ) continue;

            m_UserID++;

            playerRow.SetUserID( m_UserID );
            playerRow.GetScript( rowScript );

            if ( rowScript == NULL ) continue;

            rowScript.SetPlayer( NULL );

            rowScript.Menu = this;

            m_PlayerList.Insert( rowScript );
        }                
    }

    void UpdatePlayerList()
    {
        array< ref AuthPlayer > players = GetPermissionsManager().GetPlayers();
        
        for ( int k = 0; k < m_PlayerList.Count(); k++ )
        {
            m_PlayerList[k].SetPlayer( NULL );
        }

        for ( int i = 0; i < players.Count(); i++ )
        {
            m_PlayerList[i].SetPlayer( players[i] );

            if ( PlayerAlreadySelected( players[i] ) )
            {
                // OnPlayerSelected( players[i], true );
                m_PlayerList[i].Checkbox.SetChecked( true );
            } else
            {
                OnPlayerSelected( players[i], false );
                m_PlayerList[i].Checkbox.SetChecked( false );
            }
        }

        if (GetSelectedPlayers().Count() > 0 )
        {
            UpdateActionsFields( GetSelectedPlayers()[0].Data );
        }

        int playerCount = players.Count();

        if ( playerCount == 1 )
        {
            m_PlayerCount.SetText( "1 Online" );
        } else 
        {
            m_PlayerCount.SetText( "" + playerCount + " Online" );
        }
    }
}