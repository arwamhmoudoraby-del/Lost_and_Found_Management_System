#include "framework.h"
#include "Final Project.h"

#include <pqxx/pqxx>
#include "Database.h"

// ImGui Headers
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>


#include <iostream>
#include <string>
#include <vector>

using namespace std;


// ============================================================
//                    DOMAIN CLASSES
// ============================================================

// Person
class Person {
    // ...
protected:

    int id;
    string username;
    string passwordHash;

public:
    int getId();
    string getuserName();
    bool verifyPassword(string passwod);
    virtual bool login(string username, string password) = 0;

    virtual ~Person() = default;
};

// User
class User : public Person {
    // ...
private:
    string email;
public:

    bool registerAccount();
    bool upadteProfile( const string& newUserName,const string& newEmail);
    bool changePassword( const string& oldPassword ,const string& newpassword);
    bool login(string userName , string password) override ;

};


// Admin
class Admin : public Person {
    // ...

  public:
        bool login(string username, string password) override ;
        bool reviewClaim(int claimId, bool isApproved);
        bool markItemAsReturned(int itemId);
   

};

// Category
class Category {
    // ...
private:

    int id;
    string name;
public:

    int getId();
    string getName();



};

// Item
class Item {
    // ...
protected:

    int id;
    Category category;
    string color;
    string location;
    string date;
    string imagePath;
    string status;

public:
    virtual string getDetails() = 0;
    string getStatus();


};


// LostReport
class LostReport : public Item {
    // ...
private:

    int userId;
    string privateDescription;

public:

    bool saveToDb();
    bool updateReport();
    bool deleteReport();
    string getPrivateDescription();
    string getDetails() override ;


};


// FoundReport
class FoundReport : public Item {
    // ...
private:

    int adminId;
    string publicDescription;
    string privateNotes;

public:
    bool saveToDb();
    bool updateReport();
    string getPrivateNotes();
    string getDetails() override ;



};


// PossibleMatch
class PossibleMatch {
    // ...

private:

    int id;
    int lostReportId;
    int foundReportId;
    int matchScore;
    string status; 

public:

    int getId();
    int getMatchScore();


};


// ClaimRequest
class ClaimRequest {
    // ...
private:

    int id;
    int userId;
    int matchId;
    string identifyingDetails;
    string status;   
    int reviewedByAdminId;
    
public:

    bool submitClaim();
    bool updateStatus(string newStatus);



};

// MatchingEngine
class MatchingEngine {
    // ...

private:

    int calculateScore( const LostReport& lost, const FoundReport& found);

public:

    void runMatchForLostReport( const LostReport& report);
    void runMatchForFoundReport( const FoundReport& report);


};

//Notification 
class Notification {
private:
    int id;
    int userId;
    string message;
    string type;
    bool isRead;
public:
    Notification(int id,int userId,const string & message,const string & type,bool isRead)
        :id(id),userId(userId),message(message),type(type),isRead(isRead){}
public:
    int getId()const {
        return id;
    }
    int getUserId() const{
        return userId;
    }
    string getMessage()const {
        return message;
    }
    string getType() const {
        return type;
    }
    bool getIsRead()const {
        return isRead;
    }
};

// NotificationService
class NotificationService {
    // ...
public:

    bool sendNotification(int userid, const string& msg, const string& type) {

        string query = "INSERT INTO notifications (user_id,message,type) VALUES ("
            +to_string (userid)+",'"
            + msg + "','"
            + type + "');";

        return Database::getInstance().executeNonSelect(query);
    }

    vector<Notification> getUserNotifications(int userid) {

        vector<Notification> userNotifications;

        string query = "SELECT id,user_id,message,type,is_read FROM notifications WHERE user_id=" + to_string(userid) + ";";
        pqxx::result result = Database::getInstance().executeQuery(query);

        for (const auto& row : result) {
            int id = row["id"].as<int>();
            int uid = row["user_id"].as<int>();
            string msg = row["message"].as<string>();
            string type = row["type"].as<string>();
            bool isRead = row["is_read"].as<bool>();

            Notification notify(id, uid, msg, type, isRead);
            userNotifications.push_back(notify);
        }
        return userNotifications;
    }

    bool markAsRead(int notificationId) {
        string query = "UPDATE notifications SET is_read=TRUE WHERE id=" + to_string(notificationId) + ";";
        return Database::getInstance().executeNonSelect(query);
    }

};

// SearchService
class SearchService {
    // ...
public:

    vector<LostReport> searchLostReports(int categoryId, string color, string location);
    vector<FoundReport> searchFoundReports(int categoryId, string color, string location);


};

// AdminDashboard
class AdminDashboard {
    // ...
public:

    int getTotalUsers() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM users");
        return r[0][0].as<int>();
        


    }
    int getTotalLostItems() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM lost_reports");
        return r[0][0].as<int>();


    }
    int getTotalFoundItems() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM found_reports");
        return r[0][0].as<int>();


    }
    int getPendingClaimsCount() {
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'pending'"
        );
        return r[0][0].as<int>();

    }
    int getApprovedClaimsCount() {
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'approved'"
        );
        return r[0][0].as<int>();

    }
    int getReturnedItemsCount() {

        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'returned'"
        );
        return r[0][0].as<int>();
    }
    


};


// Forward declarations
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global Variables for DirectX 11
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



// RENDER FUNCTIONS 

void RenderNotificationsTab(int currentUserId)
{
    NotificationService notificationService;
    vector<Notification> notifications = notificationService.getUserNotifications(currentUserId);

    if (notifications.empty())
    {
        ImGui::Text("No notifications yet.");
    }
    else
    {
        for (auto& n : notifications)
        {
            ImGui::PushID(n.getId());

            if (n.getIsRead())
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", n.getMessage().c_str());
            else
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "%s", n.getMessage().c_str());

            ImGui::SameLine();

            if (!n.getIsRead())
            {
                if (ImGui::Button("Mark as read"))
                {
                    notificationService.markAsRead(n.getId());
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

}



void RenderDashboardTab()
{
    AdminDashboard dashboard;

    ImGui::Separator();
    ImGui::Text("Admin Dashboard");

    ImGui ::Text("Total Users: %d", dashboard.getTotalUsers());
    ImGui::Text("Total Lost Items: %d" ,dashboard.getTotalLostItems());
    ImGui::Text("Total Found Items: %d" , dashboard.getTotalFoundItems());
    ImGui::Text("Pending Claims: %d", dashboard.getPendingClaimsCount());
    ImGui::Text("Approved Claims: %d", dashboard.getApprovedClaimsCount());
    ImGui::Text("Returned Items: %d", dashboard.getReturnedItemsCount());
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 1. Create Application Window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Class", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Main Application", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // 2. Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // 3. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // --- تكبير حجم الخط لجميع عناصر الواجهة ---
    io.FontGlobalScale = 1.5f;

    // Setup Style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Main Loop
   
    int currentPage = 0;

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start ImGui Frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //RENDR 
       
        // --- Application Window UI ---
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Dashboard");

        ImGui::Text("Welcome to the System Dashboard!");
        ImGui::Separator();

        if (ImGui::Button("Lost Item"))
        {
            currentPage = 1;
        }

        if (ImGui::Button("Found Item"))
        {
            currentPage = 2;
        }

        if (ImGui::Button("Search"))
        {
            currentPage = 3;
        }

        if (ImGui::Button("Notifications"))
        {
            currentPage = 4;
        }

        // Dashboard
        if (currentPage == 0)
        {
            RenderDashboardTab();
        }


        if (currentPage == 1)
        {
            ImGui::Separator();
            ImGui::Text("Lost Item Page");
        }

        if (currentPage == 2)
        {
            ImGui::Separator();  //line
            ImGui::Text("Found Item Page");
        }

        if (currentPage == 3)
        {
            ImGui::Separator();
            ImGui::Text("Search Page");
        }

        if (currentPage == 4)
        {

            RenderNotificationsTab(1);

        }


        ImGui::End();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions for DirectX 11
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}