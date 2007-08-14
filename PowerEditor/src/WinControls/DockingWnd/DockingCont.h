// this file is part of docking functionality for Notepad++
// Copyright (C)2005 Jens Lorenz <jens.plugin.npp@gmx.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#ifndef DOCKINGCONT
#define DOCKINGCONT

#include "StaticDialog.h"
#include "Resource.h"
#include "Docking.h"
#include <windows.h>
#include <string>
#include <vector>
#include <commctrl.h>

using namespace std;


/* window styles */
#define POPUP_STYLES		(WS_POPUP|WS_CLIPSIBLINGS|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MAXIMIZEBOX)
#define POPUP_EXSTYLES		(WS_EX_CONTROLPARENT|WS_EX_WINDOWEDGE|WS_EX_TOOLWINDOW)
#define CHILD_STYLES		(WS_CHILD|WS_VISIBLE)
#define CHILD_EXSTYLES		(0x00000000L)


enum eMousePos {
	posOutside,
	posCaption,
	posClose
};



class DockingCont : public StaticDialog
{
public:
	DockingCont();
	~DockingCont();

    void init(HINSTANCE hInst, HWND hWnd) {
		Window::init(hInst, hWnd);
	};

	HWND getTabWnd(void) {
		HWND	hRet = NULL;
		if (isCreated())
			hRet = _hContTab;

		return hRet;
	};
	HWND getCaptionWnd(void) {
		HWND	hRet = NULL;
		if (isCreated())
		{
			if (_isFloating == false)
				hRet = _hCaption;
			else
				hRet = _hSelf;
		}

		return hRet;
	};

	tTbData* createToolbar(tTbData data, Window **ppWin);
	tTbData  destroyToolbar(tTbData data);
	tTbData* findToolbarByWnd(HWND hClient);
	tTbData* findToolbarByName(char* pszName);

	void showToolbar(tTbData *pTbData, BOOL state);

	BOOL updateInfo(HWND hClient) {
		for (size_t iTb = 0; iTb < _vTbData.size(); iTb++)
		{
			if (_vTbData[iTb]->hClient == hClient)
			{
				updateCaption();
				return TRUE;
			}
		}
		return FALSE;
	};

	void setActiveTb(tTbData* pTbData);
	void setActiveTb(int iItem);
	int getActiveTb(void);
	tTbData* getDataOfActiveTb(void);
	vector<tTbData *> getDataOfAllTb(void) {
		return _vTbData;
	};
	vector<tTbData *> getDataOfVisTb(void);
	bool isTbVis(tTbData* data);

	void doDialog(bool willBeShown = true, bool isFloating = false);

	bool isFloating(void) {
		return _isFloating;
	}

	int getElementCnt(void) {
		return _vTbData.size();
	}

	/* interface function for gripper */
	BOOL startMovingFromTab(void) {
		BOOL	dragFromTabTemp = _dragFromTab;
		_dragFromTab = FALSE;
		return dragFromTabTemp;
	};

	void setCaptionTop(BOOL isTopCaption) {
		_isTopCaption = (isTopCaption == CAPTION_TOP);
		onSize();
	};

	void focusClient(void);

    virtual void destroy() {
		for (int iTb = _vTbData.size(); iTb > 0; iTb--)
		{
			delete _vTbData[iTb-1];
		}
		::DestroyWindow(_hSelf);
	};

protected :

	/* Subclassing caption */
	LRESULT runProcCaption(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndCaptionProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		return (((DockingCont *)(::GetWindowLong(hwnd, GWL_USERDATA)))->runProcCaption(hwnd, Message, wParam, lParam));
	};

	/* Subclassing tab */
	LRESULT runProcTab(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndTabProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		return (((DockingCont *)(::GetWindowLong(hwnd, GWL_USERDATA)))->runProcTab(hwnd, Message, wParam, lParam));
	};

    virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

	/* drawing functions */
	void drawCaptionItem(DRAWITEMSTRUCT *pDrawItemStruct);
	void drawTabItem(DRAWITEMSTRUCT *pDrawItemStruct);
	void onSize(void);

	/* functions for caption handling and drawing */
	eMousePos isInRect(HWND hwnd, int x, int y);

	/* handling of toolbars */
	void doClose(void);

	/* return new item */
	int  SearchPosInTab(tTbData* pTbData);
	void SelectTab(int item);

	int  hideToolbar(tTbData* pTbData);
	void viewToolbar(tTbData *pTbData);

	void updateCaption(void);
	LPARAM NotifyParent(UINT message);

private:
	/* handles */
	bool					_isFloating;
	HWND					_hCaption;
	HWND					_hContTab;

	/* horizontal font for caption and tab */
	HFONT					_hFont;

	/* caption params */
	BOOL					_isTopCaption;
	char					_pszCaption[256];
	BOOL					_isMouseDown;
	BOOL					_isMouseClose;
	BOOL					_isMouseOver;
	RECT					_rcCaption;

	/* Important value for DlgMoving class */
	BOOL					_dragFromTab;

	/* subclassing handle for caption */
	WNDPROC					_hDefaultCaptionProc;

	/* subclassing handle for tab */
	WNDPROC					_hDefaultTabProc;

	/* for moving and reordering */
	UINT					_prevItem;
	BOOL					_beginDrag;
	HIMAGELIST				_hImageList;

	/* data of added windows */
	vector<tTbData *>		_vTbData;
};



#endif // DOCKINGCONT