//=============================================================================
// Copyright © 2017 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: HelpSupportPage.cpp,v 1.9 2010-03-23 21:50:56 soowei Exp $
//=============================================================================

#include "Precompiled.h"
#include "HelpSupportPage.h"

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#endif

namespace FlyCapture2
{
	const char* HelpSupportPage::sk_linkButtonKB = "linkbuttonKB";
	const char* HelpSupportPage::sk_linkButtonSupportTickets = "linkbuttonSupportTickets";
	const char* HelpSupportPage::sk_linkButtonDownloads = "linkbuttonDownloads";

	HelpSupportPage::HelpSupportPage()
	{

	}

	HelpSupportPage::HelpSupportPage( CameraBase* pCamera, Glib::RefPtr<Gnome::Glade::Xml> refXml ) : BasePage( pCamera, refXml )
	{
	}

	HelpSupportPage::~HelpSupportPage()
	{
	}

	void HelpSupportPage::UpdateWidgets()
	{
	}

	void HelpSupportPage::OnLinkButtonClicked(
			Gtk::LinkButton* /*button*/,
			const Glib::ustring& uri )
	{
		Utilities::LaunchBrowser( uri.c_str() );
	}

	void HelpSupportPage::GetWidgets()
	{
	}

	void HelpSupportPage::AttachSignals()
	{
		Gtk::LinkButton::set_uri_hook(
				sigc::mem_fun( *this, &HelpSupportPage::OnLinkButtonClicked ) );
	}
}
