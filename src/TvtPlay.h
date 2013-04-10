﻿#ifndef INCLUDE_TVT_PLAY_H
#define INCLUDE_TVT_PLAY_H

// プラグインクラス
class CTvtPlay : public TVTest::CTVTestPlugin, public ITvtPlayController
{
    static const int BUTTON_MAX = 18;
    static const int BUTTON_TEXT_MAX = 192;
    static const int TIMER_AUTO_HIDE_INTERVAL = 100;
    static const int TIMER_SYNC_CHAPTER_INTERVAL = 1000;
    static const int TIMER_WATCH_POS_GT_INTERVAL = 1000;
    static const int HASH_LIST_MAX_MAX = 10000;
    static const int POPUP_MAX_MAX = 100;
public:
    // CTVTestPlugin
    CTvtPlay();
    virtual bool GetPluginInfo(TVTest::PluginInfo *pInfo);
    virtual bool Initialize();
    virtual bool Finalize();
    // ITvtPlayController
    bool IsOpen() const { return m_hThread ? true : false; }
    int GetPosition() { CBlockLock lock(&m_tsInfoLock); return m_infoPos; }
    int GetDuration() { CBlockLock lock(&m_tsInfoLock); return m_infoDur; }
    int GetTotTime() { CBlockLock lock(&m_tsInfoLock); return m_infoTot; }
    int IsExtending() { CBlockLock lock(&m_tsInfoLock); return m_infoExtMode; }
    bool IsPaused() { CBlockLock lock(&m_tsInfoLock); return m_fInfoPaused; }
    CChapterMap& GetChapter() { return m_chapter; }
    bool IsAllRepeat() const { return m_fAllRepeat; }
    bool IsSingleRepeat() const { return m_fSingleRepeat; }
    bool IsRepeatChapterEnabled() const { return m_fRepeatChapter; }
    bool IsSkipXChapterEnabled() const { return m_fSkipXChapter; }
    bool IsPosDrawTotEnabled() const { return m_fPosDrawTot; }
    int GetStretchID();
    void SetupWithPopup(const POINT &pt, UINT flags);
    void EditChapterWithPopup(int pos, const POINT &pt, UINT flags);
    void EditAllChaptersWithPopup(const POINT &pt, UINT flags);
    void Pause(bool fPause);
    void SeekToBegin();
    void SeekToEnd();
    void Seek(int msec);
    void SeekAbsolute(int msec);
    void OnCommand(int id, const POINT *pPt = NULL, UINT flags = 0);
private:
    void AnalyzeCommandLine(LPCWSTR cmdLine, bool fIgnoreFirst);
    void LoadSettings();
    void LoadTVTestSettings();
    void SaveSettings(bool fWriteDefault = false) const;
    bool InitializePlugin();
    bool EnablePlugin(bool fEnable);
    bool IsAppMaximized() { return (::GetWindowLong(m_pApp->GetAppWindow(), GWL_STYLE) & WS_MAXIMIZE) != 0; }
    HWND GetFullscreenWindow();
    bool OpenWithDialog();
    bool OpenWithPopup(const POINT &pt, UINT flags);
    bool OpenWithPlayListPopup(const POINT &pt, UINT flags);
    int TrackPopup(HMENU hmenu, const POINT &pt, UINT flags);
    bool OpenCurrent(int offset = -1);
    bool Open(LPCTSTR fileName, int offset);
    void Close();
    void SetupDestination();
    void WaitAndPostToSender(UINT Msg, WPARAM wParam, LPARAM lParam, bool fResetAll);
    void SetModTimestamp(bool fModTimestamp);
    void SetRepeatFlags(bool fAllRepeat, bool fSingleRepeat);
    void Stretch(int stretchID);
    void BeginWatchingNextChapter(bool fDoDelay);
    bool CalcStatusRect(RECT *pRect, bool fInit = false);
    void OnResize(bool fInit = false);
    void OnDispModeChange(bool fStandby, bool fInit = false);
    void OnFrameResize();
    void EnablePluginByDriverName();
    void OnPreviewChange(bool fPreview);
    static LRESULT CALLBACK EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData);
    static BOOL CALLBACK WindowMsgCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult, void *pUserData);
    static LRESULT CALLBACK FrameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void UpdateInfos();
    static DWORD WINAPI TsSenderThread(LPVOID pParam);

    // 初期パラメータ
    bool m_fInitialized;
    bool m_fSettingsLoaded;
    bool m_fForceEnable, m_fIgnoreExt;
    bool m_fAutoEnUdp, m_fAutoEnPipe;
    bool m_fEventExecute;
    bool m_fEventStartupDone;
    bool m_fPausedOnPreviewChange;
    TCHAR m_szIniFileName[MAX_PATH];
    TCHAR m_szSpecFileName[MAX_PATH];
    int m_specOffset;
    bool m_fShowOpenDialog;

    // コントロール
    HWND m_hwndFrame;
    bool m_fAutoHide, m_fAutoHideActive;
    bool m_fHoveredFromOutside;
    int m_statusRow, m_statusRowFull;
    int m_statusHeight;
    bool m_fSeekDrawOfs, m_fSeekDrawTot, m_fPosDrawTot;
    int m_posItemWidth;
    int m_timeoutOnCmd, m_timeoutOnMove;
    int m_dispCount;
    DWORD m_lastDropCount;
    int m_resetDropInterval;
    POINT m_lastCurPos, m_idleCurPos;
    CStatusView m_statusView;
    CStatusViewEventHandler m_eventHandler;
    TCHAR m_szIconFileName[MAX_PATH];
    int m_seekList[COMMAND_S_MAX];
    int m_stretchList[COMMAND_S_MAX];
    int m_seekListNum, m_stretchListNum;
    TCHAR m_buttonList[BUTTON_MAX][BUTTON_TEXT_MAX];
    int m_popupMax;
    TCHAR m_szPopupPattern[MAX_PATH];
    bool m_fPopupDesc, m_fPopuping;
    bool m_fDialogOpen;

    // TS送信
    HANDLE m_hThread, m_hThreadEvent;
    DWORD m_threadID;
    int m_threadPriority;
    CTsSender m_tsSender;
    CCriticalLock m_tsInfoLock;
    int m_infoPos, m_infoDur, m_infoTot, m_infoExtMode, m_infoSpeed;
    bool m_fInfoPaused;
    bool m_fHalt, m_fAllRepeat, m_fSingleRepeat;
    bool m_fRepeatChapter, m_fSkipXChapter;
    int m_supposedDispDelay;
    int m_resetMode;
    int m_stretchMode, m_noMuteMax, m_noMuteMin;
    bool m_fConvTo188, m_fUseQpc, m_fModTimestamp;
    int m_pcrThresholdMsec;

    // ファイルごとの固有情報
    int m_salt, m_hashListMax;
    struct HASH_INFO {
        LONGLONG hash; // 56bitハッシュ値
        int resumePos; // レジューム位置(msec)
    };
    std::list<HASH_INFO> m_hashList;
    // 再生リスト
    CPlaylist m_playlist;
    // 現在再生中のチャプター
    CChapterMap m_chapter;
};

#endif // INCLUDE_TVT_PLAY_H
