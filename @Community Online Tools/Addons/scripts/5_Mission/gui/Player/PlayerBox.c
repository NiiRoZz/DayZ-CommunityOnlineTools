class PlayerBox extends ScriptedWidgetEventHandler 
{
    protected ref Widget layoutRoot;

    ref TextWidget      Name;
    ref ButtonWidget    Button;
    ref CheckBoxWidget  Checkbox;

    ref AuthPlayer  Player;

    ref PlayerMenu  Menu;

    bool ShowOnScreen;

    int Width;
    int Height;
    float FOV;
    vector ScreenPos;

    float BoxWidth;
    float BoxHeight;

    bool UseSyncedPosition;

    void OnWidgetScriptInit( Widget w )
    {
        layoutRoot = w;
        layoutRoot.SetHandler( this );

        Init();
    }

    void ~PlayerBox()
    {
    }

    void Init() 
    {
        Name = TextWidget.Cast(layoutRoot.FindAnyWidget("text_name"));
        Button = ButtonWidget.Cast(layoutRoot.FindAnyWidget("button"));
        Checkbox = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("checkbox"));

        layoutRoot.GetScreenSize( BoxWidth, BoxHeight );
    }

    void Show()
    {
        layoutRoot.Show( true );
        Button.Show( true );
        Checkbox.Show( true );
        Name.Show( true );
        OnShow();
    }

    void Hide()
    {
        OnHide();
        Name.Show( false );
        Button.Show( false );
        Checkbox.Show( false );
        layoutRoot.Show( false );
    }

    void OnShow()
    {
    }

    void OnHide() 
    {
    }

    float ATan( float a )
    {
        return Math.Asin( a ) / Math.Acos( a );
    }

    vector GetPosition()
    {
        if ( Player.PlayerObject && !UseSyncedPosition )
        {
            if ( Player.PlayerObject.IsInTransport() )
            {
                return Player.PlayerObject.GetPosition() + "0 1.1 0";
            } else 
            {
                vector position = Player.PlayerObject.GetPosition() + "0 1.85 0";

                int bone = Player.PlayerObject.GetBoneIndexByName( "Head" );

                if ( bone != -1 )
                {
                    position = Player.PlayerObject.GetBonePositionWS( bone ) + "0 0.2 0";
                }

                return position;
            }
        } else 
        {
            return Player.Data.VPosition + "0 1.85 0";
        }
    }

    void Update() 
    {
        vector position = GetPosition();

        vector normalize = ( position - GetGame().GetCurrentCameraPosition() );
        float dot = vector.Dot( normalize.Normalized(), GetGame().GetCurrentCameraDirection().Normalized() );
        
        float limit = FOV / 1.5;

        if ( dot < limit )
        {
            ShowOnScreen = false;
        } else
        {
            ShowOnScreen = true;
        }
            
        ScreenPos = GetGame().GetScreenPos( position );

        if ( ShowOnScreen )
        {
            if ( ScreenPos[2] > 1000 || ScreenPos[2] < 0 )
            {
                ShowOnScreen = false;
            }
        }

        if ( ShowOnScreen && Player )
        {
            layoutRoot.SetPos( ScreenPos[0] - ( BoxWidth / 8 ), ScreenPos[1] - ( Height / 2 ) - ( BoxHeight / 2 ), true );
            Show();
        } else 
        {
            Hide();
        }
    }

    ref Widget GetLayoutRoot() 
    {
        return layoutRoot;
    }

    void SetPlayer( ref AuthPlayer player )
    {
        Player = player;
        
        if ( Player == NULL || !COT_ESP_Toggled ) 
        {
            ShowOnScreen = false;
            Hide();
            GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Remove( this.Update );
        } else 
        {
            ShowOnScreen = true;

            if ( Player.PlayerObject == NULL )
            {
                Hide();
                ShowOnScreen = false;
                GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Remove( this.Update );
                return;
            }

            Name.SetText( Player.GetName() );

            if ( !GetGame().IsMultiplayer() )
            {
                ShowOnScreen = false;
                Name.SetColor( 0xFF4B77BE );
                Hide();
                return;
            }

            PlayerBase controllingPlayer = PlayerBase.Cast( GetGame().GetPlayer() );

            if ( controllingPlayer && Player.GetGUID() == controllingPlayer.GetIdentity().GetId() )
            {
                Name.SetColor( 0xFF2ECC71 );

                bool cameraExists = controllingPlayer.GetCurrentCamera() != NULL;

                if ( cameraExists && controllingPlayer.GetCurrentCamera().IsInherited( DayZPlayerCamera3rdPerson ) )
                {
                    ShowOnScreen = true;
                } else if ( cameraExists && controllingPlayer.GetCurrentCamera().IsInherited( DayZPlayerCamera3rdPersonVehicle ) )
                {
                    ShowOnScreen = true;
                } else if ( CurrentActiveCamera )
                {
                    ShowOnScreen = true;
                } else 
                {
                    ShowOnScreen = false;
                    Hide();
                }
            } 

            if ( ShowOnScreen )
            {
                Name.SetColor( 0xFFFFFFFF );
            
                ScreenPos = GetGame().GetScreenPos( GetPosition() );

                if ( ScreenPos[2] > 1000 || ScreenPos[2] < 0 )
                {
                    Hide();
                    ShowOnScreen = false;
                } else 
                {
                    if ( ScreenPos[2] > 100 )
                    {
                        UseSyncedPosition = true;
                    } else
                    {
                        UseSyncedPosition = false;
                    }

                    Show();

                    ShowOnScreen = true;

                    GetScreenSize( Width, Height );

                    FOV = Camera.GetCurrentFOV() * ( Height * 1.0 ) / ( Width * 1.0 );

                    GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert( this.Update );

                    Update();
                }
            }
        }
    }

    string GetName()
    {
        return Player.GetName();
    }

    ref AuthPlayer GetPlayer()
    {
        return Player;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {        
        if ( w == Checkbox )
        {
            Menu.OnPlayer_Checked( Player, Checkbox.IsChecked() );
        }

        if ( w == Button )
        {
            Checkbox.SetChecked( Menu.OnPlayer_Button( Player ) );
        }

        return true;
    }
}