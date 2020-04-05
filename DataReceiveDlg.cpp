
// DataReceiveDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DataReceive.h"
#include "DataReceiveDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	
protected:
	afx_msg LRESULT OnGetsensordata(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CDataReceiveDlg 对话框



CDataReceiveDlg::CDataReceiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATARECEIVE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataReceiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDataReceiveDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_PORTEDIT, &CDataReceiveDlg::OnEnChangePortedit)
	ON_BN_CLICKED(IDC_STARTCAR, &CDataReceiveDlg::OnBnClickedStartcar)
END_MESSAGE_MAP()


// CDataReceiveDlg 消息处理程序
static CDataReceiveDlg* p;
BOOL CDataReceiveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: 在此添加额外的初始化代码
	p = this;
	
	//初始化地址编辑框
	CString str = L"112.74.89.58";
	GetDlgItem(IDC_IPADDRESS)->SetWindowTextW(str);

	//初始化套接字
	if (!AfxSocketInit()) {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	}
	else {
		MessageBox(L"Socket 初始化成功！",  MB_OK);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDataReceiveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataReceiveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataReceiveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDataReceiveDlg::OnEnChangePortedit() {
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}




DWORD WINAPI Thread_Listen(LPVOID params) {
	SOCKET skt = (SOCKET)params;
	sockaddr_in addr = { 0 };
	int addrLen = sizeof(addr);
	getpeername(skt, (sockaddr*)&addr, &addrLen);//获取套接字的信息

	char buf[1024];
	int ret;
	while (true) {
		memset(buf, 0, sizeof(buf));
		ret = recv(skt, buf, sizeof(buf), 0);
		strcat(buf, "\0");
		CString str(buf);
		//CString str = L"WTF";
		
		p->GetDlgItem(IDC_SENSOR)->SetWindowTextW((LPCTSTR)str);
		
	}

	return 0;
}

DWORD WINAPI Thread_ACCEPT(LPVOID params) {
	while (true) {
		SOCKET sktCli = INVALID_SOCKET;
		SOCKET sktServ = (SOCKET)params;
		sockaddr_in addr = { 0 };
		int addrLen = sizeof(addr);
		sktCli = accept(sktServ, (sockaddr*)&addr, &addrLen);
		if (sktCli == INVALID_SOCKET) {
			MessageBox(NULL, L"[client] accpet error ...\n", NULL, MB_OK);
		}
		//MessageBox(NULL,L"[client] accept ...\n", L"connect success!",  MB_OK);
		//创建线程
		HANDLE hThread = CreateThread(NULL, 0, Thread_Listen, (LPVOID)sktCli, 0, NULL);
	}
	return 0;
}

void CDataReceiveDlg::OnBnClickedStartcar() {
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(L"click", MB_OK);
	SOCKET sktServ = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);
	serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	bind(sktServ, (struct sockaddr*) & serv_addr, sizeof(serv_addr));

	DWORD threadId;
	
	
	listen(sktServ, 5);
	
	HANDLE hThread = CreateThread(NULL, 0, Thread_ACCEPT, (LPVOID)sktServ, 0, NULL);
}



