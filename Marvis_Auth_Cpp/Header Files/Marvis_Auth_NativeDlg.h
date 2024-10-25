#pragma once
#include "afxwin.h"

#ifdef __cplusplus 
extern "C" {
#endif 

#include "Marvis_Auth.h"

#include "colorstatic.h"
#include "picturectrl.h"


#ifdef __cplusplus 
}
#endif 
#include "afxbutton.h"

#define BITMAP_FILE_HEADER_LENGTH   14      //Length in Bytes
#define BITMAP_IMAGE_HEADER_LENGTH  40      //Length in Bytes
#define BITMAP_COLOR_PALETTE_LENGTH 1024    //Length in Bytes
#define BITMAP_TOTAL_LENGTH         (BITMAP_FILE_HEADER_LENGTH + BITMAP_IMAGE_HEADER_LENGTH + BITMAP_COLOR_PALETTE_LENGTH)

#define MantraColor RGB(0, 112, 192)
#define ErrorColor RGB(255, 215, 0)
#define SuccessColor RGB(255, 255, 255)



typedef struct _IMAGE_PREVIEW_PARAMS
{
	unsigned char *pCapturedImage;
	int imageBytes;
	MARVIS_IMAGE_QUALITY imgQtyInfo;
} IMAGE_PREVIEW_PARAMS;


typedef struct _DEVICE_AUTOCONNECT
{
	int pEventName;
}  DEVICE_AUTOCONNECT;


class CCommTesterDlg : public CDialog
{
// Construction
public:
	CCommTesterDlg(CWnd* pParent = NULL);	// standard constructor
    ~CCommTesterDlg();

// Dialog Data
	enum { IDD = IDD_BIOMETRIC_REFLASHER_TOOL_DIALOG };

	CPictureCtrl* GetImageControlHandle()
    {
        return &m_imgCapturedImage;
    }
	/*CStatic* GetImageControlHandle()
	{
		return &m_imgCapturedImage;
	}*/

	CStatic* GetImageQualityControlHandle2()
    {
        return &m_ctrlImageQuality2;
    }
	CStatic* GetStatusTextControlHandle()
    {
        return &m_ctrlStatusText;
    }
	CPictureCtrl* GetPictureControlHandle()
	{
		return &m_PicDeviceConnect;
	}
	
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	MARVIS_DEVICE_INFO StDeviceInfo;

	PREVIEW_CALLBACK m_CapturePreviewCallBack;
	CAPTURE_COMPLETE_CALLBACK m_CaptureCompleteCallBack;
	DEVICE_DETECTION m_DEVICE_DETECTION;
    CComboBox m_cmbDeviceType;
	CComboBox m_cmbDeviceTemp;
    CComboBox m_cmbDataResponseMode;
	
	IMAGE_FORMAT m_ImageFormat;
   
    HBITMAP m_hBlankBitmap;
    bool m_bKillFocusProcessing;
    
    BOOL IsManufacturingDataValid();
	void SetFontFormat(CFont &t_Font, BOOL t_Is_Bold);
	void SetColourFormat(CMFCButton &t_btn, COLORREF t_face_color, COLORREF t_text_color, CFont &t_Font);
	void SetColourFormat(CColorStatic &t_btn, COLORREF t_face_color, COLORREF t_text_color, CFont &t_Font);
	void StartAutoCapture(); 
	void StartAutoCaptureFMR();
	void MatchFingerAutoCapture();
	int ValidateQualityTimeout();

	// Generated message map functions
	virtual BOOL OnInitDialog();
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	DECLARE_MESSAGE_MAP()
public:
	CFont m_Font;
	CBrush m_brush;
	CPngImage logoImage;
	CStatic pictureCtrl;
	CBrush bgBrush;

    afx_msg void OnSelChangeComboDataResponseMode();
    afx_msg void OnInitDeviceButtonClicked();
    afx_msg void OnStartCaptureButtonClicked();
    afx_msg void OnStopCaptureButtonClicked();
    afx_msg void OnSaveAsBitmapButtonClicked();
    afx_msg LRESULT OnMessageImagePreview(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceConnection(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUninitDeviceButtonClicked();
    afx_msg void OnQuality1EditKillFocus();
    afx_msg void OnQuality2EditKillFocus();
    afx_msg void OnTimeOutEditKillFocus();
    afx_msg void OnClose();
	
	void ClearData();
private:
	int m_strTimeOut1;
	int m_strQty1;
	int m_iImageFinalWidth;
    int m_iImageFinalHeight;
	HBITMAP          m_hBitmap;

    virtual void OnOK();
    virtual void OnCancel();

public:
	afx_msg  int GetTemplateDeviceButtonClicked();
	afx_msg void OnSelChangeComboDeviceTemp();
	afx_msg void OnBnClickedButtonAutoCapture();
	CComboBox m_cmb_ImageType;
	afx_msg void OnCbnSelchangeComboImageType();
	afx_msg void OnBnClickedButtonMatchtempDevice();
	afx_msg void OnBnClickedButtonMatchtempAnsi();
	afx_msg void OnBnClickedButtonCheckDevice();
	afx_msg void OnBnClickedButtonVerDevice();
	afx_msg void OnBnClickedButtonVerDevicelist();
	afx_msg void OnBnClickedButtonVerDevicesuplist();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	CMFCButton m_btnInitDevice;
	CMFCButton m_btnStartCapture;
	CMFCButton m_btnStopCapture;
	CMFCButton m_btnSaveAsBitmap;
	CMFCButton m_btnUninitDevice;
	CMFCButton m_btnMatchAnsi;
	CMFCButton m_btnCheckDevice;
	CMFCButton m_btnGetTemplate;
	CMFCButton m_btnAutoCapture;
	CMFCButton m_btnVersion;
	CMFCButton m_supportDevice;
	CMFCButton m_connectDvc;
	CMFCButton m_selectimage;

	
	CColorStatic m_ctrlImageQuality2;
	CColorStatic m_ctrlStatusText;

	CColorStatic m_ctrlOutput_info;
	CStatic m_ctrlVersion;
	//CStatic m_imgCapturedImage;
	CPictureCtrl m_imgCapturedImage;
	
	CStatic m_cntrlVersion;
	CComboBox m_cmb_SupportedDevices;
	CPictureCtrl m_PicDeviceConnect;
	CPictureCtrl m_Logo;
	int m_editQtyUpperThreshold;
	CEdit m_editQty;
	CEdit m_editTimeout;
	CString m_EmpId;
	int Capture_Type_Value;
};
