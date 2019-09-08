class JMWindowBase extends ScriptedWidgetEventHandler 
{
	protected ref Widget layoutRoot;

	protected ref ButtonWidget m_CloseButton;
	protected ref Widget m_TitleWrapper;

	ref JMFormBase form;

	float offsetX;
	float offsetY;

	void JMWindowBase() 
	{
	}

	void ~JMWindowBase() 
	{
		layoutRoot.Unlink();

		delete m_CloseButton;
		delete m_TitleWrapper;

		delete layoutRoot;
	}

	void OnWidgetScriptInit( Widget w )
	{
		layoutRoot = w;
		layoutRoot.SetHandler( this );

		Init();
	}

	void Init() 
	{
		m_CloseButton = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "close_button" ) );
		m_TitleWrapper = Widget.Cast( layoutRoot.FindAnyWidget( "title_bar_drag" ) );
	}

	void SetSize( float w, float h )
	{
		layoutRoot.SetSize( w, h + 25 );
	}

	bool IsVisible()
	{
		return layoutRoot.IsVisible();
	}
	
	void Show()
	{
		layoutRoot.Show( true );
		OnShow();
	}

	void Hide()
	{
		OnHide();
		layoutRoot.Show( false );
	}

	void OnShow()
	{
	}

	void OnHide() 
	{
	}

	override bool OnUpdate(Widget w)
	{
		if ( w == m_TitleWrapper )
		{
			m_TitleWrapper.SetPos( 0, 0, true );
		}
		return true;
	}

	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( w == m_CloseButton )
		{
			form.Hide();
		}

		return false;
	}

	override bool OnDrag( Widget w, int x, int y )
	{
		if ( w == m_TitleWrapper )
		{
			layoutRoot.GetPos( offsetX, offsetY );

			offsetX = x - offsetX;
			offsetY = y - offsetY;

			m_TitleWrapper.SetPos( 0, 0, true );
			m_TitleWrapper.SetPos( 0, 0, false );
			
			return false;
		}

		return true;
	}

	override bool OnDragging( Widget w, int x, int y, Widget reciever )
	{
		if ( w == m_TitleWrapper )
		{
			SetPosition( x - offsetX, y - offsetY );

			return false;
		}

		return true;
	}

	override bool OnDrop( Widget w, int x, int y, Widget reciever )
	{
		if ( w == m_TitleWrapper )
		{
			SetPosition( x - offsetX, y - offsetY );
			
			return false;
		}

		return true;
	}

	void SetPosition( int x, int y )
	{
		layoutRoot.SetPos( x, y, true );
		
		m_TitleWrapper.SetPos( 0, 0, true );
	}

	ref Widget GetLayoutRoot() 
	{
		return layoutRoot;
	}
}