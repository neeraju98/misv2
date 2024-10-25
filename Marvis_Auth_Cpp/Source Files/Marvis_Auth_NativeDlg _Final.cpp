#include "stdafx.h"
#include "Marvis_Auth_Native.h"
#include "Marvis_Auth_NativeDlg.h"
#include <fstream>
#include <iostream>
#include <sstream>                      // For std::stringstream
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
using namespace std;

#pragma warning( push )
#pragma warning( disable : 4189)
#pragma warning( disable : 4702)
#pragma warning( disable : 4800)

#pragma region Variable and Event Declaration
time_t tmStartTime, tmEndTime;
unsigned char  *out_pTemplateOld = NULL;
int TemplateLenOld = 0;
static bool IsGetQtyInCapTemp = false;
#define WM_IMAGE_PREVIEW        WM_USER + 2000
#define WM_DEVICE_CONNECTION    WM_USER + 2001
void  obCapturePreviewCallBack(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *PreviewImageData);
void  obCaptureCompleteCallBack(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *PreviewImageData);
void  obDeviceDetectionCallBack(const char* DeviceName, DEVICE_DETECTION_EVENT dvcStatus);
CString ImageFormatEnumToString(IMAGE_FORMAT t_format);//AUTH_00001
CCommTesterDlg *gpCommTesterDlg = NULL;
static int nFPSCounter = 0;
IMAGE_PREVIEW_PARAMS *g_ImagePreviewFrame;//AUTH_00001
std::thread *g_auto_capture; //AUTH_00002
std::thread *g_auto_matchfinger; //AUTH_00002
IRIS_COORDINATES irisCoordinates[2] = { 0 };
static int nTemplateSaveCnt = 0;
int nNoOfFrameCash = 6;

BOOL BitmapToStream(HBITMAP hbm, IStream **ppStream);
IStream *ppStream;

std::string getTodaysDateAsString()
{
	std::time_t rawtime;
	std::tm* timeinfo;
	char buffer[80];

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);
	std::strftime(buffer, 80, "%d%m%Y%H%M%S", timeinfo);
	std::string timestamp(buffer);
	return timestamp;
}

#pragma endregion

#pragma region Constructor Destructure Event
CCommTesterDlg::CCommTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommTesterDlg::IDD, pParent)

	, m_hBlankBitmap(NULL)
	, m_bKillFocusProcessing(false)
	, m_strTimeOut1(10000)
	, m_strQty1(55)
	, m_editQtyUpperThreshold(85)
	, m_EmpId(_T("MN00"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gpCommTesterDlg = this;
}
CCommTesterDlg::~CCommTesterDlg()
{
	DeleteObject(m_hBlankBitmap);
	DestroyIcon(m_hIcon);
}
#pragma endregion

#pragma region Form and Color Event
void CCommTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE_TYPE, m_cmbDeviceType);
	DDX_Control(pDX, IDC_COMBO_DEVICE_TEMP, m_cmbDeviceTemp);
	DDX_Control(pDX, IDC_BUTTON_INIT_DEVICE, m_btnInitDevice);
	DDX_Control(pDX, IDC_BUTTON_START_CAPTURE, m_btnStartCapture);
	DDX_Control(pDX, IDC_BUTTON_AUTO_CAPTURE, m_btnAutoCapture);
	DDX_Control(pDX, IDC_BUTTON_STOP_CAPTURE, m_btnStopCapture);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AS_BITMAP, m_btnSaveAsBitmap);
	DDX_Control(pDX, IDC_BUTTON_UNINIT_DEVICE, m_btnUninitDevice);
	DDX_Control(pDX, IDC_BUTTON_GETTEMP_DEVICE, m_btnGetTemplate);
	DDX_Control(pDX, IDC_STATIC_INFO_STATUS, m_ctrlOutput_info);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_imgCapturedImage);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_ctrlStatusText);
	DDX_Control(pDX, IDC_STATIC_QUALITY2, m_ctrlImageQuality2);
	DDX_Control(pDX, IDC_BUTTON_GETTEMP_DEVICE, m_btnGetTemplate);
	DDX_Control(pDX, IDC_BUTTON_AUTO_CAPTURE, m_btnAutoCapture);
	DDX_Text(pDX, IDC_EDIT_TMO, m_strTimeOut1);
	DDX_Text(pDX, IDC_EDIT_QTY1, m_strQty1);
	DDX_Control(pDX, IDC_COMBO_IMAGE_TYPE, m_cmb_ImageType);
	DDX_Control(pDX, IDC_BUTTON_MATCHTEMP_ANSI, m_btnMatchAnsi);
	DDX_Control(pDX, IDC_BUTTON_CHECK_DEVICE, m_btnCheckDevice);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_cntrlVersion);
	DDX_Control(pDX, IDC_COMBO_SUPPORT_DEVICE, m_cmb_SupportedDevices);
	DDX_Control(pDX, IDC_BUTTON_VER_DEVICE, m_btnVersion);
	DDX_Control(pDX, IDC_BUTTON_VER_DEVICESUPLIST, m_supportDevice);
	DDX_Control(pDX, IDC_BUTTON_VER_DEVICELIST, m_connectDvc);
	DDX_Control(pDX, IDC_MFCBUTTON1, m_selectimage);
	DDX_Control(pDX, IDC_STATIC_DEVICE_CONNECT, m_PicDeviceConnect);
	DDX_Control(pDX, IDC_STATIC_LOGO, m_Logo);
	DDX_Text(pDX, IDC_EDIT_QTY_UPPER_THRESHOLD, m_editQtyUpperThreshold);
	DDX_Control(pDX, IDC_EDIT_QTY1, m_editQty);
	DDX_Control(pDX, IDC_EDIT_TMO, m_editTimeout);
	DDX_Text(pDX, IDC_EDIT2, m_EmpId);
}
BEGIN_MESSAGE_MAP(CCommTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TEMP, &CCommTesterDlg::OnSelChangeComboDeviceTemp)
	ON_BN_CLICKED(IDC_BUTTON_INIT_DEVICE, &CCommTesterDlg::OnInitDeviceButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPTURE, &CCommTesterDlg::OnStartCaptureButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAPTURE, &CCommTesterDlg::OnStopCaptureButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AS_BITMAP, &CCommTesterDlg::OnSaveAsBitmapButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_UNINIT_DEVICE, &CCommTesterDlg::OnUninitDeviceButtonClicked)
	//ON_BN_CLICKED(IDC_BUTTON_GETTEMP_DEVICE, &CCommTesterDlg::GetTemplateDeviceButtonClicked)
	ON_MESSAGE(WM_IMAGE_PREVIEW, &CCommTesterDlg::OnMessageImagePreview)
	ON_MESSAGE(WM_DEVICE_CONNECTION, &CCommTesterDlg::OnDeviceConnection)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TEMP, &CCommTesterDlg::OnSelChangeComboDeviceTemp)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_CAPTURE, &CCommTesterDlg::OnBnClickedButtonAutoCapture)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_TYPE, &CCommTesterDlg::OnCbnSelchangeComboImageType)
	ON_BN_CLICKED(IDC_BUTTON_MATCHTEMP_DEVICE, &CCommTesterDlg::OnBnClickedButtonMatchtempDevice)
	ON_BN_CLICKED(IDC_BUTTON_MATCHTEMP_ANSI, &CCommTesterDlg::OnBnClickedButtonMatchtempAnsi)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_DEVICE, &CCommTesterDlg::OnBnClickedButtonCheckDevice)
	ON_BN_CLICKED(IDC_BUTTON_VER_DEVICE, &CCommTesterDlg::OnBnClickedButtonVerDevice)
	ON_BN_CLICKED(IDC_BUTTON_VER_DEVICELIST, &CCommTesterDlg::OnBnClickedButtonVerDevicelist)
	ON_BN_CLICKED(IDC_BUTTON_VER_DEVICESUPLIST, &CCommTesterDlg::OnBnClickedButtonVerDevicesuplist)
END_MESSAGE_MAP()
BOOL CCommTesterDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu * pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	char szOutput[512] = "";
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	//AUTH_00001
	for (int l_i = 0; l_i <= IMAGE_FORMAT::K7; l_i++)
		m_cmb_ImageType.AddString(ImageFormatEnumToString((IMAGE_FORMAT)l_i));
	m_cmb_ImageType.SetCurSel(0);

	SetFontFormat(m_Font, true);

	SetColourFormat(m_btnInitDevice, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnMatchAnsi, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnStartCapture, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnStopCapture, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnUninitDevice, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnSaveAsBitmap, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnGetTemplate, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnAutoCapture, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnCheckDevice, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_btnVersion, MantraColor, SuccessColor, m_Font);

	SetColourFormat(m_supportDevice, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_connectDvc, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_selectimage, MantraColor, SuccessColor, m_Font);

	SetColourFormat(m_ctrlImageQuality2, MantraColor, SuccessColor, m_Font);
	SetColourFormat(m_ctrlStatusText, MantraColor, SuccessColor, m_Font);

	SetColourFormat(m_ctrlOutput_info, MantraColor, SuccessColor, m_Font);
	

	//AUTH_00001
	m_CapturePreviewCallBack = (PREVIEW_CALLBACK)obCapturePreviewCallBack;
	m_CaptureCompleteCallBack = (CAPTURE_COMPLETE_CALLBACK)obCaptureCompleteCallBack;
	g_ImagePreviewFrame = (IMAGE_PREVIEW_PARAMS *)malloc(sizeof(IMAGE_PREVIEW_PARAMS));

	g_ImagePreviewFrame->imageBytes = 0;
	g_ImagePreviewFrame->imgQtyInfo.Quality = 0;
	g_ImagePreviewFrame->imgQtyInfo.X = 0;
	g_ImagePreviewFrame->imgQtyInfo.Y = 0;
	g_ImagePreviewFrame->imgQtyInfo.R = 0;
	g_ImagePreviewFrame->pCapturedImage = NULL;

	//m_imgCapturedImage.ModifyStyle(0xF, SS_BITMAP | SS_REALSIZECONTROL, SWP_NOSIZE);
	m_hBlankBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BLANK));
	BitmapToStream(m_hBlankBitmap, &ppStream);

	m_Logo.Load(CString(_T("Icon\\logo.png")));

	OnBnClickedButtonVerDevice();
	OnBnClickedButtonVerDevicesuplist();
	//OnBnClickedButtonVerDevicelist();

	m_btnInitDevice.EnableWindow(true);
	//m_imgCapturedImage.SetBitmap(m_hBlankBitmap);

	m_DEVICE_DETECTION = (DEVICE_DETECTION)obDeviceDetectionCallBack;
	int returnValue = MARVIS_Auth_RegisterDetectionCallback(m_DEVICE_DETECTION);

	MARVIS_Auth_EnableLogs(MARVIS_AUTH_LOG_LEVEL_OFF, NULL);

	//FILE* fptr = NULL;
	//fopen_s(&fptr, "Mean_StdDev.csv", "ab");
	//fprintf(fptr, "%s,%s,%s,%s,%s,%s,%s\n", "Emp_ID", "First_Mean", "Last_Mean", "First_stddev", "Last_stddev","Last Blur", "Result");
	////fwrite(data.c_str(), 1, strlen(data.c_str()), pEncodedFile1);
	//fclose(fptr);

	FILE* fptr1 = NULL;
	fopen_s(&fptr1, "Image_Properties.csv", "ab");
	fprintf(fptr1, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", "Emp_ID", "PWM_Value", "Mean", "Blur", "Texture", "Status", "Max High Texture", "Min High Texture", "Max low Texture", "Min low Texture", "texture_framecount", "texture_iriscount", "average_textureff", "max_texture_iris_count", "max_high_meanvalue", "min_high_meanvalue", "max_low_meanvalue", "min_low_meanvalue", "average_high_mean", "average_low_mean", "iris_mean", "pupil_mean", "mean ratio", "Laplcian count", "max_lap", "min_lap","magnitude","average_mag");
	
	//fwrite(data.c_str(), 1, strlen(data.c_str()), pEncodedFile1);
	fclose(fptr1);

	UpdateWindow();
	CenterWindow();
	return FALSE; // return TRUE  unless you set the focus to a control
}
void CCommTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void PaintBordersAndBackgroundColor(CWnd *pWnd)
{
	CPaintDC dc(pWnd);

	CRect rect;
	pWnd->GetClientRect(&rect);
	CPen pen;
	CBrush brush;

	pen.CreatePen(PS_SOLID, 4, MantraColor);

	brush.CreateSolidBrush(SuccessColor);
	// select brush and pen
	dc.SelectObject(&pen);
	dc.SelectObject(&brush);

	dc.Rectangle(rect.top, rect.left, rect.right, rect.bottom);
}
void CCommTesterDlg::OnPaint()
{
	PaintBordersAndBackgroundColor(this);

	CDialog::OnPaint();

}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCommTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#pragma endregion

#pragma region Combo Change Events
void CCommTesterDlg::OnCbnSelchangeComboImageType()
{//AUTH_00001
	m_ImageFormat = (IMAGE_FORMAT)m_cmb_ImageType.GetCurSel();
}
void CCommTesterDlg::OnSelChangeComboDeviceTemp()
{
	CString strDataResponseMode;
	m_cmbDeviceTemp.GetLBText(m_cmbDeviceTemp.GetCurSel(), strDataResponseMode);

	/*if (strDataResponseMode.Compare(CString(TEMPLATE_FORMAT_FMR_V2005)) == 0)
		m_TemplateFormat = FMR_V2005;

	if (strDataResponseMode.Compare(CString(TEMPLATE_FORMAT_FMR_V2011)) == 0)
		m_TemplateFormat = FMR_V2011;

	if (strDataResponseMode.Compare(CString(TEMPLATE_FORMAT_ANSI_V378)) == 0)
		m_TemplateFormat = ANSI_V378;*/
}
#pragma endregion

#pragma region Button Click Events
void CCommTesterDlg::OnInitDeviceButtonClicked()
{
	if (-1 == m_cmbDeviceType.GetCurSel())
	{
		AfxMessageBox(_T("Please select Device Type."));
		m_cmbDeviceType.SetFocus();
		return;
	}

	if (-1 == m_cmbDataResponseMode.GetCurSel())
	{
		AfxMessageBox(_T("Please select Data Response Mode."));
		m_cmbDataResponseMode.SetFocus();
		return;
	}
	ClearData();

	IsGetQtyInCapTemp = false;

	CWaitCursor cWaitCursor;

	CString strSelectedDeviceType;

	m_cmbDeviceType.GetLBText(m_cmbDeviceType.GetCurSel(), strSelectedDeviceType);
	char* pcProductName = CT2A(strSelectedDeviceType);//(char*)(LPCTSTR)strSelectedDeviceType;
	
	memset(&StDeviceInfo, 0, sizeof(MARVIS_DEVICE_INFO));

	int returnValue = MARVIS_Auth_InitDevice(0, pcProductName, strlen(pcProductName), &StDeviceInfo);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		SetColourFormat(m_ctrlStatusText, MantraColor, SuccessColor, m_Font);
		//SetFontFormat(m_Font, false);
		m_iImageFinalWidth = StDeviceInfo.Width;
		m_iImageFinalHeight = StDeviceInfo.Height;
		m_ctrlStatusText.SetWindowText(L"Init Success.");

		SetFontFormat(m_Font, false);
		m_ctrlOutput_info.SetFont(&m_Font);
		

		char szInitResult[512] = "";
		_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "SrNo:%s, Make:%s, Model :%s,Fw :%s, W:%d, H:%d",
			StDeviceInfo.SerialNo, StDeviceInfo.Make, StDeviceInfo.Model, StDeviceInfo.Firmware, StDeviceInfo.Width, StDeviceInfo.Height);
		m_ctrlOutput_info.SetWindowText(CA2CT((const char*)szInitResult));

		SetFontFormat(m_Font, true);
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
}
void CCommTesterDlg::OnStartCaptureButtonClicked()
{
	if (ValidateQualityTimeout() == -1)
		return;

	UpdateData(TRUE);
	ClearData();

	IsGetQtyInCapTemp = false;
	// Convert CString to char*
	CStringA strA(m_EmpId); // Convert to ANSI string
	char empID[2048];
	const char* empID1 = strA;
	memset(empID, 0, 2048);
	strcpy(empID, empID1);
	int returnValue = MARVIS_Auth_StartCapture(m_strQty1, m_strTimeOut1, m_CapturePreviewCallBack, m_CaptureCompleteCallBack, empID);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);
		m_ctrlStatusText.SetWindowText(CA2CT("Start Capture In Progress..."));
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
	UpdateData(FALSE);
}
void CCommTesterDlg::StartAutoCapture()//AUTH_00002
{
	try
	{
		UpdateData(TRUE);

		char szInitResult[512] = "";
		int Quality = 0;
		int returnValue = 0;

		MARVIS_IMAGE_DATA imageData;

		imageData.m_BitmapImage = (unsigned char *)malloc((m_iImageFinalHeight * m_iImageFinalWidth) + 1078);
		
		auto start = std::chrono::system_clock::now();
		
		tmStartTime = time(0);
		//returnValue = MARVIS_Auth_AutoCapture_RD(m_strQty1, m_editQtyUpperThreshold, nNoOfFrameCash, m_strTimeOut1, &Quality, m_CapturePreviewCallBack, &imageData);
		returnValue = MARVIS_Auth_AutoCapture(m_strTimeOut1, &Quality, m_CapturePreviewCallBack, &imageData);
		if (MARVIS_AUTH_E_SUCCESS == returnValue)
		{
			//auto end = std::chrono::system_clock::now();
			//tmEndTime = time(0);
			//std::chrono::duration<double> elapsed_seconds1 = end - start;
			//float elapsed_seconds2 = elapsed_seconds1.count();
			////strftime(buf, sizeof(buf), "%Y-%m-%d.%X", start);
			//long elapsed_seconds = tmEndTime - tmStartTime;
			//
			//char       buf[80] = { "\0" };
			//char       buf1[80] = { "\0" };
			//struct tm  tstruct;
			//
			//tstruct = *localtime(&tmStartTime);
			//strftime(buf, sizeof(buf), "%H%M%S", &tstruct);
			//tstruct = *localtime(&tmEndTime);
			//strftime(buf1, sizeof(buf1), "%H%M%S", &tstruct);
			//string f_name;
			//f_name =  buf ;
			//f_name = f_name+"_" + buf1 + ".bmp";
			
			
 			m_ctrlStatusText.SetWindowText(L"Auto Capture Completed!");

			gpCommTesterDlg->OnSaveAsBitmapButtonClicked();

		/*	FILE* pEncodedFile = NULL;
			fopen_s(&pEncodedFile, f_name.c_str(), "wb");
			fwrite(imageData.m_BitmapImage, 1, imageData.m_BitmapImageLen, pEncodedFile);
			fclose(pEncodedFile);

			FILE* pEncodedFile1 = NULL;
			fopen_s(&pEncodedFile1, "Test_Report.csv", "ab");
			std::string data = buf ;
			data = data + "," + buf1 + "," + to_string(tmStartTime) + "," + to_string(tmEndTime) + ',' + to_string(elapsed_seconds2) + "\n";
			fwrite(data.c_str(), 1, strlen(data.c_str()), pEncodedFile1);
			fclose(pEncodedFile1);*/

			GetTemplateDeviceButtonClicked();

			_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "QLTY: %d",
				Quality);

			CStatic *StatusTextControlHandle = gpCommTesterDlg->GetImageQualityControlHandle2();
			StatusTextControlHandle->SetWindowText(CA2CT((const char*)szInitResult));
		}
		else
		{
			m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
		}

		free(imageData.m_BitmapImage);

		UpdateData(FALSE);
	}
	catch (...)
	{
		return;
	}
	
}
void CCommTesterDlg::MatchFingerAutoCapture()//AUTH_00002
{
	MARVIS_IMAGE_DATA imageData;
	int Quality = 0;
	int returnValue = 0;

	imageData.m_BitmapImage = (unsigned char *)malloc((StDeviceInfo.Height * StDeviceInfo.Width) + 1078);

	//returnValue = MARVIS_Auth_AutoCapture_RD(m_strQty1, m_editQtyUpperThreshold, nNoOfFrameCash, m_strTimeOut1, &Quality, m_CapturePreviewCallBack, &imageData);
	returnValue = MARVIS_Auth_AutoCapture(m_strTimeOut1, &Quality, m_CapturePreviewCallBack, &imageData);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetWindowText(L"Auto Capture Completed!");

		char szInitResult[512] = "";
		_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "Quality: %d",
			Quality);
		CStatic *StatusTextControlHandle = gpCommTesterDlg->GetImageQualityControlHandle2();
		StatusTextControlHandle->SetWindowText(CA2CT((const char*)szInitResult));

	}
	else
	{
		free(imageData.m_BitmapImage);
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
		m_btnStopCapture.EnableWindow(true);
		return;
	}

	unsigned char* out_pTemplate = (unsigned char*)calloc(m_iImageFinalWidth*m_iImageFinalHeight + 20000, sizeof(unsigned char));
	int TemplateLen = 0;
	int intTemplateLen = 0;
	int matchScore = 0;
	returnValue = MARVIS_Auth_GetImage(out_pTemplate, &TemplateLenOld, m_ImageFormat, 10);
	if (MARVIS_AUTH_E_SUCCESS != returnValue)
	{
		free(imageData.m_BitmapImage);
		free(out_pTemplate);
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
		return;
	}

	returnValue = MARVIS_Auth_MatchIrisData(out_pTemplate, out_pTemplateOld, &matchScore);
	if (MARVIS_AUTH_E_SUCCESS != returnValue)
	{
		free(imageData.m_BitmapImage);
		free(out_pTemplate);
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
		return;
	}
	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		stringstream ss;
		ss << matchScore;
		string str = "Template Match With Score : " + ss.str();
		m_ctrlStatusText.SetWindowText(CA2CT((const char*)str.c_str()));
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}

	free(imageData.m_BitmapImage);
	free(out_pTemplate);

	UpdateData(FALSE);
}
void CCommTesterDlg::OnBnClickedButtonAutoCapture()
{
	if (ValidateQualityTimeout() == -1)
		return;
	
	/*if (m_iImageFinalHeight == 0 || m_iImageFinalWidth == 0)
	{
		m_ctrlStatusText.SetWindowText(L"MIS100V2 not initialized...");		
		return;
	}
	*/

	UpdateData(TRUE);
	ClearData();

	int strQuality1, strQuality2, strTimeout;
	int s = m_strTimeOut1;

	CWaitCursor cWaitCursor;
	
	m_ctrlStatusText.SetBkColor(MantraColor);
	m_ctrlStatusText.SetWindowText(L"Auto Capture In Progress...");

	g_auto_capture = new std::thread(&CCommTesterDlg::StartAutoCapture, this);

	UpdateData(FALSE);
}
void CCommTesterDlg::OnStopCaptureButtonClicked()
{
	UpdateData(TRUE);

	IsGetQtyInCapTemp = false;
	CWaitCursor cWaitCursor;

	int returnValue = MARVIS_Auth_StopCapture();
	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);
		m_ctrlStatusText.SetWindowText(L"Capture stopped.");
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}

	UpdateData(FALSE);
}

void CCommTesterDlg::OnSaveAsBitmapButtonClicked()
{
	//ClearData();
	UpdateData(TRUE);
	OnCbnSelchangeComboImageType();

	CWaitCursor cWaitCursor;
	unsigned char* out_pImage = (unsigned char*)calloc(m_iImageFinalWidth*m_iImageFinalHeight + 20000, sizeof(unsigned char));

	int ImageLen = 0;
	int intTemplateLen = 0;

	int returnValue = MARVIS_Auth_GetImage(out_pImage, &ImageLen, m_ImageFormat, 22);

	out_pImage[ImageLen] = 0;

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		std::stringstream fmrStream;

		CString strDataResponseMode;
		CString strMsg = L"";
		CString strExt;
		int Jp2Offset = 0;

		std::string curTime = getTodaysDateAsString();
		
		// Convert std::string to CString
		CString cTimeStr(curTime.c_str());

		m_cmb_ImageType.GetLBText(m_cmb_ImageType.GetCurSel(), strDataResponseMode);
		strExt = _T("output\\") + m_EmpId + _T("_") + cTimeStr  + strExt;

		if (m_ImageFormat == BMP)
		{
			strExt += _T(".bmp");
		}
		if (m_ImageFormat == RAW)
		{
			strExt += _T(".raw");
		}
		if (m_ImageFormat == K3)
		{
			strExt += _T(".jp2");
			//strExt.Format(L"%S_%d%S", "K3", nTemplateSaveCnt++, ".jp2");
			Jp2Offset = 68;
		}
		if (m_ImageFormat == K7)
		{
			strExt += _T(".jp2");
			//strExt.Format(L"%S_%d%S", "K7", nTemplateSaveCnt++, ".jp2");
			Jp2Offset = 68;
		}

		FILE* pEncodedFile = NULL;
		fopen_s(&pEncodedFile, CT2A(strExt), "wb");
		fwrite(out_pImage + Jp2Offset, 1, ImageLen - Jp2Offset, pEncodedFile);
		fclose(pEncodedFile);

		m_ctrlStatusText.SetBkColor(MantraColor);

		/*if (GetTemplateDeviceButtonClicked() == 0)
		{
		strMsg = L"and Template";
		}*/

		m_ctrlStatusText.SetWindowText(L"Image " + strMsg + " Save Successfully On Application Path.");
		free(out_pImage);
	}
	else
	{

		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
	IsGetQtyInCapTemp = false;
	UpdateData(FALSE);
}
void CCommTesterDlg::OnUninitDeviceButtonClicked()
{
	UpdateData(TRUE);
	CWaitCursor cWaitCursor;
	
	ClearData();
	//m_ctrlOutput_info.SetWindowText(CA2CT(""));
	IsGetQtyInCapTemp = false;

	int returnValue = MARVIS_Auth_UninitDevice();

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlOutput_info.SetWindowText(CA2CT(""));
		m_ctrlStatusText.SetBkColor(MantraColor);
		m_ctrlStatusText.SetWindowText(L"Uninit device succeeded.");

		m_btnInitDevice.EnableWindow(true);
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
	UpdateData(FALSE);
}
void CCommTesterDlg::OnBnClickedButtonMatchtempDevice()
{
	//ClearData();
	UpdateData(TRUE);

	UpdateData(FALSE);
}
void CCommTesterDlg::OnBnClickedButtonMatchtempAnsi()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	ClearData();
	CWaitCursor cWaitCursor;

	if (IsGetQtyInCapTemp == false)
	{

		m_ctrlStatusText.SetWindowText(L"First Follow Auto Capture Request !");
		return;
	}

	m_ctrlStatusText.SetWindowText(L"Auto Capture requested!");

	g_auto_matchfinger = new std::thread(&CCommTesterDlg::MatchFingerAutoCapture, this); //AUTH_00002

}

int CCommTesterDlg::GetTemplateDeviceButtonClicked()
{
	//ClearData();
	
	if (out_pTemplateOld == NULL)
		out_pTemplateOld = (unsigned char*)calloc(m_iImageFinalWidth*m_iImageFinalHeight + 20000, sizeof(unsigned char));

	int returnValue = MARVIS_Auth_GetImage(out_pTemplateOld, &TemplateLenOld, m_ImageFormat, 10);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);

		//m_ctrlStatusText.SetWindowText(L"Template Save Successfully On Application Path..");
		std::stringstream fmrStream;
		IsGetQtyInCapTemp = true;
		//CString strDataResponseMode = L"MIS100V2_Template";
		
		/*FILE* pEncodedFile = NULL;
		fopen_s(&pEncodedFile, CT2A(strDataResponseMode) + ".bmp", "wb");
		fwrite(out_pTemplateOld, 1, TemplateLenOld, pEncodedFile);
		fclose(pEncodedFile);*/
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
	
	/*if (out_pTemplateOld != NULL)
	{
		free(out_pTemplateOld);
		out_pTemplateOld = NULL;
	}*/
	return returnValue;
	return 0;
}

void CCommTesterDlg::OnBnClickedButtonCheckDevice()
{
	// TODO: Add your control notification handler code here

	if (-1 == m_cmbDeviceType.GetCurSel())
	{
		AfxMessageBox(_T("Please select Device Type."));
		m_cmbDeviceType.SetFocus();
		return;
	}

	if (-1 == m_cmbDataResponseMode.GetCurSel())
	{
		AfxMessageBox(_T("Please select Data Response Mode."));
		m_cmbDataResponseMode.SetFocus();
		return;
	}

	ClearData();
	
	CWaitCursor cWaitCursor;

	CString strSelectedDeviceType;
	m_cmbDeviceType.GetLBText(m_cmbDeviceType.GetCurSel(), strSelectedDeviceType);

	char* pcProductName = CT2A(strSelectedDeviceType);//(char*)(LPCTSTR)strSelectedDeviceType;

	int returnValue = MARVIS_Auth_IsDeviceConnected(0, pcProductName, strlen(pcProductName));

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);

		m_ctrlStatusText.SetWindowText(L"Device Connected.");
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));

	}

}
void CCommTesterDlg::OnBnClickedButtonVerDevice()
{
	char szMARVIS_Auth_DllVe1r[20] = { 0 };
	int returnValue = MARVIS_Auth_GetVersion(szMARVIS_Auth_DllVe1r);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);

		stringstream ss;
		ss << szMARVIS_Auth_DllVe1r;
		string str = ss.str();
		m_cntrlVersion.SetWindowText(CA2CT((const char*)str.c_str()));
		//m_cntrlVersion.setwi
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));

	}
	// TODO: Add your control notification handler code here
}
void CCommTesterDlg::OnBnClickedButtonVerDevicelist()
{

	int DeviceCnt = 0;
	int returnValue = MARVIS_Auth_GetDeviceList(NULL, &DeviceCnt);
	if (MARVIS_AUTH_E_SUCCESS != returnValue)
	{

		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));

		return;
	}

	DEVICE_LIST* StMIDEngine_DeviceList = new DEVICE_LIST[DeviceCnt];
	memset(StMIDEngine_DeviceList, 0, sizeof(StMIDEngine_DeviceList));
	returnValue = MARVIS_Auth_GetDeviceList(StMIDEngine_DeviceList, &DeviceCnt);

	if (MARVIS_AUTH_E_SUCCESS == returnValue && DeviceCnt > 0)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);

		m_PicDeviceConnect.Load(CString(_T("Icon\\Connect.png")));
		//m_ctrlStatusText.SetWindowText(L"Get Connected Device List Success.");
		std::string str;
		m_cmbDeviceType.ResetContent();
		for (int i = 0; i < DeviceCnt; i++)
		{
			char szInitResult[512] = "";
			_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "%s",
				StMIDEngine_DeviceList[i].Model);


			str.append(szInitResult);
			m_cmbDeviceType.AddString(CA2CT(szInitResult));

		}
		m_cmbDeviceType.SetCurSel(0);
		str = "Device Name : " + str;

	}
	else
	{
		m_cmbDeviceType.ResetContent();
		m_cmbDeviceType.SetCurSel(-1);
		m_ctrlStatusText.SetWindowText(CA2CT("Device Not Connected."));

	}
}
void CCommTesterDlg::OnBnClickedButtonVerDevicesuplist()
{
	// TODO: Add your control notification handler code here

	ClearData();

	int DeviceCnt = 0;
	int returnValue = MARVIS_Auth_GetSupportedDeviceList(NULL, &DeviceCnt);
	if (MARVIS_AUTH_E_SUCCESS != returnValue)
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
		return;
	}

	DEVICE_LIST* StMIDEngine_DeviceList = new DEVICE_LIST[DeviceCnt];
	memset(StMIDEngine_DeviceList, 0, sizeof(StMIDEngine_DeviceList));
	returnValue = MARVIS_Auth_GetSupportedDeviceList(StMIDEngine_DeviceList, &DeviceCnt);

	if (MARVIS_AUTH_E_SUCCESS == returnValue)
	{
		m_ctrlStatusText.SetBkColor(MantraColor);
		m_cmb_SupportedDevices.ResetContent();
		std::string str;
		for (int i = 0; i < DeviceCnt; i++)
		{
			char szInitResult[512] = "";
			_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "%s",
				StMIDEngine_DeviceList[i].Model);
			str.append(szInitResult);
			m_cmb_SupportedDevices.AddString(CA2CT(szInitResult));

		}
		m_cmb_SupportedDevices.SetCurSel(0);
	}
	else
	{
		m_ctrlStatusText.SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(returnValue)));
	}
	delete[] StMIDEngine_DeviceList;

}
#pragma endregion

#pragma region Functions
void CCommTesterDlg::OnClose()
{
	CWaitCursor cWaitCursor;
	EndDialog(IDCANCEL);
}
void CCommTesterDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}
void CCommTesterDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}
void CCommTesterDlg::ClearData()
{
	m_imgCapturedImage.SetBitmap(m_hBlankBitmap);
	m_imgCapturedImage.Load(ppStream);
	m_ctrlStatusText.SetWindowText(L"");
	m_ctrlImageQuality2.SetWindowText(L"");
	UpdateWindow();
}
void CCommTesterDlg::SetFontFormat(CFont &t_Font, BOOL t_Is_Bold)
{
	LOGFONT l_lf;

	// clear out structure
	memset(&l_lf, 0, sizeof(LOGFONT));

	if (t_Is_Bold == true)
	{
		// request a 17-pixel-height font
		l_lf.lfHeight = 17;
		l_lf.lfWeight = FW_BOLD; //FW_NORMAL, FW_REGULAR, and FW_BOLD
		// request a face name "Calibri"
		_tcsncpy_s(l_lf.lfFaceName, LF_FACESIZE, _T("Calibri"), 9);

		t_Font.CreateFontIndirect(&l_lf); // create the font
	}
	else
	{
		// request a 14-pixel-height font
		l_lf.lfHeight = 15;
		l_lf.lfWeight = FW_BOLD; //FW_NORMAL, FW_REGULAR, and FW_BOLD
		// request a face name "Calibri"
		_tcsncpy_s(l_lf.lfFaceName, LF_FACESIZE, _T("Calibri"), 9);

		t_Font.CreateFontIndirect(&l_lf); // create the font
	}
}
void CCommTesterDlg::SetColourFormat(CMFCButton &t_btn, COLORREF t_face_color, COLORREF t_text_color, CFont &t_Font)
{
	t_btn.EnableWindowsTheming(FALSE);      // (important!)
	t_btn.SetFaceColor(t_face_color);     // Change to your desired Background Color
	t_btn.SetTextColor(t_text_color); // Change it to your desired Foreground Color
	t_btn.SetFont(&t_Font);
}
void CCommTesterDlg::SetColourFormat(CColorStatic &t_obj, COLORREF t_back_color, COLORREF t_text_color, CFont &t_Font)
{
	t_obj.SetBkColor(t_back_color);     // Change to your desired Background Color
	t_obj.SetTextColor(t_text_color); // Change it to your desired Foreground Color
	t_obj.SetFont(&t_Font);
}
CString ImageFormatEnumToString(IMAGE_FORMAT t_format) {//AUTH_00001
	switch (t_format) {
	case BMP:
		return (CString)"BMP";
	case RAW:
		return (CString)"RAW";
	case K3:
		return (CString)"K3";
	case K7:
		return (CString)"K7";
	default:
		return (CString)"";
	}
}
int CCommTesterDlg::ValidateQualityTimeout()
{
	CString lTmp;
	m_editQty.GetWindowTextW(lTmp);

	if (lTmp == L"")
	{
		MessageBox(L"Enter an integer.");
		m_editQty.SetFocus();
		return -1;
	}

	m_editTimeout.GetWindowTextW(lTmp);

	if (lTmp == L"")
	{
		MessageBox(L"Enter an integer.");
		m_editTimeout.SetFocus();
		return -1;
	}

	return 0;
}
BOOL BitmapToStream(HBITMAP hbm, IStream **ppStream)
{
	if (hbm == NULL)
		return FALSE;

	CImage   img;
	IStream* ps = NULL;
	HRESULT  hr = CreateStreamOnHGlobal(NULL, TRUE, &ps);

	if (FAILED(hr))
		return FALSE;

	img.Attach(hbm);
	hr = img.Save(ps, Gdiplus::ImageFormatBMP);
	if (FAILED(hr))
	{
		ps->Release();
		return FALSE;
	}

	*ppStream = ps;
	return TRUE;
}
#pragma endregion

#pragma region Callback and Events
void obCaptureCompleteCallBack(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *m_previewImageData)
{
	if (MARVIS_AUTH_E_SUCCESS == ErrorCode)
	{
		if (g_ImagePreviewFrame)
		{
			if (g_ImagePreviewFrame->pCapturedImage == NULL)
			{
				g_ImagePreviewFrame->pCapturedImage = (unsigned char *)malloc(m_previewImageData->m_BitmapImageLen);
			}

			if (g_ImagePreviewFrame->pCapturedImage)
			{
				memset(g_ImagePreviewFrame->pCapturedImage, 0, m_previewImageData->m_BitmapImageLen);
				memcpy(g_ImagePreviewFrame->pCapturedImage, m_previewImageData->m_BitmapImage, m_previewImageData->m_BitmapImageLen);
				g_ImagePreviewFrame->imageBytes = m_previewImageData->m_BitmapImageLen;
				g_ImagePreviewFrame->imgQtyInfo.Quality = QtyInfo->Quality;
				g_ImagePreviewFrame->imgQtyInfo.X = QtyInfo->X;
				g_ImagePreviewFrame->imgQtyInfo.Y = QtyInfo->Y;
				g_ImagePreviewFrame->imgQtyInfo.R = QtyInfo->R;

				//CPictureCtrl* pImageControl = gpCommTesterDlg->GetImageControlHandle();
				//pImageControl->Load(g_ImagePreviewFrame->pCapturedImage, g_ImagePreviewFrame->imageBytes);

				//	char exeTimeInfo[25];
				//	MARVIS_Auth_GetFunctionalInfo(exeTimeInfo);

				irisCoordinates[0].r = g_ImagePreviewFrame->imgQtyInfo.R;
				irisCoordinates[0].x = g_ImagePreviewFrame->imgQtyInfo.X;
				irisCoordinates[0].y = g_ImagePreviewFrame->imgQtyInfo.Y;

				//pImageControl->setFaceRect(&irisCoordinates[0], 1);

				char szInitResult[512] = "";
#ifdef FPS_DEVELOPEMENT_MODE
				_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "Quality: %d : %s", QtyInfo->Quality, QtyInfo->CaptureTime);
#else
				//_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), "Quality: %d", QtyInfo->Quality);
#endif

				::PostMessage(gpCommTesterDlg->m_hWnd, WM_IMAGE_PREVIEW, 0, reinterpret_cast<LPARAM>(g_ImagePreviewFrame));

				CStatic *StatusTextControlHandle = gpCommTesterDlg->GetImageQualityControlHandle2();
				StatusTextControlHandle->SetWindowText(CA2CT((const char*)szInitResult));


				CStatic *pStatusTextControlHandle = gpCommTesterDlg->GetStatusTextControlHandle();
				pStatusTextControlHandle->SetWindowText(CA2CT("Capture Completed"));

				gpCommTesterDlg->OnSaveAsBitmapButtonClicked();
			}
		}
	}
	else
	{
		CStatic *pStatusTextControlHandle = gpCommTesterDlg->GetStatusTextControlHandle();
		pStatusTextControlHandle->SetWindowText(CA2CT(MARVIS_Auth_GetErrDescription(ErrorCode)));
	}
}
void obDeviceDetectionCallBack(const char* DeviceName, DEVICE_DETECTION_EVENT dvcStatus)
{
	const char* EventName = "";

	if (dvcStatus == EVENT_CONNECTED)
	{
		EventName = "ATTACHED";
	}
	if (dvcStatus == EVENT_DISCONNECTED)
	{
		EventName = "DETACHED";
	}

	char szInitResult[512] = "";
	_snprintf_s(szInitResult, sizeof(szInitResult), sizeof(szInitResult), " Device Name: [%s] , Status: [%s]", DeviceName, EventName);
	CStatic *StatusTextControlHandle = gpCommTesterDlg->GetStatusTextControlHandle();


	CPictureCtrl *StatusPicControlHandle = gpCommTesterDlg->GetPictureControlHandle();
	string DeviceStatus = "ATTACHED";
	DEVICE_AUTOCONNECT *pImagePreviewParams = (DEVICE_AUTOCONNECT *)malloc(sizeof(DEVICE_AUTOCONNECT));

	if ((DeviceStatus.compare(EventName) == 0))
	{
		pImagePreviewParams->pEventName = 1;
		StatusPicControlHandle->Load(CString(_T("Icon\\Connect.png")));
	}
	else
	{
		pImagePreviewParams->pEventName = 0;
		StatusPicControlHandle->Load(CString(_T("Icon\\Disconnect.png")));

		gpCommTesterDlg->m_ctrlOutput_info.SetWindowText(CA2CT(""));

		gpCommTesterDlg->ClearData();
	}
	::PostMessage(gpCommTesterDlg->m_hWnd, WM_DEVICE_CONNECTION, 0,
		reinterpret_cast<LPARAM>(pImagePreviewParams));

	StatusTextControlHandle->SetWindowText(CA2CT(szInitResult));


}
void obCapturePreviewCallBack(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *m_previewImageData)
{//AUTH_00001
	if (MARVIS_AUTH_E_SUCCESS == ErrorCode)
	{
		if (g_ImagePreviewFrame)
		{
			if (g_ImagePreviewFrame->pCapturedImage == NULL)
			{
				g_ImagePreviewFrame->pCapturedImage = (unsigned char *)malloc(m_previewImageData->m_BitmapImageLen);
			}

			if (g_ImagePreviewFrame->pCapturedImage)
			{
				memset(g_ImagePreviewFrame->pCapturedImage, 0, m_previewImageData->m_BitmapImageLen);
				memcpy(g_ImagePreviewFrame->pCapturedImage, m_previewImageData->m_BitmapImage, m_previewImageData->m_BitmapImageLen);
				g_ImagePreviewFrame->imageBytes = m_previewImageData->m_BitmapImageLen;
				g_ImagePreviewFrame->imgQtyInfo.Quality = QtyInfo->Quality;
				g_ImagePreviewFrame->imgQtyInfo.X = QtyInfo->X;
				g_ImagePreviewFrame->imgQtyInfo.Y = QtyInfo->Y;
				g_ImagePreviewFrame->imgQtyInfo.R = QtyInfo->R;
				g_ImagePreviewFrame->imgQtyInfo.percent_occlusion = QtyInfo->percent_occlusion;
				g_ImagePreviewFrame->imgQtyInfo.sharpness = QtyInfo->sharpness;
				g_ImagePreviewFrame->imgQtyInfo.pupil_blur = QtyInfo->pupil_blur;
				g_ImagePreviewFrame->imgQtyInfo.pupil_diameter = QtyInfo->pupil_diameter;
				g_ImagePreviewFrame->imgQtyInfo.sclera_mean = QtyInfo->sclera_mean;
				g_ImagePreviewFrame->imgQtyInfo.iris_mean = QtyInfo->iris_mean;
				g_ImagePreviewFrame->imgQtyInfo.iris_diameter = QtyInfo->iris_diameter;
				g_ImagePreviewFrame->imgQtyInfo.pupil_mean = QtyInfo->pupil_mean;

				
#ifdef FPS_DEVELOPEMENT_MODE
				memcpy(g_ImagePreviewFrame->imgQtyInfo.CaptureTime, QtyInfo->CaptureTime, 49);
#endif
			
				::PostMessage(gpCommTesterDlg->m_hWnd, WM_IMAGE_PREVIEW, 0, reinterpret_cast<LPARAM>(g_ImagePreviewFrame));
			}
		}
	}

}
LRESULT CCommTesterDlg::OnDeviceConnection(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	UNREFERENCED_PARAMETER(wParam);
	int returnValue = MARVIS_AUTH_E_SUCCESS;
	try
	{
		OnBnClickedButtonVerDevicelist();
		DEVICE_AUTOCONNECT *pImagePreviewParams = reinterpret_cast<DEVICE_AUTOCONNECT *>(lParam);
		free(pImagePreviewParams);
	}
	catch (const std::exception& /* ex */)
	{
		returnValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		returnValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	if (ValidateQualityTimeout() == -1)
		return -1;

	UpdateData(FALSE);
	return *reinterpret_cast<LRESULT const *>(&returnValue);
}
LRESULT CCommTesterDlg::OnMessageImagePreview(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	int returnValue = MARVIS_AUTH_E_SUCCESS;
	try
	{
		IMAGE_PREVIEW_PARAMS *pImagePreviewParams;
		pImagePreviewParams = g_ImagePreviewFrame;


		if (pImagePreviewParams)
		{
			int nImageLengthWithBitmapHeader = pImagePreviewParams->imageBytes;
			unsigned char *pBitmapImage = (unsigned char *)malloc(nImageLengthWithBitmapHeader);

			memcpy(pBitmapImage, pImagePreviewParams->pCapturedImage, pImagePreviewParams->imageBytes);

			if (gpCommTesterDlg)
			{
				if (nImageLengthWithBitmapHeader > 0 && strlen((char *)pBitmapImage) > 0)
				{
					CPictureCtrl* pImageControl = gpCommTesterDlg->GetImageControlHandle();
					pImageControl->Load(pBitmapImage, nImageLengthWithBitmapHeader);

					/*char exeTimeInfo[25];
					MARVIS_Auth_GetFunctionalInfo(exeTimeInfo);*/

					CHAR szImageQuality[512];

#ifdef FPS_DEVELOPEMENT_MODE
					_snprintf_s(szImageQuality, sizeof(szImageQuality), sizeof(szImageQuality), "QLTY : %d : %s", pImagePreviewParams->imgQtyInfo.Quality, pImagePreviewParams->imgQtyInfo.CaptureTime);
#else
					//_snprintf_s(szImageQuality, sizeof(szImageQuality), sizeof(szImageQuality), "QLTY : [%d] P_Blur:[ %d] P_Per: [%d] P_Sharp: [%d] p_Dia: [%d] sc_mean: [%d] ", pImagePreviewParams->imgQtyInfo.Quality, pImagePreviewParams->imgQtyInfo.pupil_blur, pImagePreviewParams->imgQtyInfo.percent_occlusion,pImagePreviewParams->imgQtyInfo.sharpness,pImagePreviewParams->imgQtyInfo.pupil_diameter,pImagePreviewParams->imgQtyInfo.sclera_mean);
					_snprintf_s(szImageQuality, sizeof(szImageQuality), sizeof(szImageQuality), 
						"QLTY : [%d] Frame:[ %d] "/*FM: [%d]  PM: [%d] BL: [%d] WC: [%d] BC: [%d] SP: [%d] PDD: [%d]"*/, 
						pImagePreviewParams->imgQtyInfo.Quality, pImagePreviewParams->imgQtyInfo.percent_occlusion/*, pImagePreviewParams->imgQtyInfo.iris_mean, 
						pImagePreviewParams->imgQtyInfo.pupil_mean, pImagePreviewParams->imgQtyInfo.pupil_blur, pImagePreviewParams->imgQtyInfo.iris_diameter,
						pImagePreviewParams->imgQtyInfo.pupil_diameter, pImagePreviewParams->imgQtyInfo.sharpness, pImagePreviewParams->imgQtyInfo.sclera_mean*/);
					//_snprintf_s(szImageQuality, sizeof(szImageQuality), sizeof(szImageQuality), "QLTY : %d ", pImagePreviewParams->imgQtyInfo.Quality);
#endif
					//_snprintf_s(szImageQuality, sizeof(szImageQuality), sizeof(szImageQuality), "QLTY : %d", pImagePreviewParams->imgQtyInfo.Quality);
					CStatic *pImageQualityControlHandle2 = gpCommTesterDlg->GetImageQualityControlHandle2();
					pImageQualityControlHandle2->SetWindowText(CA2CT(szImageQuality));
					pImageQualityControlHandle2->ShowWindow(SW_SHOW);

					irisCoordinates[0].r = pImagePreviewParams->imgQtyInfo.R;
					irisCoordinates[0].x = pImagePreviewParams->imgQtyInfo.X;
					irisCoordinates[0].y = pImagePreviewParams->imgQtyInfo.Y;

					pImageControl->setFaceRect(&irisCoordinates[0], 1);
				}
			}
			
			free(pBitmapImage);
		}
	}
	catch (const std::exception& /* ex */)
	{
		returnValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		returnValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return *reinterpret_cast<LRESULT const *>(&returnValue);
}
#pragma endregion

#pragma warning(pop)
